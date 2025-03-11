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
    UtraRobot_QWrapper(UtRobotConfig::TestConfig config, QObject *parent = nullptr);

    Q_INVOKABLE bool robot_connect();

    Q_INVOKABLE bool robot_drag_activate(bool on);

    Q_INVOKABLE bool test_graivty_and_spin();
    Q_INVOKABLE bool test_accuracy_and_spin();
    Q_INVOKABLE bool test_workspace_and_spin();
    Q_INVOKABLE bool test_repeatability_and_spin();

    Q_INVOKABLE bool laserCalib_load_file(QString filePath);
    Q_INVOKABLE bool laserCalib_Robot_MoveTo_NextPos_and_spin();

    Q_INVOKABLE bool PostLaserCalib_Write_MDH_offset();

    Q_INVOKABLE bool Check_MDH_Offset_Parm_All_Zero();
    Q_INVOKABLE bool ZeroOut_MDH_offset();

    Q_INVOKABLE bool Move_To_Joint_Position(QVariantList jointpos);

    Q_PROPERTY( bool arm_connect READ arm_connect WRITE setArm_connect NOTIFY arm_connectChanged FINAL)

    bool arm_connect() const;
    void setArm_connect(bool newArm_connect);

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

};

#endif // UTRROBOT_QWRAPPER_H
