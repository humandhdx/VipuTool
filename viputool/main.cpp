#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "cameramanager.h"
#include "sshmanager.h"
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/Main.qml"));
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
    cameraManager *m_cameraManager =new cameraManager();
    //创建ssh通道
    // sshManager *my_ssh_manager=new sshManager();
    // QString host="192.168.1.82";
    // QString user="john";
    // QString password="foxpg1348";
    // int port=22;
    // my_ssh_manager->sshConnect(host,user,password,port);


    return app.exec();
}
