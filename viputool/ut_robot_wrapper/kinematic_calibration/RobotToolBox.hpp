#ifndef ROBOT_TOOL_BOX_HPP_
#define ROBOT_TOOL_BOX_HPP_
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <vector>

namespace RobotToolBox {
///@brief
///@param DH alpha a d theta, rad mm
///@return
Eigen::Isometry3d mdh_2_t(const Eigen::VectorXd &mdh);
Eigen::MatrixXd mdh_2_urdf(const Eigen::MatrixXd &mdh);
Eigen::MatrixXd mdh_2_urdf_matlab(const Eigen::MatrixXd &mdh);
Eigen::MatrixXd mdh_2_urdf_matlab_without_theta(const Eigen::MatrixXd &mdh);
Eigen::VectorXd forward_kinematics_for_calibration(const Eigen::MatrixXd &mdh);
Eigen::MatrixXd jacobin_by_numerical_method(const Eigen::MatrixXd &mdh_act);
Eigen::Isometry3d fixxyz_2_t(const Eigen::VectorXd &inp);
bool mdh_calibration(const Eigen::MatrixXd &mdh0,
                     const Eigen::MatrixXd &joint_theta,
                     const std::vector<Eigen::Isometry3d> &flange_pose,
                     Eigen::MatrixXd &mdh_cali, double allowable_deviation);
bool residual(const Eigen::MatrixXd &mdh0, const Eigen::MatrixXd &joint_theta,
              const std::vector<Eigen::Isometry3d> &flange_pose,
              Eigen::MatrixXd &residual);
}; // namespace RobotToolBox

#endif // ROBOT_TOOL_BOX_HPP_