#include "utrrobot.hpp"
#include "iostream"
// UtrRobot::UtrRobot() {}

#define ROBOT_NO_ERROR 0
#define TIMEOUT_MS_ROBOT_START_MOV 1000
#define CYCLE_MS_ROTOT_STATUS_REPORT 10

UtrRobot::UtrRobot(UtRobotConfig::TestConfig config):config_{config} {
    ubot_ = new UtraApiTcp(config.ip.data());

    is_robot_connected = !ubot_->is_error();

    utra_report_ = new UtraReportStatus100Hz(config.ip.data(), 7);

    thd_refresh_robot_status_ = std::thread(&UtrRobot::ThreadFunction_UpdateRobotStatus, this);
}

UtrRobot::~UtrRobot()
{
    this->thread_flag = false;
    this->thd_refresh_robot_status_.join();
    if (utra_report_ != nullptr) {
        utra_report_->close();
        delete utra_report_;
        utra_report_ = nullptr;
    }
    if (ubot_ != nullptr) {
        delete ubot_;
        ubot_ = nullptr;
    }
}

bool UtrRobot::getJointPos(UtRobotConfig::JointPos &jPos)
{
    jPos[0] = this->rx_data_.joint[0];
    jPos[1] = this->rx_data_.joint[1];
    jPos[2] = this->rx_data_.joint[2];
    jPos[3] = this->rx_data_.joint[3];
    jPos[4] = this->rx_data_.joint[4];
    jPos[5] = this->rx_data_.joint[5];
    jPos[6] = this->rx_data_.joint[6];
    return true;
}

bool UtrRobot::getTcpPos(UtRobotConfig::CartesianPos &pose)
{
    pose[0] = this->rx_data_.pose[0];
    pose[1] = this->rx_data_.pose[1];
    pose[2] = this->rx_data_.pose[2];
    pose[3] = this->rx_data_.pose[3];
    pose[4] = this->rx_data_.pose[4];
    pose[5] = this->rx_data_.pose[5];
    return true;
}

bool UtrRobot::RobotTest_Gravity()
{
    return RobotCommand_JointPtp(config_.gravityTest.target_JointPos);
}

