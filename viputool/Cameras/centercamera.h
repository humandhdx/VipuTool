#include "basecamera.h"

class CenterCamera : public BaseCamera
{
public:
    explicit CenterCamera(QObject* parent = nullptr);
    ~CenterCamera() override {}

protected:
    bool createCamera() override;
};
