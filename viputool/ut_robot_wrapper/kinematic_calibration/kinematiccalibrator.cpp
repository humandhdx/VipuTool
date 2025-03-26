#include "ut_robot_wrapper/kinematic_calibration/kinematiccalibrator.hpp"
#include "utils/ReadAndWriteFile.hpp"
#include "RobotToolBox.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <libgen.h>
#include <limits.h>
#include <nlohmann/json.hpp>
#include <string>
#include <unistd.h>
#include <vector>
#include "ut_robot_wrapper/kinematic_calibration/kinematiccalib_config.hpp"

using json = nlohmann::json;
using namespace KinematicCalib_Config::FileRelativePath;

KinematicCalibrator::KinematicCalibrator()
{
    convert_MdhModel_to_matrix(UtRobotConfig::TestConfig_RobotLeft.mdh_model_origin, calib_info_left_.mdh_origin);
    convert_MdhModel_to_matrix(UtRobotConfig::TestConfig_RobotRight.mdh_model_origin, calib_info_right_.mdh_origin);
}

KinematicCalibrator::~KinematicCalibrator() {}

bool KinematicCalibrator::calibration_and_output(bool isLeftArm, std::string& failed_info)
{
    std::string mdh_json_template_path;
    std::string mdh_json_output_path;
    std::string urdf_xml_oupput_path;
    std::string residual_oupput_path;

    if(isLeftArm)
    {
        SourceDataInjection_left();
        mdh_json_template_path  = CONFIG_JSON_TEMPALTE_left_arm_info;
        mdh_json_output_path    = OUTPUT_JSON_left_arm_info;
        urdf_xml_oupput_path    = OUTPUT_XML_left_arm_urdf;
        residual_oupput_path    = OUTPUT_left_arm_residual;
    }
    else
    {
        SourceDataInjection_right();
        mdh_json_template_path  = CONFIG_JSON_TEMPALTE_right_arm_info;
        mdh_json_output_path    = OUTPUT_JSON_right_arm_info;
        urdf_xml_oupput_path    = OUTPUT_XML_right_arm_urdf;
        residual_oupput_path    = OUTPUT_right_arm_residual;
    }

    Kinematic_Calib_Info& calib_info = isLeftArm?calib_info_left_:calib_info_right_;
    std::string info_prefix = isLeftArm?"Left Arm - ":"Right Arm -";
    bool calculation_succ = calculate_kinematics(calib_info);
    failed_info.clear();
    if(!calculation_succ)
    {
        failed_info = info_prefix + "kinematics calibration 'calculation' failed!\r\n";
        return false;
    }
    UtRobotConfig::Identity_Info identityInfo = isLeftArm?
                                                UtRobotConfig::TestConfig_RobotLeft.identity_Info
                                               :UtRobotConfig::TestConfig_RobotRight.identity_Info;
    UtRobotConfig::RobotDhModel_UtraRobot & mdh_model_calib = isLeftArm?
                                                UtRobotConfig::TestConfig_RobotLeft.mdh_model_calib
                                               :UtRobotConfig::TestConfig_RobotRight.mdh_model_calib;
    bool result = true;
    if(!write_arm_mdh_json_file(calib_info, identityInfo, mdh_model_calib, mdh_json_template_path, mdh_json_output_path))
    {
        failed_info += info_prefix + "kinematics calibration 'write mdh file' failed!\r\n";
        result = false;
    }

    if(!write_urdf_file(calib_info, isLeftArm, OUTPUT_XML_left_arm_urdf))
    {
        failed_info += info_prefix + "kinematics calibration 'write urdf file' failed!\r\n";
        result = false;
    }

    if(!write_residual_file(calib_info, residual_oupput_path))
    {
        failed_info += info_prefix + "kinematics calibration 'write residual file' failed!\r\n";
        result = false;
    }

    std::string qualified_info_str;
    if(!check_qualified(calib_info, qualified_info_str))
    {
        failed_info += info_prefix + " qualification check failed:\r\n" + qualified_info_str;
        result = false;
    }
    return result;
}

