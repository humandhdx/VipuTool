#include "ut_robot_wrapper/kinematic_calibration/kinematiccalibqwrapper.hpp"
#include <QDebug>
#include <QFileInfo>
#include "utils/qfilesystemmonitor.hpp"
#include "ut_robot_wrapper/kinematic_calibration/kinematiccalib_config.hpp"
#include <fstream>
#include "utils/ReadAndWriteFile.hpp"
#include <nlohmann/json.hpp>
#include <QStringList>

using namespace KinematicCalib_Config::FileRelativePath;

#define QStr_ABS_PATH(rel_path) QString::fromStdString(Utilities::BIN_ABSOLUTE_DIR_PATH + '/' + rel_path)

KinematicCalib_QWrapper::KinematicCalib_QWrapper(QObject *parent)
    : QObject{parent}
{
    this->m_kinematic_calib_data_ready_left = false;
    this->m_kinematic_calib_data_ready_right = false;

    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(CONFIG_PREV_LASER_DATA_right_base_in_laser,
                                                                       std::bind(&KinematicCalib_QWrapper::read_frames_from_file, this, std::placeholders::_1, std::ref(vector2d_frame_prev_right_base_in_laser_)));
    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(CONFIG_PREV_LASER_DATA_left_base_in_laser,
                                                                       std::bind(&KinematicCalib_QWrapper::read_frames_from_file, this, std::placeholders::_1, std::ref(vector2d_frame_prev_left_base_in_laser_)));
    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(INPUT_LASER_DATA_right_base_in_laser,
                                                                       std::bind(&KinematicCalib_QWrapper::read_frames_from_file, this, std::placeholders::_1, std::ref(vector2d_frame_current_right_base_in_laser_)));
    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(INPUT_LASER_DATA_left_base_in_laser,
                                                                       std::bind(&KinematicCalib_QWrapper::read_frames_from_file, this, std::placeholders::_1, std::ref(vector2d_frame_current_left_base_in_laser_)));

    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(INPUT_LASER_DATA_right_tcp_frames,
                                                                       std::bind(&KinematicCalib_QWrapper::read_frames_from_file, this, std::placeholders::_1, std::ref(vector2d_frame_list_right_tcp_in_laser_)));
    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(INPUT_LASER_DATA_left_tcp_frames,
                                                                       std::bind(&KinematicCalib_QWrapper::read_frames_from_file, this, std::placeholders::_1, std::ref(vector2d_frame_list_left_tcp_in_laser_)));

    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(INPUT_LASER_DATA_tool_frame,
                                                                       std::bind(&KinematicCalib_QWrapper::read_frames_from_file, this, std::placeholders::_1, std::ref(vector2d_frame_tool_in_flange_)));

    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(INPUT_LASER_DATA_laser_offset,
                                                                       std::bind(&KinematicCalib_QWrapper::read_laser_angle_calibrate_from_file, this, std::placeholders::_1, std::ref(vector2d_laser_angle_calibrate_)));

    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(CONFIG_ROBOT_DATA_left_arm_joint_pose,
                                                                       std::bind(&KinematicCalib_QWrapper::read_jpos_from_file, this, std::placeholders::_1, std::ref(vector2d_jpos_list_left_)));

    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(CONFIG_ROBOT_DATA_right_arm_joint_pose,
                                                                       std::bind(&KinematicCalib_QWrapper::read_jpos_from_file, this, std::placeholders::_1, std::ref(vector2d_jpos_list_right_)));
}

void KinematicCalib_QWrapper::reset_kinematic_calib()
{
    m_lst_masked_robot_pose_index.clear();
    emit lst_masked_robot_pose_index_changed();
    remove_all_output_files();
}

void KinematicCalib_QWrapper::add_mask_index_for_position_recorder(uint32_t index_of_position_record)
{
    m_lst_masked_robot_pose_index.append(index_of_position_record);
    emit lst_masked_robot_pose_index_changed();
}

