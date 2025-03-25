#include "utrarobotqwrapper.hpp"
#include <QFile>
#include <QDebug>
#include <QEventLoop>
#include <future>

#define RAD_TO_DEGREE (180.0/3.14)

UtraRobot_QWrapper::UtraRobot_QWrapper(UtRobotConfig::TestConfig& config, QObject *parent):QObject(parent), UtraRobot(config)
{
    connect(&timer_refresh_robot_joint_pos_, &QTimer::timeout, this, &UtraRobot_QWrapper::refresh_Robot_Joint_Pos);

    timer_refresh_robot_joint_pos_.start(500);

    // connect(this, &UtraRobot_QWrapper::Robot_Connection_State_Updated, this, [this](bool connect){
    //     if(connect)
    //     {
    //         if(!Robot_Check_Mdh_Offset_Are_All_Zero())
    //         {
    //             emit robot_Warning_MDH_OFFSET_NOT_ZERO();
    //         }
    //     }
    // });

    // connect(this, &UtraRobot_QWrapper::robot_Warning_MDH_OFFSET_NOT_ZERO, this, &UtraRobot_QWrapper::ZeroOut_MDH_offset);
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
        qDebug() << "Robot UUID:" << QString::fromStdString(this->config_.identity_Info.UUID);
        qDebug() << "Robot SW version:" << QString::fromStdString(this->config_.identity_Info.VERSION_SW);
        qDebug() << "Robot HW version:" << QString::fromStdString(this->config_.identity_Info.VERSION_HW);
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
    if(on)
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
        fut = std::async(std::launch::async, [&spinner, &executionResult, this](){
            executionResult = this->RobotCommand_EnterTeachMode();
            spinner.exit();
            return executionResult;
        });
        spinner.exec();
        if(executionResult)
        {
            qDebug() << "开启拖拽，成功";
            return true;
        }
        else
        {
            qWarning() << "开启拖拽，失败";
            return false;
        }
    }
    else
    {
        QEventLoop spinner;
        bool executionResult = false;
        std::future<bool> fut;
        fut = std::async(std::launch::async, [&spinner, &executionResult, this](){
            executionResult = this->RobotCommand_Hold();
            spinner.exit();
            return executionResult;
        });
        spinner.exec();
        if(executionResult)
        {
            qDebug() << "关闭拖拽，成功";
            return true;
        }
        else
        {
            qWarning() << "关闭拖拽，失败";
            return false;
        }
    }
}

void UtraRobot_QWrapper::robot_set_speed_override(double percent)
{
    this->set_speed_override(percent);
}

void UtraRobot_QWrapper::spin_until_all_action_finished()
{
    QEventLoop spinner;
    std::future<void> fut = std::async(std::launch::async, [&spinner, this](){
        this->mutext.lock();
        this->mutext.unlock();
        spinner.exit();
        return ;
    });
    spinner.exec();
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
    if(executionResult)
    {
        qDebug() << "机械臂负载测试，走位成功";
        return true;
    }
    else
    {
        qWarning() << "机械臂负载测试，走位失败";
        return false;
    }
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
    if(executionResult)
    {
        qDebug() << "机械臂位置准确性测试，走位成功";
        return true;
    }
    else
    {
        qWarning() << "机械臂位置准确性测试，走位失败";
        return false;
    }
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
    if(executionResult)
    {
        qDebug() << "机械臂工作空间运动测试，走位成功";
        return true;
    }
    else
    {
        qWarning() << "机械臂工作空间运动测试，走位失败";
        return false;
    }
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
    if(executionResult)
    {
        qDebug() << "机械臂位置重复性测试，走位成功";
        return true;
    }
    else
    {
        qWarning() << "机械臂位置重复性测试，走位失败";
        return false;
    }
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

bool UtraRobot_QWrapper::postLaserCalib_Write_MDH_offset()
{
    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult, this](){
        executionResult = this->Robot_Set_Mdh_Offset_With_Calib_Result();
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    if(executionResult)
    {
        qDebug() << "机械臂标定参数已经写入控制器，接下来重启机械臂控制器";
        return reboot_Robot_And_Wait_Reconnect();
    }
    else
    {
        qWarning() << "机械臂标定参数写入控制器失败";
        return false;
    }
}

bool UtraRobot_QWrapper::check_MDH_Offset_Parm_All_Zero()
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
        executionResult = this->Robot_Check_Mdh_Offset_Are_All_Zero();
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    if(executionResult)
    {
        qWarning() << "MDH参数补偿值为零，说明未进行过机械臂标定，请进行机械臂标定后再使用";
        return true;
    }
    else
    {
        qWarning() << "MDH参数补偿值不为零，运行机械臂标定前，请调用ZeroOut_MDH_offset来清零";
        return false;
    }
}

bool UtraRobot_QWrapper::zeroOut_MDH_offset()
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
        executionResult = this->Robot_ZeroOut_Mdh_offset();
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    if(executionResult)
    {
        qDebug() << "Mdh参数已经清零, 请调用功能Reboot_Robot_And_Wait_Reconnect来进行重启";
        return true;
    }
    else
    {
        qWarning() << "Mdh清零失败";
        return false;
    }
}