void KinematicCalibrator::SourceDataInjection_left()
{
    // inject joint pos list data
    std::vector<std::vector<double>> data_vector_2d_filted;
    filtered_data(vector2d_jpos_list_left_, disable_data_set_, data_vector_2d_filted);
    size_t rows = data_vector_2d_filted.size();
    size_t cols = rows > 0 ? data_vector_2d_filted[0].size() : 0;
    calib_info_left_.joint_theta.resize(rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            calib_info_left_.joint_theta(i, j) = data_vector_2d_filted[i][j];
        }
    }
    // get tool frame in flange
    Eigen::Isometry3d tool_in_flange;
    {
        convert_2dVector_to_frame(vector2d_frame_tool_in_flange_, tool_in_flange);
    }

    Eigen::Isometry3d base_frame;
    {// get base frame in laser
        Eigen::Isometry3d left_base_in_laser                    = Eigen::Isometry3d::Identity();
        Eigen::Isometry3d right_base_in_laser                   = Eigen::Isometry3d::Identity();
        Eigen::Isometry3d measured_base_frame_right_in_laser   = Eigen::Isometry3d::Identity();
        convert_2dVector_to_frame(vector2d_frame_prev_right_base_in_laser_, right_base_in_laser);
        convert_2dVector_to_frame(vector2d_frame_prev_left_base_in_laser_, left_base_in_laser);
        convert_2dVector_to_frame(vector2d_frame_current_right_base_in_laser_, measured_base_frame_right_in_laser);

        Eigen::Isometry3d left_base_in_right_base = right_base_in_laser.inverse() * left_base_in_laser;
        base_frame = measured_base_frame_right_in_laser * left_base_in_right_base;
    }

    // inject flange frame in base list data
    {
        // get laser orienatial compensation
        double r_gain_;
        double r_offset_;
        double p_gain_;
        double p_offset_;
        double y_gain_;
        double y_offset_;
        r_gain_     = vector2d_laser_angle_calibrate_.at(0).at(0);
        p_gain_     = vector2d_laser_angle_calibrate_.at(1).at(0);
        y_gain_     = vector2d_laser_angle_calibrate_.at(2).at(0);
        r_offset_   = vector2d_laser_angle_calibrate_.at(0).at(1);
        p_offset_   = vector2d_laser_angle_calibrate_.at(1).at(1);
        y_offset_   = vector2d_laser_angle_calibrate_.at(2).at(1);

        // calculate list of flange in base frame from list of collected tcp frame in laser
        Eigen::VectorXd frame_vector(6);
        Eigen::Isometry3d frame;
        Eigen::Isometry3d flange_frame = Eigen::Isometry3d::Identity();
        std::vector<std::vector<double>> data_vector_2d_filted;
        filtered_data(vector2d_frame_list_left_tcp_in_laser_, disable_data_set_, data_vector_2d_filted);
        size_t rows = data_vector_2d_filted.size();
        calib_info_left_.flange_frame_in_base.clear();
        for (size_t i = 0; i < rows; ++i) {
            frame_vector << data_vector_2d_filted.at(i).at(0),
                data_vector_2d_filted.at(i).at(1),
                data_vector_2d_filted.at(i).at(2),
                data_vector_2d_filted.at(i).at(3),
                data_vector_2d_filted.at(i).at(4),
                data_vector_2d_filted.at(i).at(5);
            frame_vector(3) =
                (r_gain_ * frame_vector(3) + r_offset_) / 180.0 * M_PI;
            frame_vector(4) =
                (p_gain_ * frame_vector(4) + p_offset_) / 180.0 * M_PI;
            frame_vector(5) =
                (y_gain_ * frame_vector(5) + y_offset_) / 180.0 * M_PI;
            frame = RobotToolBox::fixxyz_2_t(frame_vector);
            flange_frame =
                base_frame.inverse() * frame * tool_in_flange.inverse();
            calib_info_left_.flange_frame_in_base.push_back(flange_frame);
        }
    }
}

