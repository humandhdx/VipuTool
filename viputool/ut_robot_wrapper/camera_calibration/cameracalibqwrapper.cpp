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

#include <QThread>
CameraCalibQWrapper::~CameraCalibQWrapper()
{
    qWarning() << __FUNCTION__ << " - 0!";
    QThread::sleep(2);
}

void CameraCalibQWrapper::calibration_resource_load()
{
    set_joint_pos_index_global_right_LeftEye(0);
    set_joint_pos_index_global_right_RightEye(0);
    set_joint_pos_index_global_right_DuelEye(0);
    set_joint_pos_index_following_right(0);
    set_joint_pos_index_local_left(0);
    set_joint_pos_total_num_global_right_LeftEye(0);
    set_joint_pos_total_num_global_right_RightEye(0);
    set_joint_pos_total_num_global_right_DuelEye(0);
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
    set_current_joint_pos_global_right_LeftEye(zero_jointPos);
    set_current_joint_pos_global_right_RightEye(zero_jointPos);
    set_current_joint_pos_global_right_DuelEye(zero_jointPos);
    set_current_joint_pos_following_right(zero_jointPos);
    set_current_joint_pos_local_left(zero_jointPos);

    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(CONFIG_GLOBAL_CAMERA_right_leftEye_arm_joint_pose,
                                                                       std::bind(&CameraCalibQWrapper::read_jpos_from_file, this, CameraCalib_Type::GLOBAL_CAMERA_RIGHT_ARM_LEFT, std::placeholders::_1, std::ref(vector2d_jpos_list_global_right_LeftEye_)));
    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(CONFIG_GLOBAL_CAMERA_right_rightEye_arm_joint_pose,
                                                                       std::bind(&CameraCalibQWrapper::read_jpos_from_file, this, CameraCalib_Type::GLOBAL_CAMERA_RIGHT_ARM_RIGHT, std::placeholders::_1, std::ref(vector2d_jpos_list_global_right_RightEye_)));
    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(CONFIG_GLOBAL_CAMERA_right_duelEye_arm_joint_pose,
                                                                       std::bind(&CameraCalibQWrapper::read_jpos_from_file, this, CameraCalib_Type::GLOBAL_CAMERA_RIGHT_ARM_DUEL, std::placeholders::_1, std::ref(vector2d_jpos_list_global_right_DuelEye_)));
    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(CONFIG_FOLLOWING_CAMERA_right_arm_joint_pose,
                                                                       std::bind(&CameraCalibQWrapper::read_jpos_from_file, this, CameraCalib_Type::FOLLOWING_CAMERA_RIGHT_ARM, std::placeholders::_1, std::ref(vector2d_jpos_list_following_right_)));
    QFileSystemMonitor::instance()->Register_Callback_On_File_Modified(CONFIG_LOCAL_CAMERA_left_arm_joint_pose,
                                                                       std::bind(&CameraCalibQWrapper::read_jpos_from_file, this, CameraCalib_Type::LOCAL_CAMERA_LEFT_ARM, std::placeholders::_1, std::ref(vector2d_jpos_list_local_left_)));
}

void CameraCalibQWrapper::calibration_resource_unload()
{
    QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(CONFIG_GLOBAL_CAMERA_right_arm_joint_pose);
    QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(CONFIG_GLOBAL_CAMERA_right_leftEye_arm_joint_pose);
    QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(CONFIG_GLOBAL_CAMERA_right_rightEye_arm_joint_pose);
    QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(CONFIG_GLOBAL_CAMERA_right_duelEye_arm_joint_pose);
    QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(CONFIG_FOLLOWING_CAMERA_right_arm_joint_pose);
    QFileSystemMonitor::instance()->Deregister_Callback_On_File_Modified(CONFIG_LOCAL_CAMERA_left_arm_joint_pose);
}

bool CameraCalibQWrapper::updata_Joint_Pos_Global_Right_LeftEye(QString source_file_path)
{
    QString target_filepath =  QStr_ABS_PATH(CONFIG_GLOBAL_CAMERA_right_leftEye_arm_joint_pose);
    return copy_replace_file(source_file_path, target_filepath);
}

bool CameraCalibQWrapper::updata_Joint_Pos_Global_Right_RightEye(QString source_file_path)
{
    QString target_filepath =  QStr_ABS_PATH(CONFIG_GLOBAL_CAMERA_right_rightEye_arm_joint_pose);
    return copy_replace_file(source_file_path, target_filepath);
}

