#ifndef V4L2CAPTURE_H
#define V4L2CAPTURE_H

#include "v4l2access.h"
// ---------------------------------
// V4L2 Capture
// ---------------------------------
class V4l2Capture : public V4l2Access
{
protected:
    explicit V4l2Capture(V4l2Device* device);

public:
    static V4l2Capture* create(const V4L2DeviceParameters & param);
    virtual ~V4l2Capture();

    size_t read(char* buffer, size_t bufferSize);
    bool   isReadable(timeval* tv);
};

#endif // V4L2CAPTURE_H