void KinematicCalibrator::SourceDataInjection_right()
{
    // inject joint pos list data
    std::vector<std::vector<double>> data_vector_2d_filted;
    filtered_data(vector2d_jpos_list_right_, disable_data_set_, data_vector_2d_filted);
    size_t rows = data_vector_2d_filted.size();
    size_t cols = rows > 0 ? data_vector_2d_filted[0].size() : 0;
    calib_info_right_.joint_theta.resize(rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            calib_info_right_.joint_theta(i, j) = data_vector_2d_filted[i][j];
        }
    }

    // get tool frame in flange
    Eigen::Isometry3d tool_in_flange;
    {
        convert_2dVector_to_frame(vector2d_frame_tool_in_flange_, tool_in_flange);
    }

    Eigen::Isometry3d base_frame;
    {// get base frame in laser
        Eigen::Isometry3d left_base_in_laser                    = Eigen::Isometry3d::Identity();
        Eigen::Isometry3d right_base_in_laser                   = Eigen::Isometry3d::Identity();
        Eigen::Isometry3d measured_base_frame_left_in_laser     = Eigen::Isometry3d::Identity();
        convert_2dVector_to_frame(vector2d_frame_prev_right_base_in_laser_, right_base_in_laser);
        convert_2dVector_to_frame(vector2d_frame_prev_left_base_in_laser_, left_base_in_laser);
        convert_2dVector_to_frame(vector2d_frame_current_left_base_in_laser_, measured_base_frame_left_in_laser);

        Eigen::Isometry3d right_base_in_left_base = left_base_in_laser.inverse() * right_base_in_laser;
        base_frame = measured_base_frame_left_in_laser * right_base_in_left_base;
    }

    // inject flange frame in base list data
    {
        // get laser orienatial compensation
        double r_gain_;
        double r_offset_;
        double p_gain_;
        double p_offset_;
        double y_gain_;
        double y_offset_;
        r_gain_     = vector2d_laser_angle_calibrate_.at(0).at(0);
        p_gain_     = vector2d_laser_angle_calibrate_.at(1).at(0);
        y_gain_     = vector2d_laser_angle_calibrate_.at(2).at(0);
        r_offset_   = vector2d_laser_angle_calibrate_.at(0).at(1);
        p_offset_   = vector2d_laser_angle_calibrate_.at(1).at(1);
        y_offset_   = vector2d_laser_angle_calibrate_.at(2).at(1);

        // calculate list of flange in base frame from list of collected tcp frame in laser
        Eigen::VectorXd frame_vector(6);
        Eigen::Isometry3d frame;
        Eigen::Isometry3d flange_frame = Eigen::Isometry3d::Identity();
        std::vector<std::vector<double>> data_vector_2d_filted;
        filtered_data(vector2d_frame_list_right_tcp_in_laser_, disable_data_set_, data_vector_2d_filted);
        size_t rows = data_vector_2d_filted.size();
        calib_info_right_.flange_frame_in_base.clear();
        for (size_t i = 0; i < rows; ++i) {
            frame_vector << data_vector_2d_filted.at(i).at(0),
                data_vector_2d_filted.at(i).at(1),
                data_vector_2d_filted.at(i).at(2),
                data_vector_2d_filted.at(i).at(3),
                data_vector_2d_filted.at(i).at(4),
                data_vector_2d_filted.at(i).at(5);
            frame_vector(3) =
                (r_gain_ * frame_vector(3) + r_offset_) / 180.0 * M_PI;
            frame_vector(4) =
                (p_gain_ * frame_vector(4) + p_offset_) / 180.0 * M_PI;
            frame_vector(5) =
                (y_gain_ * frame_vector(5) + y_offset_) / 180.0 * M_PI;
            frame = RobotToolBox::fixxyz_2_t(frame_vector);
            flange_frame =
                base_frame.inverse() * frame * tool_in_flange.inverse();
            calib_info_right_.flange_frame_in_base.push_back(flange_frame);
        }
    }
}

bool KinematicCalibrator::calculate_kinematics(Kinematic_Calib_Info &calibinfo, double allowable_deviation)
{
    bool calculation_succ = RobotToolBox::mdh_calibration(
        calibinfo.mdh_origin, calibinfo.joint_theta, calibinfo.flange_frame_in_base, calibinfo.mdh_cali, allowable_deviation);
    calibinfo.urdf_cali                 = RobotToolBox::mdh_2_urdf_matlab(calibinfo.mdh_cali);
    calibinfo.urdf_cali_without_theta   = RobotToolBox::mdh_2_urdf_matlab_without_theta(calibinfo.mdh_cali);
    RobotToolBox::residual(calibinfo.mdh_cali, calibinfo.joint_theta, calibinfo.flange_frame_in_base, calibinfo.residual);
    // std::cout << "urdf_cali_:" << std::endl;
    // std::cout << urdf_cali_ << std::endl;
    return calculation_succ;
}

