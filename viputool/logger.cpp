#include "logger.h"

#include <QFile>

logger::logger(QObject *parent)
    : QObject{parent}
{}

logger *logger::instance()
{
    static logger instance;
    return &instance;
}

void logger::myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 时间戳
    QString timeStamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    // 消息级别
    QString level="";
    int levelnum=0;
    switch (type) {
    case QtDebugMsg:
        level = "DEBUG";
        levelnum=QtDebugMsg;
        break;
    case QtWarningMsg:
        level = "WARNING";
        levelnum=QtWarningMsg;
        break;
    case QtCriticalMsg:
        level = "CRITICAL";
        levelnum=QtCriticalMsg;
        break;
    case QtFatalMsg:
        level = "FATAL";
        levelnum=QtFatalMsg;
        break;
    default:
        level = "INFO";
        break;
    }

    emit  instance()->sendLogMesseg(msg,levelnum);

    // 追加到日志文件
    // QFile file("app_log.txt");
    // if (file.open(QIODevice::Append | QIODevice::Text)) {
    //     QTextStream out(&file);
    //     out << logMessage << "\n";
    //     file.close();
    // }

}
