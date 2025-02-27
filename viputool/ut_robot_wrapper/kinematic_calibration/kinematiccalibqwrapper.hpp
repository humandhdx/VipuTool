#ifndef KINEMATICCALIBQWRAPPER_H
#define KINEMATICCALIBQWRAPPER_H

#include <QObject>
#include "ut_robot_wrapper/kinematic_calibration/kinematiccalibrator.hpp"
#include <QSet>
#include <QTimer>
#include "ut_robot_wrapper/kinematic_calibration/posefileparser.hpp"

class KinematicCalib_QWrapper : public QObject, KinematicCalibrator
{
    Q_OBJECT
public:
    explicit KinematicCalib_QWrapper(QObject *parent = nullptr);

    /* remove the output file and reset mask index list */
    Q_INVOKABLE void reset_kinematic_calib();
    Q_INVOKABLE void add_mask_index_for_position_recorder(uint32_t index_of_position_record);

    Q_INVOKABLE void log_calib_data_ready_info_left();
    Q_INVOKABLE void log_calib_data_ready_info_right();

    Q_INVOKABLE bool KinematicCalib_Start_left();
    Q_INVOKABLE bool KinematicCalib_Start_right();

    Q_INVOKABLE bool Merge_Left_And_Right_Calib_Result();

    Q_INVOKABLE bool Updata_Planned_Robot_Pose_left(QString source_file_path);
    Q_INVOKABLE bool Updata_Planned_Robot_Pose_right(QString source_file_path);

    Q_PROPERTY( bool kinematic_calib_data_ready_left READ kinematic_calib_data_ready_left WRITE set_kinematic_calib_data_ready_left NOTIFY kinematic_calib_data_ready_left_changed FINAL)
    Q_PROPERTY( bool kinematic_calib_data_ready_right READ kinematic_calib_data_ready_right WRITE set_kinematic_calib_data_ready_right NOTIFY kinematic_calib_data_ready_right_changed FINAL)

    Q_PROPERTY( QList<int> lst_masked_robot_pose_index READ get_lst_masked_robot_pose_index WRITE set_lst_masked_robot_pose_index NOTIFY lst_masked_robot_pose_index_changed)

signals:
    void kinematic_calib_data_ready_left_changed();

    void kinematic_calib_data_ready_right_changed();

    void lst_masked_robot_pose_index_changed();

private:
    PoseFileParser parser_;
    QSet<uint32_t> set_masked_indexes_;

    bool copy_replace_file(QString& sourceFile, QString& targetFile);

    void remove_all_output_files();

    void read_frames_from_file(const std::string& file_path , std::vector<std::vector<double>>& vector2d);
    void read_laser_angle_calibrate_from_file(const std::string& file_path , std::vector<std::vector<double>>& vector2d);
    void read_jpos_from_file(const std::string& file_path , std::vector<std::vector<double>>& vector2d);

    bool combine_json_file_kinematics_paramters();
    //TODO: not finished yet!!!!
    bool combine_xml_file_dual_arm_fue_urdf();

    bool m_kinematic_calib_data_ready_left;

    bool m_kinematic_calib_data_ready_right;

    QList<int> m_lst_masked_robot_pose_index;

public:
    bool CheckDataReady_LeftArm();
    bool CheckDataReady_RightArm();

    bool kinematic_calib_data_ready_left() const;
    void set_kinematic_calib_data_ready_left(bool newKinematic_calib_data_ready_left);
    bool kinematic_calib_data_ready_right() const;
    void set_kinematic_calib_data_ready_right(bool newKinematic_calib_data_ready_right);
    QList<int> get_lst_masked_robot_pose_index() const;
    void set_lst_masked_robot_pose_index(const QList<int> &newLst_masked_robot_pose_index);
};

#endif // KINEMATICCALIBQWRAPPER_H
