#include "ut_robot_wrapper/kinematic_calibration/kinematiccalibqwrapper.hpp"
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include "utils/qfilesystemmonitor.hpp"
#include "ut_robot_wrapper/kinematic_calibration/kinematiccalib_config.hpp"
#include <fstream>
#include "utils/ReadAndWriteFile.hpp"
#include <nlohmann/json.hpp>
#include <QStringList>
#include <QEventLoop>
#include <future>

using namespace KinematicCalib_Config::FileRelativePath;

#define QStr_ABS_PATH(rel_path) QString::fromStdString(Utilities::BIN_ABSOLUTE_DIR_PATH + '/' + rel_path)

KinematicCalibQWrapper::KinematicCalibQWrapper(QObject *parent)
    : QObject{parent}
{
}

void KinematicCalibQWrapper::calibration_resource_load(bool isLeftArm)
{
    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(CONFIG_PREV_LASER_DATA_right_base_in_laser,
                                                                       std::bind(&KinematicCalibQWrapper::read_frames_from_file, this, std::placeholders::_1, std::ref(vector2d_frame_prev_right_base_in_laser_)));
    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(CONFIG_PREV_LASER_DATA_left_base_in_laser,
                                                                       std::bind(&KinematicCalibQWrapper::read_frames_from_file, this, std::placeholders::_1, std::ref(vector2d_frame_prev_left_base_in_laser_)));
    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(INPUT_LASER_DATA_tool_frame,
                                                                       std::bind(&KinematicCalibQWrapper::read_frames_from_file, this, std::placeholders::_1, std::ref(vector2d_frame_tool_in_flange_)));
    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(INPUT_LASER_DATA_laser_offset,
                                                                       std::bind(&KinematicCalibQWrapper::read_laser_angle_calibrate_from_file, this, std::placeholders::_1, std::ref(vector2d_laser_angle_calibrate_)));

    if(isLeftArm)
    {
        this->m_kinematic_calib_data_ready_left = false;
        QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(INPUT_LASER_DATA_right_base_in_laser,
                                                                           std::bind(&KinematicCalibQWrapper::read_frames_from_file, this, std::placeholders::_1, std::ref(vector2d_frame_current_right_base_in_laser_)));
        QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(INPUT_LASER_DATA_left_tcp_frames,
                                                                           std::bind(&KinematicCalibQWrapper::read_frames_from_file, this, std::placeholders::_1, std::ref(vector2d_frame_list_left_tcp_in_laser_)));
        QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(CONFIG_ROBOT_DATA_left_arm_joint_pose,
                                                                           std::bind(&KinematicCalibQWrapper::read_jpos_from_file, this, true ,std::placeholders::_1, std::ref(vector2d_jpos_list_left_)));

    }
    else
    {
        this->m_kinematic_calib_data_ready_right = false;
        QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(INPUT_LASER_DATA_left_base_in_laser,
                                                                           std::bind(&KinematicCalibQWrapper::read_frames_from_file, this, std::placeholders::_1, std::ref(vector2d_frame_current_left_base_in_laser_)));
        QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(INPUT_LASER_DATA_right_tcp_frames,
                                                                           std::bind(&KinematicCalibQWrapper::read_frames_from_file, this, std::placeholders::_1, std::ref(vector2d_frame_list_right_tcp_in_laser_)));
        QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(CONFIG_ROBOT_DATA_right_arm_joint_pose,
                                                                           std::bind(&KinematicCalibQWrapper::read_jpos_from_file, this, false , std::placeholders::_1, std::ref(vector2d_jpos_list_right_)));
    }
}

void KinematicCalibQWrapper::calibration_resource_unload(bool isLeftArm)
{
    QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(CONFIG_PREV_LASER_DATA_right_base_in_laser);
    QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(CONFIG_PREV_LASER_DATA_left_base_in_laser);
    QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(INPUT_LASER_DATA_tool_frame);
    QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(INPUT_LASER_DATA_laser_offset);

    if(isLeftArm)
    {
        QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(INPUT_LASER_DATA_right_base_in_laser);
        QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(INPUT_LASER_DATA_left_tcp_frames);
        QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(CONFIG_ROBOT_DATA_left_arm_joint_pose);

    }
    else
    {
        QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(INPUT_LASER_DATA_left_base_in_laser);
        QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(INPUT_LASER_DATA_right_tcp_frames);
        QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(CONFIG_ROBOT_DATA_right_arm_joint_pose);
    }
}

void KinematicCalibQWrapper::reset_kinematic_calib()
{
    m_lst_masked_robot_pose_index.clear();
    emit lst_masked_robot_pose_index_changed();
    remove_all_output_files();
}

