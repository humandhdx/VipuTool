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
#include <opencv2/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include "v4l2/struct.h"
#include <thread>
#include <QProcess>

class cameraManager: public QObject
{
    Q_OBJECT
    Q_PROPERTY(double max_foc READ max_foc WRITE setMax_foc NOTIFY max_focChanged FINAL)
    Q_PROPERTY(double min_foc READ min_foc WRITE setMin_foc NOTIFY min_focChanged FINAL)
    Q_PROPERTY(double cur_foc READ cur_foc WRITE setCur_foc NOTIFY cur_focChanged FINAL)
public:
    cameraManager(QObject *parent = nullptr);
    ~cameraManager();
    double max_foc() const;
    void setMax_foc(double newMax_foc);

    double min_foc() const;
    void setMin_foc(double newMin_foc);

    double cur_foc() const;
    void setCur_foc(double newCur_foc);

public slots:
    bool startCamera(const int l_r);//0-left 1-right 2-double 3-local lfet right
    void stopCamera();
    bool start_camera_capture(const QString &path, int type, int count);
    bool deleteFisterCaptureImage(QString path);
    bool clearCaptureCount(QString path);
    void open_path(QString path);
    void openMalLab();
    void startFouc(int l_r);
    QString currentDirectory() const {
        return QDir::currentPath();
    }
private:
    void init_cam();

    bool start_left_capture();
    bool start_right_capture();
    bool start_local_left_capture();
    bool start_local_right_capture();
    bool start_middle_capture();

    void capture_left();
    void capture_right();
    void capture_local_left();
    void capture_local_right();
    void capture_middle();

    void restart_left_cam();
    void restart_right_cam();
    void restart_middle_cam();
    void restart_local_left_cam();
    void restart_local_right_cam();

    void shutdown_leftcapture();
    void shutdown_rightcapture();
    void shutdown_middlecapture();
    void shutdown_localleftcapture();
    void shutdown_localrightcapture();

    bool set_bino_bus_addr(std::pair<int, int> &left_pos, std::pair<int, int> &right_pos);
    std::pair<int, int> get_bus_position(const std::string camera_path);
    bool set_binocular_device_id(int &id_left, int &id_right);
    void EOG(const cv::Mat &mat);
    double canny_edge_detection(const cv::Mat &mat);
    bool saveImage(const std::string& path, const cv::Mat& image, const std::string& prefix, int count) ;
    bool captureImage(const std::string &path, int type, int count);

    std::atomic_bool is_left_fouc=false;
    std::atomic_bool is_right_fouc=false;
    double m_max_foc=0;
    double m_min_foc=0;
    double m_cur_foc=0;

    int format_ = V4L2_PIX_FMT_MJPEG;
    int height_ = 992;
    int width_ = 992;

    //access resource with pointer
    V4l2Capture *camera_left_ = nullptr;
    V4l2Capture *camera_middle_ = nullptr;
    V4l2Capture *camera_right_ = nullptr;
    V4l2Capture *camera_local_left_ = nullptr;
    V4l2Capture *camera_local_right_ = nullptr;

    cv::Mat left_image_0_;
    cv::Mat middle_image_0_;
    cv::Mat right_image_0_;
    cv::Mat local_left_image_0_;
    cv::Mat local_right_image_0_;

    uchar *buff_left_ = nullptr;
    uchar *buff_middle_ = nullptr;
    uchar *buff_right_ = nullptr;
    uchar *buff_local_left_ = nullptr;
    uchar *buff_local_right_ = nullptr;

    std::vector<uchar> *vec_buff_left_ = nullptr;
    std::vector<uchar> *vec_buff_middle_ = nullptr;
    std::vector<uchar> *vec_buff_right_ = nullptr;
    std::vector<uchar> *vec_buff_local_left_ = nullptr;
    std::vector<uchar> *vec_buff_local_right_ = nullptr;

    timeval tv_left_;
    timeval tv_middle_;
    timeval tv_right_;
    timeval tv_local_left_;
    timeval tv_local_right_;

    RtPeriodicMemberFun2<cameraManager> *run_servo_task_left_ = nullptr;
    RtPeriodicMemberFun2<cameraManager> *run_servo_task_right_ = nullptr;
    RtPeriodicMemberFun2<cameraManager> *run_servo_task_local_left_ = nullptr;
    RtPeriodicMemberFun2<cameraManager> *run_servo_task_local_right_ = nullptr;
    RtPeriodicMemberFun2<cameraManager> *run_servo_task_middle_ = nullptr;

    int left_id_, right_id_,middle_id_,local_left_id_,local_right_id_;
    //控制相关
    std::mutex mtx_restart_left_;
    std::mutex mtx_restart_middle_;
    std::mutex mtx_restart_right_;
    std::mutex mtx_restart_local_left_;
    std::mutex mtx_restart_local_right_;

    std::mutex mtx_camera_left_;
    std::mutex mtx_camera_middle_;
    std::mutex mtx_camera_right_;
    std::mutex mtx_camera_local_left_;
    std::mutex mtx_camera_local_right_;

    std::atomic_bool capture_left_flag_ = false;
    std::atomic_bool capture_middle_flag_ = false;
    std::atomic_bool capture_right_flag_ = false;
    std::atomic_bool capture_local_left_flag_ = false;
    std::atomic_bool capture_local_right_flag_ = false;

    double capture_period_ = 0.003;

    int failed_count_left_ = 0;
    int failed_count_right_ = 0;
    int failed_count_middle_ = 0;
    int failed_count_local_left_ = 0;
    int failed_count_local_right_ = 0;

    QProcess *matlab_process;

signals:
    void signalSendLeftImage(QImage image);
    void signalSendRightImage(QImage image);
    void signalSendMiddleImage(QImage image);
    void signalSendLocalLeftImage(QImage image);
    void signalSendLocalRightImage(QImage image);
    void signalDeviceErro();
    void max_focChanged();
    void min_focChanged();
    void cur_focChanged();
};

#endif // CAMERAMANAGER_H
