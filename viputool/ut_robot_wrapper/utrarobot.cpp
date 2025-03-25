#include "utrarobot.hpp"
#include "iostream"
#include "string"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
// UtraRobot::UtraRobot() {}

#define ROBOT_NO_ERROR 0
#define TIMEOUT_MS_ROBOT_START_MOV 1000
#define CYCLE_MS_ROTOT_STATUS_REPORT 10
#define DALAY_MS_TEACH_TO_POSITION_MODE 1050

#define UTR_ROBOT_SERVER_PORT 502
#define JOIN_MOVE_DEFAULT_SPD 5.0/57.54
#define JOIN_MOVE_DEFAULT_ACC 3.0
#define CART_MOVE_DEFAULT_VEL 50.0
#define CART_MOVE_DEFAULT_ACC 1000.0

UtraRobot::UtraRobot(UtRobotConfig::TestConfig& config):config_{config} {
    this->speed =JOIN_MOVE_DEFAULT_SPD;  // rad/s
    this->acc   =JOIN_MOVE_DEFAULT_ACC;
    this->mvvelo=CART_MOVE_DEFAULT_VEL;   // mm/s
    this->mvacc =CART_MOVE_DEFAULT_ACC;  // mm/s
    this->mvtime=0;

    memset(&this->rx_data_, 0x00, sizeof(this->rx_data_));
    thd_refresh_robot_status_ = std::thread(&UtraRobot::ThreadFunction_UpdateRobotStatus, this);
}

UtraRobot::~UtraRobot()
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

bool UtraRobot::InitRobot()
{
    if(is_robot_connected)
    {
        return true;
    }
    {
        std::unique_lock lck{mtx_connect_};
        if(nullptr != ubot_)
        {
            delete ubot_;
        }
        ubot_ = new UtraApiTcp(config_.ip.data());
        is_robot_connected = !ubot_->is_error();

        if(!is_robot_connected)
        {
            std::cout << __FUNCTION__ << " UtraApiTcp connection failed" << std::endl;
            return false;
        }

        if(nullptr != utra_report_)
        {
            delete utra_report_;
        }
        utra_report_ = new UtraReportStatus100Hz(config_.ip.data(), 7);
        if(utra_report_->is_error())
        {
            std::cout << __FUNCTION__ << " UtraReportStatus100Hz connection failed" << std::endl;;
            return false;
        }
        cv_connect_.notify_all();
    }
    std::cout << __FUNCTION__ << " UtApi Initialized!!!" << std::endl;;
    fflush(nullptr);
    int retry_cnt=4;
    while((!this->RobotCommand_ResetError()) && (retry_cnt > 0))
    {
        std::cout << __FUNCTION__ << " RobotCommand_ResetError failed, try " << (retry_cnt--) << std::endl;
        sleep(1);
    }
    fflush(nullptr);
    uint8_t uuid[18] = {0};
    uint8_t version_sw[21] = {0};
    uint8_t version_hw[21] = {0};

    int ret{-1};
    //initilize after reboot robot cause the first api call failed
    //here feed one request without caring the api result
    ubot_->get_uuid(uuid);
    sleep(1);
    ret = ubot_->get_uuid(uuid);
    if (ret)
    {
        uint8_t code;
        int ret_err = ubot_->get_error_code(&code);
        printf("Failed to get uuid, Error info error info[%d] code[%d]!\r\n", ret_err, (uint16_t)code);
        fflush(nullptr);
        return false;
    }
    std::string str_uuid{uuid, uuid + (sizeof uuid)/(sizeof uuid[0]) - 1};

    ret = ubot_->get_sw_version(version_sw);
    if (ret)
    {
        uint8_t code;
        int ret_err = ubot_->get_error_code(&code);
        printf("Failed to get software version, Error info error info[%d] code[%d]!\r\n", ret_err, (uint16_t)code);
        return false;
    }
    std::string str_version_sw{version_sw, version_sw + (sizeof version_sw)/(sizeof version_sw[0]) - 1};
    str_version_sw = str_version_sw.substr(0,10);//prune the last 10 character which contains the time stamp

    ret = ubot_->get_hw_version(version_hw);
    if (ret)
    {
        uint8_t code;
        int ret_err = ubot_->get_error_code(&code);
        printf("Failed to get hardware version, Error info error info[%d] code[%d]!\r\n", ret_err, (uint16_t)code);
        return false;
    }
    std::string str_version_hw{version_hw, version_hw + (sizeof version_hw)/(sizeof version_hw[0]) -1};

    // bool hold_result = this->RobotCommand_Hold();
    // std::this_thread::sleep_for(std::chrono::seconds(1));

    config_.identity_Info.UUID          = str_uuid;
    config_.identity_Info.VERSION_SW    = str_version_sw;
    config_.identity_Info.VERSION_HW    = str_version_hw;
    printf("Config ip {%s}\r\n", config_.ip.c_str());
    fflush(nullptr);
    return true;
}

