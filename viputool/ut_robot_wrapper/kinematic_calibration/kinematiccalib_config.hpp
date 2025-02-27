#ifndef KINEMATICCALIB_CONFIG_H
#define KINEMATICCALIB_CONFIG_H
#include <string>

//KinematicCalib_Config::FileRelativePath::
namespace KinematicCalib_Config
{
    /* File Path relative to executabale path */
    namespace FileRelativePath {
        extern const char CONFIG_PREV_LASER_DATA_right_base_in_laser[];
        extern const char CONFIG_PREV_LASER_DATA_left_base_in_laser[];

        extern const char INPUT_LASER_DATA_right_base_in_laser[];
        extern const char INPUT_LASER_DATA_left_base_in_laser[];

        extern const char INPUT_LASER_DATA_laser_offset[];
        extern const char INPUT_LASER_DATA_tool_frame[];

        extern const char INPUT_LASER_DATA_right_tcp_frames[];
        extern const char INPUT_LASER_DATA_left_tcp_frames[];

        extern const char CONFIG_ROBOT_DATA_right_arm_joint_pose[];
        extern const char CONFIG_ROBOT_DATA_left_arm_joint_pose[];

        extern const char CONFIG_JSON_TEMPALTE_left_arm_info[];
        extern const char CONFIG_JSON_TEMPALTE_right_arm_info[];

        extern const char OUTPUT_JSON_left_arm_info[];
        extern const char OUTPUT_JSON_right_arm_info[];

        extern const char OUTPUT_XML_left_arm_urdf[];
        extern const char OUTPUT_XML_right_arm_urdf[];

        extern const char OUTPUT_left_arm_residual[];
        extern const char OUTPUT_right_arm_residual[];

        extern const char CONFIG_JSON_TEMPALTE_kinematics_paramters[];
        extern const char COMBINE_JSON_kinematics_paramters[];

        extern const char CONFIG_XML_TEMPALTE_dual_arm_urdf[];
        extern const char COMBINE_XML_dual_arm_urdf[];
    }
}


// class CalculatorConfiguration
// {
// public:
//     CalculatorConfiguration();
// };

#endif // KINEMATICCALIB_CONFIG_H
