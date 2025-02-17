#ifndef UTRROBOT_QWRAPPER_H
#define UTRROBOT_QWRAPPER_H

#include <QObject>
#include "utrrobot.hpp"
#include <QVariantList>
#include <QTimer>
#include <QList>
#include <QMutex>

class UtraRobot_QWrapper : public QObject, UtrRobot
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

signals:
    //robotIndex:  0->left 1-right
    void update_Robot_Joint_Pos(QVariant jointpos);
    void laserCalib_file_loaded(int pos_number);
    void laserCalib_next_pos_updated(int pos_number);
    void Robot_Drag_Activate(bool activate);
    void Robot_Connection_State_Updated(bool connect);

private:
    QMutex mutext;

    QVariantList convert_JPos_To_VariantList(UtRobotConfig::JointPos& Jpos);
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
};

#endif // UTRROBOT_QWRAPPER_H
