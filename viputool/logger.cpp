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
    // 静态变量标记是否在处理中（线程安全版本）
    static QAtomicInt handling(0);

    // 如果已经在处理中，直接调用默认处理器后返回
    if (handling.fetchAndAddRelaxed(1) > 0) {
        QtMessageHandler defaultHandler = qInstallMessageHandler(nullptr);
        if (defaultHandler) defaultHandler(type, context, msg);
        qInstallMessageHandler(myMessageHandler);
        handling.fetchAndSubRelaxed(1);
        return;
    }

    // 主处理逻辑（保证只执行一次）
    int levelnum = [](QtMsgType t){
        switch(t) {
        case QtDebugMsg:    return QtDebugMsg;
        case QtWarningMsg:  return QtWarningMsg;
        case QtCriticalMsg: return QtCriticalMsg;
        case QtFatalMsg:    return QtFatalMsg;
        default:            return QtInfoMsg;
        }
    }(type);

    // 发送信号（此时确保不会递归）
    emit instance()->sendLogMesseg(msg, levelnum);

    // 调用默认处理器输出到控制台
    QtMessageHandler defaultHandler = qInstallMessageHandler(nullptr);
    if (defaultHandler) defaultHandler(type, context, msg);
    qInstallMessageHandler(myMessageHandler);

    // 重置处理标志
    handling.fetchAndSubRelaxed(1);


}
