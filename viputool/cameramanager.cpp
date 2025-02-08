#include "cameramanager.h"

cameraManager::cameraManager(QObject *parent): QObject{parent},m_left_camera(nullptr),m_right_camera(nullptr)
    ,m_left_imagecapture(nullptr),m_right_imagecapture(nullptr)
{

}

cameraManager::~cameraManager()
{
    stopCamera();
}

void cameraManager::stopCamera()
{
    if(m_left_imagecapture){
        delete m_left_imagecapture;
        m_left_imagecapture=nullptr;
    }
    if(m_right_imagecapture){
        delete m_right_imagecapture;
        m_right_imagecapture=nullptr;
    }
    if (m_left_camera) {
        m_left_camera->stop();
        m_left_camera = nullptr;
    }
    if (m_right_camera) {
        m_right_camera->stop();
        m_right_camera = nullptr;
    }

}

bool cameraManager::captureImage(const QString &savePath)
{

}


void cameraManager::setLeft_camera( QCamera *newLeft_camera)
{
    if (m_left_camera) {
        delete m_left_camera;
        m_left_camera = nullptr;
    }
    m_left_camera= new QCamera(left_camera_device,this);
    m_left_imagecapture=new QImageCapture(this);
    m_left_captureSession.setCamera(m_left_camera);
    m_left_captureSession.setImageCapture(m_left_imagecapture);
    emit left_cameraChanged();
}

void cameraManager::setRight_camera( QCamera *newRight_camera)
{
    if (m_right_camera) {
        delete m_right_camera;
        m_right_camera = nullptr;
    }
    m_right_camera= new QCamera(right_camera_device,this);
    m_right_imagecapture=new QImageCapture(this);
    m_right_captureSession.setCamera(m_right_camera);
    m_right_captureSession.setImageCapture(m_right_imagecapture);
    emit right_cameraChanged();
}

bool cameraManager::startCamera(const int l_r)
{
    if(l_r>2||l_r<0){
        return false;
    }
    QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    if(cameras.isEmpty()){
        return false;
    }
    stopCamera();
    for(auto camera :cameras){
        if(camera.description()=="FueCamLeft"){
            left_camera_device=camera;
        }
        if(camera.description()=="FueCamRight"){
            right_camera_device=camera;
        }
    }
    QCamera *camera=new QCamera();
    switch (l_r) {
    case 0:
        if(left_camera_device.isNull()){
            return false;
        }
        setLeft_camera(camera);
        m_left_camera->start();
        break;
    case 1:
        if(right_camera_device.isNull()){
            return false;
        }
        setRight_camera(camera);
        m_right_camera->start();
        break;
    case 2:
        if(left_camera_device.isNull()||right_camera_device.isNull()){
            return false;
        }
        setLeft_camera(camera);
        setRight_camera(camera);
        m_left_camera->start();
        m_right_camera->start();
        break;
    default:
        break;
    }

}
