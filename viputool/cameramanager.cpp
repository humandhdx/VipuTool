#include "cameramanager.h"

#include <QtWidgets/QFileDialog>

cameraManager::cameraManager(QObject *parent): QObject{parent},m_left_camera(nullptr),m_right_camera(nullptr)
    ,m_left_imagecapture(nullptr),m_right_imagecapture(nullptr),savePath(QString())
{

}

cameraManager::~cameraManager()
{
    stopCamera();
}

void cameraManager::stopCamera()
{
    if(m_left_imagecapture!=nullptr){
        delete m_left_imagecapture;
        m_left_imagecapture=nullptr;
    }
    if(m_right_imagecapture!=nullptr){
        delete m_right_imagecapture;
        m_right_imagecapture=nullptr;
    }
    if (m_left_camera!=nullptr) {
        m_left_camera->stop();
        m_left_camera = nullptr;
    }
    if (m_right_camera!=nullptr) {
        m_right_camera->stop();
        m_right_camera = nullptr;
    }

}

bool cameraManager::captureImage()
{
    // if(!m_left_camera){
    //     return false;
    // }
    // 如果 savePath 为空，则打开系统文件选择器
    if (savePath.isEmpty()) {
        savePath = QFileDialog::getExistingDirectory(nullptr,
                                                      tr("选择保存图片的文件夹"),
                                                      QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

        if (savePath.isEmpty()) {
            qWarning() << "用户取消了保存路径选择";
            return false;
        }
    }
    QString savePath2 = savePath+"/test.jpg";
    qDebug()<<savePath2;
    m_right_imagecapture->captureToFile(savePath2);
}

void cameraManager::setVideoOutput(QVideoSink *sink,const int l_r)
{
    if (l_r == 0) {
        m_left_captureSession.setVideoOutput(sink);
    } else {
        m_right_captureSession.setVideoOutput(sink);
    }
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
    // 设置分辨率
    QSize desiredResolution(4000, 3000);
    QCameraFormat selectedFormat;
    const QList<QCameraFormat> availableFormats = right_camera_device.videoFormats();
    for (const QCameraFormat &format : availableFormats) {
        // 检查该格式的分辨率是否符合要求
        //qDebug()<<"resolution:"<<format.resolution();
        if (format.resolution() == desiredResolution&&format.pixelFormat()==QVideoFrameFormat::Format_Jpeg)//format.pixelFormat()==QVideoFrameFormat::Format_YUYV
        {
            selectedFormat = format;
            qDebug()<<"resolution:"<<format.resolution();
            qDebug()<<"resolution:"<<format.pixelFormat();
           // break;
        }
    }
    // 设置摄像头格式（注意必须在启动摄像头前调用）
    m_right_camera->setCameraFormat(selectedFormat);
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
        connect(m_right_camera, &QCamera::errorChanged, this, [this]() {
            qWarning() << "右侧摄像头错误:" << m_right_camera->errorString();
            // 根据错误状态进行相应的处理，比如重启摄像头、停止采集等
        });
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
    return true;

}
