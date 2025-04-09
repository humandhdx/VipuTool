#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <filedialogwrap.h>
#include "cameramanager.h"
#include "imageprovider.h"
#include "sshmanager.h"
#include <QQmlContext>
#include <QApplication>
#include <QQuickWindow>
#include <logger.h>
#include "imageprovider.h"
#include "handeyecalculate.h"
#include "ut_robot_wrapper/utrarobotqwrapper.hpp"
#include <QtMessageHandler>
#include "filedialogwrap.h"
#include "ut_robot_wrapper/kinematic_calibration/kinematiccalibqwrapper.hpp"
#include "ut_robot_wrapper/camera_calibration/cameracalibqwrapper.h"
#include "unixsignalqhandler.h"
#include "imagepaintitem.h"

// #define TEST_MAIN

#ifdef TEST_MAIN
#include <QCoreApplication>
#include <ut_robot_wrapper/camera_calibration/cameracalibqwrapper.h>
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    CameraCalibQWrapper CameraCalibWrapper;
    return app.exec();
}
#else

int main(int argc, char *argv[])
{
    // QQuickWindow::setSceneGraphBackend("software");
    //QGuiApplication  app(argc, argv);
    QApplication app(argc, argv);
    UnixSignal::Register_Hookers();

    logger* logInstance = logger::instance();
    qInstallMessageHandler(logger::myMessageHandler);
    qmlRegisterType<FileDialogWrap>("VTool", 1, 0, "VFileDialog");
    qmlRegisterType<ImagePaintItem>("PaintImageItemModule", 1, 0, "ImagePaintItem");
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/Main.qml"));
    //camera
    cameraManager *m_cameraManager =new cameraManager();
    //handeye
    handeyecalculate *m_heculate=new handeyecalculate();
    //robot
    UtraRobot_QWrapper *m_urtrobot_left=new UtraRobot_QWrapper(UtRobotConfig::TestConfig_RobotLeft);
    UtraRobot_QWrapper *m_urtrobot_right=new UtraRobot_QWrapper(UtRobotConfig::TestConfig_RobotRight);
    KinematicCalibQWrapper *m_kinematiccalibqwrapper=new KinematicCalibQWrapper();
    CameraCalibQWrapper *m_cameracalibqwrapper=new CameraCalibQWrapper();

    UnixSignal::Register_SystemInterrupt_Callback([&](int system_signal){
        char msg[128];
        sprintf(msg, "unix signal[%d] coming, now try to stop robot movements\r\n", system_signal);
        write(STDOUT_FILENO, msg, sizeof(msg) - 1);
        std::thread thread_robot_hold_left{&UtraRobot::RobotCommand_Hold, m_urtrobot_left};
        std::thread thread_robot_hold_right{&UtraRobot::RobotCommand_Hold, m_urtrobot_right};
        thread_robot_hold_left.join();
        thread_robot_hold_right.join();
        const char msg_end[] = "Robot Stoped, about to quit Qapp!\r\n";
        write(STDOUT_FILENO, msg_end, sizeof(msg_end) - 1);
        qApp->exit(0);
    });
    //video
    ImageProvider *image_provider_gl = new ImageProvider();
    ImageProvider *image_provider_gr = new ImageProvider();
    ImageProvider *image_provider_ll = new ImageProvider();
    ImageProvider *image_provider_lr = new ImageProvider();
    ImageProvider *image_provider_ml = new ImageProvider();
    // ImagePaintItem *image_painter_gl = new ImagePaintItem();
    // ImagePaintItem *image_painter_gr = new ImagePaintItem();
    // ImagePaintItem *image_painter_ll = new ImagePaintItem();
    // ImagePaintItem *image_painter_lr = new ImagePaintItem();
    // ImagePaintItem *image_painter_ml = new ImagePaintItem();


    sshManager *my_ssh_manager=new sshManager();
    QObject::connect(
        m_cameraManager, &cameraManager::signalSendLeftImage, image_provider_gl, &ImageProvider::recvEmitImg);
    QObject::connect(
        m_cameraManager, &cameraManager::signalSendRightImage, image_provider_gr, &ImageProvider::recvEmitImg);
    QObject::connect(
        m_cameraManager, &cameraManager::signalSendMiddleImage, image_provider_ml, &ImageProvider::recvEmitImg);
    QObject::connect(
        m_cameraManager, &cameraManager::signalSendLocalLeftImage, image_provider_ll, &ImageProvider::recvEmitImg);
    QObject::connect(
        m_cameraManager, &cameraManager::signalSendLocalRightImage, image_provider_lr, &ImageProvider::recvEmitImg);
    //regester qml
    engine.rootContext()->setContextProperty("cameraManager", m_cameraManager);
    engine.rootContext()->setContextProperty("urtrobot_left", m_urtrobot_left);
    engine.rootContext()->setContextProperty("urtrobot_right", m_urtrobot_right);

    engine.rootContext()->setContextProperty("image_provider_gl", image_provider_gl);
    engine.rootContext()->setContextProperty("image_provider_gr", image_provider_gr);
    engine.rootContext()->setContextProperty("image_provider_ll", image_provider_ll);
    engine.rootContext()->setContextProperty("image_provider_lr", image_provider_lr);
    engine.rootContext()->setContextProperty("image_provider_ml", image_provider_ml);

    engine.rootContext()->setContextProperty("handeyeCulate", m_heculate);
    engine.rootContext()->setContextProperty("sshManager", my_ssh_manager);
    engine.rootContext()->setContextProperty("logger", logInstance);
    engine.rootContext()->setContextProperty("kinematiccalibqwrapper",m_kinematiccalibqwrapper);
    engine.rootContext()->setContextProperty("cameracalibqwrapper",m_cameracalibqwrapper);

    engine.addImageProvider(QLatin1String("GlImg"), image_provider_gl);
    engine.addImageProvider(QLatin1String("GrImg"), image_provider_gr);
    engine.addImageProvider(QLatin1String("LlImg"), image_provider_ll);
    engine.addImageProvider(QLatin1String("LrImg"), image_provider_lr);
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
    // QString host="192.168.1.82";
    // QString user="john";
    // QString password="foxpg1348";
    //int port=22;
    //my_ssh_manager->sshConnect(host,user,password,port);
    //my_ssh_manager->sshCommandExecut(QString("ros2 run demo_nodes_cpp talker"));
    //m_heculate->runCalibration();
    return app.exec();
}

#endif
