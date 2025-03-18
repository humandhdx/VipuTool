#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <QObject>
#include <vector>
#include <memory>
#include "basecamera.h" // 假设所有子类都继承于 BaseCamera
#include "globalcamera.h"
#include "localcamera.h"
#include "centercamera.h"

class CameraControl : public QObject
{
    Q_OBJECT
public:
    explicit CameraControl(QObject* parent = nullptr);
    ~CameraControl();


signals:
    //void cameraImageCaptured(int cameraIndex, QImage image); // 当任一相机捕获到图像时发射信号
    void sendGlobalLeftIamge(QImage image);
    void sendGlobalRightIamge(QImage image);
    void sendLocalLeftIamge(QImage image);
    void sendLocalRightIamge(QImage image);
    void sendCenterIamge(QImage image);
    void cameraStopped(int cameraIndex);                    // 当任一相机停止时发射信号

public slots:
    void handleImageCaptured(QImage image);
    void handleCameraStopped();
    // 统一管理接口
    int  findCameraIndex(QString type) const;
    bool startCamera(QString type);         // 启动指定相机
    bool stopCamera(QString type);          // 停止指定相机
    void stopAllCameras();                     // 停止所有相机
    int cameraCount() const;                   // 获取相机数量
    // 状态监控
    QString getCameraStatus(QString type) const; // 获取指定相机的运行状态（例如 capturing, stopped 等）

private:
    // 动态管理具体子类相机的创建和销毁
    void findCamera();
    void addCamera(const QString& cameraType, int deviceId, int width, int height);
    struct ManagedCamera {
        std::unique_ptr<BaseCamera> camera; // 相机对象（基类指针，支持多态）
        int deviceId;                       // 相机设备 ID
        QString type;                       // 相机类型（如 "GlobalCamera"、"LocalLeftCamera"）
        QString status;                     // 相机运行状态描述
    };

    std::vector<ManagedCamera> cameras_; // 所有被管理的相机对象
    mutable std::mutex mtx_;             // 保证线程安全
};

#endif // CAMERACONTROL_H
