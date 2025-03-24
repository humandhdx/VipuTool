#include "sshmanager.h"
#include <QtConcurrent>
sshManager::sshManager(QObject *parent)
    : QObject{parent},my_ssh_session(nullptr)
{

}

sshManager::~sshManager()
{
    qWarning() << __FUNCTION__ << " - 0!";
    QThread::sleep(2);
    cleanup();
}

void sshManager::sshConnect(const QString &host, const QString &user, const QString &password, const int port)
{
    if (port < 1) {
        emit sshError(tr("Invalid port number"));
        return;
    }
    if (port > 65535) {
        emit sshError(tr("Invalid port number"));
        return;
    }
    // 连接操作开始前先清理已有连接
    cleanup();
    // 使用 QPointer 防止对象在后台线程中被意外销毁
    QPointer<sshManager> self(this);
    QtConcurrent::run([self, host, user, password, port]() {
        if (self)
            self->sshConnectImpl(host, user, password, port);
    });
}

void sshManager::sshConnectImpl(const QString &host,const QString &user,const QString &password ,const int port)
{
    // 创建 SSH 会话（局部变量，待连接成功后再保存到对象中）
    ssh_session session = ssh_new();
    if (session == nullptr) {
        QMetaObject::invokeMethod(this, [this]() {
                emit sshError(tr("Error creating SSH session."));
            }, Qt::QueuedConnection);
        return;
    }

    // 设置 SSH 连接选项
    QByteArray hostData = host.toUtf8();
    QByteArray userData = user.toUtf8();
    int portNumber = port;
    ssh_options_set(session, SSH_OPTIONS_HOST, hostData.constData());
    ssh_options_set(session, SSH_OPTIONS_USER, userData.constData());
    ssh_options_set(session, SSH_OPTIONS_PORT, &portNumber);

    // 建立 SSH 连接（耗时操作）
    int rc = ssh_connect(session);
    if (rc != SSH_OK) {
        QMetaObject::invokeMethod(this, [this, session]() {
                emit sshError(tr("连接服务器出错: %1").arg(ssh_get_error(session)));
            }, Qt::QueuedConnection);
        ssh_disconnect(session);
        ssh_free(session);
        return;
    }

    // 验证主机密钥
    rc = verify_knownhost(session);
    if (rc < 0) {
        QMetaObject::invokeMethod(this, [this, session]() {
                emit sshError(tr("未识别的服务器地址"));
            }, Qt::QueuedConnection);
        ssh_disconnect(session);
        ssh_free(session);
        return;
    }

    // 用户认证
    QByteArray passwordData = password.toUtf8();
    rc = ssh_userauth_password(session, nullptr, passwordData.constData());
    if (rc != SSH_AUTH_SUCCESS) {
        QMetaObject::invokeMethod(this, [this, session]() {
                emit sshError(tr("用户认证失败: %1").arg(ssh_get_error(session)));
            }, Qt::QueuedConnection);
        ssh_disconnect(session);
        ssh_free(session);
        return;
    }

    // 连接成功，将 session  保存到对象成员中，并发出连接成功信号，
    // 回到主线程中更新状态，保证后续命令发送在主线程执行时能使用正确的连接。
    QMetaObject::invokeMethod(this, [this, session]() {
            my_ssh_session = session;
            qDebug()<<"SSH Connect Sucess";
            emit sshConnected();
            //sshCommandExecut(QString("ros2 run demo_nodes_cpp talker"));
            //sshSudoCommandExecut("pwd");
        }, Qt::QueuedConnection);
}

void sshManager::sshDisConnect()
{
    cleanup();
    emit sshDisConnected();
}