bool UtraRobot_QWrapper::reboot_Robot_And_Wait_Reconnect()
{
    QMutexTryLocker lck{mutext};
    if(!lck.isLocked())
    {
        qDebug() << __FUNCTION__ << " - please wait other robot action finish, then call this funciton!";
        return false;
    }
    QEventLoop spinner_cmd_reboot;
    bool executionResult = false;
    std::future<bool> fut_reboot = std::async(std::launch::async, [&spinner_cmd_reboot, &executionResult, this](){
        executionResult = this->Robot_Reboot();
        spinner_cmd_reboot.exit();
        return executionResult;
    });
    spinner_cmd_reboot.exec();
    if(!executionResult)
    {
        qWarning() << __FUNCTION__ << " - failed to send reboot command to controller!";
        return false;
    }
    qDebug() << "已经给机械臂下发重启指令，等待断开链接";

    QEventLoop spinner_disconnect;
    std::future<bool> fut_diconnect = std::async(std::launch::async, [&spinner_disconnect, &executionResult, this](){
        executionResult = this->Robot_Wait_Until_Disconnected();
        spinner_disconnect.exit();
        return executionResult;
    });
    spinner_disconnect.exec();
    if(!executionResult)
    {
        qWarning() << __FUNCTION__ << " - wait controller disconnection timeout!";
        return false;
    }
    qDebug() << "机械臂控制器已经断开链接，等待重新链接";

    QEventLoop spinner_connect;
    std::future<bool> fut_connect = std::async(std::launch::async, [&spinner_connect, &executionResult, this](){
        executionResult = this->Robot_Wait_Until_Connected();
        spinner_connect.exit();
        return executionResult;
    });
    spinner_connect.exec();
    if(!executionResult)
    {
        qWarning() << __FUNCTION__ << " - wait controller reconnection timeout!";
        return false;
    }
    qDebug() << "机械臂重新链接， 准备初始化";

    QEventLoop spinner_init;
    std::future<bool> fut = std::async(std::launch::async, [&spinner_init, &executionResult, this](){
        executionResult = this->InitRobot();
        spinner_init.exit();
        return executionResult;
    });
    spinner_init.exec();
    if(!executionResult)
    {
        qWarning() << __FUNCTION__ << " - 机械臂初始化失败，建议重新打开程序!";
        return false;
    }
    qDebug() << "机械臂初始化成功";
    return true;
}

bool UtraRobot_QWrapper::move_To_Joint_Position(QVariantList jointpos)
{
    QMutexTryLocker lck{mutext};
    if(!lck.isLocked())
    {
        qDebug() << __FUNCTION__ << " - please wait other robot action finish, then call this funciton!";
        return false;
    }
    UtRobotConfig::JointPos target_jpos;
    int jp_index=0;
    QString joint_pos_str = "{";
    for(auto single_joint : jointpos)
    {
        target_jpos[jp_index] = single_joint.toFloat();
        joint_pos_str += QString::number(single_joint.toFloat()) + ",";
        jp_index++;
    }
    joint_pos_str += "}";
    if(7 != jp_index)
    {
        qWarning() << "机械臂肘关节运动，给予的肘关节数量不正确，应该长度为[7]";
        return false;
    }

    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult, &target_jpos, this](){
        executionResult = this->RobotCommand_JointPtp(target_jpos);
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    if(executionResult)
    {
        qDebug() << "Robot Arm move to given joint position of radian:" << joint_pos_str;
        return true;
    }
    else
    {
        qWarning() << "Robot Arm failed to move to given joint position of radian:" << joint_pos_str;
        return false;
    }
}