void KinematicCalibQWrapper::add_mask_index_for_position_recorder(uint32_t index_of_position_record)
{
    if(!m_lst_masked_robot_pose_index.contains(index_of_position_record))
    {
        m_lst_masked_robot_pose_index.append(index_of_position_record);
        emit lst_masked_robot_pose_index_changed();
        emit mask_list_changed();
    }
}

void KinematicCalibQWrapper::remove_mask_index_for_position_recorder(uint32_t index_of_position_record)
{
    if(0 != m_lst_masked_robot_pose_index.removeAll(index_of_position_record))
    {
        m_lst_masked_robot_pose_index.removeAll(index_of_position_record);
        m_lst_masked_robot_pose_index.remove(index_of_position_record);
        emit lst_masked_robot_pose_index_changed();
        emit mask_list_changed();
    }
}

bool KinematicCalibQWrapper::check_calib_data_ready(bool isLeftArm)
{
    bool check_result = true;
    if(isLeftArm)
    {
        check_result = CheckDataReady_LeftArm();

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
    else
    {
        check_result = CheckDataReady_RightArm();

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
    return check_result;
}

bool KinematicCalibQWrapper::kinematicCalib_Calculate_Start(bool isLeftArm)
{
    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult, isLeftArm, this](){
        executionResult = this->kinematicCalib_Calculate(isLeftArm);
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    if(executionResult)
    {
        qDebug() << "kinematic Calibration Passed";
        return true;
    }
    else
    {
        qWarning() << "kinematic Calibration failed";
        return false;
    }
}


// export_Calib_Result(bool isLeftArm, QString export_dir_path, QString robot_serial_number)
bool KinematicCalibQWrapper::export_Calib_Result(bool isLeftArm, QString export_dir_path, QString robot_serial_number)
{
    QDir export_dir{export_dir_path};
    if(!export_dir.exists())
    {
        qWarning()<<"the given export directory"<< export_dir_path <<"not exist!";
        return false;
    }
    if(17 != robot_serial_number.size())
    {
        qWarning()<<"the given robot serial number:"<< robot_serial_number <<"not match length 17!";
        return false;
    }
    if(isLeftArm && !(robot_serial_number.startsWith("CL")))
    {
        qWarning()<<"the given left robot serial number:"<< robot_serial_number <<"should start with 'CL'!";
        return false;
    }
    if(!isLeftArm && !(robot_serial_number.startsWith("CR")))
    {
        qWarning()<<"the given right robot serial number:"<< robot_serial_number <<"should start with 'CR'!";
        return false;
    }


    QDir export_sn_dir{export_dir_path+'/'+robot_serial_number};
    if(QDir().mkdir(export_dir_path+'/'+robot_serial_number))
    {
        qWarning()<<"fail to create dir "<< (export_dir_path+'/'+robot_serial_number) <<"!";
        return false;
    }


    QStringList output_files_path;

    // = isLeftArm?({""}):({""});
    if(isLeftArm)
    {
        output_files_path = {
            QStr_ABS_PATH(OUTPUT_JSON_left_arm_info),
            QStr_ABS_PATH(OUTPUT_XML_left_arm_urdf),
            QStr_ABS_PATH(OUTPUT_left_arm_residual)
        };
    }
    else
    {
        output_files_path = {
            QStr_ABS_PATH(OUTPUT_JSON_right_arm_info),
            QStr_ABS_PATH(OUTPUT_XML_right_arm_urdf),
            QStr_ABS_PATH(OUTPUT_right_arm_residual)
        };
    }

    for(auto output_file_path : output_files_path)
    {
        if(!QFile::exists(output_file_path))
        {
            qWarning()<<"the calibration output file:"<< robot_serial_number <<"exist!";
            return false;
        }
    }

    for(auto output_file_path : output_files_path)
    {
        if(!QFile::exists(output_file_path))
        {
            qWarning()<<"the calibration output file:"<< robot_serial_number <<"exist!";
            return false;
        }
    }
    QString target_dir_path = export_dir_path+'/'+robot_serial_number+'/';
    for(auto output_file_path : output_files_path)
    {
        QString output_file_name = output_file_path.mid(output_file_path.lastIndexOf("/"));
        QString target_file_path = target_dir_path + output_file_name;
        if(!copy_replace_file(output_file_path, target_file_path))
        {
            return false;
        }
    }
    return true;
}

// bool KinematicCalibQWrapper::Merge_Left_And_Right_Calib_Result()
// {

// }

bool KinematicCalibQWrapper::updata_Planned_Robot_Pose(bool isLeftArm, QString source_file_path)
{
    QString target_filepath = isLeftArm?
        QStr_ABS_PATH(CONFIG_ROBOT_DATA_left_arm_joint_pose)
        :QStr_ABS_PATH(CONFIG_ROBOT_DATA_right_arm_joint_pose);
    return copy_replace_file(source_file_path, target_filepath);
}

bool KinematicCalibQWrapper::updata_Laser_Observed_Pose(bool isLeftArm, QString source_file_path)
{
    QString target_filepath = isLeftArm?
        QStr_ABS_PATH(INPUT_LASER_DATA_left_tcp_frames)
        :QStr_ABS_PATH(INPUT_LASER_DATA_right_tcp_frames);
    return copy_replace_file(source_file_path, target_filepath);
}

QVariantList KinematicCalibQWrapper::get_mask_list()
{
    QVariantList list_mask;
    for(auto mask_index: m_lst_masked_robot_pose_index)
    {
        list_mask.append(mask_index);
    }
    return list_mask;
}

void KinematicCalibQWrapper::set_mask_list(QVariantList list_mask)
{
    m_lst_masked_robot_pose_index.clear();
    for(auto mask_index: list_mask)
    {
        m_lst_masked_robot_pose_index.append(mask_index.toInt());
    }
    emit mask_list_changed();
}

bool KinematicCalibQWrapper::copy_replace_file(QString &path_sourceFile, QString &path_targetFile)
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

void KinematicCalibQWrapper::remove_all_output_files()
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

void KinematicCalibQWrapper::read_frames_from_file(const std::string &file_path, std::vector<std::vector<double> > &vector2d)
{
    QFileInfo fileInfo{QString::fromStdString(file_path)};
    vector2d.clear();
    if(fileInfo.exists())
    {
        parser_.set_double_number_per_line(6).set_File_Encoder_Type(PoseFileParser::EncoderType::UTF_8);
        parser_.parse_double_vector(file_path, vector2d);
        if(vector2d.size())
            qDebug() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "finish parse,\r\nget rows:" << vector2d.size();
        else
            qWarning() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "failed to parse";
    }
    else
    {
        qDebug() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "finish parse,\r\nfile deleted!";
    }
    set_kinematic_calib_data_ready_left(CheckDataReady_LeftArm());
    set_kinematic_calib_data_ready_right(CheckDataReady_RightArm());
}

void KinematicCalibQWrapper::read_laser_angle_calibrate_from_file(const std::string &file_path, std::vector<std::vector<double> > &vector2d)
{
    QFileInfo fileInfo{QString::fromStdString(file_path)};
    vector2d.clear();
    if(fileInfo.exists())
    {
        parser_.set_double_number_per_line(2).set_File_Encoder_Type(PoseFileParser::EncoderType::UTF_8);
        parser_.parse_double_vector(file_path, vector2d);
        if(vector2d.size())
            qDebug() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "finish parse,\r\nget rows:" << vector2d.size();
        else
            qWarning() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "failed to parse";
    }
    else
    {
        qDebug() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "finish parse,\r\nfile deleted!";
    }
    set_kinematic_calib_data_ready_left(CheckDataReady_LeftArm());
    set_kinematic_calib_data_ready_right(CheckDataReady_RightArm());
}

void KinematicCalibQWrapper::read_jpos_from_file(bool isLeftArm, const std::string &file_path, std::vector<std::vector<double> > &vector2d)
{
    QFileInfo fileInfo{QString::fromStdString(file_path)};
    vector2d.clear();
    if(fileInfo.exists())
    {
        parser_.set_double_number_per_line(7).set_File_Encoder_Type(PoseFileParser::EncoderType::UTF_8);
        parser_.parse_double_vector(file_path, vector2d);
        if(vector2d.size())
            qDebug() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "finish parse,\r\nget rows:" << vector2d.size();
        else
            qWarning() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "failed to parse";
    }
    else
    {
        qDebug() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "finish parse,\r\nfile deleted!";
    }
    if(isLeftArm)
    {
        set_kinematic_calib_data_ready_left(CheckDataReady_LeftArm());
        this->set_joint_pos_index(0);
        this->set_joint_pos_total_num_left(vector2d.size());
    }
    else
    {
        set_kinematic_calib_data_ready_right(CheckDataReady_RightArm());
        this->set_joint_pos_index(0);
        this->set_joint_pos_total_num_right(vector2d.size());
    }
}

bool KinematicCalibQWrapper::kinematicCalib_Calculate(bool isLeftArm)
{
    if(isLeftArm)
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
    else
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
}

bool KinematicCalibQWrapper::combine_json_file_kinematics_paramters()
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

bool KinematicCalibQWrapper::combine_xml_file_dual_arm_fue_urdf()
{

}

bool KinematicCalibQWrapper::CheckDataReady_LeftArm()
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

bool KinematicCalibQWrapper::CheckDataReady_RightArm()
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


bool KinematicCalibQWrapper::kinematic_calib_data_ready_left() const
{
    return m_kinematic_calib_data_ready_left;
}

void KinematicCalibQWrapper::set_kinematic_calib_data_ready_left(bool newKinematic_calib_data_ready_left)
{
    if (m_kinematic_calib_data_ready_left == newKinematic_calib_data_ready_left)
        return;
    m_kinematic_calib_data_ready_left = newKinematic_calib_data_ready_left;
    emit kinematic_calib_data_ready_left_changed();
}

bool KinematicCalibQWrapper::kinematic_calib_data_ready_right() const
{
    return m_kinematic_calib_data_ready_right;
}

void KinematicCalibQWrapper::set_kinematic_calib_data_ready_right(bool newKinematic_calib_data_ready_right)
{
    if (m_kinematic_calib_data_ready_right == newKinematic_calib_data_ready_right)
        return;
    m_kinematic_calib_data_ready_right = newKinematic_calib_data_ready_right;
    emit kinematic_calib_data_ready_right_changed();
}

QList<int> KinematicCalibQWrapper::get_lst_masked_robot_pose_index() const
{
    return m_lst_masked_robot_pose_index;
}

void KinematicCalibQWrapper::set_lst_masked_robot_pose_index(const QList<int> &newLst_masked_robot_pose_index)
{
    if (m_lst_masked_robot_pose_index == newLst_masked_robot_pose_index)
        return;
    m_lst_masked_robot_pose_index = newLst_masked_robot_pose_index;
    emit lst_masked_robot_pose_index_changed();
}

int KinematicCalibQWrapper::joint_pos_index() const
{
    return m_joint_pos_index;
}

void KinematicCalibQWrapper::set_joint_pos_index(int newJoint_pos_index)
{
    // if (m_joint_pos_index == newJoint_pos_index)
    //     return;
    m_joint_pos_index = newJoint_pos_index;
    if(m_joint_pos_index < vector2d_jpos_list_left_.size())
    {
        QVariantList list_left;
        for(auto singleJointPos: vector2d_jpos_list_left_[m_joint_pos_index])
        {
            list_left.append(singleJointPos);
        }
        set_calib_target_joint_pose_left(list_left);
    }
    if(m_joint_pos_index < vector2d_jpos_list_right_.size())
    {
        QVariantList list_right;
        for(auto singleJointPos: vector2d_jpos_list_right_[m_joint_pos_index])
        {
            list_right.append(singleJointPos);
        }
        set_calib_target_joint_pose_right(list_right);
    }
    emit joint_pos_indexChanged();
}

int KinematicCalibQWrapper::joint_pos_total_num_left() const
{
    return m_joint_pos_total_num_left;
}

void KinematicCalibQWrapper::set_joint_pos_total_num_left(int newJoint_pos_total_num_left)
{
    if (m_joint_pos_total_num_left == newJoint_pos_total_num_left)
        return;
    m_joint_pos_total_num_left = newJoint_pos_total_num_left;
    emit joint_pos_index_leftChanged();
}

int KinematicCalibQWrapper::joint_pos_total_num_right() const
{
    return m_joint_pos_total_num_right;
}

void KinematicCalibQWrapper::set_joint_pos_total_num_right(int newJoint_pos_total_num_right)
{
    if (m_joint_pos_total_num_right == newJoint_pos_total_num_right)
        return;
    m_joint_pos_total_num_right = newJoint_pos_total_num_right;
    emit joint_pos_index_rightChanged();
}

QVariantList KinematicCalibQWrapper::get_calib_target_joint_pose_left() const
{
    return m_calib_target_joint_pose_left;
}

void KinematicCalibQWrapper::set_calib_target_joint_pose_left(const QVariantList &newCalib_target_joint_pose_left)
{
    if (m_calib_target_joint_pose_left == newCalib_target_joint_pose_left)
        return;
    m_calib_target_joint_pose_left = newCalib_target_joint_pose_left;
    emit calib_target_joint_pose_left_changed();
}

QVariantList KinematicCalibQWrapper::get_calib_target_joint_pose_right() const
{
    return m_calib_target_joint_pose_right;
}

void KinematicCalibQWrapper::set_calib_target_joint_pose_right(const QVariantList &newCalib_target_joint_pose_right)
{
    if (m_calib_target_joint_pose_right == newCalib_target_joint_pose_right)
        return;
    m_calib_target_joint_pose_right = newCalib_target_joint_pose_right;
    emit calib_target_joint_pose_right_changed();
}
