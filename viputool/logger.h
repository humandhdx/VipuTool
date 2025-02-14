#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QTextStream>
#include <QDateTime>

class logger : public QObject
{
    Q_OBJECT
public:
    explicit logger(QObject *parent = nullptr);
    static logger *instance();
    static void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

signals:
    void sendLogMesseg(QString msg ,int level);
};

#endif // LOGGER_H