void KinematicCalib_QWrapper::log_calib_data_ready_info_left()
{
    if(CheckDataReady_LeftArm())
    {
        qDebug() << "Kinematic Calibration Data is ready for left arm!";
        return;
    }

    if(1 !=vector2d_frame_tool_in_flange_.size())
    {
        qWarning() << "Please check file ready:" << INPUT_LASER_DATA_tool_frame;
    }
    if(1 !=vector2d_frame_prev_left_base_in_laser_.size())
    {
        qWarning() << "Please check file ready:" << CONFIG_PREV_LASER_DATA_left_base_in_laser;
    }
    if(1 !=vector2d_frame_prev_right_base_in_laser_.size())
    {
        qWarning() << "Please check file ready:" << CONFIG_PREV_LASER_DATA_right_base_in_laser;
    }
    if(1 !=vector2d_frame_current_right_base_in_laser_.size())
    {
        qWarning() << "Please check file ready:" << INPUT_LASER_DATA_right_base_in_laser;
    }
    if(3 !=vector2d_laser_angle_calibrate_.size())
    {
        qWarning() << "Please check file ready:" << INPUT_LASER_DATA_laser_offset;
    }
    if(6 >= vector2d_frame_list_left_tcp_in_laser_.size())
    {
        qWarning() << "Please check file ready:" << INPUT_LASER_DATA_left_tcp_frames;
        qWarning() << "which need to extract at least 7 laser records";
    }
    if(6 >= vector2d_jpos_list_left_.size())
    {
        qWarning() << "Please check file ready:" << CONFIG_ROBOT_DATA_left_arm_joint_pose;
        qWarning() << "which need to extract at least 7 laser records";
    }
    if((6 < vector2d_frame_list_left_tcp_in_laser_.size())
        && (6 < vector2d_jpos_list_left_.size())
        && (vector2d_frame_list_left_tcp_in_laser_.size() != vector2d_jpos_list_left_.size()))
    {
        qWarning() << "records number not match for the following two files:";
        qWarning() << INPUT_LASER_DATA_left_tcp_frames;
        qWarning() << CONFIG_ROBOT_DATA_left_arm_joint_pose;
    }
}

void KinematicCalib_QWrapper::log_calib_data_ready_info_right()
{
    if(CheckDataReady_RightArm())
    {
        qDebug() << "Kinematic Calibration Data is ready for right arm!";
        return;
    }

    if(1 !=vector2d_frame_tool_in_flange_.size())
    {
        qWarning() << "Please check file ready:" << INPUT_LASER_DATA_tool_frame;
    }
    if(1 !=vector2d_frame_prev_left_base_in_laser_.size())
    {
        qWarning() << "Please check file ready:" << CONFIG_PREV_LASER_DATA_left_base_in_laser;
    }
    if(1 !=vector2d_frame_prev_right_base_in_laser_.size())
    {
        qWarning() << "Please check file ready:" << CONFIG_PREV_LASER_DATA_right_base_in_laser;
    }
    if(1 !=vector2d_frame_current_left_base_in_laser_.size())
    {
        qWarning() << "Please check file ready:" << INPUT_LASER_DATA_left_base_in_laser;
    }
    if(3 !=vector2d_laser_angle_calibrate_.size())
    {
        qWarning() << "Please check file ready:" << INPUT_LASER_DATA_laser_offset;
    }
    if(6 >= vector2d_frame_list_right_tcp_in_laser_.size())
    {
        qWarning() << "Please check file ready:" << INPUT_LASER_DATA_right_tcp_frames;
        qWarning() << "which need to extract at least 7 laser records";
    }
    if(6 >= vector2d_jpos_list_right_.size())
    {
        qWarning() << "Please check file ready:" << CONFIG_ROBOT_DATA_right_arm_joint_pose;
        qWarning() << "which need to extract at least 7 laser records";
    }
    if((6 < vector2d_frame_list_right_tcp_in_laser_.size())
        && (6 < vector2d_jpos_list_right_.size())
        && (vector2d_frame_list_right_tcp_in_laser_.size() != vector2d_jpos_list_right_.size()))
    {
        qWarning() << "records number not match for the following two files:";
        qWarning() << INPUT_LASER_DATA_right_tcp_frames;
        qWarning() << CONFIG_ROBOT_DATA_right_arm_joint_pose;
    }
}

