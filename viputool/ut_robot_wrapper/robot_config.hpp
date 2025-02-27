#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

#include <stdint.h>
#include <array>
#include <string>

#define DEGREE_TO_RAD  (M_PI/180.0)

namespace UtRobotConfig
{

    struct EndEffector_WorkSettings {
        int work_mode;        // 工作模式
        int slide_depth;      // 滑动深度
        int slide_velocity;   // 滑动速度
        int spin_velocity;     // 旋转速度
        int spin_revolution;   // 旋转圈数
        int hold_millisecond;  // 持续时间（毫秒）
    };

    typedef std::array<float, 6> CartesianPos;
    typedef std::array<float, 7> JointPos;

    struct RepeatTest_Config
    {
        JointPos initial_JointPose = {{0}};

        CartesianPos mid_CartPoses[8] = {{0}};

        JointPos final_JointPose = {{0}};
    };

    struct AccuracyTest_Config
    {
        JointPos initial_JointPose = {{0}};

        CartesianPos mid_CartPoses[8] = {{0}};

        JointPos final_JointPose = {{0}};
    };

    struct WorkspaceTest_Config
    {
        JointPos initial_JointPose = {{0}};

        CartesianPos mid_CartPoses[8] = {{0}};

        JointPos final_JointPose = {{0}};
    };

    struct GravityTest_Config
    {
        JointPos target_JointPos = {{0}};
    };

    struct Identity_Info{
        std::string UUID{""};
        std::string VERSION_SW{""};
        std::string VERSION_HW{""};
    };
    extern Identity_Info Id_INFO_left;
    extern Identity_Info Id_INFO_right;

    void update_robot_identity_info_left(std::string& uuid, std::string& version_sw, std::string& version_hw);
    void update_robot_identity_info_right(std::string& uuid, std::string& version_sw, std::string& version_hw);


    // struct MDH_Param_Arr{
    //     float   theta_Rot;//radian
    //     float   d_Trans;//m
    //     float   alpha_Rot;//radian
    //     float   a_Trans;//m
    // };
    //MDH: Modified Denavit–Hartenberg parameters
    struct MDH_Param_Struct{
        float   alpha_Rot;//radian
        float   a_Trans;//mm
        float   d_Trans;//mm
        float   theta_Rot;//radian
    };

    typedef std::array<MDH_Param_Struct, 7> RobotDhModel_UtraRobot;
    // extern RobotDhModel_UtraRobot RobotDhModel_UtrRobot_left;
    // extern RobotDhModel_UtraRobot RobotDhModel_UtrRobot_right;

    struct TestConfig{
        std::string             ip;
        Identity_Info           identity_Info;
        RobotDhModel_UtraRobot  mdh_model_origin;
        RobotDhModel_UtraRobot  mdh_model_calib;
        RepeatTest_Config       repeatTest;
        AccuracyTest_Config     accuracyTest;
        WorkspaceTest_Config    workspaceTest;
        GravityTest_Config      gravityTest;

    };

    extern TestConfig TestConfig_RobotLeft;
    extern TestConfig TestConfig_RobotRight;

    // //mdh array mainly used for utr robot api, mdh struct mainly used for calculation
    void Convertion_RobotDhModel_Array_To_Struct(std::array<std::array<float,4>,7>& mdh_arr, RobotDhModel_UtraRobot & mdh_model);
    void Convertion_RobotDhModel_Struct_To_Array(RobotDhModel_UtraRobot & mdh_model, std::array<std::array<float,4>,7>& mdh_arr);

}

// left arm
// tcp_offset[0]: 92.3950424
// tcp_offset[1]: 26.5216942
// tcp_offset[2]: 283.470001
// tcp_offset[3]: -3.09154582
// tcp_offset[4]: 1.53642249
// tcp_offset[5]: -2.84260941
// tcp_load[0]: 1.29999995
// tcp_load[1]: 0.699999988
// tcp_load[2]: -4
// tcp_load[3]: 104.800003
// gravity_dir[0]: 0.499998033
// gravity_dir[1]: -0
// gravity_dir[2]: -0.86602658

// right arm
// tcp_offset[0]: 211
// tcp_offset[1]: -115
// tcp_offset[2]: 188
// tcp_offset[3]: -0.268293202
// tcp_offset[4]: -1.56629062
// tcp_offset[5]: 2.90501571
// tcp_load[0]: 1.29999995
// tcp_load[1]: 2.5
// tcp_load[2]: 39.2999992
// tcp_load[3]: 118
// gravity_dir[0]: 0.499998033
// gravity_dir[1]: -0
// gravity_dir[2]: -0.86602658


#endif //ROBOT_CONFIG
