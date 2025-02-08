#ifndef SSHMANAGER_H
#define SSHMANAGER_H

#include <QObject>
#include <libssh/libsshpp.hpp>
#include <libssh/libssh.h>
#include <iostream>
#include <string.h>
#include <sstream>
#include <errno.h>
#include <QDebug>
class sshManager : public QObject
{
    Q_OBJECT
public:
    explicit sshManager(QObject *parent = nullptr);
    ~sshManager();
    void sshConnect(const QString &host,const QString &user,const QString &password ,const int port);
    void sshDisConnect();
    void sshCommandExecut(const QString &command);
    void sshSudoCommandExecut(const QString &command);
private:
    ssh_session my_ssh_session;
    ssh_channel my_ssh_channel;
    int verify_knownhost(ssh_session session);
    void sshConnectImpl(const QString &host,const QString &user,const QString &password ,const int port);
    void cleanup();

signals:
    void sshError(const QString &message);
    void sshConnected();
    void sshDisConnected();
    void commandExecuted(const QString output);
    void sshCommandSuccess();

};

#endif // SSHMANAGER_H