void sshManager::sshCommandExecut(const QString &command)
{
    // 前置检查
    if (!my_ssh_session || !ssh_is_connected(my_ssh_session)) {
        emit sshError(tr("SSH session is not connected"));
        return;
    }
    // 创建 SSH 通道
    ssh_channel channel = ssh_channel_new(my_ssh_session);
    if (channel == nullptr) {
        QMetaObject::invokeMethod(this, [this]() {
                emit sshError(tr("创建通道失败: %1").arg(ssh_get_error(my_ssh_session)));
            }, Qt::QueuedConnection);
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return;
    }

    // 打开 SSH 通道
    int rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK) {
        QMetaObject::invokeMethod(this, [this, channel]() {
                emit sshError(tr("打开通道失败: %1").arg(ssh_get_error(my_ssh_session)));
            }, Qt::QueuedConnection);
        ssh_channel_free(channel);
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return;
    }

    // 准备命令
    QByteArray cmdData = command.toUtf8();
    if (cmdData.isEmpty()) {
        emit sshError(tr("Empty command cannot be sent"));
        return;
    }

    // 执行命令

    //int rc = ssh_channel_request_exec(my_ssh_channel, cmdData.constData());
    //QString cmd = "bash -c \"source /opt/ros/humble/setup.sh && ros2 run demo_nodes_cpp talker\"";
    //int rc = ssh_channel_request_exec(my_ssh_channel, cmd.toUtf8().constData());
    rc = ssh_channel_request_exec(channel, cmdData.constData());
    if (rc != SSH_OK) {
        QString errorDetail = QString("[Channel] %1\n[Session] %2")
                                  .arg(ssh_get_error(channel))
                                  .arg(ssh_get_error(my_ssh_session));
        emit sshError(tr("Command execution failed: %1").arg(errorDetail));
        return;
    }

    // 读取输出
    QString output;
    char buffer[1024];
    int bytesRead;
    int totalBytes = 0;
    const int maxOutputSize = 1024 * 1024; // 限制最大输出1MB

    while ((bytesRead = ssh_channel_read_timeout(
                channel, buffer, sizeof(buffer), 0, 500)) > 0)
    {
        output.append(QByteArray(buffer, bytesRead));
        totalBytes += bytesRead;

        if (totalBytes >= maxOutputSize) {
            emit sshError(tr("Output exceeded maximum size limit"));
            break;
        }
    }

    if (bytesRead < 0) {
        emit sshError(tr("Error reading output: %1").arg(ssh_get_error(my_ssh_session)));
    } else {
        qDebug()<<"output:"<<output;
        emit commandExecuted(output);
    }
    ssh_channel_free(channel);
    emit sshCommandSuccess();
}

void sshManager::sshSudoCommandExecut(const QString &command)
{
    // 前置检查
    if (!my_ssh_session || !ssh_is_connected(my_ssh_session)) {
        emit sshError(tr("SSH session is not connected"));
        return;
    }
    ssh_channel channel = ssh_channel_new(my_ssh_session);
    if (channel == nullptr) {
        QMetaObject::invokeMethod(this, [this]() {
                emit sshError(tr("创建通道失败: %1").arg(ssh_get_error(my_ssh_session)));
            }, Qt::QueuedConnection);
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return;
    }

    // 打开 SSH 通道
    int rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK) {
        QMetaObject::invokeMethod(this, [this, channel]() {
                emit sshError(tr("打开通道失败: %1").arg(ssh_get_error(my_ssh_session)));
            }, Qt::QueuedConnection);
        ssh_channel_free(channel);
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return;
    }

    // 准备命令
    QByteArray cmdData = command.toUtf8();
    if (cmdData.isEmpty()) {
        emit sshError(tr("Empty command cannot be sent"));
        return;
    }
    // 构造 sudo 命令，使用 -S 参数让 sudo 从标准输入读取密码
    //QString sudoCmd = "sudo -S ls /root";
    QString sudoCmd = "sudo -S "+command;
    // 执行命令
    rc = ssh_channel_request_pty(channel);
    if (rc!= SSH_OK) {
        emit sshError(tr("SSH channel Failed to request pty"));
    }
    rc = ssh_channel_request_exec(channel, sudoCmd.toUtf8().constData());
    if (rc != SSH_OK) {
        QString errorDetail = QString("[Channel] %1\n[Session] %2")
                                  .arg(ssh_get_error(channel))
                                  .arg(ssh_get_error(my_ssh_session));
        emit sshError(tr("Command execution failed: %1").arg(errorDetail));
        return;
    }
    // 如果 sudo 要求密码，则写入密码到 channel
    QString sudoPassword = "foxpg1348\n";  // 注意包含换行符
    ssh_channel_write(channel, sudoPassword.toUtf8().constData(), sudoPassword.toUtf8().size());
    // 读取输出
    QString output;
    char buffer[1024];
    int bytesRead;
    int totalBytes = 0;
    const int maxOutputSize = 1024 * 1024; // 限制最大输出1MB

    while ((bytesRead = ssh_channel_read_timeout(
                channel, buffer, sizeof(buffer), 0, 100)) > 0)
    {
        output.append(QByteArray(buffer, bytesRead));
        totalBytes += bytesRead;

        if (totalBytes >= maxOutputSize) {
            emit sshError(tr("Output exceeded maximum size limit"));
            break;
        }
    }

    if (bytesRead < 0) {
        emit sshError(tr("Error reading output: %1").arg(ssh_get_error(my_ssh_session)));
    } else {
        output.replace("\r\n", "");
        qDebug()<<"output:"<<output;
        emit commandExecuted(output);
    }
    ssh_channel_free(channel);
}

