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
#include "Cameras/cameracontrol.h"
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
    //QQuickWindow::setSceneGraphBackend("opengl");
    //QGuiApplication  app(argc, argv);
    QApplication app(argc, argv);
    UnixSignal::Register_Hookers();

    logger* logInstance = logger::instance();
    //qInstallMessageHandler(logger::myMessageHandler);
    qmlRegisterType<FileDialogWrap>("VTool", 1, 0, "VFileDialog");
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
    QObject::connect(&app, &QCoreApplication::aboutToQuit, [m_urtrobot_left, m_urtrobot_right](){
        const char msg[] = "qt about to quit, now try to stop robot movements\n";
        write(STDOUT_FILENO, msg, sizeof(msg) - 1);
        std::thread thread_robot_hold_left{&UtraRobot::RobotCommand_Hold, m_urtrobot_left};
        std::thread thread_robot_hold_right{&UtraRobot::RobotCommand_Hold, m_urtrobot_right};
        thread_robot_hold_left.join();
        thread_robot_hold_right.join();
        const char msg_end[] = "Robot Stoped\n";
        write(STDOUT_FILENO, msg_end, sizeof(msg_end) - 1);
    });
    //video
    ImageProvider *image_provider_gl = new ImageProvider();
    ImageProvider *image_provider_gr = new ImageProvider();
    ImageProvider *image_provider_ml = new ImageProvider();
    sshManager *my_ssh_manager=new sshManager();
    CameraControl *m_cameraControl=new CameraControl();
    // QObject::connect(
    //     m_cameraManager, &cameraManager::signalSendLeftImage, image_provider_gl, &ImageProvider::recvEmitImg);
    // QObject::connect(
    //     m_cameraManager, &cameraManager::signalSendRightImage, image_provider_gr, &ImageProvider::recvEmitImg);
    // QObject::connect(
    //     m_cameraManager, &cameraManager::signalSendMiddleImage, image_provider_ml, &ImageProvider::recvEmitImg);
    QObject::connect(
        m_cameraControl, &CameraControl::sendGlobalLeftIamge, image_provider_gl, &ImageProvider::recvEmitImg);
    QObject::connect(
        m_cameraControl, &CameraControl::sendGlobalRightIamge, image_provider_gr, &ImageProvider::recvEmitImg);
    QObject::connect(
        m_cameraControl, &CameraControl::sendCenterIamge, image_provider_ml, &ImageProvider::recvEmitImg);
    //regester qml
    //engine.rootContext()->setContextProperty("cameraManager", m_cameraManager);
    engine.rootContext()->setContextProperty("cameraManager", m_cameraControl);
    engine.rootContext()->setContextProperty("urtrobot_left", m_urtrobot_left);
    engine.rootContext()->setContextProperty("urtrobot_right", m_urtrobot_right);
    engine.rootContext()->setContextProperty("image_provider_gl", image_provider_gl);
    engine.rootContext()->setContextProperty("image_provider_gr", image_provider_gr);
    engine.rootContext()->setContextProperty("image_provider_ml", image_provider_ml);
    engine.rootContext()->setContextProperty("handeyeCulate", m_heculate);
    engine.rootContext()->setContextProperty("sshManager", my_ssh_manager);
    engine.rootContext()->setContextProperty("logger", logInstance);
    engine.rootContext()->setContextProperty("kinematiccalibqwrapper",m_kinematiccalibqwrapper);
    engine.rootContext()->setContextProperty("cameracalibqwrapper",m_cameracalibqwrapper);
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
