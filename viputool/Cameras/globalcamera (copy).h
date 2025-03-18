#ifndef GLOBALCAMERA_H
#define GLOBALCAMERA_H

#include "basecamera.h"

#include <QObject>
#include <mutex>

#include <v4l2/Periodlc_rt2.hpp>
#include <v4l2/v4l2capture.h>

class GlobalCamera : public QObject,BaseCamera
{
    Q_OBJECT
public:
    explicit GlobalCamera(QObject *parent = nullptr);
    ~GlobalCamera();
    bool openCamera(const int id)override;
    void stopCamera()override
    {

    }
    void startCapture()override;
    void saveImage(const QString &path,const int count)override;
private:
    void creat_camera()override;
    void capture_image()override;
    timeval timeval_;                                // 摄像头操作超时时间
    uchar* buff_ = nullptr;                          // 显式的字节缓冲区
    V4l2Capture* camera_ = nullptr;                 // V4L2 驱动封装
    RtPeriodicMemberFun2<GlobalCamera>* run_servo_task_ = nullptr; // 定期任务
    double capture_period_ = 0.003;   //定期任务执行间隔
    std::mutex mtx_creat_;

    std::mutex mtx_camera_;

    std::atomic_bool captureing_flag_=false;
    int format_ = V4L2_PIX_FMT_MJPEG;
signals:
};

#endif // GLOBALCAMERA_H
