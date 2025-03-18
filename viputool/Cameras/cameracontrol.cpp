#include "cameracontrol.h"
#include <sstream>
#include <iostream>
#include <dirent.h>
#include <QThread>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
CameraControl::CameraControl(QObject *parent)
    : QObject{parent}
{}

CameraControl::~CameraControl()
{
   stopAllCameras();
    cameras_.clear();
}

void CameraControl::addCamera(const QString &cameraType, int deviceId, int width, int height)
{
    for (size_t i = 0; i < cameras_.size(); ++i) {
        if (cameras_[i].type==cameraType){
            return;
        }
    }
    std::unique_ptr<BaseCamera>camera;
    if(cameraType=="GlobalLeftCamera"){
        camera = std::make_unique<GlobalCamera>();
        connect(camera.get(), &BaseCamera::sendImage, this, &CameraControl::sendGlobalLeftIamge);
    }
    else if(cameraType=="GlobalRightCamera"){
        camera=std::make_unique<GlobalCamera>();
        connect(camera.get(), &BaseCamera::sendImage, this, &CameraControl::sendGlobalRightIamge);
    }
    else if(cameraType=="LocalRightCamera"){
        camera=std::make_unique<LocalCamera>();
        connect(camera.get(), &BaseCamera::sendImage, this, &CameraControl::sendLocalLeftIamge);
    }
    else if(cameraType=="LocalRightCamera"){
        camera=std::make_unique<LocalCamera>();
        connect(camera.get(), &BaseCamera::sendImage, this, &CameraControl::sendLocalRightIamge);
    }
    else if(cameraType=="CenterCamera"){
        camera=std::make_unique<CenterCamera>();
        connect(camera.get(), &BaseCamera::sendImage, this, &CameraControl::sendCenterIamge);
    }
    else {
        qWarning() << "Unsupported camera type: " << cameraType;
        return;
    }
    camera->openCamera(deviceId,width,height);
    connect(camera.get(), &BaseCamera::cameraStopped, this, &CameraControl::handleCameraStopped);
    // 保存相机到管理列表
    ManagedCamera managedCamera{std::move(camera), deviceId ,cameraType, "stopped"};
    cameras_.emplace_back(std::move(managedCamera));
    qDebug() << "Camera of type " << cameraType << " added with device ID " << deviceId;
}

bool CameraControl::startCamera(QString type)
{
    auto cameraIndex= findCameraIndex(type);
    if(cameraIndex<0){
        findCamera();
        QThread::sleep(0.1);
        cameraIndex= findCameraIndex(type);{
            if(cameraIndex<0){
                return false;
            }
        }
    }
    std::lock_guard<std::mutex> lock(mtx_);
    if(cameraIndex < 0 || cameraIndex >= cameras_.size()) return false;
    auto& camera = cameras_[cameraIndex];
    if (camera.status == "capturing") {
        qDebug() << "Camera at index " << cameraIndex << " is already capturing.";
        return false;
    }

    camera.camera->startCapture(); // 启动相机捕获
    camera.status = "capturing";
    qDebug() << "Camera at index " << cameraIndex << " started capturing.";
    return true;
}

bool CameraControl::stopCamera(QString type)
{
    auto cameraIndex= findCameraIndex(type);
    std::lock_guard<std::mutex> lock(mtx_);
    if (cameraIndex < 0 || cameraIndex >= cameras_.size()) return false;

    auto& camera = cameras_[cameraIndex];
    if (camera.status != "capturing") {
        qDebug() << "Camera at index " << cameraIndex << " is already capturing.";
        return false;
    }

    camera.camera->stopCamera(); // 停止相机捕获
    camera.status = "stopped";
    cameras_.erase(cameras_.begin() + cameraIndex); // 删除相机
    qDebug() << "Camera at index " << cameraIndex << " has been removed.";

    return true;
}

void CameraControl::stopAllCameras()
{
    std::lock_guard<std::mutex> lock(mtx_);
    for (auto& camera : cameras_) {
        camera.camera->stopCamera();
        camera.status = "stopped";
    }
    qDebug() << "All cameras have been stopped.";
}

int CameraControl::cameraCount() const
{
    std::lock_guard<std::mutex> lock(mtx_);
    return cameras_.size();
}

QString CameraControl::getCameraStatus(QString type) const
{
    auto cameraIndex= findCameraIndex(type);
    std::lock_guard<std::mutex> lock(mtx_);
    if (cameraIndex < 0 || cameraIndex >= cameras_.size()) return "invalid_index";
    return cameras_[cameraIndex].status;
}

void CameraControl::findCamera()
{
    int id=-1;
    //读取当前系统所有设备
    std::vector<std::string> devs{};
    {  //read carmera devs
        DIR *dfd;
        const std::string pathname = "/dev";
        dfd = opendir(pathname.c_str());
        dirent *dp = nullptr;
        while ((dp = readdir(dfd)) != nullptr) {
            if (strncmp(dp->d_name, ".", 1) == 0)
                continue; /* 跳过当前文件夹和上一层文件夹以及隐藏文件*/
            std::string name{dp->d_name};
            if (strncmp(dp->d_name, "video", 5) == 0 && strlen(dp->d_name) <= 7) {
                std::string dev = pathname + "/" + dp->d_name;
                devs.push_back(dev);
            }
        }
        closedir(dfd);
    }
    //获取相机的id
    v4l2_capability cap;
    for (const auto &name : devs) {
        int vfd = open(name.c_str(), O_RDWR);
        if (vfd < 0) {
            //need not close
            continue;
        }

        if (ioctl(vfd, VIDIOC_QUERYCAP, &cap) == -1) {
            close(vfd);
            continue;
        }
        if (!(V4L2_CAP_VIDEO_CAPTURE & cap.device_caps)) {
            close(vfd);
            continue;
        }
        if (!(V4L2_CAP_VIDEO_CAPTURE & cap.device_caps)) {
            close(vfd);
            continue;
        }

        int dev_num_cur =atoi(name.substr(10).c_str());  //10 = len for "/dev/video", to get the num behind

        if (strncmp((const char *)cap.card, "FueCamLeft", 10) == 0) {
            id = dev_num_cur;
            addCamera("GlobalLeftCamera",id,4000,3000);
        }
        else if (strncmp((const char *)cap.card, "FueCamRight", 11) == 0) {
            id = dev_num_cur;
            addCamera("GlobalRightCamera",id,4000,3000);
        }
        else if (strncmp((const char *)cap.card, "SPCA2100", 8) == 0) {
            id=dev_num_cur;
            addCamera("CenterCamera",id,1920,1080);
        }
        close(vfd);
    }

}

void CameraControl::handleImageCaptured(QImage image)
{
    // 转发来自相机的图像信号
    for (size_t i = 0; i < cameras_.size(); ++i) {
        if (cameras_[i].camera->signalsBlocked()) continue;
        //emit cameraImageCaptured(static_cast<int>(i), image);
    }
}

void CameraControl::handleCameraStopped()
{
    // 对停止操作进行处理，转发信号
    for (size_t i = 0; i < cameras_.size(); ++i) {
        if (cameras_[i].camera->signalsBlocked()) continue;
        emit cameraStopped(static_cast<int>(i));
        stopCamera(cameras_[i].type);
    }
}

int CameraControl::findCameraIndex(QString type) const
{
    for (size_t i = 0; i < cameras_.size(); ++i) {
        if (cameras_[i].type==type){
            return i;
        }
    }
    return -1;
}
