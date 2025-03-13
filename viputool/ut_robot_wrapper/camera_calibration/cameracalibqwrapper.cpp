#include "ut_robot_wrapper/camera_calibration/cameracalibqwrapper.h"
#include "utils/qfilesystemmonitor.hpp"
#include "ut_robot_wrapper/camera_calibration/cameracalib_config.h"
#include <fstream>
#include "utils/ReadAndWriteFile.hpp"

using namespace CameraCalib_Config::FileRelativePath;

#define QStr_ABS_PATH(rel_path) QString::fromStdString(Utilities::BIN_ABSOLUTE_DIR_PATH + '/' + rel_path)

CameraCalibQWrapper::CameraCalibQWrapper(QObject *parent)
    : QObject{parent}
{
}

void CameraCalibQWrapper::calibration_resource_load()
{
    set_joint_pos_index_global_right(0);
    set_joint_pos_index_following_right(0);
    set_joint_pos_index_local_left(0);
    set_joint_pos_total_num_global_right(0);
    set_joint_pos_total_num_following_right(0);
    set_joint_pos_total_num_local_left(0);
    QVariantList zero_jointPos;
    zero_jointPos.append(QVariant::fromValue((double)1.0));
    zero_jointPos.append(QVariant::fromValue((double)1.0));
    zero_jointPos.append(QVariant::fromValue((double)1.0));
    zero_jointPos.append(QVariant::fromValue((double)1.0));
    zero_jointPos.append(QVariant::fromValue((double)1.0));
    zero_jointPos.append(QVariant::fromValue((double)1.0));
    zero_jointPos.append(QVariant::fromValue((double)1.0));
    set_current_joint_pos_global_right(zero_jointPos);
    set_current_joint_pos_following_right(zero_jointPos);
    set_current_joint_pos_local_left(zero_jointPos);

    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(CONFIG_GLOBAL_CAMERA_right_arm_joint_pose,
                                                                       std::bind(&CameraCalibQWrapper::read_jpos_from_file, this, CameraCalib_Type::GLOBAL_CAMERA_RIGHT_ARM, std::placeholders::_1, std::ref(vector2d_jpos_list_global_right_)));
    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(CONFIG_FOLLOWING_CAMERA_right_arm_joint_pose,
                                                                       std::bind(&CameraCalibQWrapper::read_jpos_from_file, this, CameraCalib_Type::FOLLOWING_CAMERA_RIGHT_ARM, std::placeholders::_1, std::ref(vector2d_jpos_list_following_right_)));
    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(CONFIG_LOCAL_CAMERA_left_arm_joint_pose,
                                                                       std::bind(&CameraCalibQWrapper::read_jpos_from_file, this, CameraCalib_Type::LOCAL_CAMERA_LEFT_ARM, std::placeholders::_1, std::ref(vector2d_jpos_list_local_left_)));
}

void CameraCalibQWrapper::calibration_resource_unload()
{
    QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(CONFIG_GLOBAL_CAMERA_right_arm_joint_pose);
    QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(CONFIG_FOLLOWING_CAMERA_right_arm_joint_pose);
    QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(CONFIG_LOCAL_CAMERA_left_arm_joint_pose);
}

bool CameraCalibQWrapper::Updata_Joint_Pos_Global_Right(QString source_file_path)
{
    QString target_filepath =  QStr_ABS_PATH(CONFIG_GLOBAL_CAMERA_right_arm_joint_pose);
    return copy_replace_file(source_file_path, target_filepath);
}

bool CameraCalibQWrapper::Updata_Joint_Pos_Following_Right(QString source_file_path)
{
    QString target_filepath =  QStr_ABS_PATH(CONFIG_FOLLOWING_CAMERA_right_arm_joint_pose);
    return copy_replace_file(source_file_path, target_filepath);
}

bool CameraCalibQWrapper::Updata_Joint_Pos_Local_Left(QString source_file_path)
{
    QString target_filepath =  QStr_ABS_PATH(CONFIG_LOCAL_CAMERA_left_arm_joint_pose);
    return copy_replace_file(source_file_path, target_filepath);
}

int CameraCalibQWrapper::joint_pos_index_global_right() const
{
    return m_joint_pos_index_global_right;
}

void CameraCalibQWrapper::set_joint_pos_index_global_right(int newJoint_pos_index_global_right)
{
    m_joint_pos_index_global_right = newJoint_pos_index_global_right;
    if(m_joint_pos_index_global_right < vector2d_jpos_list_global_right_.size())
    {
        QVariantList list_joint_pos;
        for(auto singleJointPos: vector2d_jpos_list_global_right_[m_joint_pos_index_global_right])
        {
            list_joint_pos.append(singleJointPos);
        }
        set_current_joint_pos_global_right(list_joint_pos);
    }
    emit joint_pos_index_global_right_Changed();
}

int CameraCalibQWrapper::joint_pos_total_num_global_right() const
{
    return m_joint_pos_total_num_global_right;
}

void CameraCalibQWrapper::set_joint_pos_total_num_global_right(int newJoint_pos_total_num_global_right)
{
    if (m_joint_pos_total_num_global_right == newJoint_pos_total_num_global_right)
        return;
    m_joint_pos_total_num_global_right = newJoint_pos_total_num_global_right;
    emit joint_pos_total_num_global_right_Changed();
}

QVariantList CameraCalibQWrapper::get_current_joint_pos_global_right() const
{
    return m_current_joint_pos_global_right;
}