bool CameraCalibQWrapper::updata_Joint_Pos_Global_Right_DuelEye(QString source_file_path)
{
    QString target_filepath =  QStr_ABS_PATH(CONFIG_GLOBAL_CAMERA_right_rightEye_arm_joint_pose);
    return copy_replace_file(source_file_path, target_filepath);
}

bool CameraCalibQWrapper::updata_Joint_Pos_Following_Right(QString source_file_path)
{
    QString target_filepath =  QStr_ABS_PATH(CONFIG_FOLLOWING_CAMERA_right_arm_joint_pose);
    return copy_replace_file(source_file_path, target_filepath);
}

bool CameraCalibQWrapper::updata_Joint_Pos_Local_Left(QString source_file_path)
{
    QString target_filepath =  QStr_ABS_PATH(CONFIG_LOCAL_CAMERA_left_arm_joint_pose);
    return copy_replace_file(source_file_path, target_filepath);
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
            case FOLLOWING_CAMERA_RIGHT_ARM:
                set_joint_pos_index_following_right(0);
                set_joint_pos_total_num_following_right(vector2d.size());
                break;
            case LOCAL_CAMERA_LEFT_ARM:
                set_joint_pos_index_local_left(0);
                set_joint_pos_total_num_local_left(vector2d.size());
                break;
            case GLOBAL_CAMERA_RIGHT_ARM_LEFT:
                set_joint_pos_index_global_right_LeftEye(0);
                set_joint_pos_total_num_global_right_LeftEye(vector2d.size());
                break;
            case GLOBAL_CAMERA_RIGHT_ARM_RIGHT:
                set_joint_pos_index_global_right_RightEye(0);
                set_joint_pos_total_num_global_right_RightEye(vector2d.size());
                break;
            case GLOBAL_CAMERA_RIGHT_ARM_DUEL:
                set_joint_pos_index_global_right_DuelEye(0);
                set_joint_pos_total_num_global_right_DuelEye(vector2d.size());
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

int CameraCalibQWrapper::joint_pos_index_global_right_LeftEye() const
{
    return m_joint_pos_index_global_right_LeftEye;
}

void CameraCalibQWrapper::set_joint_pos_index_global_right_LeftEye(int newJoint_pos_index_global_right_LeftEye)
{
    m_joint_pos_index_global_right_LeftEye = newJoint_pos_index_global_right_LeftEye;
    if(m_joint_pos_index_global_right_LeftEye < vector2d_jpos_list_global_right_LeftEye_.size())
    {
        QVariantList list_joint_pos;
        for(auto singleJointPos: vector2d_jpos_list_global_right_LeftEye_[m_joint_pos_index_global_right_LeftEye])
        {
            list_joint_pos.append(singleJointPos);
        }
        set_current_joint_pos_global_right_LeftEye(list_joint_pos);
    }
    emit joint_pos_index_global_right_LeftEye_Changed();
}

int CameraCalibQWrapper::joint_pos_total_num_global_right_LeftEye() const
{
    return m_joint_pos_total_num_global_right_LeftEye;
}

void CameraCalibQWrapper::set_joint_pos_total_num_global_right_LeftEye(int newJoint_pos_total_num_global_right_LeftEye)
{
    if (m_joint_pos_total_num_global_right_LeftEye == newJoint_pos_total_num_global_right_LeftEye)
        return;
    m_joint_pos_total_num_global_right_LeftEye = newJoint_pos_total_num_global_right_LeftEye;
    emit joint_pos_total_num_global_right_LeftEye_Changed();
}

QVariantList CameraCalibQWrapper::get_current_joint_pos_global_right_LeftEye() const
{
    return m_current_joint_pos_global_right_LeftEye;
}

void CameraCalibQWrapper::set_current_joint_pos_global_right_LeftEye(const QVariantList &newCurrent_joint_pos_global_right_LeftEye)
{
    if (m_current_joint_pos_global_right_LeftEye == newCurrent_joint_pos_global_right_LeftEye)
        return;
    m_current_joint_pos_global_right_LeftEye = newCurrent_joint_pos_global_right_LeftEye;
    emit current_joint_pos_global_right_LeftEye_changed();
}

int CameraCalibQWrapper::joint_pos_index_global_right_RightEye() const
{
    return m_joint_pos_index_global_right_RightEye;
}

void CameraCalibQWrapper::set_joint_pos_index_global_right_RightEye(int newJoint_pos_index_global_right_RightEye)
{
    m_joint_pos_index_global_right_RightEye = newJoint_pos_index_global_right_RightEye;
    if(m_joint_pos_index_global_right_RightEye < vector2d_jpos_list_global_right_RightEye_.size())
    {
        QVariantList list_joint_pos;
        for(auto singleJointPos: vector2d_jpos_list_global_right_RightEye_[m_joint_pos_index_global_right_RightEye])
        {
            list_joint_pos.append(singleJointPos);
        }
        set_current_joint_pos_global_right_RightEye(list_joint_pos);
    }
    emit joint_pos_index_global_right_RightEye_Changed();
}

int CameraCalibQWrapper::joint_pos_total_num_global_right_RightEye() const
{
    return m_joint_pos_total_num_global_right_RightEye;
}

void CameraCalibQWrapper::set_joint_pos_total_num_global_right_RightEye(int newJoint_pos_total_num_global_right_RightEye)
{
    if (m_joint_pos_total_num_global_right_RightEye == newJoint_pos_total_num_global_right_RightEye)
        return;
    m_joint_pos_total_num_global_right_RightEye = newJoint_pos_total_num_global_right_RightEye;
    emit joint_pos_total_num_global_right_RightEye_Changed();
}

QVariantList CameraCalibQWrapper::get_current_joint_pos_global_right_RightEye() const
{
    return m_current_joint_pos_global_right_RightEye;
}

void CameraCalibQWrapper::set_current_joint_pos_global_right_RightEye(const QVariantList &newCurrent_joint_pos_global_right_RightEye)
{
    if (m_current_joint_pos_global_right_RightEye == newCurrent_joint_pos_global_right_RightEye)
        return;
    m_current_joint_pos_global_right_RightEye = newCurrent_joint_pos_global_right_RightEye;
    emit current_joint_pos_global_right_RightEye_changed();
}

int CameraCalibQWrapper::joint_pos_index_global_right_DuelEye() const
{
    return m_joint_pos_index_global_right_DuelEye;
}

void CameraCalibQWrapper::set_joint_pos_index_global_right_DuelEye(int newJoint_pos_index_global_right_DuelEye)
{
    m_joint_pos_index_global_right_DuelEye = newJoint_pos_index_global_right_DuelEye;
    if(m_joint_pos_index_global_right_DuelEye < vector2d_jpos_list_global_right_DuelEye_.size())
    {
        QVariantList list_joint_pos;
        for(auto singleJointPos: vector2d_jpos_list_global_right_DuelEye_[m_joint_pos_index_global_right_DuelEye])
        {
            list_joint_pos.append(singleJointPos);
        }
        set_current_joint_pos_global_right_DuelEye(list_joint_pos);
    }
    emit joint_pos_index_global_right_DuelEye_Changed();
}

int CameraCalibQWrapper::joint_pos_total_num_global_right_DuelEye() const
{
    return m_joint_pos_total_num_global_right_DuelEye;
}

void CameraCalibQWrapper::set_joint_pos_total_num_global_right_DuelEye(int newJoint_pos_total_num_global_right_DuelEye)
{
    if (m_joint_pos_total_num_global_right_DuelEye == newJoint_pos_total_num_global_right_DuelEye)
        return;
    m_joint_pos_total_num_global_right_DuelEye = newJoint_pos_total_num_global_right_DuelEye;
    emit joint_pos_total_num_global_right_DuelEye_Changed();
}

QVariantList CameraCalibQWrapper::get_current_joint_pos_global_right_DuelEye() const
{
    return m_current_joint_pos_global_right_DuelEye;
}

void CameraCalibQWrapper::set_current_joint_pos_global_right_DuelEye(const QVariantList &newCurrent_joint_pos_global_right_DuelEye)
{
    if (m_current_joint_pos_global_right_DuelEye == newCurrent_joint_pos_global_right_DuelEye)
        return;
    m_current_joint_pos_global_right_DuelEye = newCurrent_joint_pos_global_right_DuelEye;
    emit current_joint_pos_global_right_DuelEye_changed();
}
