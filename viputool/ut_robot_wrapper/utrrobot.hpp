#ifndef UTRROBOT_HPP
#define UTRROBOT_HPP

#include "ut_robot_wrapper/robot_config.hpp"
#include "utra/utra_api_tcp.h"
#include "utra/utra_report_config.h"
#include "utra/utra_report_status.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class UtrRobot
{
public:
    UtrRobot(UtRobotConfig::TestConfig config);

    ~UtrRobot();
    struct JointPos{
        double data[7];
    };

    enum class UtrRobotMode{
        PositionMode = 0,
        TeachMode   =  2,
    };

    enum class UtrRobotStatus:uint8_t{
        Standby     = 2,
        Moving      = 1,
        Pausing     = 3,
        Stopping    = 4,
        Initilizing = 0,
        OTHER       = 0xFF
    };

    enum class UtrRobotCommand{
        Standby     = 2,
        Moving      = 1,
        Pausing     = 3,
        Stopping    = 4,
    };

    enum class ArmStatus {
        HOLD        = 0,
        InMOTION    = 1,
        ADJUST      = 2,
        PAUSING     = 3,
        STOPPING    = 4,
        // STANDY      = 0xF0,
        OTHER       = 0xFFFF
    };

    bool getJointPos(UtRobotConfig::JointPos &jPos);
    bool getTcpPos(UtRobotConfig::CartesianPos &pose);

    bool RobotTest_Gravity();
    bool RobotTest_Accuracy();
    bool RobotTest_Workspace();
    bool RobotTest_Repeat();

    bool RobotCommand_JointPtp(UtRobotConfig::JointPos jPos, int timeout_ms=30000);
    bool RobotCommand_CartesianLine(UtRobotConfig::CartesianPos Pos, int timeout_ms=30000);
    bool RobotCommand_EnterTeachMode();
    bool RobotCommand_Hold();
    bool RobotCommand_ResetError();

    bool Robot_Set_ToolCenterlPoint();

protected:
    bool is_robot_connected{false};

private:
    UtRobotConfig::TestConfig config_;

private:
    UtraApiTcp* ubot_;
    UtraReportStatus100Hz* utra_report_;
    int utra_report_data_missing_counter_;
    arm_report_status_t rx_data_;
    std::thread thd_refresh_robot_status_;
    bool thread_flag=true;

    struct RobotStatus_SnapShot{
        bool        comm_error{false};
        uint8_t     motion_status{0};
        uint8_t     motion_mode{0};
        uint8_t     err_code{0};
        uint8_t     war_code{0};
    };

    float speed=5.0/57.54;  // rad/s
    float acc=3.0;
    float mvvelo=50.0;   // mm/s
    float mvacc=1000;  // mm/s
    float mvtime=0;

    std::mutex              mtx_exclusivity_command_;

    RobotStatus_SnapShot    robotState_;
    std::mutex              mtx_robotState_;
    std::condition_variable cv_robotState_;

    bool isDragging{false};
    bool cmd_stop{false};

    // bool error_missing_reportMsg{false};
    void ThreadFunction_UpdateRobotStatus();

    bool Predicate_Robot_Standby_Or_Error();

    bool Predicate_Robot_Moving_Without_Error();

};

#endif // UTRROBOT_HPP