void CameraCalibQWrapper::set_current_joint_pos_global_right(const QVariantList &newCurrent_joint_pos_global_right)
{
    if (m_current_joint_pos_global_right == newCurrent_joint_pos_global_right)
        return;
    m_current_joint_pos_global_right = newCurrent_joint_pos_global_right;
    emit current_joint_pos_global_right_changed();
}

int CameraCalibQWrapper::joint_pos_index_following_right() const
{
    return m_joint_pos_index_following_right;
}

void CameraCalibQWrapper::set_joint_pos_index_following_right(int newJoint_pos_index_following_right)
{
    m_joint_pos_index_following_right = newJoint_pos_index_following_right;
    if(m_joint_pos_index_following_right < vector2d_jpos_list_following_right_.size())
    {
        QVariantList list_joint_pos;
        for(auto singleJointPos: vector2d_jpos_list_following_right_[m_joint_pos_index_following_right])
        {
            list_joint_pos.append(singleJointPos);
        }
        set_current_joint_pos_following_right(list_joint_pos);
    }
    emit joint_pos_index_following_right_Changed();
}

int CameraCalibQWrapper::joint_pos_total_num_following_right() const
{
    return m_joint_pos_total_num_following_right;
}

void CameraCalibQWrapper::set_joint_pos_total_num_following_right(int newJoint_pos_total_num_following_right)
{
    if (m_joint_pos_total_num_following_right == newJoint_pos_total_num_following_right)
        return;
    m_joint_pos_total_num_following_right = newJoint_pos_total_num_following_right;
    emit joint_pos_total_num_following_right_Changed();
}

QVariantList CameraCalibQWrapper::get_current_joint_pos_following_right() const
{
    return m_current_joint_pos_following_right;
}

void CameraCalibQWrapper::set_current_joint_pos_following_right(const QVariantList &newCurrent_joint_pos_following_right)
{
    if (m_current_joint_pos_following_right == newCurrent_joint_pos_following_right)
        return;
    m_current_joint_pos_following_right = newCurrent_joint_pos_following_right;
    emit current_joint_pos_following_right_changed();
}

int CameraCalibQWrapper::joint_pos_index_local_left() const
{
    return m_joint_pos_index_local_left;
}

void CameraCalibQWrapper::set_joint_pos_index_local_left(int newJoint_pos_index_local_left)
{
    m_joint_pos_index_local_left = newJoint_pos_index_local_left;
    if(m_joint_pos_index_local_left < vector2d_jpos_list_local_left_.size())
    {
        QVariantList list_joint_pos;
        for(auto singleJointPos: vector2d_jpos_list_local_left_[m_joint_pos_index_local_left])
        {
            list_joint_pos.append(singleJointPos);
        }
        set_current_joint_pos_local_left(list_joint_pos);
    }
    emit joint_pos_index_local_left_Changed();
}

int CameraCalibQWrapper::joint_pos_total_num_local_left() const
{
    return m_joint_pos_total_num_local_left;
}

void CameraCalibQWrapper::set_joint_pos_total_num_local_left(int newJoint_pos_total_num_local_left)
{
    if (m_joint_pos_total_num_local_left == newJoint_pos_total_num_local_left)
        return;
    m_joint_pos_total_num_local_left = newJoint_pos_total_num_local_left;
    emit joint_pos_total_num_local_left_Changed();
}

QVariantList CameraCalibQWrapper::get_current_joint_pos_local_left() const
{
    return m_current_joint_pos_local_left;
}

void CameraCalibQWrapper::set_current_joint_pos_local_left(const QVariantList &newCurrent_joint_pos_local_left)
{
    if (m_current_joint_pos_local_left == newCurrent_joint_pos_local_left)
        return;
    m_current_joint_pos_local_left = newCurrent_joint_pos_local_left;
    emit current_joint_pos_local_left_changed();
}

bool CameraCalibQWrapper::copy_replace_file(QString &path_sourceFile, QString &path_targetFile)
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

void CameraCalibQWrapper::read_jpos_from_file(CameraCalib_Type type, const std::string &file_path, std::vector<std::vector<double> > &vector2d)
{
    QFileInfo fileInfo{QString::fromStdString(file_path)};
    vector2d.clear();
    if(fileInfo.exists())
    {
        parser_.set_double_number_per_line(7).set_File_Encoder_Type(PoseFileParser::EncoderType::UTF_8);
        parser_.parse_double_vector(file_path, vector2d);
        if(vector2d.size())
        {
            qDebug() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "finish parse,\r\nget rows:" << vector2d.size();
            switch (type) {
            case GLOBAL_CAMERA_RIGHT_ARM:
                set_joint_pos_index_global_right(0);
                set_joint_pos_total_num_global_right(vector2d.size());
                break;
            case FOLLOWING_CAMERA_RIGHT_ARM:
                set_joint_pos_index_following_right(0);
                set_joint_pos_total_num_following_right(vector2d.size());
                break;
            case LOCAL_CAMERA_LEFT_ARM:
                set_joint_pos_index_local_left(0);
                set_joint_pos_total_num_local_left(vector2d.size());
                break;
            default :
                break;
            }
        }
        else
            qWarning() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "failed to parse";
    }
    else
    {
        qDebug() << __FUNCTION__  << "- file" << QString::fromStdString(file_path) << "finish parse,\r\nfile deleted!";
    }


}