bool UtraRobot::getJointPos(UtRobotConfig::JointPos &jPos)
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

bool UtraRobot::getTcpPos(UtRobotConfig::CartesianPos &pose)
{
    pose[0] = this->rx_data_.pose[0];
    pose[1] = this->rx_data_.pose[1];
    pose[2] = this->rx_data_.pose[2];
    pose[3] = this->rx_data_.pose[3];
    pose[4] = this->rx_data_.pose[4];
    pose[5] = this->rx_data_.pose[5];
    return true;
}

bool UtraRobot::RobotTest_Gravity()
{
    if(!this->is_robot_connected)
    {
        std::cout << __FUNCTION__ << " - connect robot before call this function!";
        return false;
    }
    return RobotCommand_JointPtp(config_.gravityTest.target_JointPos);
}

bool UtraRobot::RobotTest_Accuracy()
{
    if(!this->is_robot_connected)
    {
        std::cout << __FUNCTION__ << " - connect robot before call this function!";
        return false;
    }
    float zero_tcp_offset[6]{0};
    if(!Robot_Set_Param_Tcp_Offset(zero_tcp_offset))
    {
        std::cout << __FUNCTION__ << " - failed to zero out tcp offset!";
        return false;
    }
    if(!RobotCommand_JointPtp(config_.accuracyTest.initial_JointPose))
    {
        std::cout << __FUNCTION__ << " inital joint position move failed!" << std::endl;
        return false;
    }
    std::array<float, 6> cartesian_pose_target;
    memcpy(cartesian_pose_target.data(), this->rx_data_.pose, sizeof(cartesian_pose_target));

    for(unsigned long i=0; (sizeof(config_.accuracyTest.mid_CartPoses)/sizeof(config_.accuracyTest.mid_CartPoses[0])) > i; i++)
    {
        memcpy(cartesian_pose_target.data(), config_.accuracyTest.mid_CartPoses[i].data(), sizeof(float)*3);
        if(!RobotCommand_CartesianLine(cartesian_pose_target))
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

bool UtraRobot::RobotTest_Workspace()
{
    if(!this->is_robot_connected)
    {
        std::cout << __FUNCTION__ << " - connect robot before call this function!";
        return false;
    }
    float zero_tcp_offset[6]{0};
    if(!Robot_Set_Param_Tcp_Offset(zero_tcp_offset))
    {
        std::cout << __FUNCTION__ << " - failed to zero out tcp offset!";
        return false;
    }
    if(!RobotCommand_JointPtp(config_.workspaceTest.initial_JointPose))
    {
        std::cout << __FUNCTION__ << " inital joint position move failed!" << std::endl;
        return false;
    }
    std::array<float, 6> cartesian_pose_target;
    memcpy(cartesian_pose_target.data(), this->rx_data_.pose, sizeof(cartesian_pose_target));

    for(unsigned long i=0; (sizeof(config_.workspaceTest.mid_CartPoses)/sizeof(config_.workspaceTest.mid_CartPoses[0])) > i; i++)
    {
        memcpy(cartesian_pose_target.data(), config_.workspaceTest.mid_CartPoses[i].data(), sizeof(float)*3);
        if(!RobotCommand_CartesianLine(cartesian_pose_target))
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

bool UtraRobot::RobotTest_Repeat()
{
    if(!this->is_robot_connected)
    {
        std::cout << __FUNCTION__ << " - connect robot before call this function!";
        return false;
    }
    float zero_tcp_offset[6]{0};
    if(!Robot_Set_Param_Tcp_Offset(zero_tcp_offset))
    {
        std::cout << __FUNCTION__ << " - failed to zero out tcp offset!";
        return false;
    }
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

bool UtraRobot::RobotCommand_JointPtp(UtRobotConfig::JointPos jPos, int timeout_ms)
{
    if(!this->is_robot_connected)
    {
        std::cout << __FUNCTION__ << " - connect robot before call this function!";
        return false;
    }
    std::lock_guard lck_exclusive{this->mtx_exclusivity_command_};
    ubot_->reset_err();
    std::unique_lock lck{this->mtx_robotState_};
    if(((uint8_t)(UtrRobotStatus::Standby) != this->robotState_.motion_status)
        || (this->isDragging) || this->cmd_stop)
    {
        printf("%s - utr robot is not standy, motion status{%d}, isDragging{%d}, cmd_stop{%d}\r\n",
               __FUNCTION__, this->robotState_.motion_status, this->isDragging, this->cmd_stop);
        this->cmd_stop = false;
        return false;
    }
    this->cmd_stop = false;
    auto timestamp_timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(timeout_ms);
    lck.unlock();

    printf("%s - target pos {%.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f}\r\n",__FUNCTION__,
           jPos[0], jPos[1], jPos[2], jPos[3], jPos[4], jPos[5], jPos[6]);
    if(ROBOT_NO_ERROR != ubot_->moveto_joint_p2p(jPos.data(), speed, acc, 0))
    {
        std::cout << __FUNCTION__ << " utr robot API 'moveto_joint_p2p' failed!"<<std::endl;
        Print_Error_Info();
        return false;
    }

    lck.lock();
    bool waitStartMove_Success = this->cv_robotState_.wait_for(lck, std::chrono::milliseconds(TIMEOUT_MS_ROBOT_START_MOV),
                                                               [this](){return this->Predicate_Robot_Moving_Without_Error();
                                                               });
    if(!waitStartMove_Success)
    {
        std::cout << __FUNCTION__ << " utr robot start move timeout!"<<std::endl;
        Print_Error_Info();
        return false;
    }
    std::cout << __FUNCTION__ << " utr robot start to move!"<<std::endl;
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
        Print_Error_Info();
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

bool UtraRobot::RobotCommand_CartesianLine(UtRobotConfig::CartesianPos Pos, int timeout_ms)
{
    if(!this->is_robot_connected)
    {
        std::cout << __FUNCTION__ << " - connect robot before call this function!";
        return false;
    }
    std::lock_guard lck_exclusive{this->mtx_exclusivity_command_};
    ubot_->reset_err();
    std::unique_lock lck{this->mtx_robotState_};
    if(((uint8_t)(UtrRobotStatus::Standby) != this->robotState_.motion_status)
        || (this->isDragging) || this->cmd_stop)
    {
        printf("%s - utr robot is not standy, motion status{%d}, isDragging{%d}, cmd_stop{%d}\r\n",
               __FUNCTION__, this->robotState_.motion_status, this->isDragging, this->cmd_stop);
        this->cmd_stop = false;
        return false;
    }
    this->cmd_stop = false;
    auto timestamp_timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(timeout_ms);
    lck.unlock();
    printf("%s - target cartesian pos {%.4f, %.4f, %.4f, %.4f, %.4f, %.4f}\r\n",__FUNCTION__,
           Pos[0], Pos[1], Pos[2], Pos[3], Pos[4], Pos[5]);
    if(ROBOT_NO_ERROR != ubot_->moveto_cartesian_line(Pos.data(), mvvelo, mvacc, 0))
    {
        std::cout << __FUNCTION__ << " utr robot API 'moveto_cartesian_line' failed!"<<std::endl;
        Print_Error_Info();
        return false;
    }

    lck.lock();
    bool waitStartMove_Success = this->cv_robotState_.wait_for(lck, std::chrono::milliseconds(TIMEOUT_MS_ROBOT_START_MOV),
                                                               [this](){return this->Predicate_Robot_Moving_Without_Error();});
    if(!waitStartMove_Success)
    {
        std::cout << __FUNCTION__ << " utr robot start move timeout!"<<std::endl;
        return false;
    }
    std::cout << __FUNCTION__ << " utr robot start to move!"<<std::endl;
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
        Print_Error_Info();
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

bool UtraRobot::RobotCommand_EnterTeachMode()
{
    if(!this->is_robot_connected)
    {
        std::cout << __FUNCTION__ << " - connect robot before call this function!";
        return false;
    }
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

bool UtraRobot::RobotCommand_Hold()
{
    if(!this->is_robot_connected)
    {
        std::cout << __FUNCTION__ << " - connect robot before call this function!";
        return false;
    }
    std::unique_lock lck{this->mtx_robotState_};
    this->cmd_stop = true;
    this->cv_robotState_.notify_all();
    lck.unlock();
    if((uint8_t)(UtrRobotMode::TeachMode)== this->robotState_.motion_mode)//
    {
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
        std::this_thread::sleep_for(std::chrono::milliseconds(DALAY_MS_TEACH_TO_POSITION_MODE));
        if(ROBOT_NO_ERROR != ubot_->set_motion_enable(8, 1))
        {
            std::cout << __FUNCTION__ << " utr robot start enable failed!" <<std::endl;
            Print_Error_Info();
            return false;
        }
        if(ROBOT_NO_ERROR != ubot_->set_motion_status(0))
        {
            std::cout << __FUNCTION__ << " utr robot reset motion status failed!"<<std::endl;
            Print_Error_Info();
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
    }
    else if((uint8_t)(UtrRobotMode::PositionMode) == this->robotState_.motion_mode)//
    {
        if(127 != this->rx_data_.mt_able)
        {
            if(ROBOT_NO_ERROR != ubot_->set_motion_enable(8, 1))
            {
                std::cout << __FUNCTION__ << " utr robot call API 'set_motion_enable(8, 1)' failed!"<<std::endl;
                Print_Error_Info();
                return false;
            }
        }

        if(ROBOT_NO_ERROR != ubot_->set_motion_status(4))  //set pause
        {
            std::cout << __FUNCTION__ << " utr robot call API 'set_motion_status(4)' failed!"<<std::endl;
            Print_Error_Info();
            return false;
        }
        lck.lock();
        bool waiStatusPausing_Success = this->cv_robotState_.wait_for(lck, std::chrono::seconds(5), [this](){
            return ((uint8_t)(ArmStatus::PAUSING) == this->robotState_.motion_mode)
                   || ((uint8_t)(UtrRobotStatus::Stopping) == this->robotState_.motion_status);//
        });
        if(!waiStatusPausing_Success)
        {
            std::cout << __FUNCTION__ << " utr robot wait motion mode toggled to '0' timeout"<<std::endl;
            return false;
        }
        if(ROBOT_NO_ERROR != ubot_->set_motion_status(0))  //set pause
        {
            std::cout << __FUNCTION__ << " utr robot call API 'set_motion_status(4)' failed!"<<std::endl;
            Print_Error_Info();
            return false;
        }
        bool waitInitialize_Success = this->cv_robotState_.wait_for(lck, std::chrono::milliseconds(TIMEOUT_MS_ROBOT_START_MOV), [this](){
            return ((uint8_t)(UtrRobotStatus::Initilizing) == this->robotState_.motion_status);
        });
        if(!waitInitialize_Success)
        {
            std::cout << __FUNCTION__ << " utr robot wait initilized after enable timeout"<<std::endl;
            return false;
        }
        bool waitReStandby_Success = this->cv_robotState_.wait_for(lck, std::chrono::milliseconds(TIMEOUT_MS_ROBOT_START_MOV), [this](){
            return ((uint8_t)(UtrRobotStatus::Standby) == this->robotState_.motion_status);
        });
        if(!waitReStandby_Success)
        {
            std::cout << __FUNCTION__ << " utr robot wait re-standby after enable timeout"<<std::endl;
            return false;
        }
    }
    else
    {
        printf("%s - current robot not in teach or position mode!\r\n", __FUNCTION__);
        return false;
    }

    printf("set_into_motion_mode position hold\n");
    this->isDragging = false;
    this->cmd_stop = false;
    return true;
}

bool UtraRobot::RobotCommand_ResetError()
{
    if(!this->is_robot_connected)
    {
        std::cout << __FUNCTION__ << " - connect robot before call this function!";
        return false;
    }
    int ret{-1};
    ret = ubot_->reset_err();
    std::cout << __FUNCTION__ << " finish reset error, current error code: " << (uint16_t)this->robotState_.err_code <<std::endl;
    printf("%s - reset_err   : %d\n", __FUNCTION__, ret);
    ret = ubot_->set_motion_mode(0);
    printf("%s - set_motion_mode   : %d\n", __FUNCTION__, ret);
    ret = ubot_->set_motion_enable(8, 1);
    printf("%s - set_motion_enable : %d\n", __FUNCTION__, ret);
    ret = ubot_->set_motion_status(0);
    printf("%s - set_motion_status : %d\n", __FUNCTION__, ret);
    std::unique_lock lck{this->mtx_robotState_};
    bool waitNoError_Success = this->cv_robotState_.wait_for(lck, std::chrono::seconds(1), [this](){
        return (ROBOT_NO_ERROR == this->robotState_.err_code);
    });
    if(!waitNoError_Success)
    {
        std::cout << __FUNCTION__ << " utr robot wait error clear out timeout," << (uint16_t)this->robotState_.err_code <<std::endl;
        return false;
    }
    return true;
}

bool UtraRobot::Robot_Set_Mdh_Offset_With_Calib_Result()
{
    UtRobotConfig::RobotDhModel_UtraRobot mdh_offset_struct;

    for(int i=0; i<7; i++)
    {
        mdh_offset_struct[i].a_Trans    = this->config_.mdh_model_calib[i].a_Trans   - this->config_.mdh_model_origin[i].a_Trans;
        mdh_offset_struct[i].d_Trans    = this->config_.mdh_model_calib[i].d_Trans   - this->config_.mdh_model_origin[i].d_Trans;
        mdh_offset_struct[i].alpha_Rot  = this->config_.mdh_model_calib[i].alpha_Rot - this->config_.mdh_model_origin[i].alpha_Rot;
        mdh_offset_struct[i].theta_Rot  = this->config_.mdh_model_calib[i].theta_Rot - this->config_.mdh_model_origin[i].theta_Rot;
    }
    std::array<std::array<float,4>,7> mdh_offset_array;
    UtRobotConfig::Convertion_RobotDhModel_Struct_To_Array(mdh_offset_struct, mdh_offset_array);
    int ret = 0;
    bool all_joint_updated = true;
    for(int i=0; i<7; i++)
    {
        ret = ubot_->set_dh_offset(i + 1, mdh_offset_array[i].data());
        if (0 == ret)
        {
            std::cout << __FUNCTION__ << " - succeed mdh offset update for joint " << (i+1) << std::endl;
        }
        else
        {
            std::cout << __FUNCTION__ << " - failed to update mdh offset for joint " << (i+1) << std::endl;
            all_joint_updated = false;
        }
    }
    if(!all_joint_updated)
    {
        std::cout << __FUNCTION__ << " - failed to update mdh param of some joint " << std::endl;
        return false;
    }
    sleep(2);
    ret = ubot_->saved_parm();
    if(0 != ret)
    {
        std::cout << __FUNCTION__ << " - Robot Failed to save param after update mdh offset!" << std::endl;
        uint8_t code;
        int ret_err = ubot_->get_error_code(&code);
        printf("Error info error info[%d] code[%d]!\r\n", ret_err, (uint16_t)code);
        return false;
    }
    return true;
}

bool UtraRobot::Robot_Check_Mdh_Offset_Are_All_Zero()
{
    if(!RobotCommand_Hold())
    {
        std::cout << __FUNCTION__ << " - failed to hold robot " << std::endl;
    }
    std::array<std::array<float,4>,7> mdh_offset_array;
    int ret = 0;
    bool all_joint_mdh_zero = true;
    float Template_Zero_Arr[4] = {0};
    for(int i=0; i<7; i++)
    {
        ret = ubot_->get_dh_offset(i + 1, &mdh_offset_array[i][0]);
        if (0 == ret)
        {
            // std::cout << __FUNCTION__ << " - succeed mdh offset update for joint " << (i+1) << std::endl;
            if(memcmp(&mdh_offset_array[i][0], Template_Zero_Arr, sizeof((Template_Zero_Arr))))
            {
                // std::cout << __FUNCTION__ << " - mdh offset update for joint " << (i+1) << " is not zero" << std::endl;
                printf("%s - Robot mdh offset for [%d]th axis is {%f, %f, %f, %f}\r\n"
                       , __FUNCTION__, (i+1), mdh_offset_array[i][0], mdh_offset_array[i][1], mdh_offset_array[i][2], mdh_offset_array[i][3]);
                all_joint_mdh_zero = false;
            }
            else
            {
                std::cout << __FUNCTION__ << " - mdh offset update for joint " << (i+1) << " is zeror" << std::endl;
            }
        }
        else
        {
            std::cout << __FUNCTION__ << " - failed to update mdh offset for joint " << (i+1) << std::endl;
            all_joint_mdh_zero = false;
        }
    }

    return all_joint_mdh_zero;
}

bool UtraRobot::Robot_ZeroOut_Mdh_offset()
{
    std::array<std::array<float,4>,7> mdh_offset_array={{{0.0}, {0.0}, {0.0}, {0.0}, {0.0}, {0.0}, {0.0}}};
    int ret = 0;
    bool all_joint_updated = true;
    for(int i=0; i<7; i++)
    {
        ret = ubot_->set_dh_offset(i + 1, mdh_offset_array[i].data());
        if (0 == ret)
        {
            std::cout << __FUNCTION__ << " - succeed mdh offset update for joint " << (i+1) << std::endl;
        }
        else
        {
            std::cout << __FUNCTION__ << " - failed to update mdh offset for joint " << (i+1) << std::endl;
            all_joint_updated = false;
            uint8_t code;
            int ret_err = ubot_->get_error_code(&code);
            printf("Error info error info[%d] code[%d]!\r\n", ret_err, (uint16_t)code);
        }
    }
    if(!all_joint_updated)
    {
        std::cout << __FUNCTION__ << " - failed to update mdh param of some joint " << std::endl;
        return false;
    }
    sleep(2);
    ret = ubot_->saved_parm();
    if(0 != ret)
    {
        std::cout << __FUNCTION__ << " - Robot Failed to save param after update mdh offset!" << std::endl;
        uint8_t code;
        int ret_err = ubot_->get_error_code(&code);
        printf("Error info error info[%d] code[%d]!\r\n", ret_err, (uint16_t)code);
        return false;
    }
    return true;
}

bool UtraRobot::Robot_Get_Param_Gravity_Direct(float gravity_direction[3])
{
    int ret = ubot_->get_gravity_dir(gravity_direction);
    if (0 != ret)
    {
        std::cout << __FUNCTION__ << " - Robot Failed to Get Parameter Gravity Direction!" << std::endl;
        return false;
    }
    printf("%s - Robot Gravity Direction {%0.4f}, {%0.4f}, {%0.4f}\r\n"
           , __FUNCTION__, gravity_direction[0], gravity_direction[1], gravity_direction[2]);
    return true;
}

bool UtraRobot::Robot_Get_Param_Tcp_Offset(float tcp_over_flange[6])
{
    int ret = ubot_->get_tcp_offset(tcp_over_flange);
    if (0 != ret)
    {
        std::cout << __FUNCTION__ << " - Robot Failed to Get Parameter Gravity Direction!" << std::endl;
        return false;
    }
    printf("%s - Robot tcp offset {%f}, {%f}, {%f}, {%f}, {%f}, {%f}\r\n"
           , __FUNCTION__, tcp_over_flange[0], tcp_over_flange[1], tcp_over_flange[2]
           , tcp_over_flange[3], tcp_over_flange[4], tcp_over_flange[5]);
    return true;
}

bool UtraRobot::Robot_Set_Param_Tcp_Offset(float tcp_over_flange[6])
{
    int ret = ubot_->set_tcp_offset(tcp_over_flange);
    if (0 != ret)
    {
        std::cout << __FUNCTION__ << " - Robot Failed to Get Parameter Gravity Direction!" << std::endl;
        return false;
    }
    return true;
}

bool UtraRobot::Robot_Get_Param_Tcp_Load(float tcp_load[4])
{
    printf("Config ip {%s}\r\n", config_.ip.c_str());
    fflush(nullptr);
    int ret = ubot_->get_tcp_load(tcp_load);
    if (0 != ret)
    {
        std::cout << __FUNCTION__ << " - Robot Failed to Get Tcp Load!" << std::endl;
        return false;
    }
    printf("%s - Robot tcp load {%0.3f}kg, {%0.4f}mm, {%0.4f}mm, {%0.4f}mm\r\n"
           , __FUNCTION__, tcp_load[0], tcp_load[1], tcp_load[2], tcp_load[3]);
    fflush(nullptr);
    return true;
}

bool UtraRobot::Robot_Reboot()
{

    if(!this->is_robot_connected)
    {
        std::cout << __FUNCTION__ << " - connect robot before call this function!";
        return false;
    }
    if(!RobotCommand_Hold())
    {
        std::cout << __FUNCTION__ << " - Robot Failed to Hold!" << std::endl;
    }
    int ret = ubot_->reboot_system();
    if(0 != ret)
    {
        std::cout << __FUNCTION__ << " - failed to call api 'reboot_system'!" << std::endl;
        return false;
    }
    {
        std::unique_lock lck{mtx_connect_};
        if(nullptr != ubot_)
        {
            delete ubot_;
            ubot_ = nullptr;
        }
        if(nullptr != utra_report_)
        {
            delete utra_report_;
            utra_report_ = nullptr;
        }

        is_robot_connected = false;
        cv_connect_.notify_all();
    }
    return true;
}

bool UtraRobot::Robot_Wait_Until_Disconnected(int timeout_sec)
{
    printf("%s - start wait disconnect!\r\n", config_.ip.c_str());
    fflush(nullptr);
    int sockfd;
    struct sockaddr_in server_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("%s - Socket creation failed\r\n", __FUNCTION__);
        return false;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UTR_ROBOT_SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(config_.ip.c_str());

    int cnt = timeout_sec;
    while (cnt) {
        printf("Attempting [%d] wait disconnect of %s:%d\n", --cnt, config_.ip.c_str(), UTR_ROBOT_SERVER_PORT);
        fflush(nullptr);
        if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
            // printf("Connected successfully!\n");
            close(sockfd);
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
        } else {
            printf("%s - wait disconnection success\r\n", __FUNCTION__);
            fflush(nullptr);
            return true;
        }
        sleep(1); // 等待1秒后重试
    }
    close(sockfd);
    return false;
}

bool UtraRobot::Robot_Wait_Until_Connected(int timeout_sec)
{
    printf("%s - start wait reconnect!\r\n", config_.ip.c_str());
    fflush(nullptr);
    int sockfd;
    struct sockaddr_in server_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("%s - Socket creation failed\r\n", __FUNCTION__);
        return false;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UTR_ROBOT_SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(config_.ip.c_str());
    int cnt = timeout_sec;
    while (cnt) {
        printf("Attempting [%d] wait reconnect of %s:%d\n", --cnt, config_.ip.c_str(), UTR_ROBOT_SERVER_PORT);
        fflush(nullptr);
        if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0)
        {
            printf("%s - wait Connected successfully!\n", __FUNCTION__);
            fflush(nullptr);
            close(sockfd);
            return true;
        }
        sleep(1); // 等待1秒后重试
    }
    close(sockfd);
    sleep(4);
    return false;
}

bool UtraRobot::Robot_Enable_Single_Axis(int axis_idex, bool enalbe)
{
    if(axis_idex > 7 || axis_idex < 1)
    {
        std::cout << __FUNCTION__ << " - given axis_index not within range [1,7]: " << axis_idex << std::endl;
        return false;
    }

    if(!this->is_robot_connected)
    {
        std::cout << __FUNCTION__ << " - connect robot before call this function!";
        return false;
    }
    std::lock_guard lck_exclusive{this->mtx_exclusivity_command_};

    if(ROBOT_NO_ERROR != ubot_->set_motion_enable(8, 1))
    {
        std::cout << __FUNCTION__ << " utr robot start enable failed!"<<std::endl;
        return false;
    }
    std::unique_lock lck{this->mtx_robotState_};
    bool waitAxisEnable_Success = this->cv_robotState_.wait_for(lck, std::chrono::seconds(1), [this, axis_idex](){
        return (this->rx_data_.mt_able & (0b1 << (axis_idex -1)));
    });
    if(!waitAxisEnable_Success)
    {
        std::cout << __FUNCTION__ << " utr robot wait error clear out timeout," << (uint16_t)this->robotState_.err_code <<std::endl;
        return false;
    }
    return true;
}

void UtraRobot::set_speed_override(double percent)
{
    this->speed =JOIN_MOVE_DEFAULT_SPD * percent/100.0;  // rad/s
    this->acc   =JOIN_MOVE_DEFAULT_ACC * percent/100.0;
    this->mvvelo=CART_MOVE_DEFAULT_VEL * percent/100.0;   // mm/s
    this->mvacc =CART_MOVE_DEFAULT_ACC * percent/100.0;  // mm/s
}

uint32_t UtraRobot::get_enables_status()
{
    return this->robotState_.mt_brake;
}

uint32_t UtraRobot::get_brake_status()
{
    return this->robotState_.mt_able;
}

void UtraRobot::ThreadFunction_UpdateRobotStatus()
{
    while(this->thread_flag)
    {
        {
            std::unique_lock lck{mtx_connect_};
            cv_connect_.wait(lck,[this](){return (nullptr != this->ubot_) && (nullptr != this->utra_report_);});
        }

        RobotStatus_SnapShot prev_state = this->robotState_;
        if (utra_report_->is_update()) {
            utra_report_data_missing_counter_ = 0;
            utra_report_->get_data(&rx_data_);
            this->robotState_.motion_status = rx_data_.motion_status;
            this->robotState_.motion_mode   = rx_data_.motion_mode;
            this->robotState_.err_code      = rx_data_.err_code;
            this->robotState_.war_code      = rx_data_.war_code;
            this->robotState_.mt_brake      = rx_data_.mt_brake;
            this->robotState_.mt_able       = rx_data_.mt_able;

            if ((this->robotState_.motion_status ^ prev_state.motion_status) ||
                (this->robotState_.motion_mode   ^ prev_state.motion_mode) ||
                (this->robotState_.err_code      ^ prev_state.err_code) ||
                (this->robotState_.war_code      ^ prev_state.war_code) ||
                (this->robotState_.mt_brake      ^ prev_state.mt_brake) ||
                (this->robotState_.mt_able      ^ prev_state.mt_able)
                ) {

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

                if(this->robotState_.mt_brake ^ prev_state.mt_brake)
                {
                    printf("brak status changed from {%04X}->{%04X}\r\n",  prev_state.mt_brake, this->robotState_.mt_brake);
                }

                if(this->robotState_.mt_able ^ prev_state.mt_able)
                {
                    printf("enable status code changed from {%04X}->{%04X}\r\n",  prev_state.mt_able, this->robotState_.mt_able);
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


bool UtraRobot::Predicate_Robot_Standby_Or_Error()
{
    return ((uint8_t)(UtrRobotStatus::Standby) == this->robotState_.motion_status)
           || (ROBOT_NO_ERROR != this->robotState_.err_code);
}

bool UtraRobot::Predicate_Robot_Moving_Without_Error()
{
    return ((uint8_t)(UtrRobotStatus::Moving) == this->robotState_.motion_status)
           && (ROBOT_NO_ERROR == this->robotState_.err_code);
}

void UtraRobot::Print_Error_Info()
{
    uint8_t err_code[2];
    int result =  ubot_->get_error_code(err_code);
    printf("Robot error error code {%d} warning code {%d}, api return code {%d}\r\n", err_code[0], err_code[1], result);
}
