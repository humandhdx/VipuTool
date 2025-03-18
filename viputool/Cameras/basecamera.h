#ifndef BASECAMERA_H
#define BASECAMERA_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include <mutex>
#include <atomic>
#include <vector>
#include <QImage>
#include <v4l2/Periodlc_rt2.hpp>
#include <v4l2/v4l2capture.h>
#include <QDebug>

class BaseCamera : public QObject {
    Q_OBJECT
public:
    explicit BaseCamera(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~BaseCamera() {
        stopCamera();
        releaseBuffers();
    }

    virtual bool openCamera(const int id,const int width ,const int height) {
        if (captureing_flag_) {
            return false; // 相机已在捕获状态
        }
        stopCamera(); // 停止已有的任务
        width_=width;
        height_=height;
        device_id_ = id;
        if (!createCamera()) {
            return false;
        }
        return true;
    }

    virtual void stopCamera() {
        std::lock_guard<std::mutex> lock(stopmtx_);
        if (run_servo_task_) {
            run_servo_task_->stop();
            delete run_servo_task_;
            run_servo_task_ = nullptr;

        }
        if (camera_) {
            delete camera_;
            camera_ = nullptr;
        }
        device_id_ = -1;
        captureing_flag_ = false;
        failed_count_ = 0;
        releaseBuffers();
    }

    virtual void startCapture() {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!run_servo_task_) {
            int priority = 49;
            run_servo_task_ = new RtPeriodicMemberFun2<BaseCamera>(capture_period_,
                                                                   "capture_task",
                                                                   1024 * 1024,
                                                                   priority,
                                                                   &BaseCamera::captureImage,
                                                                   this);
            run_servo_task_->start();
        }
        captureing_flag_ = true;
    }

protected:
    virtual bool createCamera() = 0; // 子类实现具体的创建逻辑

    virtual void captureImage() {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!captureing_flag_) return;
        if (!camera_) {
            incrementFailedCount();
            return;
        }

        timeval tv{};
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        if (!camera_->isReadable(&tv)) {
            incrementFailedCount();
            return;
        }

        memset(buff_, 0, width_ * height_);
        int rsize = camera_->read(reinterpret_cast<char*>(buff_), width_ * height_ - 1);
        handleFrameData(rsize);
    }

    void handleFrameData(int rsize) {
        if (rsize <= 0) {
            incrementFailedCount();
            return;
        }
        failed_count_ = 0; // 成功读取初始化失败统计
        if (camera_->getFormat() == target_format_) {
            vec_buff_->clear();
            vec_buff_->assign(buff_, buff_ + rsize);
            try {
                cv_mat_ = cv::imdecode(*vec_buff_, cv::IMREAD_COLOR);
            } catch (const std::exception& e) {
                qWarning() << "解码失败:" << e.what();
                emit cameraStopped();
                captureing_flag_ = false;
                return;
            }
            if (!cv_mat_.empty()) {
                QImage image(cv_mat_.data,
                             cv_mat_.cols,
                             cv_mat_.rows,
                             cv_mat_.step,
                             QImage::Format_BGR888);
                emit sendImage(image);
            }
        } else {
            qWarning() << "格式不受支持";
        }
    }

    virtual void incrementFailedCount() {
        failed_count_++;
        if (failed_count_ >= max_failed_count_) {
            qWarning() << "获取帧失败次数过多，停止相机。";
            captureing_flag_ = false;
            emit cameraStopped();
        }
    }

    void allocateBuffers() {
        releaseBuffers();
        buff_ = new uchar[width_ * height_];
        vec_buff_ = new std::vector<uchar>();
        vec_buff_->reserve(width_ * height_);
    }

    void releaseBuffers() {
        if (buff_) {
            delete[] buff_;
            buff_ = nullptr;
        }
        if (vec_buff_) {
            delete vec_buff_;
            vec_buff_ = nullptr;
        }
    }

protected:
    std::mutex mtx_;
    std::mutex stopmtx_;
    V4l2Capture*    camera_ = nullptr;
    RtPeriodicMemberFun2<BaseCamera>* run_servo_task_ = nullptr;

    uchar* buff_ = nullptr;
    std::vector<uchar>* vec_buff_ = nullptr;

    int device_id_ = -1;
    int width_ = 0;
    int height_ = 0;
    int failed_count_ = 0;
    constexpr static int max_failed_count_ = 10;

    double capture_period_ = 0.003;
    bool captureing_flag_ = false;

    // 摄像头目标格式：子类可以修改该值
    int target_format_ = V4L2_PIX_FMT_MJPEG;

    cv::Mat cv_mat_;

signals:
    void sendImage(QImage image);
    void cameraStopped();
};

#endif // BASECAMERA_H