bool KinematicCalibrator::write_arm_mdh_json_file(Kinematic_Calib_Info &calibinfo, UtRobotConfig::Identity_Info identityInfo, UtRobotConfig::RobotDhModel_UtraRobot& mdh_model_calib,
                                                  const std::string &template_file_path, const std::string &output_file_path)
{
    std::ifstream inFile(template_file_path);
    if (!inFile.is_open()) {
        std::cerr << "无法打开模板文件 " << template_file_path << std::endl;
        return false;
    }
    json j;
    inFile >> j;
    inFile.close();

    j["uuid"] = identityInfo.UUID;
    j["software_version"] = identityInfo.VERSION_SW;
    j["hardware_version"] = identityInfo.VERSION_HW;

    if(j.end() == j.find("joints"))
    {
        std::cerr << "模板文件未包含键值 'joints' " << std::endl;
        return false;
    }

    // 查找并修改指定的 a_m 值
    for (auto &joint : j["joints"]) {
        int index = joint["index"];
        joint["alpha_rad"] = calibinfo.mdh_cali(index - 1, 0);
        joint["a_m"] = calibinfo.mdh_cali(index - 1, 1) / 1000.0; // mm -> meter
        joint["d_m"] = calibinfo.mdh_cali(index - 1, 2) / 1000.0; // mm -> meter
        joint["theta_rad"] = calibinfo.mdh_cali(index - 1, 3);

        // stored in gloabal varaible;
        mdh_model_calib[index - 1].alpha_Rot    = calibinfo.mdh_cali(index - 1, 0);
        mdh_model_calib[index - 1].a_Trans      = calibinfo.mdh_cali(index - 1, 1);
        mdh_model_calib[index - 1].d_Trans      = calibinfo.mdh_cali(index - 1, 2);
        mdh_model_calib[index - 1].theta_Rot    = calibinfo.mdh_cali(index - 1, 3);
    }
    //将更新后的 JSON 写入输出文件
    std::ofstream outFile(output_file_path);
    if (!outFile.is_open()) {
        std::cerr << "无法写入文件 "<< output_file_path << std::endl;
        return false;
    }
    outFile << std::setw(4) << j << std::endl; //格式化输出
    outFile.close();
    return true;
}