bool UtrRobot::RobotTest_Accuracy()
{
    if(!RobotCommand_JointPtp(config_.accuracyTest.initial_JointPose))
    {
        std::cout << __FUNCTION__ << " inital joint position move failed!" << std::endl;
        return false;
    }

    for(unsigned long i=0; (sizeof(config_.accuracyTest.mid_CartPoses)/sizeof(config_.accuracyTest.mid_CartPoses[0])) > i; i++)
    {
        if(!RobotCommand_CartesianLine(config_.accuracyTest.mid_CartPoses[i]))
        {
            std::cout << __FUNCTION__ << " " << (i+1) << "th mid cartisian pos failed to reach!" << std::endl;
            return false;
        }
        //to mimic the original sleep logic
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    if(!RobotCommand_JointPtp(config_.accuracyTest.final_JointPose))
    {
        std::cout << __FUNCTION__ << " final joint position move failed!" << std::endl;
        return false;
    }

    return true;
}

bool UtrRobot::RobotTest_Workspace()
{
    if(!RobotCommand_JointPtp(config_.workspaceTest.initial_JointPose))
    {
        std::cout << __FUNCTION__ << " inital joint position move failed!" << std::endl;
        return false;
    }

    for(unsigned long i=0; (sizeof(config_.workspaceTest.mid_CartPoses)/sizeof(config_.workspaceTest.mid_CartPoses[0])) > i; i++)
    {
        if(!RobotCommand_CartesianLine(config_.workspaceTest.mid_CartPoses[i]))
        {
            std::cout << __FUNCTION__ << " " << (i+1) << "th mid cartisian pos failed to reach!" << std::endl;
            return false;
        }
        //to mimic the original sleep logic
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    if(!RobotCommand_JointPtp(config_.workspaceTest.final_JointPose))
    {
        std::cout << __FUNCTION__ << " final joint position move failed!" << std::endl;
        return false;
    }

    return true;
}

bool UtrRobot::RobotTest_Repeat()
{
    if(!RobotCommand_JointPtp(config_.repeatTest.initial_JointPose))
    {
        std::cout << __FUNCTION__ << " inital joint position move failed!" << std::endl;
        return false;
    }

    for(unsigned long i=0; (sizeof(config_.repeatTest.mid_CartPoses)/sizeof(config_.repeatTest.mid_CartPoses[0])) > i; i++)
    {
        if(!RobotCommand_CartesianLine(config_.repeatTest.mid_CartPoses[i]))
        {
            std::cout << __FUNCTION__ << " " << (i+1) << "th mid cartisian pos failed to reach!" << std::endl;
            return false;
        }
        //to mimic the original sleep logic
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    if(!RobotCommand_JointPtp(config_.repeatTest.final_JointPose))
    {
        std::cout << __FUNCTION__ << " final joint position move failed!" << std::endl;
        return false;
    }

    return true;
}

bool UtrRobot::RobotCommand_JointPtp(UtRobotConfig::JointPos jPos, int timeout_ms)
{
    std::lock_guard lck_exclusive{this->mtx_exclusivity_command_};
    ubot_->reset_err();
    std::unique_lock lck{this->mtx_robotState_};
    if(((uint8_t)(UtrRobotStatus::Standby) == this->robotState_.motion_status)
        || (this->isDragging) || this->cmd_stop)
    {
        std::cout << __FUNCTION__ << " utr robot is not standy"<<std::endl;
        this->cmd_stop = false;
        return false;
    }
    this->cmd_stop = false;
    auto timestamp_timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(timeout_ms);
    lck.unlock();
    if(ROBOT_NO_ERROR != ubot_->moveto_joint_p2p(jPos.data(), speed, acc, 0))
    {
        uint8_t error_code[2];
        int ret = ubot_->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        return false;
    }

    lck.lock();
    bool waitStartMove_Success = this->cv_robotState_.wait_for(lck, std::chrono::milliseconds(TIMEOUT_MS_ROBOT_START_MOV),
        [this](){return this->Predicate_Robot_Moving_Without_Error();
    });
    if(!waitStartMove_Success)
    {
        std::cout << __FUNCTION__ << " utr robot start move timeout!"<<std::endl;
        return false;
    }

    bool waitFinishMove_Success = this->cv_robotState_.wait_until(lck, timestamp_timeout,
        [this](){ return this->Predicate_Robot_Standby_Or_Error() || this->cmd_stop;});

    if(!waitFinishMove_Success)
    {
        std::cout << __FUNCTION__ << " utr robot wait move finish timeout!"<<std::endl;
        return false;
    }
    if(ROBOT_NO_ERROR != this->robotState_.err_code)
    {
        std::cout << __FUNCTION__ << " utr robot occurs error: " << this->robotState_.err_code;
        ubot_->reset_err();
        return false;
    }
    else if(this->cmd_stop)
    {
        std::cout << __FUNCTION__ << " utr robot motion is commanded to be canceled: ";
        return false;
    }

    std::cout << __FUNCTION__ << " utr robot finish moving"<<std::endl;
    return true;
}

bool UtrRobot::RobotCommand_CartesianLine(UtRobotConfig::CartesianPos Pos, int timeout_ms)
{
    std::lock_guard lck_exclusive{this->mtx_exclusivity_command_};
    ubot_->reset_err();
    std::unique_lock lck{this->mtx_robotState_};
    if(((uint8_t)(UtrRobotStatus::Standby) == this->robotState_.motion_status)
        || (this->isDragging) || this->cmd_stop)
    {
        std::cout << __FUNCTION__ << " utr robot is not standy"<<std::endl;
        this->cmd_stop = false;
        return false;
    }
    this->cmd_stop = false;
    auto timestamp_timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(timeout_ms);
    lck.unlock();

    if(ROBOT_NO_ERROR != ubot_->moveto_cartesian_line(Pos.data(), speed, acc, 0))
    {
        uint8_t error_code[2];
        int ret = ubot_->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        return false;
    }

    lck.lock();
    bool waitStartMove_Success = this->cv_robotState_.wait_for(lck, std::chrono::milliseconds(TIMEOUT_MS_ROBOT_START_MOV),
                                                               [this](){return this->Predicate_Robot_Moving_Without_Error();
                                                               });
    if(!waitStartMove_Success)
    {
        std::cout << __FUNCTION__ << " utr robot start move timeout!"<<std::endl;
        return false;
    }

    bool waitFinishMove_Success = this->cv_robotState_.wait_until(lck, timestamp_timeout,
                                                                  [this](){ return this->Predicate_Robot_Standby_Or_Error() || this->cmd_stop;});

    if(!waitFinishMove_Success)
    {
        std::cout << __FUNCTION__ << " utr robot wait move finish timeout!"<<std::endl;
        return false;
    }
    if(ROBOT_NO_ERROR != this->robotState_.err_code)
    {
        std::cout << __FUNCTION__ << " utr robot occurs error: " << this->robotState_.err_code<<std::endl;
        ubot_->reset_err();
        return false;
    }
    else if(this->cmd_stop)
    {
        std::cout << __FUNCTION__ << " utr robot motion is commanded to be canceled: ";
        return false;
    }

    std::cout << __FUNCTION__ << " utr robot finish moving"<<std::endl;
    return true;
}

bool UtrRobot::RobotCommand_EnterTeachMode()
{
    std::lock_guard lck_exclusive{this->mtx_exclusivity_command_};
    std::unique_lock lck{this->mtx_robotState_};
    if((this->isDragging) || this->cmd_stop)
    {
        std::cout << __FUNCTION__ << " utr robot is dragging or cmd to be stopped!"<<std::endl;
        this->cmd_stop = false;
        return false;
    }
    bool waitStandby_Success = this->cv_robotState_.wait_for(lck, std::chrono::seconds(1), [this](){
        return ((uint8_t)(UtrRobotStatus::Standby) == this->robotState_.motion_status);
    });
    if(!waitStandby_Success)
    {
        std::cout << __FUNCTION__ << " utr robot wait standy timeout"<<std::endl;
        return false;
    }
    this->cmd_stop = false;
    lck.unlock();
    if(ROBOT_NO_ERROR != ubot_->set_motion_enable(8, 1))
    {
        std::cout << __FUNCTION__ << " utr robot start enable failed!"<<std::endl;
        return false;
    }

    std::cout << __FUNCTION__ << " utr robot reset motion status begin!"<<std::endl;
    if(ROBOT_NO_ERROR != ubot_->set_motion_status(0))
    {
        std::cout << __FUNCTION__ << " utr robot reset motion status failed!"<<std::endl;
        return false;
    }
    lck.lock();
    bool waitInitialize_Success = this->cv_robotState_.wait_for(lck, std::chrono::seconds(1), [this](){
        return ((uint8_t)(UtrRobotStatus::Initilizing) == this->robotState_.motion_status);
    });
    if(!waitInitialize_Success)
    {
        std::cout << __FUNCTION__ << " utr robot wait initilized after enable timeout"<<std::endl;
        return false;
    }
    bool waitReStandby_Success = this->cv_robotState_.wait_for(lck, std::chrono::seconds(1), [this](){
        return ((uint8_t)(UtrRobotStatus::Standby) == this->robotState_.motion_status);
    });
    if(!waitReStandby_Success)
    {
        std::cout << __FUNCTION__ << " utr robot wait re-standby after enable timeout"<<std::endl;
        return false;
    }
    lck.unlock();
    if(ROBOT_NO_ERROR != ubot_->set_motion_mode(2))
    {
        std::cout << __FUNCTION__ << " utr robot set motion mode '2' failed!"<<std::endl;
        return false;
    }
    lck.lock();
    bool waitModeToggled_Success = this->cv_robotState_.wait_for(lck, std::chrono::seconds(1), [this](){
        return (2 == this->robotState_.motion_mode);
    });
    if(!waitModeToggled_Success)
    {
        std::cout << __FUNCTION__ << " utr robot wait motion mode toggled to '2' timeout"<<std::endl;
        return false;
    }
    printf("set_into_motion_mode_teach\n");
    this->isDragging = true;
    return true;
}

bool UtrRobot::RobotCommand_Hold()
{
    std::unique_lock lck{this->mtx_robotState_};
    this->cmd_stop = true;
    this->cv_robotState_.notify_all();
    lck.unlock();
    if(ROBOT_NO_ERROR != ubot_->set_motion_mode(0))
    {
        std::cout << __FUNCTION__ << " utr robot set motion mode '0' failed!"<<std::endl;
        return false;
    }
    lck.lock();
    bool waitModeToggled_Success = this->cv_robotState_.wait_for(lck, std::chrono::seconds(1), [this](){
        return (0 == this->robotState_.motion_mode);
    });
    if(!waitModeToggled_Success)
    {
        std::cout << __FUNCTION__ << " utr robot wait motion mode toggled to '0' timeout"<<std::endl;
        return false;
    }
    lck.unlock();
    if(ROBOT_NO_ERROR != ubot_->set_motion_enable(8, 1))
    {
        std::cout << __FUNCTION__ << " utr robot start enable failed!"<<std::endl;
        return false;
    }
    if(ROBOT_NO_ERROR != ubot_->set_motion_status(0))
    {
        std::cout << __FUNCTION__ << " utr robot reset motion status failed!"<<std::endl;
        return false;
    }
    lck.lock();
    bool waitInitialize_Success = this->cv_robotState_.wait_for(lck, std::chrono::seconds(1), [this](){
        return ((uint8_t)(UtrRobotStatus::Initilizing) == this->robotState_.motion_status);
    });
    if(!waitInitialize_Success)
    {
        std::cout << __FUNCTION__ << " utr robot wait initilized after enable timeout"<<std::endl;
        return false;
    }
    bool waitReStandby_Success = this->cv_robotState_.wait_for(lck, std::chrono::seconds(1), [this](){
        return ((uint8_t)(UtrRobotStatus::Standby) == this->robotState_.motion_status);
    });
    if(!waitReStandby_Success)
    {
        std::cout << __FUNCTION__ << " utr robot wait re-standby after enable timeout"<<std::endl;
        return false;
    }
    printf("set_into_motion_mode position hold\n");
    this->isDragging = false;
    this->cmd_stop = false;
    return true;
}

bool UtrRobot::RobotCommand_ResetError()
{
    ubot_->reset_err();
    std::cout << __FUNCTION__ << " finish reset error, current error code: " << this->robotState_.err_code <<std::endl;
    std::unique_lock lck{this->mtx_robotState_};
    bool waitNoError_Success = this->cv_robotState_.wait_for(lck, std::chrono::seconds(1), [this](){
        return (ROBOT_NO_ERROR == this->robotState_.err_code);
    });
    if(!waitNoError_Success)
    {
        std::cout << __FUNCTION__ << " utr robot wait error clear out timeout," << this->robotState_.err_code <<std::endl;
        return false;
    }
    return true;
}

void UtrRobot::ThreadFunction_UpdateRobotStatus()
{
    while(this->thread_flag)
    {
        RobotStatus_SnapShot prev_state = this->robotState_;
        if (utra_report_->is_update()) {
            utra_report_data_missing_counter_ = 0;
            utra_report_->get_data(&rx_data_);
            this->robotState_.motion_status = rx_data_.motion_status;
            this->robotState_.motion_mode   = rx_data_.motion_mode;
            this->robotState_.err_code      = rx_data_.err_code;
            this->robotState_.war_code      = rx_data_.war_code;

            if ((this->robotState_.motion_status ^ prev_state.motion_status) ||
                (this->robotState_.motion_mode   ^ prev_state.motion_mode) ||
                (this->robotState_.err_code      ^ prev_state.err_code) ||
                (this->robotState_.war_code      ^ prev_state.war_code)) {
                
                if(this->robotState_.motion_status ^ prev_state.motion_status)
                {
                    printf("motion status changed from {%d}->{%d}\r\n",  prev_state.motion_status, this->robotState_.motion_status);
                }

                if(this->robotState_.motion_mode ^ prev_state.motion_mode)
                {
                    printf("motion mode changed from {%d}->{%d}\r\n",  prev_state.motion_mode, this->robotState_.motion_mode);
                }

                if(this->robotState_.err_code ^ prev_state.err_code)
                {
                    printf("motion error code changed from {%d}->{%d}\r\n",  prev_state.err_code, this->robotState_.err_code);
                }

                std::lock_guard lck{this->mtx_robotState_};
                this->cv_robotState_.notify_all();
            }
        }
        else
        {
            utra_report_data_missing_counter_++;
        }

        if ((utra_report_data_missing_counter_ > 20) ^ (this->robotState_.comm_error)) {
            std::lock_guard lck{this->mtx_robotState_};
            this->cv_robotState_.notify_all();
            this->robotState_.comm_error = (utra_report_data_missing_counter_ > 20);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(CYCLE_MS_ROTOT_STATUS_REPORT));
    }
}


bool UtrRobot::Predicate_Robot_Standby_Or_Error()
{
    return ((uint8_t)(UtrRobotStatus::Standby) == this->robotState_.motion_status)
           || (ROBOT_NO_ERROR == this->robotState_.err_code);
}

bool UtrRobot::Predicate_Robot_Moving_Without_Error()
{
    return ((uint8_t)(UtrRobotStatus::Moving) == this->robotState_.motion_status)
           && (ROBOT_NO_ERROR != this->robotState_.err_code);
}
