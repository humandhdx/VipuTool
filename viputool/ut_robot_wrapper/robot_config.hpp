#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

#include <stdint.h>
#include <array>
#include <math.h>
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

    struct TestConfig{
        std::string             ip;
        RepeatTest_Config       repeatTest;
        AccuracyTest_Config     accuracyTest;
        WorkspaceTest_Config    workspaceTest;
        GravityTest_Config      gravityTest;
    };

    extern TestConfig TestConfig_RobotLeft;
    extern TestConfig TestConfig_RobotRight;
    
}


#endif //ROBOT_CONFIG
