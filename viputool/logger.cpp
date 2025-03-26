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
    // 1. 先调用默认处理器，确保输出到终端/调试窗口
    QtMessageHandler defaultHandler = qInstallMessageHandler(nullptr); // 临时恢复默认处理器
    if (defaultHandler) {
        defaultHandler(type, context, msg); // 输出到原有位置
    }
    qInstallMessageHandler(myMessageHandler); // 重新注册我们的处理器

    // 2. 您的自定义逻辑（保持不变）
    QString level;
    int levelnum = 0;
    switch (type) {
    case QtDebugMsg: level = "DEBUG"; levelnum = QtDebugMsg; break;
    case QtWarningMsg: level = "WARNING"; levelnum = QtWarningMsg; break;
    case QtCriticalMsg: level = "CRITICAL"; levelnum = QtCriticalMsg; break;
    case QtFatalMsg: level = "FATAL"; levelnum = QtFatalMsg; break;
    default: level = "INFO"; break;
    }
    emit instance()->sendLogMesseg(msg, levelnum); // 发送信号

    // 追加到日志文件
    // QFile file("app_log.txt");
    // if (file.open(QIODevice::Append | QIODevice::Text)) {
    //     QTextStream out(&file);
    //     out << logMessage << "\n";
    //     file.close();
    // }

}