bool KinematicCalibrator::write_urdf_file(Kinematic_Calib_Info& calibinfo, bool isLeftArm, const std::string &output_file_path)
{
    std::ofstream outFile(output_file_path);
    if (!outFile.is_open()) {
        std::cerr << "无法写入文件 "<< output_file_path << std::endl;
            return false;
    }
    //
    std::string placeholder_arm_name = isLeftArm ? "left":"right";
    outFile << std::fixed << std::setprecision(10);
    outFile << "<xacro:macro name=\"" << placeholder_arm_name << "_origin_joint1\"> <origin xyz=\""
            << calibinfo.urdf_cali(0, 0) << " " << calibinfo.urdf_cali(0, 1) << " "
            << calibinfo.urdf_cali(0, 2) << "\" rpy=\"" << calibinfo.urdf_cali(0, 3) << " "
            << calibinfo.urdf_cali(0, 4) << " " << calibinfo.urdf_cali(0, 5)
            << "\" /> </xacro:macro>" << std::endl;

    outFile << "<xacro:macro name=\"" << placeholder_arm_name << "_origin_joint2\"> <origin xyz=\""
            << calibinfo.urdf_cali(1, 0) << " " << calibinfo.urdf_cali(1, 1) << " "
            << calibinfo.urdf_cali(1, 2) << "\" rpy=\"" << calibinfo.urdf_cali(1, 3) << " "
            << calibinfo.urdf_cali(1, 4) << " " << calibinfo.urdf_cali(1, 5)
            << "\" /> </xacro:macro>" << std::endl;

    outFile << "<xacro:macro name=\"" << placeholder_arm_name << "_origin_joint3\"> <origin xyz=\""
            << calibinfo.urdf_cali(2, 0) << " " << calibinfo.urdf_cali(2, 1) << " "
            << calibinfo.urdf_cali(2, 2) << "\" rpy=\"" << calibinfo.urdf_cali(2, 3) << " "
            << calibinfo.urdf_cali(2, 4) << " " << calibinfo.urdf_cali(2, 5)
            << "\" /> </xacro:macro>" << std::endl;

    outFile << "<xacro:macro name=\"" << placeholder_arm_name << "_origin_joint4\"> <origin xyz=\""
            << calibinfo.urdf_cali(3, 0) << " " << calibinfo.urdf_cali(3, 1) << " "
            << calibinfo.urdf_cali(3, 2) << "\" rpy=\"" << calibinfo.urdf_cali(3, 3) << " "
            << calibinfo.urdf_cali(3, 4) << " " << calibinfo.urdf_cali(3, 5)
            << "\" /> </xacro:macro>" << std::endl;

    outFile << "<xacro:macro name=\"" << placeholder_arm_name << "_origin_joint5\"> <origin xyz=\""
            << calibinfo.urdf_cali(4, 0) << " " << calibinfo.urdf_cali(4, 1) << " "
            << calibinfo.urdf_cali(4, 2) << "\" rpy=\"" << calibinfo.urdf_cali(4, 3) << " "
            << calibinfo.urdf_cali(4, 4) << " " << calibinfo.urdf_cali(4, 5)
            << "\" /> </xacro:macro>" << std::endl;

    outFile << "<xacro:macro name=\"" << placeholder_arm_name << "_origin_joint6\"> <origin xyz=\""
            << calibinfo.urdf_cali(5, 0) << " " << calibinfo.urdf_cali(5, 1) << " "
            << calibinfo.urdf_cali(5, 2) << "\" rpy=\"" << calibinfo.urdf_cali(5, 3) << " "
            << calibinfo.urdf_cali(5, 4) << " " << calibinfo.urdf_cali(5, 5)
            << "\" /> </xacro:macro>" << std::endl;

    outFile << "<xacro:macro name=\"" << placeholder_arm_name << "_origin_joint7\"> <origin xyz=\""
            << calibinfo.urdf_cali(6, 0) << " " << calibinfo.urdf_cali(6, 1) << " "
            << calibinfo.urdf_cali(6, 2) << "\" rpy=\"" << calibinfo.urdf_cali(6, 3) << " "
            << calibinfo.urdf_cali(6, 4) << " " << calibinfo.urdf_cali(6, 5)
            << "\" /> </xacro:macro>" << std::endl;

    outFile.close();
    return true;
}

bool KinematicCalibrator::write_residual_file(Kinematic_Calib_Info &calibinfo, const std::string &output_file_path)
{
    std::ofstream outFile(output_file_path);
    if (!outFile.is_open()) {
        std::cerr << "无法写入文件 "<< output_file_path << std::endl;
            return false;
    }
    std::vector<std::vector<double>> residual_output;
    std::vector<double> res;
    for (int i = 0; i < calibinfo.residual.rows(); i++) {
        res.clear();
        res.resize(calibinfo.residual.cols());
        for (int j = 0; j < calibinfo.residual.cols(); j++) {
            res.at(j) = calibinfo.residual(i, j);
        }
        residual_output.push_back(res);
    }
    Utilities::write_file_data(residual_output, output_file_path);
    return true;
}

