#ifndef KINEMATIC_CALIBRATOR_HPP_
#define KINEMATIC_CALIBRATOR_HPP_
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <set>
#include <ut_robot_wrapper/robot_config.hpp>

struct ElementSmaller
{
    bool operator()(uint32_t a, uint32_t b) const { return a < b; }
};

class KinematicCalibrator {
public:
    explicit KinematicCalibrator();
    virtual ~KinematicCalibrator();
    bool calibration_and_output(bool isLeftArm, std::string& failed_info);

protected:
    struct Kinematic_Calib_Info{
        Eigen::MatrixXd                 joint_theta;
        std::vector<Eigen::Isometry3d>  flange_frame_in_base;

        Eigen::MatrixXd mdh_origin;
        Eigen::MatrixXd mdh_cali;

        Eigen::MatrixXd urdf_cali;
        Eigen::MatrixXd urdf_cali_without_theta;

        Eigen::MatrixXd residual;
    };

    Kinematic_Calib_Info calib_info_left_;
    Kinematic_Calib_Info calib_info_right_;

protected:
    void SourceDataInjection_left();

    void SourceDataInjection_right();

    bool calculate_kinematics(Kinematic_Calib_Info& calibinfo, double allowable_deviation = 2.5);

    bool write_arm_mdh_json_file(Kinematic_Calib_Info& calibinfo, UtRobotConfig::Identity_Info identityInfo, UtRobotConfig::RobotDhModel_UtraRobot& mdh_model_calib,
                                 const std::string &template_file_path, const std::string &output_file_path);

    bool write_urdf_file(Kinematic_Calib_Info& calibinfo, bool isLeftArm, const std::string &output_file_path);

    bool write_residual_file(Kinematic_Calib_Info& calibinfo, const std::string &output_file_path);

    bool check_qualified(Kinematic_Calib_Info& calibinfo, std::string& failed_info);

    //frame{x(mm),y(mm),z(mm),Rx(degree),Ry(degree),Rz(degree)} => vector [6][1]
    std::vector<std::vector<double>> vector2d_frame_tool_in_flange_;
    std::vector<std::vector<double>> vector2d_frame_prev_right_base_in_laser_;
    std::vector<std::vector<double>> vector2d_frame_prev_left_base_in_laser_;
    std::vector<std::vector<double>> vector2d_frame_current_right_base_in_laser_;
    std::vector<std::vector<double>> vector2d_frame_current_left_base_in_laser_;

    //calibration{gain, offset} => vector [2][3]
    std::vector<std::vector<double>> vector2d_laser_angle_calibrate_;

    //frame list => vector [6][n]
    std::vector<std::vector<double>> vector2d_frame_list_right_tcp_in_laser_;
    std::vector<std::vector<double>> vector2d_frame_list_left_tcp_in_laser_;

    //joint pose{radian} list => vector [7][n]
    std::vector<std::vector<double>> vector2d_jpos_list_right_;
    std::vector<std::vector<double>> vector2d_jpos_list_left_;

    /* '0' -> first record */
    std::set<uint32_t, ElementSmaller> disable_data_set_;

    void convert_2dVector_to_frame(const std::vector<std::vector<double>>& frame_vector_2d, Eigen::Isometry3d &frame);

    void filtered_data(const std::vector<std::vector<double>> &ipt,
                       const std::set<uint32_t, ElementSmaller> &disable_data_set_,
                       std::vector<std::vector<double>> &opt);

    void convert_MdhModel_to_matrix(UtRobotConfig::RobotDhModel_UtraRobot dhModel, Eigen::MatrixXd& matrix);
};
#endif  //KINEMATIC_CALIBRATOR_HPP_
