#ifndef UTRROBOT_QWRAPPER_H
#define UTRROBOT_QWRAPPER_H

#include <QObject>
#include "utrarobot.hpp"
#include <QVariantList>
#include <QTimer>
#include <QList>
#include <QMutex>
#include <QVector2D>

class UtraRobot_QWrapper : public QObject, public UtraRobot
{
    Q_OBJECT
public:
    UtraRobot_QWrapper(UtRobotConfig::TestConfig& config, QObject *parent = nullptr);

    Q_INVOKABLE bool robot_connect();

    Q_INVOKABLE bool robot_drag_activate(bool on);

    Q_INVOKABLE void robot_set_speed_override(double percent);

    Q_INVOKABLE void spin_until_all_action_finished();

    Q_INVOKABLE bool test_graivty_and_spin();
    Q_INVOKABLE bool test_accuracy_and_spin();
    Q_INVOKABLE bool test_workspace_and_spin();
    Q_INVOKABLE bool test_repeatability_and_spin();

    Q_INVOKABLE bool laserCalib_load_file(QString filePath);
    // deprecated, robot laser calibration next position should be fetched from kinematiccalibqwrapper
    Q_INVOKABLE bool laserCalib_Robot_MoveTo_NextPos_and_spin();

    Q_INVOKABLE bool postLaserCalib_Write_MDH_offset();

    Q_INVOKABLE bool check_MDH_Offset_Parm_All_Zero();
    Q_INVOKABLE bool zeroOut_MDH_offset();

    Q_INVOKABLE bool reboot_Robot_And_Wait_Reconnect();

    Q_INVOKABLE bool move_To_Joint_Position(QVariantList jointpos);

    Q_INVOKABLE bool move_To_Joint_Position_Degree(QVariantList jointpos);

    Q_PROPERTY( bool arm_connect READ arm_connect WRITE setArm_connect NOTIFY arm_connectChanged FINAL)

    Q_PROPERTY( bool axis_enable_1 READ axis_enabled_1 NOTIFY axis_enable_1_Changed FINAL)

    Q_PROPERTY( bool axis_enable_2 READ axis_enabled_2 NOTIFY axis_enable_2_Changed FINAL)

    Q_PROPERTY( bool axis_enable_3 READ axis_enabled_3 NOTIFY axis_enable_3_Changed FINAL)

    Q_PROPERTY( bool axis_enable_4 READ axis_enabled_4 NOTIFY axis_enable_4_Changed FINAL)

    Q_PROPERTY( bool axis_enable_5 READ axis_enabled_5 NOTIFY axis_enable_5_Changed FINAL)

    Q_PROPERTY( bool axis_enable_6 READ axis_enabled_6 NOTIFY axis_enable_6_Changed FINAL)

    Q_PROPERTY( bool axis_enable_7 READ axis_enabled_7 NOTIFY axis_enable_7_Changed FINAL)

    bool arm_connect() const;
    void setArm_connect(bool newArm_connect);

    bool axis_enabled_1() const;
    Q_INVOKABLE bool enable_axis_1(bool enable);

    bool axis_enabled_2() const;
    Q_INVOKABLE bool enable_axis_2(bool enable);

    bool axis_enabled_3() const;
    Q_INVOKABLE bool enable_axis_3(bool enable);

    bool axis_enabled_4() const;
    Q_INVOKABLE bool enable_axis_4(bool enable);

    bool axis_enabled_5() const;
    Q_INVOKABLE bool enable_axis_5(bool enable);

    bool axis_enabled_6() const;
    Q_INVOKABLE bool enable_axis_6(bool enable);

    bool axis_enabled_7() const;
    Q_INVOKABLE bool enable_axis_7(bool enable);

signals:
    //joint1(degree), joint2(degree), joint3(degree), joint4(degree), joint5(degree), joint6(degree), joint7(degree)
    void update_Robot_Joint_Pos(QVariantList jointpos);
    //x(mm), y(mm), z(mm), Rx(rad), Ry(rad), Rz(rad)
    void update_Tcp_Cartesian_Pos(QVariantList jointpos);
    void laserCalib_file_loaded(int pos_number);
    void laserCalib_next_pos_updated(int pos_number);
    void Robot_Drag_Activate(bool activate);
    void Robot_Connection_State_Updated(bool connect);

    void arm_connectChanged();
    void robot_Warning_MDH_OFFSET_NOT_ZERO();

    void axis_enable_1_Changed();

    void axis_enable_2_Changed();

    void axis_enable_3_Changed();

    void axis_enable_4_Changed();

    void axis_enable_5_Changed();

    void axis_enable_6_Changed();

    void axis_enable_7_Changed();

private:
    QMutex mutext;

    QVariantList convert_JPos_To_VariantList(UtRobotConfig::JointPos& Jpos);
    QVariantList convert_CartPos_To_VariantList(UtRobotConfig::CartesianPos& CartPos);
    void refresh_Robot_Joint_Pos();
    QTimer timer_refresh_robot_joint_pos_;

    struct Laser_Calibration_Context{
        QString filePath_robot_preteached_Jpose;
        int     total_num_preteached_Jpose{0};
        int     current_index_preteached_Jpose{0};
        QList<UtRobotConfig::JointPos> preteached_Jpose;
    };

    Laser_Calibration_Context context_laser_calib_;

    class QMutexTryLocker {
        QMutex &m_;
        bool locked_;
    public:
        QMutexTryLocker(QMutex &m) : m_(m), locked_(m.tryLock()) {}
        ~QMutexTryLocker() { if (locked_) m_.unlock(); }
        bool isLocked() const { return locked_; }
    };
    bool m_arm_connect=false;

    // 4 == sizeof(AxisGroup_EnableStatus)
    struct AxisGroup_EnableStatus{
        bool        enable_1    : 1;
        bool        enable_2    : 1;
        bool        enable_3    : 1;
        bool        enable_4    : 1;
        bool        enable_5    : 1;
        bool        enable_6    : 1;
        bool        enable_7    : 1;
        uint32_t    placeholder : 25;
    };
    AxisGroup_EnableStatus axis_total_status;

    bool m_axis_enable_1;
    bool m_axis_enable_2;
    bool m_axis_enable_3;
    bool m_axis_enable_4;
    bool m_axis_enable_5;
    bool m_axis_enable_6;
    bool m_axis_enable_7;
};

#endif // UTRROBOT_QWRAPPER_H
