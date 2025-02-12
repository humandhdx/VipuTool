#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "cameramanager.h"
#include "imageprovider.h"
#include "sshmanager.h"
#include <QQmlContext>
#include <QApplication>
#include <QQuickWindow>
#include "imageprovider.h"
int main(int argc, char *argv[])
{
    //QQuickWindow::setSceneGraphBackend("opengl");
    QApplication  app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/Main.qml"));
    //camera
    cameraManager *m_cameraManager =new cameraManager();
    //video
    ImageProvider *image_provider_gl = new ImageProvider();
    ImageProvider *image_provider_gr = new ImageProvider();
    ImageProvider *image_provider_ml = new ImageProvider();
    QObject::connect(
        m_cameraManager, &cameraManager::signalSendLeftImage, image_provider_gl, &ImageProvider::recvEmitImg);
    QObject::connect(
        m_cameraManager, &cameraManager::signalSendRightImage, image_provider_gr, &ImageProvider::recvEmitImg);
    QObject::connect(
        m_cameraManager, &cameraManager::signalSendMiddleImage, image_provider_ml, &ImageProvider::recvEmitImg);
    //regester qml
    engine.rootContext()->setContextProperty("cameraManager", m_cameraManager);
    engine.rootContext()->setContextProperty("image_provider_gl", image_provider_gl);
    engine.rootContext()->setContextProperty("image_provider_gr", image_provider_gr);
    engine.rootContext()->setContextProperty("image_provider_ml", image_provider_ml);
    engine.addImageProvider(QLatin1String("GlImg"), image_provider_gl);
    engine.addImageProvider(QLatin1String("GrImg"), image_provider_gr);
    engine.addImageProvider(QLatin1String("MlImg"), image_provider_ml);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);
    //创建ssh通道
    // sshManager *my_ssh_manager=new sshManager();
    // QString host="192.168.1.82";
    // QString user="john";
    // QString password="foxpg1348";
    // int port=22;
    // my_ssh_manager->sshConnect(host,user,password,port);
    return app.exec();
}
