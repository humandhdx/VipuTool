#ifndef V4L2READWRITEDEVICE_H
#define V4L2READWRITEDEVICE_H


#include "v4l2device.h"


class V4l2ReadWriteDevice : public V4l2Device
{
protected:
    virtual size_t writeInternal(char* buffer, size_t bufferSize);
    virtual size_t readInternal(char* buffer, size_t bufferSize);

public:
    V4l2ReadWriteDevice(const V4L2DeviceParameters&  params, v4l2_buf_type deviceType);
};

#endif //V4l2ReadWriteDevice_H