bool UtraRobot_QWrapper::move_To_Joint_Position_Degree(QVariantList jointpos)
{
    QMutexTryLocker lck{mutext};
    if(!lck.isLocked())
    {
        qDebug() << __FUNCTION__ << " - please wait other robot action finish, then call this funciton!";
        return false;
    }
    UtRobotConfig::JointPos target_jpos;
    int jp_index=0;
    QString joint_pos_str = "{";
    int cnt = 1;
    for(auto single_joint : jointpos)
    {
        qDebug() << "Original Pos " << (cnt++) << " is" << single_joint;
        target_jpos[jp_index] = single_joint.toFloat() * M_PI / 180.0;
        joint_pos_str += QString::number(single_joint.toFloat()) + ",";
        jp_index++;
    }
    joint_pos_str += "}";
    if(7 != jp_index)
    {
        qWarning() << "机械臂肘关节运动，给予的肘关节数量不正确，应该长度为[7]";
        return false;
    }

    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult, &target_jpos, this](){
        executionResult = this->RobotCommand_JointPtp(target_jpos);
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    if(executionResult)
    {
        qDebug() << "Robot Arm move to given joint position of dgree:" << joint_pos_str;
        return true;
    }
    else
    {
        qWarning() << "Robot Arm failed to move to given joint position of dgree:" << joint_pos_str;
        return false;
    }
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

    uint32_t enable_state = this->get_enables_status();
    auto prev_enable_status = this->axis_total_status;
    memcpy(&this->axis_total_status, &enable_state, 4);

    if(prev_enable_status.enable_1 ^ this->axis_total_status.enable_1)
    {
        m_axis_enable_1 = this->axis_total_status.enable_1;
        emit axis_enable_1_Changed();
    }
    if(prev_enable_status.enable_2 ^ this->axis_total_status.enable_2)
    {
        m_axis_enable_2 = this->axis_total_status.enable_2;
        emit axis_enable_2_Changed();
    }
    if(prev_enable_status.enable_3 ^ this->axis_total_status.enable_3)
    {
        m_axis_enable_3 = this->axis_total_status.enable_3;
        emit axis_enable_3_Changed();
    }
    if(prev_enable_status.enable_4 ^ this->axis_total_status.enable_4)
    {
        m_axis_enable_4 = this->axis_total_status.enable_4;
        emit axis_enable_4_Changed();
    }
    if(prev_enable_status.enable_5 ^ this->axis_total_status.enable_5)
    {
        m_axis_enable_5 = this->axis_total_status.enable_5;
        emit axis_enable_5_Changed();
    }
    if(prev_enable_status.enable_6 ^ this->axis_total_status.enable_6)
    {
        m_axis_enable_6 = this->axis_total_status.enable_6;
        emit axis_enable_6_Changed();
    }
    if(prev_enable_status.enable_7 ^ this->axis_total_status.enable_7)
    {
        m_axis_enable_7 = this->axis_total_status.enable_7;
        emit axis_enable_7_Changed();
    }
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

bool UtraRobot_QWrapper::axis_enabled_1() const
{
    return m_axis_enable_1;
}

bool UtraRobot_QWrapper::enable_axis_1(bool enable)
{
    QMutexTryLocker lck{mutext};
    if(!lck.isLocked())
    {
        qDebug() << __FUNCTION__ << " - please wait other robot action finish, then call this funciton!";
        return false;
    }
    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult, this, enable](){
        executionResult = this->Robot_Enable_Single_Axis(1, enable);
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    return executionResult;
}

bool UtraRobot_QWrapper::axis_enabled_2() const
{
    return m_axis_enable_2;
}

bool UtraRobot_QWrapper::enable_axis_2(bool enable)
{
    QMutexTryLocker lck{mutext};
    if(!lck.isLocked())
    {
        qDebug() << __FUNCTION__ << " - please wait other robot action finish, then call this funciton!";
        return false;
    }
    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult, this, enable](){
        executionResult = this->Robot_Enable_Single_Axis(2, enable);
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    return executionResult;
}

bool UtraRobot_QWrapper::axis_enabled_3() const
{
    return m_axis_enable_3;
}

bool UtraRobot_QWrapper::enable_axis_3(bool enable)
{
    QMutexTryLocker lck{mutext};
    if(!lck.isLocked())
    {
        qDebug() << __FUNCTION__ << " - please wait other robot action finish, then call this funciton!";
        return false;
    }
    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult, this, enable](){
        executionResult = this->Robot_Enable_Single_Axis(3, enable);
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    return executionResult;
}

bool UtraRobot_QWrapper::axis_enabled_4() const
{
    return m_axis_enable_4;
}

bool UtraRobot_QWrapper::enable_axis_4(bool enable)
{
    QMutexTryLocker lck{mutext};
    if(!lck.isLocked())
    {
        qDebug() << __FUNCTION__ << " - please wait other robot action finish, then call this funciton!";
        return false;
    }
    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult, this, enable](){
        executionResult = this->Robot_Enable_Single_Axis(4, enable);
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    return executionResult;
}

bool UtraRobot_QWrapper::axis_enabled_5() const
{
    return m_axis_enable_5;
}

bool UtraRobot_QWrapper::enable_axis_5(bool enable)
{
    QMutexTryLocker lck{mutext};
    if(!lck.isLocked())
    {
        qDebug() << __FUNCTION__ << " - please wait other robot action finish, then call this funciton!";
        return false;
    }
    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult, this, enable](){
        executionResult = this->Robot_Enable_Single_Axis(5, enable);
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    return executionResult;
}

bool UtraRobot_QWrapper::axis_enabled_6() const
{
    return m_axis_enable_6;
}

bool UtraRobot_QWrapper::enable_axis_6(bool enable)
{
    QMutexTryLocker lck{mutext};
    if(!lck.isLocked())
    {
        qDebug() << __FUNCTION__ << " - please wait other robot action finish, then call this funciton!";
        return false;
    }
    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult, this, enable](){
        executionResult = this->Robot_Enable_Single_Axis(6, enable);
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    return executionResult;
}

bool UtraRobot_QWrapper::axis_enabled_7() const
{
    return m_axis_enable_7;
}

bool UtraRobot_QWrapper::enable_axis_7(bool enable)
{
    QMutexTryLocker lck{mutext};
    if(!lck.isLocked())
    {
        qDebug() << __FUNCTION__ << " - please wait other robot action finish, then call this funciton!";
        return false;
    }
    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult, this, enable](){
        executionResult = this->Robot_Enable_Single_Axis(7, enable);
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    return executionResult;
}
