#include "utrrobotqwrapper.hpp"
#include <QFile>
#include <QDebug>
#include <QEventLoop>
#include <future>

#define RAD_TO_DEGREE (180.0/3.14)

UtraRobot_QWrapper::UtraRobot_QWrapper(UtRobotConfig::TestConfig config, QObject *parent):QObject(parent), UtrRobot(config)
{
    connect(&timer_refresh_robot_joint_pos_, &QTimer::timeout, this, &UtraRobot_QWrapper::refresh_Robot_Joint_Pos);

    timer_refresh_robot_joint_pos_.start(500);
}

bool UtraRobot_QWrapper::robot_connect()
{
    QMutexTryLocker lck{mutext};
    if(!lck.isLocked())
    {
        qDebug() << __FUNCTION__ << " - please wait other robot action finish, then call this funciton!";
        return false;
    }
    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult, this](){
        executionResult = this->InitRobot();
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    setArm_connect(executionResult);
    emit Robot_Connection_State_Updated(this->is_robot_connected);
    return executionResult;
}

bool UtraRobot_QWrapper::robot_drag_activate(bool on)
{
    QMutexTryLocker lck{mutext};
    if(!lck.isLocked())
    {
        qDebug() << __FUNCTION__ << " - please wait other robot action finish, then call this funciton!";
        return false;
    }
    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut;
    if(on)
    {
        fut = std::async(std::launch::async, [&spinner, &executionResult, this](){
            executionResult = this->RobotCommand_EnterTeachMode();
            spinner.exit();
            return executionResult;
        });
    }
    else
    {
        fut = std::async(std::launch::async, [&spinner, &executionResult, this](){
            executionResult = this->RobotCommand_Hold();
            spinner.exit();
            return executionResult;
        });
    }
    spinner.exec();
    if(executionResult)
    {
        emit Robot_Drag_Activate(on);
    }
    return executionResult;
}

bool UtraRobot_QWrapper::test_graivty_and_spin()
{
    QMutexTryLocker lck{mutext};
    if(!lck.isLocked())
    {
        qDebug() << __FUNCTION__ << " - please wait other robot action finish, then call this funciton!";
        return false;
    }
    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult, this](){
        executionResult = this->RobotTest_Gravity();
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    return executionResult;
}

bool UtraRobot_QWrapper::test_accuracy_and_spin()
{
    QMutexTryLocker lck{mutext};
    if(!lck.isLocked())
    {
        qDebug() << __FUNCTION__ << " - please wait other robot action finish, then call this funciton!";
        return false;
    }
    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult, this](){
        executionResult = this->RobotTest_Accuracy();
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    return executionResult;
}

bool UtraRobot_QWrapper::test_workspace_and_spin()
{
    QMutexTryLocker lck{mutext};
    if(!lck.isLocked())
    {
        qDebug() << __FUNCTION__ << " - please wait other robot action finish, then call this funciton!";
        return false;
    }
    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult, this](){
        executionResult = this->RobotTest_Workspace();
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    return executionResult;
}

bool UtraRobot_QWrapper::test_repeatability_and_spin()
{
    QMutexTryLocker lck{mutext};
    if(!lck.isLocked())
    {
        qDebug() << __FUNCTION__ << " - please wait other robot action finish, then call this funciton!";
        return false;
    }
    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult, this](){
        executionResult = this->RobotTest_Repeat();
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    return executionResult;
}

bool UtraRobot_QWrapper::laserCalib_load_file(QString filePath)
{
    // QString filePath = QFileDialog::getOpenFileName(this,"打开文件","..//..//data/");
    QFile file{filePath};
    if(!file.exists())
    {
        qDebug() << __FUNCTION__ << " - " <<filePath << " file path not exist!";
        return false;
    }

    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << __FUNCTION__ << " - " <<filePath << " open failed!";
    }

    QByteArray array;
    while(!file.atEnd())
    {
        array = file.readLine();
        QString qstr = QString(array);
        QStringList strlist = qstr.split(",");
        UtRobotConfig::JointPos tmpPos;
        QStringList::iterator ittmp;
        ittmp = strlist.begin();
        int num = 0;
        for (;ittmp!=strlist.end();ittmp ++) {
            if (num < 7)
            {
                tmpPos[num] = ittmp->toDouble();
            }
            num ++;
        }
        context_laser_calib_.preteached_Jpose.append(tmpPos);
    }
    context_laser_calib_.total_num_preteached_Jpose = context_laser_calib_.preteached_Jpose.count();
    context_laser_calib_.current_index_preteached_Jpose = 0;

    emit laserCalib_file_loaded(context_laser_calib_.preteached_Jpose.count());
    emit laserCalib_next_pos_updated(0);
    return true;
}

bool UtraRobot_QWrapper::laserCalib_Robot_MoveTo_NextPos_and_spin()
{
    QMutexTryLocker lck{mutext};
    if(!lck.isLocked())
    {
        qDebug() << __FUNCTION__ << " - please wait other robot action finish, then call this funciton!";
        return false;
    }
    UtRobotConfig::JointPos jointPose = context_laser_calib_.preteached_Jpose.at(context_laser_calib_.current_index_preteached_Jpose);
    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult, &jointPose, this](){
        executionResult = this->RobotCommand_JointPtp(jointPose);
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    if(executionResult)
    {
        context_laser_calib_.current_index_preteached_Jpose++;
        emit laserCalib_next_pos_updated(context_laser_calib_.current_index_preteached_Jpose
                                         % context_laser_calib_.total_num_preteached_Jpose);
    }
    return executionResult;
}

QVariantList UtraRobot_QWrapper::convert_JPos_To_VariantList(UtRobotConfig::JointPos &Jpos)
{
    QVariantList list;
    for(auto singleJointPos: Jpos)
    {
        list.append(singleJointPos * RAD_TO_DEGREE);
    }
    return list;
}

QVariantList UtraRobot_QWrapper::convert_CartPos_To_VariantList(UtRobotConfig::CartesianPos &CartPos)
{
    QVariantList list;
    for(auto cartPos: CartPos)
    {
        list.append(cartPos);
    }
    return list;
}

void UtraRobot_QWrapper::refresh_Robot_Joint_Pos()
{
    UtRobotConfig::JointPos current_Jpos;
    this->getJointPos(current_Jpos);
    emit update_Robot_Joint_Pos(convert_JPos_To_VariantList(current_Jpos));
    UtRobotConfig::CartesianPos current_Tcp_CartesianPos;
    this->getTcpPos(current_Tcp_CartesianPos);
    emit update_Tcp_Cartesian_Pos(convert_CartPos_To_VariantList(current_Tcp_CartesianPos));
}

bool UtraRobot_QWrapper::arm_connect() const
{
    return m_arm_connect;
}

void UtraRobot_QWrapper::setArm_connect(bool newArm_connect)
{
    if (m_arm_connect == newArm_connect)
        return;
    m_arm_connect = newArm_connect;
    emit arm_connectChanged();
}
