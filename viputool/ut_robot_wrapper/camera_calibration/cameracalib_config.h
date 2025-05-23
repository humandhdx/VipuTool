#ifndef CAMERACALIB_CONFIG_H
#define CAMERACALIB_CONFIG_H

#include <string>

//CameraCalib_Config::FileRelativePath::
namespace CameraCalib_Config
{
    /* File Path relative to executabale path */
    namespace FileRelativePath
    {
        extern const char CONFIG_GLOBAL_CAMERA_right_leftEye_arm_joint_pose[];
        extern const char CONFIG_GLOBAL_CAMERA_right_rightEye_arm_joint_pose[];
        extern const char CONFIG_GLOBAL_CAMERA_right_duelEye_arm_joint_pose[];
        extern const char CONFIG_FOLLOWING_CAMERA_right_arm_joint_pose[];
        extern const char CONFIG_LOCAL_CAMERA_left_leftEye_arm_joint_pose[];
        extern const char CONFIG_LOCAL_CAMERA_left_rightEye_arm_joint_pose[];
        extern const char CONFIG_LOCAL_CAMERA_left_duelEye_arm_joint_pose[];
    }
}
#endif // CAMERACALIB_CONFIG_H
