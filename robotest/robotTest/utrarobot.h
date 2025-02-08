#ifndef UTRAROBOT_H
#define UTRAROBOT_H

#include "utra/utra_api_tcp.h"
#include "utra/utra_report_config.h"
#include "utra/utra_report_status.h"
#include <thread>
enum class ArmStatus {
  HOLD,
  ADJUST,
  InMOTION,
  STANDY,
  PAUSING,
  STOPPING,
  OTHER
};
struct JointPos{
    double data[7];
};
struct TcpPose{
  double data[6];
};
enum ArmType{
    left,
    right
};
struct FTSensorData
{
    double data[6];
};
class utraRobot
{
public:

    utraRobot();
    ~utraRobot();
    bool initArm(ArmType type);
    bool SetStatus(ArmStatus st);
    bool GetStatus(ArmStatus &status);
    bool getJointPos(JointPos &jPos);
    bool getTcpPos(TcpPose &pose);
    bool movetoNextPos(JointPos jPos);

    bool left_arm_gravity_test();  //左臂负载测试
    bool right_arm_gravity_test(); //右臂负载测试
    bool left_arm_accuracy_test(); //左臂精度测试
    bool left_arm_workspace_test(); //左臂工作空间测试
    bool left_arm_repeatabilitity_test(); //左臂重复性测试
    bool right_arm_accuracy_test();  //右臂精度测试
    bool right_arm_workspace_test(); //右臂工作空间测试
    bool right_arm_repeatabilitity_test(); //右臂重复性测试
    bool set_left_tcp_offset();  //左臂设置tcp
    bool set_right_tcp_offset(); //右臂设置tcp
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
private:
    UtraApiTcp *ubot;
    UtraApiTcp *ubot_;
    bool isConnect;
    JointPos m_nextPos;
    ArmStatus m_status;
    int opt=0;
    float speed=5.0/57.54;  // rad/s
    float acc=3.0;
    float mvvelo=50.0;   // mm/s
    float mvacc=1000;  // mm/s
    float mvtime=0;
};

#endif // UTRAROBOT_H
