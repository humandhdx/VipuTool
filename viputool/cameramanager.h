#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H
#include <QObject>
#include <QtMultimedia>
#include <QImageCapture>
class cameraManager: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QCamera *left_camera READ left_camera WRITE setLeft_camera NOTIFY left_cameraChanged FINAL)
    Q_PROPERTY(QCamera *right_camera READ right_camera WRITE setRight_camera NOTIFY right_cameraChanged FINAL)
public:
    cameraManager(QObject *parent = nullptr);
    ~cameraManager();
    QCamera *left_camera() const{return m_left_camera;}
    void setLeft_camera( QCamera *newLeft_camera);

    QCamera *right_camera() const{return m_right_camera ;}
    void setRight_camera( QCamera *newRight_camera);
public slots:
    bool startCamera(const int l_r);//0-left 1-right 2-double
    void stopCamera();
    bool captureImage();
    // 将 QML VideoOutput 的 videoSink 传递给 QMediaCaptureSession
    void setVideoOutput(QVideoSink *sink,const int l_r);
private:
    QImageCapture *m_left_imagecapture;
    QImageCapture *m_right_imagecapture;
    QCamera *m_left_camera;
    QCamera *m_right_camera;
    QMediaCaptureSession m_left_captureSession;  // 媒体会话管理
    QMediaCaptureSession m_right_captureSession;  // 媒体会话管理
    QCameraDevice left_camera_device;
    QCameraDevice right_camera_device;
    QString savePath;
signals:
    void left_cameraChanged();
    void right_cameraChanged();
};

#endif // CAMERAMANAGER_H
