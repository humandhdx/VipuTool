#ifndef UTRAROBOT_H
#define UTRAROBOT_H

#include "ut_robot_wrapper/robot_config.hpp"
#include "utra/utra_api_tcp.h"
#include "utra/utra_report_config.h"
#include "utra/utra_report_status.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class UtraRobot
{
public:
    UtraRobot(UtRobotConfig::TestConfig& config);

    ~UtraRobot();
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
    bool InitRobot();

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

    bool Robot_Set_Mdh_Offset();

    bool Robot_Check_Mdh_Offset_Are_All_Zero();

    bool Robot_ZeroOut_Mdh_offset();

    bool Robot_Get_Param_Gravity_Direct(float gravity_direction[3]);

    bool Robot_Get_Param_Tcp_Offset(float tcp_over_flange[6]);

    bool Robot_Set_Param_Tcp_Offset(float tcp_over_flange[6]);

    // [mass{kg}, center_x[mm], center_y[mm], center_z[mm]]
    bool Robot_Get_Param_Tcp_Load(float tcp_load[4]);

    //axis_idex start from 1
    bool Robot_Enable_Single_Axis(int axis_idex, bool enalbe);

    uint32_t get_enables_status();
    uint32_t get_brake_status();

protected:
    bool is_robot_connected{false};

protected:
    UtRobotConfig::TestConfig& config_;

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
        uint32_t    mt_brake;
        uint32_t    mt_able;
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

    void Print_Error_Info();

};

#endif // UTRAROBOT_H
