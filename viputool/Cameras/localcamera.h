#ifndef LOCALCAMERA_H
#define LOCALCAMERA_H

#include <QObject>
#include <v4l2/Periodlc_rt2.hpp>
#include <v4l2/v4l2capture.h>
#include "Cameras/basecamera.h"
#include "opencv2/opencv.hpp"


class LocalCamera : public BaseCamera
{
public:
    explicit LocalCamera(QObject *parent = nullptr);
    ~LocalCamera() override {}
protected:
    bool createCamera() override;
};

#endif // LOCALCAMERA_H
