#ifndef GLOBALCAMERA_H
#define GLOBALCAMERA_H

#include "basecamera.h"

#include <QObject>
#include <mutex>
#include "opencv2/opencv.hpp"
#include <v4l2/Periodlc_rt2.hpp>
#include <v4l2/v4l2capture.h>

class GlobalCamera : public BaseCamera
{
public:
    explicit GlobalCamera(QObject *parent = nullptr);
    ~GlobalCamera() override {}

protected:
    bool createCamera() override;
};

#endif // GLOBALCAMERA_H
