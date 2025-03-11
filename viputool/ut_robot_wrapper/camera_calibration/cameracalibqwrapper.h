#ifndef CAMERACALIBQWRAPPER_H
#define CAMERACALIBQWRAPPER_H

#include <QObject>
#include <QSet>
#include <QTimer>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include "utils/qfilesystemmonitor.hpp"
// #include "ut_robot_wrapper/kinematic_calibration/kinematiccalib_config.hpp"
#include "ut_robot_wrapper/kinematic_calibration/posefileparser.hpp"

class CameraCalibQWrapper : public QObject
{
    Q_OBJECT
public:
    enum CameraCalib_Type{
        GLOBAL_CAMERA_RIGHT_ARM     = 1,
        FOLLOWING_CAMERA_RIGHT_ARM  = 2,
        LOCAL_CAMERA_LEFT_ARM       = 3
    };

    explicit CameraCalibQWrapper(QObject *parent = nullptr);

    Q_INVOKABLE bool Updata_Joint_Pos_Global_Right(QString source_file_path);
    Q_INVOKABLE bool Updata_Joint_Pos_Following_Right(QString source_file_path);
    Q_INVOKABLE bool Updata_Joint_Pos_Local_Left(QString source_file_path);

    Q_PROPERTY( int joint_pos_index_global_right READ joint_pos_index_global_right WRITE set_joint_pos_index_global_right NOTIFY joint_pos_index_global_right_Changed FINAL)
    Q_PROPERTY( int joint_pos_total_num_global_right READ joint_pos_total_num_global_right WRITE set_joint_pos_total_num_global_right NOTIFY joint_pos_total_num_global_right_Changed FINAL)
    Q_PROPERTY( QVariantList current_joint_pos_global_right READ get_current_joint_pos_global_right WRITE set_current_joint_pos_global_right NOTIFY current_joint_pos_global_right_changed)

    Q_PROPERTY( int joint_pos_index_following_right READ joint_pos_index_following_right WRITE set_joint_pos_index_following_right NOTIFY joint_pos_index_following_right_Changed FINAL)
    Q_PROPERTY( int joint_pos_total_num_following_right READ joint_pos_total_num_following_right WRITE set_joint_pos_total_num_following_right NOTIFY joint_pos_total_num_following_right_Changed FINAL)
    Q_PROPERTY( QVariantList current_joint_pos_following_right READ get_current_joint_pos_following_right WRITE set_current_joint_pos_following_right NOTIFY current_joint_pos_following_right_changed)

    Q_PROPERTY( int joint_pos_index_local_left READ joint_pos_index_local_left WRITE set_joint_pos_index_local_left NOTIFY joint_pos_index_local_left_Changed FINAL)
    Q_PROPERTY( int joint_pos_total_num_local_left READ joint_pos_total_num_local_left WRITE set_joint_pos_total_num_local_left NOTIFY joint_pos_total_num_local_left_Changed FINAL)
    Q_PROPERTY( QVariantList current_joint_pos_local_left READ get_current_joint_pos_local_left WRITE set_current_joint_pos_local_left NOTIFY current_joint_pos_local_left_changed)


    int joint_pos_index_global_right() const;
    void set_joint_pos_index_global_right(int newJoint_pos_index_global_right);

    int joint_pos_total_num_global_right() const;
    void set_joint_pos_total_num_global_right(int newJoint_pos_total_num_global_right);

    QVariantList get_current_joint_pos_global_right() const;
    void set_current_joint_pos_global_right(const QVariantList &newCurrent_joint_pos_global_right);

    int joint_pos_index_following_right() const;
    void set_joint_pos_index_following_right(int newJoint_pos_index_following_right);

    int joint_pos_total_num_following_right() const;
    void set_joint_pos_total_num_following_right(int newJoint_pos_total_num_following_right);

    QVariantList get_current_joint_pos_following_right() const;
    void set_current_joint_pos_following_right(const QVariantList &newCurrent_joint_pos_following_right);

    int joint_pos_index_local_left() const;
    void set_joint_pos_index_local_left(int newJoint_pos_index_local_left);

    int joint_pos_total_num_local_left() const;
    void set_joint_pos_total_num_local_left(int newJoint_pos_total_num_local_left);

    QVariantList get_current_joint_pos_local_left() const;
    void set_current_joint_pos_local_left(const QVariantList &newCurrent_joint_pos_local_left);

signals:
    void joint_pos_index_global_right_Changed();
    void joint_pos_total_num_global_right_Changed();

    void current_joint_pos_global_right_changed();

    void joint_pos_index_following_right_Changed();

    void joint_pos_total_num_following_right_Changed();

    void current_joint_pos_following_right_changed();

    void joint_pos_index_local_left_Changed();

    void joint_pos_total_num_local_left_Changed();

    void current_joint_pos_local_left_changed();

private:

    int m_joint_pos_index_global_right;
    int m_joint_pos_total_num_global_right;
    QVariantList m_current_joint_pos_global_right;
    int m_joint_pos_index_following_right;
    int m_joint_pos_total_num_following_right;
    QVariantList m_current_joint_pos_following_right;
    int m_joint_pos_index_local_left;
    int m_joint_pos_total_num_local_left;
    QVariantList m_current_joint_pos_local_left;

private:
    bool copy_replace_file(QString &path_sourceFile, QString &path_targetFile);
    void read_jpos_from_file(CameraCalib_Type type, const std::string& file_path , std::vector<std::vector<double>>& vector2d);

    PoseFileParser parser_;
    //joint pose{radian} list => vector [7][n]
    std::vector<std::vector<double>> vector2d_jpos_list_global_right_;
    std::vector<std::vector<double>> vector2d_jpos_list_following_right_;
    std::vector<std::vector<double>> vector2d_jpos_list_local_left_;
};

#endif // CAMERACALIBQWRAPPER_H
