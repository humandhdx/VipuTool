#include "globalcamera.h"

#include <QEventLoop>
#include <QImage>
#include <future>

GlobalCamera::GlobalCamera(QObject *parent)
    : QObject{parent}
{
    width_=4000;
    height_=3000;
    buff_=new uchar[4000*3000];
    memset(buff,0,4000 * 3000);
    vec_buff_=new std::vector<uchar>();
    vec_buff_->reserve(4000*3000);
}

GlobalCamera::~GlobalCamera()
{
    stopCamera();
    if(buff_){
        delete []buff_;
        buff_=nullptr;
    }
    if(vec_buff_){
        vec_buff_->clear();
        delete vec_buff_;
        vec_buff_=nullptr;
    }
}

bool GlobalCamera::openCamera(const int id)
{
    if(captureing_flag_||id<0){
        return false;
    }
    device_id_=id;
    creat_camera();
    return true;
}

void GlobalCamera::stopCamera()
{
    std::lock_guard<std::mutex> lck{mtx_creat_};
    if(run_servo_task_){
        run_servo_task_->stop();
        delete run_servo_task_;
        run_servo_task_=nullptr;
    }
    if(camera_){
        delete camera_;
        camera_=nullptr;
    }
    device_id_=-1;
    failed_count_=0;
    captureing_flag_=false;
}

void GlobalCamera::startCapture()
{
    failed_count_ = 0;
    if (run_servo_task_ == nullptr) {
        int priority = 49;
        run_servo_task_ = new RtPeriodicMemberFun2<GlobalCamera>(capture_period_,
                                                                    "run_capture_left_task",
                                                                    1024 * 1024,
                                                                    priority,
                                                                    &GlobalCamera::capture_image,
                                                                    this);
        run_servo_task_->start();
    }
    captureing_flag_ = true;
}

void GlobalCamera::creat_camera()
{
    std::lock_guard<std::mutex> lck{mtx_creat_};
    if (camera_) {
        delete camera_;
        camera_ = nullptr;
    }
    std::ostringstream oss_left;
    oss_left << "/dev/video" << device_id_;
    int verbose = 0;
    V4l2IoType ioTypeIn = IOTYPE_MMAP;
    int fps = 10;
    V4L2DeviceParameters param(
        oss_left.str().c_str(), format_, width_, height_, fps, ioTypeIn, verbose);
    camera_ = V4l2Capture::create(param);
}

void GlobalCamera::capture_image()
{
    if (!captureing_flag_) return;
    std::lock_guard<std::mutex> lck{mtx_creat_};
    if (camera_ == nullptr) {
        failed_count_++;
        if (failed_count_ >= 10) {
            captureing_flag_ = false;
             emit cameraStopped();
            //RCLCPP_INFO(kLogger, "cvcamera restart left 1");
        }
        return;
    }

    tv_.tv_sec = 1;
    tv_.tv_usec = 0;
    if (camera_->isReadable(&tv_)) {//判断资源是否在 tv_left_ 设定的超时时间内可读。
        memset(buff_, 0, 4000 * 3000);
        int rsize = camera_->read((char *)buff_, 4000 * 3000 - 1);
        if (rsize == -1) {
            ++failed_count_;
            if (failed_count_ >= 10) {
                qWarning() << "设备异常";
                emit cameraStopped();
                captureing_flag_ = false;
                //RCLCPP_INFO(kLogger, "cvcamera restart left 2");
            }
            return;
        }
        else if (rsize == 0) {
            ++failed_count_;
            if (failed_count_ >= 10) {
                captureing_flag_ = false;
                qWarning() << "设备异常";
                emit cameraStopped();
                //RCLCPP_INFO(kLogger, "cvcamera restart left 3");
            }
            return;
        }
        else {
            failed_count_ = 0;
            // left_imgae_available_time_ = std::chrono::steady_clock::now();
            captureing_flag_ = true;
            if (camera_->getFormat() == V4L2_PIX_FMT_MJPEG) {
                // std::lock_guard<std::mutex> lck{mtx_camera_left_};
                vec_buff_->clear();
                vec_buff_->resize(rsize + 1);
                vec_buff_->assign(buff_, buff_ + rsize);
                try {
                    // 使用 cv::imdecode 解码 MJPEG 数据为 cv::Mat
                    cv_mat_ = cv::imdecode(*vec_buff_, cv::IMREAD_COLOR);
                }
                catch (const std::exception &e) {
                    qWarning() << "cv::imdecode 异常:" << e.what();
                    emit cameraStopped();
                    return;
                }
                if(cv_mat_.empty())return;
                QImage image(cv_mat_.data,
                             cv_mat_.cols,
                             cv_mat_.rows,
                             static_cast<int>(cv_mat_.step),
                             QImage::Format_BGR888);
                emit sendImage(image);
            }
            else {
                ++failed_count_;
                if (failed_count_ >= 10) {
                    captureing_flag_ = false;
                    emit cameraStopped();
                    //RCLCPP_INFO(kLogger, "cvcamera restart left 4");
                }
            }
        }
    }
    else {
        ++failed_count_;
        if (failed_count_ >= 10) {
            captureing_flag_ = false;
            emit cameraStopped();
            //RCLCPP_INFO(kLogger, "cvcamera restart left 5");
        }
    }
}