bool KinematicCalib_QWrapper::KinematicCalib_Start_left()
{
    if(!CheckDataReady_LeftArm())
    {
        qWarning() << "Kinematic Calibration Data is not ready for left arm!";
        return false;
    }
    if(0 == UtRobotConfig::TestConfig_RobotLeft.identity_Info.UUID.size()
        || 0 == UtRobotConfig::TestConfig_RobotLeft.identity_Info.VERSION_HW.size()
        || 0 == UtRobotConfig::TestConfig_RobotLeft.identity_Info.VERSION_SW.size())
    {
        qWarning() << "left arm identity infomation empty UUID/Version_sw/Version_hw!";
        return false;
    }
    this->disable_data_set_.clear();
    for(auto masked_pose_index: m_lst_masked_robot_pose_index)
    {
        this->disable_data_set_.insert(masked_pose_index);
    }

    std::string log_info;
    bool result = this->calibration_and_output(true, log_info);
    if(result)
    {
        qDebug() << "Kinematic Calbiration for left arm finished!";
    }
    else
    {
        qWarning() << QString::fromStdString(log_info);
        qWarning() << "Kinematic Calbiration for left arm failed!";
    }
    return result;
}

bool KinematicCalib_QWrapper::KinematicCalib_Start_right()
{
    if(!CheckDataReady_RightArm())
    {
        qWarning() << "Kinematic Calibration Data is not ready for right arm!";
        return false;
    }
    if(0 == UtRobotConfig::TestConfig_RobotRight.identity_Info.UUID.size()
        || 0 == UtRobotConfig::TestConfig_RobotRight.identity_Info.VERSION_HW.size()
        || 0 == UtRobotConfig::TestConfig_RobotRight.identity_Info.VERSION_SW.size())
    {
        qWarning() << "right arm identity infomation empty UUID/Version_sw/Version_hw!";
        return false;
    }

    this->disable_data_set_.clear();
    for(auto masked_pose_index: m_lst_masked_robot_pose_index)
    {
        this->disable_data_set_.insert(masked_pose_index);
    }

    std::string log_info;
    bool result = this->calibration_and_output(false, log_info);
    if(result)
    {
        qDebug() << "Kinematic Calbiration for right arm finished!";
    }
    else
    {
        qWarning() << QString::fromStdString(log_info);
        qWarning() << "Kinematic Calbiration for right arm failed!";
    }
    return result;
}

bool KinematicCalib_QWrapper::Merge_Left_And_Right_Calib_Result()
{

}

bool KinematicCalib_QWrapper::Updata_Planned_Robot_Pose_left(QString source_file_path)
{
    QString target_filepath = QStr_ABS_PATH(CONFIG_ROBOT_DATA_left_arm_joint_pose);
    return copy_replace_file(source_file_path, target_filepath);
}

bool KinematicCalib_QWrapper::Updata_Planned_Robot_Pose_right(QString source_file_path)
{
    QString target_filepath = QStr_ABS_PATH(CONFIG_ROBOT_DATA_right_arm_joint_pose);
    return copy_replace_file(source_file_path, target_filepath);
}

bool KinematicCalib_QWrapper::copy_replace_file(QString &path_sourceFile, QString &path_targetFile)
{
    QFile sourceFile{path_sourceFile};
    if(!sourceFile.exists())
    {
        qWarning()<<"the given source file"<< path_sourceFile <<"not exist!";
        return false;
    }

    if(QFile::exists(path_targetFile) && (!QFile::remove(path_targetFile)))
    {
        qWarning()<<"failed to override target file"<< path_targetFile;
        return false;
    }

    if(sourceFile.copy(path_targetFile))
    {
        qWarning()<<"failed to copy source file"<< path_sourceFile ;
    }
    return true;
}

void KinematicCalib_QWrapper::remove_all_output_files()
{
    QStringList qstrlst={
        QStr_ABS_PATH(OUTPUT_JSON_left_arm_info),
        QStr_ABS_PATH(OUTPUT_JSON_right_arm_info),
        QStr_ABS_PATH(OUTPUT_XML_left_arm_urdf),
        QStr_ABS_PATH(OUTPUT_XML_right_arm_urdf),
        QStr_ABS_PATH(OUTPUT_left_arm_residual),
        QStr_ABS_PATH(OUTPUT_right_arm_residual)
    };

    for(auto outputFile_absPath : qstrlst)
    {
        QFile file_to_remove{outputFile_absPath};
        if(QFile::exists(outputFile_absPath))
        {
            if(!QFile::remove(outputFile_absPath))
            {
                qWarning() << "Failed to remove output file" << outputFile_absPath;
            }
        }
    }
}

