#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QButtonGroup>
#include "utrarobot.h"
#include "FTSensorDriver.hpp"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE
struct MachineSettings {
    int work_mode;        // 工作模式
    int slide_depth;      // 滑动深度
    int slide_velocity;   // 滑动速度
    int spin_velocity;     // 旋转速度
    int spin_revolution;   // 旋转圈数
    int hold_millisecond;  // 持续时间（毫秒）
};
Q_DECLARE_METATYPE(MachineSettings)
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:

private:
    Ui::Widget *ui;
    utraRobot *rbt;

    void left_arm_gravity_test();  //左臂负载测试
    void right_arm_gravity_test(); //右臂负载测试
    void left_arm_accuracy_test(); //左臂精度测试
    void left_arm_workspace_test(); //左臂工作空间测试
    void left_arm_repeatabilitity_test(); //左臂重复性测试
    void right_arm_accuracy_test();  //右臂精度测试
    void right_arm_workspace_test(); //右臂工作空间测试
    void right_arm_repeatabilitity_test(); //右臂重复性测试
    void set_left_tcp_offset();  //左臂设置tcp
    void set_right_tcp_offset(); //右臂设置tcp

    void set_huatai_return_zero();  //设置滑台回零
    void set_huatai_move(); //设置滑台移动

    void set_needle_rotation();

    void set_needle_rotation_stop();

    void callPunchCommandService(const MachineSettings& setting);


    void initArm();
    bool SetAdjustStatus();
    bool setHoldStatusandRecord();
    bool GetStatus(ArmStatus &status);

//    void savedata();
//    void savedataduringRecord();
    void savetcpdata();
    void inputdata();
    void movetoNextPos();
    void movetoNextPosThread();
    void left_arm_gravity_thread();
    void right_arm_gravity_thread();
    void left_arm_accuracy_thread();
    void left_arm_workspace_thread();
    void left_arm_reppeat_thread();
    void right_arm_accuracy_thread();
    void right_arm_workspace_thread();
    void right_arm_repeat_thread();
    void set_left_tcp_thread();
    void set_right_tcp_thread();
    void setStateRecordFTSensorDataOrNot();

    int m_posCounter;
    int m_posIndex;
    std::vector<JointPos> m_PosRecord;
    std::vector<JointPos> m_PosData;
    std::vector<JointPos> m_PosRecord2;
    std::vector<JointPos>::iterator m_it;
    std::vector<TcpPose> m_TcpRecord;

    bool m_NeedRecordFTSensorData;
    std::vector<FTSensorData> m_FTSensorData; //unit:kg,kg.m
    FTSensorDriver *m_PtrFTSensorDevice;
    ArmType m_armChoose;
    QButtonGroup *m_box;
    QButtonGroup *m_box2;
};
#endif // WIDGET_H
