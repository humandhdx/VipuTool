#include "globalcamera.h"

#include <QEventLoop>
#include <QImage>
#include <future>

GlobalCamera::GlobalCamera(QObject *parent): BaseCamera(parent) {}

bool GlobalCamera::createCamera()
{
    allocateBuffers(); // 实现基类申请缓冲区
    std::lock_guard<std::mutex> lck{mtx_};
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
        oss_left.str().c_str(), target_format_, width_, height_, fps, ioTypeIn, verbose);
    camera_ = V4l2Capture::create(param);
    return true;
}


