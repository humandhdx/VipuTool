#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H
#include <QObject>
#include <mutex>
#include <QtWidgets/QFileDialog>
#include <QStandardPaths>
#include "v4l2/Periodlc_rt2.hpp"
#include "libv4l2.h"
#include "libv4lconvert.h"
#include "v4l2/v4l2capture.h"
#include <dirent.h>
#include <charconv>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>  //open
#include <unistd.h>  //close
#include <sstream>
#include <iostream>
#include <string>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "v4l2/struct.h"
#include <thread>

class cameraManager: public QObject
{
    Q_OBJECT
public:
    enum  CalibritionType{
        Null,
        HandEyeCalibrition,
        GlobalCalibrition,
        MiddleCalibrition,
    };
    cameraManager(QObject *parent = nullptr);
    ~cameraManager();
public slots:
    bool startCamera(const int l_r);//0-left 1-right 2-double
    void stopCamera();
    QString getCaptureImageSavePath();
    bool captureImage(QString path,CalibritionType type);
    void resetCaptureCount();
    QString currentDirectory() const {
        return QDir::currentPath();
    }
private:
    void init_cam();
    bool start_left_capture();
    bool start_right_capture();
    bool start_middle_capture();
    void capture_left();
    void capture_right();
    void capture_middle();
    void restart_left_cam();
    void restart_right_cam();
    void restart_middle_cam();
    void shutdown_leftcapture();
    void shutdown_rightcapture();
    void shutdown_middlecapture();
    QImage cvMatToQImage(cv::Mat &mat);

    int format_ = V4L2_PIX_FMT_MJPEG;
    int height_ = 992;
    int width_ = 992;

    //access resource with pointer
    V4l2Capture *camera_left_ = nullptr;
    V4l2Capture *camera_middle_ = nullptr;
    V4l2Capture *camera_right_ = nullptr;

    cv::Mat left_image_0_;
    cv::Mat middle_image_0_;
    cv::Mat right_image_0_;
    CImageTime left_image_available_0_;
    CImageTime right_image_available_0_;
    TimePoint left_imgae_available_time_;
    TimePoint right_imgae_available_time_;

    uchar *buff_left_ = nullptr;
    uchar *buff_middle_ = nullptr;
    uchar *buff_right_ = nullptr;
    std::vector<uchar> *vec_buff_left_ = nullptr;
    std::vector<uchar> *vec_buff_middle_ = nullptr;
    std::vector<uchar> *vec_buff_right_ = nullptr;

    std::unique_ptr<std::thread> thread_monitor_;

    timeval tv_left_;
    timeval tv_right_;

    RtPeriodicMemberFun2<cameraManager> *run_servo_task_left_ = nullptr;
    RtPeriodicMemberFun2<cameraManager> *run_servo_task_right_ = nullptr;
    RtPeriodicMemberFun2<cameraManager> *run_servo_task_middle_ = nullptr;
    int left_id_, right_id_,middle_id_;
    CalibritionType cali_type_;
    //控制相关
    std::mutex mtx_restart_left_;
    std::mutex mtx_restart_middle_;
    std::mutex mtx_restart_right_;
    std::mutex mtx_camera_left_;
    std::mutex mtx_camera_middle_;
    std::mutex mtx_camera_right_;
    std::mutex mtx_observer_camera_;
    std::atomic_bool capture_left_flag_ = false;
    std::atomic_bool capture_middle_flag_ = false;
    std::atomic_bool capture_right_flag_ = false;
    std::atomic_bool suc_capture_left_flag_ = false;
    std::atomic_bool suc_capture_middle_flag_ = false;
    std::atomic_bool suc_capture_right_flag_ = false;
    std::atomic_bool monite_flag_ = true;
    std::atomic_bool update_flag_ = true;
    double capture_period_ = 0.003;
    double capture_period_right_ = 0.003;
    double capture_period_update_ = 0.003;

    int failed_count_left_ = 0;
    int failed_count_right_ = 0;
    int failed_count_middle_ = 0;

    int restart_failed_left_ = 0;
    int restart_failed_right_ = 0;
    int restart_failed_middle_ = 0;
    // 用于记录当前保存的图片编号
    int handeye_image_count_;
    int middle_image_count_;
    int global_left_image_count_;
    int global_right_image_count_;

signals:
    void signalSendLeftImage(QImage image);
    void signalSendRightImage(QImage image);
    void signalSendMiddleImage(QImage image);
};

#endif // CAMERAMANAGER_H