void KinematicCalib_QWrapper::read_frames_from_file(const std::string &file_path, std::vector<std::vector<double> > &vector2d)
{
    QFileInfo fileInfo{QString::fromStdString(file_path)};
    vector2d.clear();
    if(fileInfo.exists())
    {
        parser_.set_double_number_per_line(6).set_File_Encoder_Type(PoseFileParser::EncoderType::UTF_8);
        parser_.parse_double_vector(file_path, vector2d);
        qDebug() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "finish parse,\r\nget rows:" << vector2d.size();
    }
    else
    {
        qDebug() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "finish parse,\r\nfile deleted!";
    }
    set_kinematic_calib_data_ready_left(CheckDataReady_LeftArm());
    set_kinematic_calib_data_ready_right(CheckDataReady_RightArm());
}

void KinematicCalib_QWrapper::read_laser_angle_calibrate_from_file(const std::string &file_path, std::vector<std::vector<double> > &vector2d)
{
    QFileInfo fileInfo{QString::fromStdString(file_path)};
    vector2d.clear();
    if(fileInfo.exists())
    {
        parser_.set_double_number_per_line(2).set_File_Encoder_Type(PoseFileParser::EncoderType::UTF_8);
        parser_.parse_double_vector(file_path, vector2d);
        qDebug() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "finish parse,\r\nget rows:" << vector2d.size();
    }
    else
    {
        qDebug() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "finish parse,\r\nfile deleted!";
    }
    set_kinematic_calib_data_ready_left(CheckDataReady_LeftArm());
    set_kinematic_calib_data_ready_right(CheckDataReady_RightArm());
}

void KinematicCalib_QWrapper::read_jpos_from_file(const std::string &file_path, std::vector<std::vector<double> > &vector2d)
{
    QFileInfo fileInfo{QString::fromStdString(file_path)};
    vector2d.clear();
    if(fileInfo.exists())
    {
        parser_.set_double_number_per_line(7).set_File_Encoder_Type(PoseFileParser::EncoderType::UTF_8);
        parser_.parse_double_vector(file_path, vector2d);
        qDebug() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "finish parse,\r\nget rows:" << vector2d.size();
    }
    else
    {
        qDebug() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "finish parse,\r\nfile deleted!";
    }
    set_kinematic_calib_data_ready_left(CheckDataReady_LeftArm());
    set_kinematic_calib_data_ready_right(CheckDataReady_RightArm());
}

bool KinematicCalib_QWrapper::combine_json_file_kinematics_paramters()
{
    //OUTPUT_JSON_left_arm_info
    std::ifstream inFile_left_arm_info{Utilities::BIN_ABSOLUTE_DIR_PATH + '/' + OUTPUT_JSON_left_arm_info};
    if (!inFile_left_arm_info.is_open()) {
        qWarning() << "无法打开动力学标定输出文件" << QString::fromStdString(OUTPUT_JSON_left_arm_info);
        return false;
    }
    std::ifstream inFile_right_arm_info{Utilities::BIN_ABSOLUTE_DIR_PATH + '/' + OUTPUT_JSON_right_arm_info};
    if (!inFile_right_arm_info.is_open()) {
        qWarning() << "无法打开动力学标定输出文件" << QString::fromStdString(OUTPUT_JSON_right_arm_info);
        return false;
    }
    std::ifstream inFile_json_template{Utilities::BIN_ABSOLUTE_DIR_PATH + '/' + CONFIG_JSON_TEMPALTE_kinematics_paramters};
    if (!inFile_json_template.is_open()) {
        qWarning() << "无法打开动力学标定模板文件" << QString::fromStdString(CONFIG_JSON_TEMPALTE_kinematics_paramters);
        return false;
    }

    nlohmann::json js_left_arm_info;
    inFile_left_arm_info >> js_left_arm_info;
    inFile_left_arm_info.close();

    nlohmann::json js_right_arm_info;
    inFile_right_arm_info >> js_right_arm_info;
    inFile_right_arm_info.close();

    nlohmann::json js_template;
    inFile_json_template >> js_template;
    inFile_json_template.close();

    if((js_template.end() == js_template.find("arm_info")) || !(js_template["arm_info"].is_array()))
    {
        //找不到键值"arm_info"
        qWarning() << "找不到键值'arm_info'的数组,于模板json文件" << CONFIG_JSON_TEMPALTE_kinematics_paramters;
        return false;
    }

    js_template["arm_info"].emplace_back(js_left_arm_info);
    js_template["arm_info"].emplace_back(js_right_arm_info);

    std::ofstream outFile(Utilities::BIN_ABSOLUTE_DIR_PATH + '/' + COMBINE_JSON_kinematics_paramters);
    if (!outFile.is_open()) {
        qWarning() << "无法写入文件" << COMBINE_JSON_kinematics_paramters;
        return false;
    }
    outFile << std::setw(4) << js_template << std::endl; //格式化输出
    outFile.close();
    return true;
}