void sshManager::cleanup()
{
    if (my_ssh_session) {
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        my_ssh_session = nullptr;
    }
}
//主机密钥验证功能
int sshManager::verify_knownhost(ssh_session session)
{
    int state, hlen;
    unsigned char *hash = NULL;
    char *hexa;
    char buf;
    state = ssh_is_server_known(session);//获取当前ssh服务器状态
    hlen = ssh_get_pubkey_hash(session, &hash);//获取服务器的公钥哈希值，验证服务器身份
    if (hlen < 0) return -1;//获取失败

    switch (state) {
    case SSH_SERVER_KNOWN_OK://主机已知，密钥未更改。这是理想状态，意味着服务器的身份受到信任
        break; //继续连接
    case SSH_SERVER_KNOWN_CHANGED://主机已知，但密钥已更改。这是一个严重警告，表明可能存在中间人攻击或服务器密钥已合法更新。除非用户能验证新密钥，否则应中断连接
        // 严重：除非用户可以肯定地验证密钥更改，否则提醒用户并终止连接。
        ssh_print_hexa("Public key hash", hash, hlen);
        free(hash);
        return -1;
    case SSH_SERVER_FOUND_OTHER://服务器在已知主机文件中未知。这可能是由于几个原因：服务器是新的，主机名或别名不匹配，或者已知主机文件缺失。用户必须确认 MD5 哈希（或其他指纹）是正确的
        //向用户展示密钥指纹并要求他们进行验证。如果他们验证无误，将密钥添加到已知主机文件中（ ssh_write_knownhost ）。
        free(hash);
        return -1;
    case SSH_SERVER_FILE_NOT_FOUND://无法找到已知的宿主文件。在这种情况下，该功能可能会回退到SSH_SERVER_NOT_KNOWN 行为
        //适当地处理错误（例如，显示错误消息）并可能拒绝连接
    case SSH_SERVER_NOT_KNOWN://未知服务器
        hexa = ssh_get_hexa(hash, hlen);
        fprintf(stderr,"The server is unknown. Do you trust the host key?\n");
        fprintf(stderr, "Public key hash: %s\n", hexa);
        free(hexa);
        if (fgets(&buf, sizeof(buf), stdin) == NULL) {
            free(hash);
            return -1;
        }
        if (strncasecmp(&buf, "yes", 3) != 0) {
            free(hash);
            return -1;
        }
        if (ssh_write_knownhost(session) < 0) {
            fprintf(stderr, "Error %s\n", strerror(errno));
            free(hash);
            return -1;
        }
        break;
    case SSH_SERVER_ERROR:
        fprintf(stderr, "Error %s", ssh_get_error(session));
        free(hash);
        return -1;
    }
    free(hash);
    return 0;
}