bool KinematicCalibrator::check_qualified(Kinematic_Calib_Info &calibinfo, std::string& failed_info)
{
    bool is_qualified = true;

    for (long int i = 0; i < calibinfo.mdh_cali.rows(); i++) {
        if (abs(calibinfo.mdh_cali(i, 0) - calibinfo.mdh_origin(i, 0)) >= 0.125) {
            is_qualified = false;
            std::cout << "joint: " << (i + 1)
                      << " mdh parameter delta alpha is "
                      << (calibinfo.mdh_cali(i, 0) - calibinfo.mdh_origin(i, 0))
                      << " rad, out of range[-0.125, 0.125]." << std::endl;
        }
        if (abs(calibinfo.mdh_cali(i, 1) - calibinfo.mdh_origin(i, 1)) >= 50.0) {
            is_qualified = false;
            std::cout << "joint: " << (i + 1) << " mdh parameter delta a is "
                      << (calibinfo.mdh_cali(i, 1) - calibinfo.mdh_origin(i, 1))
                      << " mm, out of range[-50, 50]." << std::endl;
        }
        if (abs(calibinfo.mdh_cali(i, 2) - calibinfo.mdh_origin(i, 2)) >= 50.0) {
            is_qualified = false;
            std::cout << "joint: " << (i + 1) << " mdh parameter delta d is "
                      << (calibinfo.mdh_cali(i, 2) - calibinfo.mdh_origin(i, 2))
                      << " mm, out of range[-50, 50]." << std::endl;
        }
        if (abs(calibinfo.mdh_cali(i, 3) - calibinfo.mdh_origin(i, 3)) >= 0.125) {
            is_qualified = false;
            double delta = calibinfo.mdh_cali(i, 3) - calibinfo.mdh_origin(i, 3);
            double delta_deg = delta / M_PI * 180.0;
            int ii = delta_deg / 5.0;
            std::stringstream ss;
            ss        << "joint: " << (i + 1)
                      << " mdh parameter delta theta is "
               << (calibinfo.mdh_cali(i, 3) - calibinfo.mdh_origin(i, 3))
                      << ", out of range[-0.125, 0.125].";
            ss        << " move this joint to " << 5.0 * double(-ii)
                      << " deg as new zero point, then try to calibration "
                         "kinematics paramters one "
                         "more time."
                      << std::endl;
            failed_info = ss.str();
        }
    }
    std::cout << "****************************" << std::endl;
    if (is_qualified == false) {
        std::cout << "*** CONCLUSION:  FAILED! ***" << std::endl;
    } else {
        std::cout << "*** CONCLUSION: SUCCEED! ***" << std::endl;
    }
    std::cout << "****************************" << std::endl;
    return is_qualified;
}

void KinematicCalibrator::convert_2dVector_to_frame(const std::vector<std::vector<double> > &frame_vector_2d, Eigen::Isometry3d &frame)
{
    Eigen::VectorXd frame_vector(6);
    frame_vector <<
        frame_vector_2d.at(0).at(0), frame_vector_2d.at(0).at(1),
        frame_vector_2d.at(0).at(2), frame_vector_2d.at(0).at(3),
        frame_vector_2d.at(0).at(4), frame_vector_2d.at(0).at(5);
    frame_vector(3) = frame_vector(3) / 180.0 * M_PI;
    frame_vector(4) = frame_vector(4) / 180.0 * M_PI;
    frame_vector(5) = frame_vector(5) / 180.0 * M_PI;
    frame = RobotToolBox::fixxyz_2_t(frame_vector);
}


void KinematicCalibrator::filtered_data(
    const std::vector<std::vector<double>> &ipt,
    const std::set<uint32_t, ElementSmaller> &disable_data_set_,
    std::vector<std::vector<double>> &opt) {
    opt.clear();
    if (disable_data_set_.empty()) {
        opt = ipt;
        return;
    }
    opt.reserve(ipt.size());
    std::set<uint32_t>::const_iterator it = disable_data_set_.begin();
    for (size_t i = 0; i < ipt.size(); ++i) {
        if (it != disable_data_set_.end() && *it == i) {
            ++it;
        } else {
            opt.push_back(ipt[i]);
        }
    }
    return;
}

void KinematicCalibrator::convert_MdhModel_to_matrix(UtRobotConfig::RobotDhModel_UtraRobot dhModel, Eigen::MatrixXd &matrix)
{
    matrix = Eigen::MatrixXd::Zero(dhModel.size(), 4);
    for(unsigned long row_index = 0; row_index < dhModel.size(); row_index++)
    {

        matrix.row(row_index)
            <<
            static_cast<double>(dhModel[row_index].alpha_Rot),
            static_cast<double>(dhModel[row_index].a_Trans),
            static_cast<double>(dhModel[row_index].d_Trans),
            static_cast<double>(dhModel[row_index].theta_Rot);
    }
}