bool KinematicCalib_QWrapper::combine_xml_file_dual_arm_fue_urdf()
{

}

bool KinematicCalib_QWrapper::CheckDataReady_LeftArm()
{
    return (1 ==vector2d_frame_tool_in_flange_.size())
        && (1 == vector2d_frame_prev_right_base_in_laser_.size())
        && (1 == vector2d_frame_prev_left_base_in_laser_.size())
        && (1 == vector2d_frame_current_right_base_in_laser_.size())
        && (3 == vector2d_laser_angle_calibrate_.size())
        && (6 < vector2d_frame_list_left_tcp_in_laser_.size())
        && (6 < vector2d_jpos_list_left_.size())
        && (vector2d_frame_list_left_tcp_in_laser_.size() == vector2d_jpos_list_left_.size());
}

bool KinematicCalib_QWrapper::CheckDataReady_RightArm()
{
    return (1 ==vector2d_frame_tool_in_flange_.size())
        && (1 == vector2d_frame_prev_right_base_in_laser_.size())
        && (1 == vector2d_frame_prev_left_base_in_laser_.size())
        && (1 == vector2d_frame_current_left_base_in_laser_.size())
        && (3 == vector2d_laser_angle_calibrate_.size())
        && (6 < vector2d_frame_list_right_tcp_in_laser_.size())
        && (6 < vector2d_jpos_list_right_.size())
        && (vector2d_frame_list_right_tcp_in_laser_.size() == vector2d_jpos_list_right_.size());
}


bool KinematicCalib_QWrapper::kinematic_calib_data_ready_left() const
{
    return m_kinematic_calib_data_ready_left;
}

void KinematicCalib_QWrapper::set_kinematic_calib_data_ready_left(bool newKinematic_calib_data_ready_left)
{
    if (m_kinematic_calib_data_ready_left == newKinematic_calib_data_ready_left)
        return;
    m_kinematic_calib_data_ready_left = newKinematic_calib_data_ready_left;
    emit kinematic_calib_data_ready_left_changed();
}

bool KinematicCalib_QWrapper::kinematic_calib_data_ready_right() const
{
    return m_kinematic_calib_data_ready_right;
}

void KinematicCalib_QWrapper::set_kinematic_calib_data_ready_right(bool newKinematic_calib_data_ready_right)
{
    if (m_kinematic_calib_data_ready_right == newKinematic_calib_data_ready_right)
        return;
    m_kinematic_calib_data_ready_right = newKinematic_calib_data_ready_right;
    emit kinematic_calib_data_ready_right_changed();
}

QList<int> KinematicCalib_QWrapper::get_lst_masked_robot_pose_index() const
{
    return m_lst_masked_robot_pose_index;
}

void KinematicCalib_QWrapper::set_lst_masked_robot_pose_index(const QList<int> &newLst_masked_robot_pose_index)
{
    if (m_lst_masked_robot_pose_index == newLst_masked_robot_pose_index)
        return;
    m_lst_masked_robot_pose_index = newLst_masked_robot_pose_index;
    emit lst_masked_robot_pose_index_changed();
}
