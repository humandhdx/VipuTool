#include "utrarobot.h"
#include "iostream"
#include "string.h"
#include <chrono>

using namespace std::literals::chrono_literals;
utraRobot::utraRobot()
{
    isConnect = false;
    for(int i = 0;i < 7;i++)
    {
        m_nextPos.data[i] = 0.0;
    }
    m_status = ArmStatus::OTHER;

}
utraRobot::~utraRobot()
{
   if(ubot!=nullptr)
    {
        delete ubot;
    }
}

bool utraRobot::initArm(ArmType type)
{
    std::cout<<"initArm"<<std::endl;
    std::string port_name;
    if(type == ArmType::left)
    {
        port_name = "192.168.3.37";
    }else{
        port_name = "192.168.5.39";
    }
    for(int i = 0;i < 7;i++)
    {
        m_nextPos.data[i] = 0.0; // current joint Pos
    }
    char ip[64];
    strcpy(ip, std::string(port_name).data());


        ubot = new UtraApiTcp(ip);
        bool is_unconnected = ubot->is_error();
        if  (is_unconnected)
        {
             std::cerr << "connect fail!"<< std::endl;
            return false;
        }
    std::cout<<"Robot: connection"<<std::endl;
    int ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
    ret = ubot->set_motion_mode(0);
    printf("set_motion_mode   : %d\n", ret);
    ret = ubot->set_motion_enable(8, 1);
    printf("set_motion_enable : %d\n", ret);
    ret = ubot->set_motion_status(0);
    printf("set_motion_status : %d\n", ret);
    std::cout<<std::endl;

    if (ret == 0)
    {
        m_status = ArmStatus::HOLD;
        isConnect = true;
        return true;
    }else{
         isConnect = false;
        return false;
    }
}


bool utraRobot::SetStatus(ArmStatus st)
{
    if(isConnect == true)
    {
        switch (st) {
        case ArmStatus::ADJUST :{
            std::cout<<"Robot: setStatus: ADJUST"<<std::endl;
            int ret;
            ret = ubot->reset_err();
            sleep(1);
            printf("reset_err   : %d\n", ret);
            ret = ubot->set_motion_enable(8, 1);
            sleep(1);
            printf("set_motion_enable : %d\n", ret);
            ret = ubot->set_motion_status(0);
            printf("set_motion_status : %d\n", ret);
            sleep(1);
            //ret = ubot->into_motion_mode_teach();
            ret = ubot->set_motion_mode(2);
            sleep(1);
            printf("set_into_motion_mode_teach: %d\n", ret);
            m_status = ArmStatus::ADJUST;
            std::cout<<std::endl;
            break;
        }
        case ArmStatus::HOLD :{
            std::cout<<"Robot: setStatus HOLD"<<std::endl;

            int ret = ubot->reset_err();
            printf("reset_err   : %d\n", ret);
            ret = ubot->set_motion_mode(0);
            printf("set_motion_mode   : %d\n", ret);
            ret = ubot->set_motion_enable(8, 1);
            printf("set_motion_enable : %d\n", ret);
            ret = ubot->set_motion_status(0);
            printf("set_motion_status : %d\n", ret);
            m_status = ArmStatus::HOLD;
            break;
        }
        default:
            break;
        }
        return true;
    }
    else
    {
        return false;
    }

}
bool utraRobot::GetStatus(ArmStatus &status)
{
    std::cout<<"Robot:GetStatus"<<std::endl;
    if(isConnect == true)
    {
        uint8_t robotstatus = 1;
        ubot->get_motion_mode(&robotstatus);
        switch (robotstatus)
        {
        case 1:
            {
            m_status = ArmStatus::InMOTION;
            break;
            }
        case 2:
            {
            m_status = ArmStatus::HOLD;
            break;
            }
        case 3:
            {
            m_status = ArmStatus::PAUSING;
            break;
            }
        case 4:
            {
            m_status = ArmStatus::STOPPING;
            break;
            }
        default:
            m_status = ArmStatus::OTHER;
            break;
        }

    }
    status = m_status;
    return true;
}
bool utraRobot::getJointPos(JointPos &jPos)
{
    std::cout<<"getJointPos:"<<std::endl;

    float value_fp[12];
    int ret;
    std::cout<<"12345"<<std::endl;
    ret = ubot->get_joint_target_pos(value_fp);
    std::cout<<"12345"<<std::endl;
    for(int i = 0;i < 7; i++)
    {
        jPos.data[i] = value_fp[i];
    }
     std::cout<<"12345"<<std::endl;
    return true;
}

bool utraRobot::getTcpPos(TcpPose &pose){
    std::cout<<"getTcpPos:"<<std::endl;
    float value_fp1[12];
    int ret = ubot->get_tcp_target_pos(value_fp1);
    for(int i = 0;i < 6; i++)
    {
        pose.data[i] = double(value_fp1[i]);
        std::cout << value_fp1[i] << std::endl;
    }
     std::cout<<"getTcpPosEnd"<<std::endl;
    return true;
}


bool utraRobot::movetoNextPos(JointPos jPos)
{
    std::cout<<"robot: move to pos:";
    float joint[7]={0};
    m_nextPos = jPos;
    for(int i = 0;i < 7;i++)
    {
        std::cout<<m_nextPos.data[i]<<",";
        joint[i] = jPos.data[i];
    }
    std::cout<<std::endl;
    float speed = 20.0 / 57.296;
    float acc = 3;
    int ret;
    uint8_t ubotstatus = 1;
    ret = ubot->moveto_joint_p2p(joint, speed, acc, 0);
    printf("moveto_joint_p2p  : %d\n", ret);
    while (ubotstatus == 1) {
       std::this_thread::sleep_for(100ms);
       ubot->get_motion_status(&ubotstatus);
       std::cout << "moving to Pos... " << ubotstatus
                 << std::endl;
     }
    return true;
}

bool utraRobot::left_arm_gravity_test()
{
  char ip[64] = "192.168.3.37";
 // arm_report_status_t rx;
  UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
  float posjointL[7] = {78.3,85.3,-110,-120,2,49.1,87.9};
  for (size_t i = 0; i < 7; i++) {
    posjointL[i] = posjointL[i] * (M_PI / 180.0);
  }
 uint8_t ubotstatus_left = 1;
  int ret = ubot->moveto_joint_p2p(posjointL, speed, acc, 0);
  printf("moveto_cartesian_p2p  : %d\n", ret);
  ubotstatus_left = 1;
  while (ubotstatus_left == 1) {
    std::this_thread::sleep_for(1000ms);
    ubot->get_motion_status(&ubotstatus_left);
    std::cout << "Moving to......" << std::endl;
  }
  std::cout << "Joint moving done!" << std::endl;
  sleep(2);
  delete utra_report;
  return true;
}

bool utraRobot::right_arm_gravity_test()
{
  char ip[64] = "192.168.5.39";

  //arm_report_status_t rx;
  //UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
  float posjointL[7] = {-80.4,-95.1,120.8,123.4,-14.3,-51.3,-61.8};
  for (size_t i = 0; i < 7; i++) {
    posjointL[i] = posjointL[i] * (M_PI / 180.0);
  }
 uint8_t ubotstatus_left = 1;
  int ret = ubot->moveto_joint_p2p(posjointL, speed, acc, 0);
  printf("moveto_cartesian_p2p  : %d\n", ret);
  ubotstatus_left = 1;
  while (ubotstatus_left == 1) {
    std::this_thread::sleep_for(1000ms);
    ubot->get_motion_status(&ubotstatus_left);
    std::cout << "Moving to......" << std::endl;
  }
  std::cout << "Joint moving done!" << std::endl;
  sleep(2);
  //delete utra_report;
  return true;
}

bool utraRobot::left_arm_accuracy_test()
{
  char ip[64] = "192.168.3.37";

  arm_report_status_t rx;
  UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
  // utra_report->get_data(&rx);
  float pos_init[6];
  float pos1[6], pos2[6], pos3[6], pos4[6], pos5[6], pos6[6], pos7[6], pos8[6];
  float posjointL[7] = {91.910320, 69.493276, -90.623914, -124.103321, -27.947194, 59.642553, 119.9};
  float posjointL2[7] = {85.1,78.1,-118.6,-93.3,-11.2,-8.1,61.7};
  for (size_t i = 0; i < 7; i++) {
    posjointL[i] = posjointL[i] * (M_PI / 180.0);
  }

  uint8_t ubotstatus_left = 1;
  int ret = ubot->moveto_joint_p2p(posjointL, speed, acc, 0);
  printf("moveto_cartesian_p2p  : %d\n", ret);
  ubotstatus_left = 1;
  while (ubotstatus_left == 1) {
    std::this_thread::sleep_for(1000ms);
    ubot->get_motion_status(&ubotstatus_left);
    std::cout << "Moving to......" << std::endl;
  }
  std::cout << "Joint moving done!" << std::endl;
  sleep(2);
  utra_report->get_data(&rx);
  for (int i = 0; i < 6; i++) {
    pos_init[i] = rx.pose[i];
  }
  for (int i = 0; i < 6; i++) {
    pos1[i] = pos_init[i];
    pos2[i] = pos_init[i];
    pos3[i] = pos_init[i];
    pos4[i] = pos_init[i];
    pos5[i] = pos_init[i];
    pos6[i] = pos_init[i];
    pos7[i] = pos_init[i];
    pos8[i] = pos_init[i];
  }
  pos1[0] = 313.870513;  // 1 lx:313.870513, ly:-11.825159,lz:249.483245
  pos1[1] = -11.825159;
  pos1[2] = 249.483245;

  pos2[0] = 400.473053;  // 5 lx:400.473053, ly:-11.825159,lz:299.483245
  pos2[1] = -11.825159;
  pos2[2] = 299.483245;

  pos3[0] = 400.473053;  // 7 lx:400.473053, ly:88.174841,lz:299.483245
  pos3[1] = 88.174841;
  pos3[2] = 299.483245;

  pos4[0] = 313.870513;  // 3 lx:313.870513, ly:88.174841,lz:249.483245
  pos4[1] = 88.174841;
  pos4[2] = 249.483245;

  pos5[0] = 213.870513;  // 4 lx:213.870513, ly:88.174841,lz:422.688325
  pos5[1] = 88.174841;
  pos5[2] = 422.688325;

  pos6[0] = 213.870513;  // 2 lx:213.870513, ly:-11.825159,lz:422.688325
  pos6[1] = -11.825159;
  pos6[2] = 422.688325;

  pos7[0] = 300.473053;  // 6 lx:300.473053, ly:-11.825159,lz:472.688325
  pos7[1] = -11.825159;
  pos7[2] = 472.688325;

  pos8[0] = 300.473053;  // 8 lx:300.473053, ly:88.174841,lz:472.688325,
  pos8[1] = 88.174841;
  pos8[2] = 472.688325;

  std::cout << "pose init: " << pos_init[0] << ", " << pos_init[1] << ", " << pos_init[2] << std::endl;
  std::cout << "pose init deg: " << pos_init[3] / (M_PI / 180) << ", " << pos_init[4] / (M_PI / 180) << ", "
            << pos_init[5] / (M_PI / 180) << std::endl;

  std::cout << "pose1: " << pos1[0] << ", " << pos1[1] << ", " << pos1[2] << std::endl;
  std::cout << "pose2: " << pos2[0] << ", " << pos2[1] << ", " << pos2[2] << std::endl;
  std::cout << "pose3: " << pos3[0] << ", " << pos3[1] << ", " << pos3[2] << std::endl;
  std::cout << "pose4: " << pos4[0] << ", " << pos4[1] << ", " << pos4[2] << std::endl;
  std::cout << "pose5: " << pos5[0] << ", " << pos5[1] << ", " << pos5[2] << std::endl;
  std::cout << "pose6: " << pos6[0] << ", " << pos6[1] << ", " << pos6[2] << std::endl;
  std::cout << "pose7: " << pos7[0] << ", " << pos7[1] << ", " << pos7[2] << std::endl;
  std::cout << "pose8: " << pos8[0] << ", " << pos8[1] << ", " << pos8[2] << std::endl;
  uint8_t ubotstatus_right;
  // while (true) {

  ret = ubot->moveto_cartesian_line(pos1, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 1 done!" << std::endl;

  sleep(10);

  ret = ubot->moveto_cartesian_line(pos2, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 2 done!" << std::endl;

  sleep(10);

    ret = ubot->moveto_cartesian_line(pos3, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 3 done!" << std::endl;

  sleep(10);

  ret = ubot->moveto_cartesian_line(pos4, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 4 done!" << std::endl;

  sleep(10);

  ret = ubot->moveto_cartesian_line(pos5, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 5 done!" << std::endl;

  sleep(10);
  ret = ubot->moveto_cartesian_line(pos6, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 6 done!" << std::endl;

  sleep(10);
    
  ret = ubot->moveto_cartesian_line(pos7, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos3, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 7 done!" << std::endl;

  sleep(10);
  ret = ubot->moveto_cartesian_line(pos8, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos3, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 8 done!" << std::endl;

  sleep(10);

 ret = ubot->moveto_joint_p2p(posjointL, speed, acc, 0);
  printf("moveto_cartesian_p2p  : %d\n", ret);
  while (ubotstatus_left == 1) {
    std::this_thread::sleep_for(1000ms);
    ubot->get_motion_status(&ubotstatus_left);
    std::cout << "Moving to......" << std::endl;
  }
  std::cout << "Joint moving done!" << std::endl;
  sleep(2);

  

  printf("finish!");
  // }
  delete utra_report;
  return true;
}
bool utraRobot::set_left_tcp_offset()
{
  char ip[64] = "192.168.3.37";

  arm_report_status_t rx;
  //UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
  float configure_tcp[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  int ret = ubot->set_tcp_offset(configure_tcp);
  std::this_thread::sleep_for(50ms);
     ret = ubot->saved_parm();
    std::this_thread::sleep_for(50ms);
  //delete utra_report;
  return true;
}

bool utraRobot::set_right_tcp_offset()
{
  char ip[64] = "192.168.5.39";

  arm_report_status_t rx;
  UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
  float configure_tcp[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  int ret = ubot->set_tcp_offset(configure_tcp);
  std::this_thread::sleep_for(50ms);
     ret = ubot->saved_parm();
    std::this_thread::sleep_for(50ms);
  delete utra_report;
  return true;
}

bool utraRobot::left_arm_workspace_test()
{
  char ip[64] = "192.168.3.37";

  arm_report_status_t rx;
  UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
  // utra_report->get_data(&rx);
  float pos_init[6];
  float pos1[6], pos2[6], pos3[6], pos4[6], pos5[6], pos6[6], pos7[6], pos8[6];
  float posjointL[7] = {91.910320, 69.493276, -90.623914, -124.103321, -27.947194, 59.642553, 119.9};
  float posjointL2[7] = {85.1,78.1,-118.6,-93.3,-11.2,-8.1,61.7};
  for (size_t i = 0; i < 7; i++) {
    posjointL[i] = posjointL[i] * (M_PI / 180.0);
  }

  uint8_t ubotstatus_left = 1;
  int ret = ubot->moveto_joint_p2p(posjointL, speed, acc, 0);
  printf("moveto_cartesian_p2p  : %d\n", ret);
  ubotstatus_left = 1;
  while (ubotstatus_left == 1) {
    std::this_thread::sleep_for(1000ms);
    ubot->get_motion_status(&ubotstatus_left);
    std::cout << "Moving to......" << std::endl;
  }
  std::cout << "Joint moving done!" << std::endl;
  sleep(2);
  utra_report->get_data(&rx);
  for (int i = 0; i < 6; i++) {
    pos_init[i] = rx.pose[i];
  }
  for (int i = 0; i < 6; i++) {
    pos1[i] = pos_init[i];
    pos2[i] = pos_init[i];
    pos3[i] = pos_init[i];
    pos4[i] = pos_init[i];
    pos5[i] = pos_init[i];
    pos6[i] = pos_init[i];
    pos7[i] = pos_init[i];
    pos8[i] = pos_init[i];
  }
  pos1[0] = 309.540386;  // 1 lx:313.870513, ly:-11.825159,lz:249.483245
  pos1[1] = -16.825159;
  pos1[2] = 246.983245;

  pos2[0] = 404.80318;  // 5 lx:400.473053, ly:-11.825159,lz:299.483245
  pos2[1] = -16.825159;
  pos2[2] = 296.983245;

  pos3[0] = 404.80318;  // 7 lx:400.473053, ly:88.174841,lz:299.483245
  pos3[1] = 93.174841;
  pos3[2] = 296.983245;

  pos4[0] = 309.540386;  // 3 lx:313.870513, ly:88.174841,lz:249.483245
  pos4[1] = 93.174841;
  pos4[2] = 246.983245;

  pos5[0] = 209.540386;  // 4 lx:213.870513, ly:88.174841,lz:422.688325
  pos5[1] = 93.174841;
  pos5[2] = 425.188325;

  pos6[0] = 209.540386;  // 2 lx:213.870513, ly:-11.825159,lz:422.688325
  pos6[1] = -16.825159;
  pos6[2] = 425.188325;

  pos7[0] = 304.80318;  // 6 lx:300.473053, ly:-11.825159,lz:472.688325
  pos7[1] = -16.825159;
  pos7[2] = 475.188325;

  pos8[0] = 304.80318;  // 8 lx:300.473053, ly:88.174841,lz:472.688325,
  pos8[1] = 93.174841;
  pos8[2] = 475.188325;

  std::cout << "pose init: " << pos_init[0] << ", " << pos_init[1] << ", " << pos_init[2] << std::endl;
  std::cout << "pose init deg: " << pos_init[3] / (M_PI / 180) << ", " << pos_init[4] / (M_PI / 180) << ", "
            << pos_init[5] / (M_PI / 180) << std::endl;

  std::cout << "pose1: " << pos1[0] << ", " << pos1[1] << ", " << pos1[2] << std::endl;
  std::cout << "pose2: " << pos2[0] << ", " << pos2[1] << ", " << pos2[2] << std::endl;
  std::cout << "pose3: " << pos3[0] << ", " << pos3[1] << ", " << pos3[2] << std::endl;
  std::cout << "pose4: " << pos4[0] << ", " << pos4[1] << ", " << pos4[2] << std::endl;
  std::cout << "pose5: " << pos5[0] << ", " << pos5[1] << ", " << pos5[2] << std::endl;
  std::cout << "pose6: " << pos6[0] << ", " << pos6[1] << ", " << pos6[2] << std::endl;
  std::cout << "pose7: " << pos7[0] << ", " << pos7[1] << ", " << pos7[2] << std::endl;
  std::cout << "pose8: " << pos8[0] << ", " << pos8[1] << ", " << pos8[2] << std::endl;
  uint8_t ubotstatus_right;
  // while (true) {

  ret = ubot->moveto_cartesian_line(pos1, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 1 done!" << std::endl;

  sleep(10);

  ret = ubot->moveto_cartesian_line(pos2, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 2 done!" << std::endl;

  sleep(10);

    ret = ubot->moveto_cartesian_line(pos3, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 3 done!" << std::endl;

  sleep(10);

  ret = ubot->moveto_cartesian_line(pos4, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 4 done!" << std::endl;

  sleep(10);

  ret = ubot->moveto_cartesian_line(pos5, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 5 done!" << std::endl;

  sleep(10);
  ret = ubot->moveto_cartesian_line(pos6, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 6 done!" << std::endl;

  sleep(10);
    
  ret = ubot->moveto_cartesian_line(pos7, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos3, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 7 done!" << std::endl;

  sleep(10);
  ret = ubot->moveto_cartesian_line(pos8, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos3, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 8 done!" << std::endl;

  sleep(10);

 ret = ubot->moveto_joint_p2p(posjointL, speed, acc, 0);
  printf("moveto_cartesian_p2p  : %d\n", ret);
  while (ubotstatus_left == 1) {
    std::this_thread::sleep_for(1000ms);
    ubot->get_motion_status(&ubotstatus_left);
    std::cout << "Moving to......" << std::endl;
  }
  std::cout << "Joint moving done!" << std::endl;
  sleep(2);

  

  printf("finish!");
  // }
  delete utra_report;
  return true;
}


bool utraRobot::left_arm_repeatabilitity_test()
{
 char ip[64] = "192.168.3.37";

  arm_report_status_t rx;
  UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
  // utra_report->get_data(&rx);
  float pos_init[6];
  float pos1[6], pos2[6], pos3[6], pos4[6], pos5[6], pos6[6], pos7[6], pos8[6];
  float posjointL[7] = {91.910320, 69.493276, -90.623914, -124.103321, -27.947194, 59.642553, 119.9};
  float posjointL2[7] = {85.1,78.1,-118.6,-93.3,-11.2,-8.1,61.7};
  for (size_t i = 0; i < 7; i++) {
    posjointL[i] = posjointL[i] * (M_PI / 180.0);
  }
  for (size_t i = 0; i < 7; i++) {
    posjointL2[i] = posjointL2[i] * (M_PI / 180.0);
  }
  uint8_t ubotstatus_left = 1;
  int ret = ubot->moveto_joint_p2p(posjointL, speed, acc, 0);
  printf("moveto_cartesian_p2p  : %d\n", ret);
  ubotstatus_left = 1;
  while (ubotstatus_left == 1) {
    std::this_thread::sleep_for(1000ms);
    ubot->get_motion_status(&ubotstatus_left);
    std::cout << "Moving to......" << std::endl;
  }
  std::cout << "Joint moving done!" << std::endl;
  sleep(2);
  utra_report->get_data(&rx);
  for (int i = 0; i < 6; i++) {
    pos_init[i] = rx.pose[i];
  }
  for (int i = 0; i < 6; i++) {
    pos1[i] = pos_init[i];
    pos2[i] = pos_init[i];
    pos3[i] = pos_init[i];
    pos4[i] = pos_init[i];
    pos5[i] = pos_init[i];
    pos6[i] = pos_init[i];
    pos7[i] = pos_init[i];
    pos8[i] = pos_init[i];
  }
  pos1[0] = 313.870513;  // 1 lx:313.870513, ly:-11.825159,lz:249.483245
  pos1[1] = -11.825159;
  pos1[2] = 249.483245;

  pos2[0] = 400.473053;  // 5 lx:400.473053, ly:-11.825159,lz:299.483245
  pos2[1] = -11.825159;
  pos2[2] = 299.483245;

  pos3[0] = 400.473053;  // 7 lx:400.473053, ly:88.174841,lz:299.483245
  pos3[1] = 88.174841;
  pos3[2] = 299.483245;

  pos4[0] = 313.870513;  // 3 lx:313.870513, ly:88.174841,lz:249.483245
  pos4[1] = 88.174841;
  pos4[2] = 249.483245;

  pos5[0] = 213.870513;  // 4 lx:213.870513, ly:88.174841,lz:422.688325
  pos5[1] = 88.174841;
  pos5[2] = 422.688325;

  pos6[0] = 213.870513;  // 2 lx:213.870513, ly:-11.825159,lz:422.688325
  pos6[1] = -11.825159;
  pos6[2] = 422.688325;

  pos7[0] = 300.473053;  // 6 lx:300.473053, ly:-11.825159,lz:472.688325
  pos7[1] = -11.825159;
  pos7[2] = 472.688325;

  pos8[0] = 300.473053;  // 8 lx:300.473053, ly:88.174841,lz:472.688325,
  pos8[1] = 88.174841;
  pos8[2] = 472.688325;

  std::cout << "pose init: " << pos_init[0] << ", " << pos_init[1] << ", " << pos_init[2] << std::endl;
  std::cout << "pose init deg: " << pos_init[3] / (M_PI / 180) << ", " << pos_init[4] / (M_PI / 180) << ", "
            << pos_init[5] / (M_PI / 180) << std::endl;

  std::cout << "pose1: " << pos1[0] << ", " << pos1[1] << ", " << pos1[2] << std::endl;
  std::cout << "pose2: " << pos2[0] << ", " << pos2[1] << ", " << pos2[2] << std::endl;
  std::cout << "pose3: " << pos3[0] << ", " << pos3[1] << ", " << pos3[2] << std::endl;
  std::cout << "pose4: " << pos4[0] << ", " << pos4[1] << ", " << pos4[2] << std::endl;
  std::cout << "pose5: " << pos5[0] << ", " << pos5[1] << ", " << pos5[2] << std::endl;
  std::cout << "pose6: " << pos6[0] << ", " << pos6[1] << ", " << pos6[2] << std::endl;
  std::cout << "pose7: " << pos7[0] << ", " << pos7[1] << ", " << pos7[2] << std::endl;
  std::cout << "pose8: " << pos8[0] << ", " << pos8[1] << ", " << pos8[2] << std::endl;
  uint8_t ubotstatus_right;
  // while (true) {
 for (int i = 0; i < 5; ++i) {
  ret = ubot->moveto_cartesian_line(pos2, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 2 done!" << std::endl;

  sleep(10);

  ret = ubot->moveto_cartesian_line(pos7, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 7 done!" << std::endl;

  sleep(10);
  ret = ubot->moveto_joint_p2p(posjointL, speed, acc, 0);
  printf("moveto_cartesian_p2p  : %d\n", ret);
  while (ubotstatus_left == 1) {
    std::this_thread::sleep_for(1000ms);
    ubot->get_motion_status(&ubotstatus_left);
    std::cout << "Moving to......" << std::endl;
  }
  std::cout << "Joint moving done!" << std::endl;
  sleep(2);
 }
  ret = ubot->moveto_joint_p2p(posjointL2, speed, acc, 0);
  printf("moveto_cartesian_p2p  : %d\n", ret);
  while (ubotstatus_left == 1) {
    std::this_thread::sleep_for(1000ms);
    ubot->get_motion_status(&ubotstatus_left);
    std::cout << "Moving to......" << std::endl;
  }
  std::cout << "Joint moving done!" << std::endl;
  sleep(1);
  printf("finish!");
  // }
  delete utra_report;
  return true;
}
bool utraRobot::right_arm_accuracy_test()
{
  char ip[64] = "192.168.5.39";
  arm_report_status_t rx;
  UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
  // utra_report->get_data(&rx);
  float pos_init[6];
  float pos1[6], pos2[6], pos3[6], pos4[6], pos5[6], pos6[6], pos7[6], pos8[6];

  float posjointR[7] = {-115.913370, -85.959911, 125.752660, 120.587977, 17.698261, -77.364478, -69.737722};
  float posjointR2[7] ={-86.9,-78.4,118,84.4,13.5,2.6,-69.5};
  for (size_t i = 0; i < 7; i++) {
    posjointR[i] = posjointR[i] * (M_PI / 180.0);
  }
  for (size_t i = 0; i < 7; i++) {
    posjointR2[i] = posjointR2[i] * (M_PI / 180.0);
  }
  uint8_t ubotstatus_left = 1;
  int ret = ubot->moveto_joint_p2p(posjointR, speed, acc, 0);
  printf("moveto_cartesian_p2p  : %d\n", ret);
  ubotstatus_left = 1;
  while (ubotstatus_left == 1) {
    std::this_thread::sleep_for(1000ms);
    ubot->get_motion_status(&ubotstatus_left);
    std::cout << "Moving to......" << std::endl;
  }
  std::cout << "Joint moving done!" << std::endl;
  sleep(2);
  utra_report->get_data(&rx);
  for (int i = 0; i < 6; i++) {
    pos_init[i] = rx.pose[i];
  }
  for (int i = 0; i < 6; i++) {
    pos1[i] = pos_init[i];
    pos2[i] = pos_init[i];
    pos3[i] = pos_init[i];
    pos4[i] = pos_init[i];
    pos5[i] = pos_init[i];
    pos6[i] = pos_init[i];
    pos7[i] = pos_init[i];
    pos8[i] = pos_init[i];
  }
  pos1[0] = 158.300209;  // 1 rx:158.300209, ry:-188.025544,rz:373.062064
  pos1[1] = -188.025544;
  pos1[2] = 373.062064;

  pos2[0] = 244.902749;  // 5 rx:244.902749, ry:-188.025544,rz:423.062064
  pos2[1] = -188.025544;
  pos2[2] = 423.062064;

  pos3[0] = 244.902749;  // 7 rx:244.902749, ry:-88.025544,rz:423.062064
  pos3[1] = -88.025544;
  pos3[2] = 423.062064;

  pos4[0] = 158.300209;  // 3 rx:158.300209, ry:-88.025544,rz:373.062064
  pos4[1] = -88.025544;
  pos4[2] = 373.062064;

  pos5[0] = 58.300209;  // 4 rx:58.300209, ry:-88.025544,rz:546.267144
  pos5[1] = -88.025544;
  pos5[2] = 546.267144;

  pos6[0] = 58.300209;  // 2 rx:58.300209, ry:-188.025544,rz:546.267144
  pos6[1] = -188.025544;
  pos6[2] = 546.267144;

  pos7[0] = 144.902749;  // 6 rx:144.902749, ry:-188.025544,rz:596.267144
  pos7[1] = -188.025544;
  pos7[2] = 596.267144;

  pos8[0] = 144.902749;  // 8 rx:144.902749, ry:-88.025544,rz:596.267144
  pos8[1] = -88.025544;
  pos8[2] = 596.267144;

  std::cout << "pose init: " << pos_init[0] << ", " << pos_init[1] << ", " << pos_init[2] << std::endl;
  std::cout << "pose init deg: " << pos_init[3] / (M_PI / 180) << ", " << pos_init[4] / (M_PI / 180) << ", "
            << pos_init[5] / (M_PI / 180) << std::endl;

  std::cout << "pose1: " << pos1[0] << ", " << pos1[1] << ", " << pos1[2] << std::endl;
  std::cout << "pose2: " << pos2[0] << ", " << pos2[1] << ", " << pos2[2] << std::endl;
  std::cout << "pose3: " << pos3[0] << ", " << pos3[1] << ", " << pos3[2] << std::endl;
  std::cout << "pose4: " << pos4[0] << ", " << pos4[1] << ", " << pos4[2] << std::endl;
  std::cout << "pose5: " << pos5[0] << ", " << pos5[1] << ", " << pos5[2] << std::endl;
  std::cout << "pose6: " << pos6[0] << ", " << pos6[1] << ", " << pos6[2] << std::endl;
  std::cout << "pose7: " << pos7[0] << ", " << pos7[1] << ", " << pos7[2] << std::endl;
  std::cout << "pose8: " << pos8[0] << ", " << pos8[1] << ", " << pos8[2] << std::endl;
  uint8_t ubotstatus_right;
  // while (true) {

  ret = ubot->moveto_cartesian_line(pos1, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 1 done!" << std::endl;

  sleep(10);

  ret = ubot->moveto_cartesian_line(pos2, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 2 done!" << std::endl;

  sleep(10);

  ret = ubot->moveto_cartesian_line(pos3, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos3, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 3 done!" << std::endl;

  sleep(10);

  ret = ubot->moveto_cartesian_line(pos4, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 4 done!" << std::endl;

  sleep(10);
  ret = ubot->moveto_cartesian_line(pos5, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 5 done!" << std::endl;

  sleep(10);
  ret = ubot->moveto_cartesian_line(pos6, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 6 done!" << std::endl;

  sleep(10);
    ret = ubot->moveto_cartesian_line(pos7, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 7 done!" << std::endl;

  sleep(10);
    ret = ubot->moveto_cartesian_line(pos8, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 8 done!" << std::endl;

  sleep(10);
  ret = ubot->moveto_joint_p2p(posjointR, speed, acc, 0);
  printf("moveto_joint_p2p  : %d\n", ret);
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
  }
  sleep(2);
  // ret = ubot->moveto_joint_p2p(posjointR2, speed, acc, 0);
  // printf("moveto_joint_p2p  : %d\n", ret);
  // ubotstatus_right = 1;
  // while (ubotstatus_right == 1) {
  //   std::this_thread::sleep_for(100ms);
  //   ubot->get_motion_status(&ubotstatus_right);

  //   // std::cout << "Moving to......" << std::endl;
  // }
  // sleep(1);

  printf("finish!");
  // }
  delete utra_report;
 return true; 
}

bool utraRobot::right_arm_workspace_test()
{
  char ip[64] = "192.168.5.39";
  arm_report_status_t rx;
 UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
  // utra_report->get_data(&rx);
  float pos_init[6];
  float pos1[6], pos2[6], pos3[6], pos4[6], pos5[6], pos6[6], pos7[6], pos8[6];

  float posjointR[7] = {-115.913370, -85.959911, 125.752660, 120.587977, 17.698261, -77.364478, -69.737722};
  float posjointR2[7] ={-86.9,-78.4,118,84.4,13.5,2.6,-69.5};
  for (size_t i = 0; i < 7; i++) {
    posjointR[i] = posjointR[i] * (M_PI / 180.0);
  }
  for (size_t i = 0; i < 7; i++) {
    posjointR2[i] = posjointR2[i] * (M_PI / 180.0);
  }
  uint8_t ubotstatus_left = 1;
  int ret = ubot->moveto_joint_p2p(posjointR, speed, acc, 0);
  printf("moveto_cartesian_p2p  : %d\n", ret);
  ubotstatus_left = 1;
  while (ubotstatus_left == 1) {
    std::this_thread::sleep_for(1000ms);
    ubot->get_motion_status(&ubotstatus_left);
    std::cout << "Moving to......" << std::endl;
  }
  std::cout << "Joint moving done!" << std::endl;
  sleep(2);
  utra_report->get_data(&rx);
  for (int i = 0; i < 6; i++) {
    pos_init[i] = rx.pose[i];
  }
  for (int i = 0; i < 6; i++) {
    pos1[i] = pos_init[i];
    pos2[i] = pos_init[i];
    pos3[i] = pos_init[i];
    pos4[i] = pos_init[i];
    pos5[i] = pos_init[i];
    pos6[i] = pos_init[i];
    pos7[i] = pos_init[i];
    pos8[i] = pos_init[i];
  }
  pos1[0] = 158.300209-4.330127;  // 1 rx:158.300209, ry:-188.025544,rz:373.062064
  pos1[1] = -188.025544-5;
  pos1[2] = 373.062064-2.5;

  pos2[0] = 244.902749+4.330127;  // 5 rx:244.902749, ry:-188.025544,rz:423.062064
  pos2[1] = -188.025544-5;
  pos2[2] = 423.062064-2.5;

  pos3[0] = 244.902749+4.330127;  // 7 rx:244.902749, ry:-88.025544,rz:423.062064
  pos3[1] = -88.025544+5;
  pos3[2] = 423.062064-5;

  pos4[0] = 158.300209-4.330127;  // 3 rx:158.300209, ry:-88.025544,rz:373.062064
  pos4[1] = -88.025544+5;
  pos4[2] = 373.062064-2.5;

  pos5[0] = 58.300209-4.330127;  // 4 rx:58.300209, ry:-88.025544,rz:546.267144
  pos5[1] = -88.025544+5;
  pos5[2] = 546.267144+2.5;

  pos6[0] = 58.300209-4.330127;  // 2 rx:58.300209, ry:-188.025544,rz:546.267144
  pos6[1] = -188.025544-5;
  pos6[2] = 546.267144+2.5;

  pos7[0] = 144.902749+4.330127;  // 6 rx:144.902749, ry:-188.025544,rz:596.267144
  pos7[1] = -188.025544-5;
  pos7[2] = 596.267144+2.5;

  pos8[0] = 144.902749+4.330127;  // 8 rx:144.902749, ry:-88.025544,rz:596.267144
  pos8[1] = -88.025544+5;
  pos8[2] = 596.267144+2.5;

  std::cout << "pose init: " << pos_init[0] << ", " << pos_init[1] << ", " << pos_init[2] << std::endl;
  std::cout << "pose init deg: " << pos_init[3] / (M_PI / 180) << ", " << pos_init[4] / (M_PI / 180) << ", "
            << pos_init[5] / (M_PI / 180) << std::endl;

  std::cout << "pose1: " << pos1[0] << ", " << pos1[1] << ", " << pos1[2] << std::endl;
  std::cout << "pose2: " << pos2[0] << ", " << pos2[1] << ", " << pos2[2] << std::endl;
  std::cout << "pose3: " << pos3[0] << ", " << pos3[1] << ", " << pos3[2] << std::endl;
  std::cout << "pose4: " << pos4[0] << ", " << pos4[1] << ", " << pos4[2] << std::endl;
  std::cout << "pose5: " << pos5[0] << ", " << pos5[1] << ", " << pos5[2] << std::endl;
  std::cout << "pose6: " << pos6[0] << ", " << pos6[1] << ", " << pos6[2] << std::endl;
  std::cout << "pose7: " << pos7[0] << ", " << pos7[1] << ", " << pos7[2] << std::endl;
  std::cout << "pose8: " << pos8[0] << ", " << pos8[1] << ", " << pos8[2] << std::endl;
  uint8_t ubotstatus_right;
  // while (true) {

  ret = ubot->moveto_cartesian_line(pos1, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 1 done!" << std::endl;

  sleep(10);

  ret = ubot->moveto_cartesian_line(pos2, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 2 done!" << std::endl;

  sleep(10);

  ret = ubot->moveto_cartesian_line(pos3, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos3, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 3 done!" << std::endl;

  sleep(10);

  ret = ubot->moveto_cartesian_line(pos4, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 4 done!" << std::endl;

  sleep(10);
  ret = ubot->moveto_cartesian_line(pos5, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 5 done!" << std::endl;

  sleep(10);
  ret = ubot->moveto_cartesian_line(pos6, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 6 done!" << std::endl;

  sleep(10);
    ret = ubot->moveto_cartesian_line(pos7, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 7 done!" << std::endl;

  sleep(10);
    ret = ubot->moveto_cartesian_line(pos8, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 8 done!" << std::endl;

  sleep(10);
  ret = ubot->moveto_joint_p2p(posjointR, speed, acc, 0);
  printf("moveto_joint_p2p  : %d\n", ret);
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
  }
  sleep(2);
  // ret = ubot->moveto_joint_p2p(posjointR2, speed, acc, 0);
  // printf("moveto_joint_p2p  : %d\n", ret);
  // ubotstatus_right = 1;
  // while (ubotstatus_right == 1) {
  //   std::this_thread::sleep_for(100ms);
  //   ubot->get_motion_status(&ubotstatus_right);

  //   // std::cout << "Moving to......" << std::endl;
  // }
  // sleep(1);

  printf("finish!");
  // }
  delete utra_report;
 return true;
}

bool utraRobot::right_arm_repeatabilitity_test()
{
  char ip[64] = "192.168.5.39";
  arm_report_status_t rx;
  UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
  // utra_report->get_data(&rx);
  float pos_init[6];
  float pos1[6], pos2[6], pos3[6], pos4[6], pos5[6], pos6[6], pos7[6], pos8[6];

  float posjointR[7] = {-115.913370, -85.959911, 125.752660, 120.587977, 17.698261, -77.364478, -69.737722};
  float posjointR2[7] ={-86.9,-78.4,118,84.4,13.5,2.6,-69.5};
  for (size_t i = 0; i < 7; i++) {
    posjointR[i] = posjointR[i] * (M_PI / 180.0);
  }
  for (size_t i = 0; i < 7; i++) {
    posjointR2[i] = posjointR2[i] * (M_PI / 180.0);
  }
  uint8_t ubotstatus_left = 1;
  int ret = ubot->moveto_joint_p2p(posjointR, speed, acc, 0);
  printf("moveto_cartesian_p2p  : %d\n", ret);
  ubotstatus_left = 1;
  while (ubotstatus_left == 1) {
    std::this_thread::sleep_for(1000ms);
    ubot->get_motion_status(&ubotstatus_left);
    std::cout << "Moving to......" << std::endl;
  }
  std::cout << "Joint moving done!" << std::endl;
  sleep(2);
  utra_report->get_data(&rx);
  for (int i = 0; i < 6; i++) {
    pos_init[i] = rx.pose[i];
  }
  for (int i = 0; i < 6; i++) {
    pos1[i] = pos_init[i];
    pos2[i] = pos_init[i];
    pos3[i] = pos_init[i];
    pos4[i] = pos_init[i];
    pos5[i] = pos_init[i];
    pos6[i] = pos_init[i];
    pos7[i] = pos_init[i];
    pos8[i] = pos_init[i];
  }
  pos1[0] = 158.300209;  // 1 rx:158.300209, ry:-188.025544,rz:373.062064
  pos1[1] = -188.025544;
  pos1[2] = 373.062064;

  pos2[0] = 244.902749;  // 5 rx:244.902749, ry:-188.025544,rz:423.062064
  pos2[1] = -188.025544;
  pos2[2] = 423.062064;

  pos3[0] = 244.902749;  // 7 rx:244.902749, ry:-88.025544,rz:423.062064
  pos3[1] = -88.025544;
  pos3[2] = 423.062064;

  pos4[0] = 158.300209;  // 3 rx:158.300209, ry:-88.025544,rz:373.062064
  pos4[1] = -88.025544;
  pos4[2] = 373.062064;

  pos5[0] = 58.300209;  // 4 rx:58.300209, ry:-88.025544,rz:546.267144
  pos5[1] = -88.025544;
  pos5[2] = 546.267144;

  pos6[0] = 58.300209;  // 2 rx:58.300209, ry:-188.025544,rz:546.267144
  pos6[1] = -188.025544;
  pos6[2] = 546.267144;

  pos7[0] = 144.902749;  // 6 rx:144.902749, ry:-188.025544,rz:596.267144
  pos7[1] = -188.025544;
  pos7[2] = 596.267144;

  pos8[0] = 144.902749;  // 8 rx:144.902749, ry:-88.025544,rz:596.267144
  pos8[1] = -88.025544;
  pos8[2] = 596.267144;

  std::cout << "pose init: " << pos_init[0] << ", " << pos_init[1] << ", " << pos_init[2] << std::endl;
  std::cout << "pose init deg: " << pos_init[3] / (M_PI / 180) << ", " << pos_init[4] / (M_PI / 180) << ", "
            << pos_init[5] / (M_PI / 180) << std::endl;

  std::cout << "pose1: " << pos1[0] << ", " << pos1[1] << ", " << pos1[2] << std::endl;
  std::cout << "pose2: " << pos2[0] << ", " << pos2[1] << ", " << pos2[2] << std::endl;
  std::cout << "pose3: " << pos3[0] << ", " << pos3[1] << ", " << pos3[2] << std::endl;
  std::cout << "pose4: " << pos4[0] << ", " << pos4[1] << ", " << pos4[2] << std::endl;
  std::cout << "pose5: " << pos5[0] << ", " << pos5[1] << ", " << pos5[2] << std::endl;
  std::cout << "pose6: " << pos6[0] << ", " << pos6[1] << ", " << pos6[2] << std::endl;
  std::cout << "pose7: " << pos7[0] << ", " << pos7[1] << ", " << pos7[2] << std::endl;
  std::cout << "pose8: " << pos8[0] << ", " << pos8[1] << ", " << pos8[2] << std::endl;
  uint8_t ubotstatus_right;
  // while (true) {
for (int i = 0; i < 5; ++i) {
  ret = ubot->moveto_cartesian_line(pos2, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 2 done!" << std::endl;

  sleep(10);

  ret = ubot->moveto_cartesian_line(pos7, mvvelo, mvacc, 0);
  // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
  printf("moveto_cartesian_line: %d\n", ret);
  if (ret != 0) {
    uint8_t error_code[2];
    ret = ubot->get_error_code(error_code);
    std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
              << int(error_code[1]) << std::endl;
    ret = ubot->reset_err();
    printf("reset_err   : %d\n", ret);
  }
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  std::cout << "pose 7 done!" << std::endl;

  sleep(10);
  ret = ubot->moveto_joint_p2p(posjointR, speed, acc, 0);
  printf("moveto_joint_p2p  : %d\n", ret);
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  sleep(1);
}
 ret = ubot->moveto_joint_p2p(posjointR2, speed, acc, 0);
  printf("moveto_joint_p2p  : %d\n", ret);
  ubotstatus_right = 1;
  while (ubotstatus_right == 1) {
    std::this_thread::sleep_for(100ms);
    ubot->get_motion_status(&ubotstatus_right);
    // std::cout << "Moving to......" << std::endl;
  }
  sleep(1);
  printf("finish!");
  // }
  delete utra_report;
 return true;
}

void utraRobot::left_arm_gravity_thread()
{
    char ip[64] = "192.168.3.37";
    // arm_report_status_t rx;
    //UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
    float posjointL[7] = {78.3,85.3,-110,-120,2,49.1,87.9};
    for (size_t i = 0; i < 7; i++) {
        posjointL[i] = posjointL[i] * (M_PI / 180.0);
    }
    uint8_t ubotstatus_left = 1;
    int ret = ubot->moveto_joint_p2p(posjointL, speed, acc, 0);
    printf("moveto_cartesian_p2p  : %d\n", ret);
    ubotstatus_left = 1;
    while (ubotstatus_left == 1) {
        std::this_thread::sleep_for(1000ms);
        ubot->get_motion_status(&ubotstatus_left);
        std::cout << "Moving to......" << std::endl;
    }
    std::cout << "Joint moving done!" << std::endl;
    sleep(2);
    //delete utra_report;
    return ;
}

void utraRobot::right_arm_gravity_thread()
{
    char ip[64] = "192.168.5.39";

    //arm_report_status_t rx;
    //UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
    float posjointL[7] = {-80.4,-95.1,120.8,123.4,-14.3,-51.3,-61.8};
    for (size_t i = 0; i < 7; i++) {
        posjointL[i] = posjointL[i] * (M_PI / 180.0);
    }
    uint8_t ubotstatus_left = 1;
    int ret = ubot->moveto_joint_p2p(posjointL, speed, acc, 0);
    printf("moveto_cartesian_p2p  : %d\n", ret);
    ubotstatus_left = 1;
    while (ubotstatus_left == 1) {
        std::this_thread::sleep_for(1000ms);
        ubot->get_motion_status(&ubotstatus_left);
        std::cout << "Moving to......" << std::endl;
    }
    std::cout << "Joint moving done!" << std::endl;
    sleep(2);
    //delete utra_report;
    return ;
}
void utraRobot::left_arm_accuracy_thread()
{
    char ip[64] = "192.168.3.37";

    arm_report_status_t rx;
    UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
    // utra_report->get_data(&rx);
    float pos_init[6];
    float pos1[6], pos2[6], pos3[6], pos4[6], pos5[6], pos6[6], pos7[6], pos8[6];
    float posjointL[7] = {91.910320, 69.493276, -90.623914, -124.103321, -27.947194, 59.642553, 119.9};
    float posjointL2[7] = {85.1,78.1,-118.6,-93.3,-11.2,-8.1,61.7};
    for (size_t i = 0; i < 7; i++) {
        posjointL[i] = posjointL[i] * (M_PI / 180.0);
    }

    uint8_t ubotstatus_left = 1;
    int ret = ubot->moveto_joint_p2p(posjointL, speed, acc, 0);
    printf("moveto_cartesian_p2p  : %d\n", ret);
    ubotstatus_left = 1;
    while (ubotstatus_left == 1) {
        std::this_thread::sleep_for(1000ms);
        ubot->get_motion_status(&ubotstatus_left);
        std::cout << "Moving to......" << std::endl;
    }
    std::cout << "Joint moving done!" << std::endl;
    sleep(2);
    utra_report->get_data(&rx);
    for (int i = 0; i < 6; i++) {
        pos_init[i] = rx.pose[i];
    }
    for (int i = 0; i < 6; i++) {
        pos1[i] = pos_init[i];
        pos2[i] = pos_init[i];
        pos3[i] = pos_init[i];
        pos4[i] = pos_init[i];
        pos5[i] = pos_init[i];
        pos6[i] = pos_init[i];
        pos7[i] = pos_init[i];
        pos8[i] = pos_init[i];
    }
    pos1[0] = 313.870513;  // 1 lx:313.870513, ly:-11.825159,lz:249.483245
    pos1[1] = -11.825159;
    pos1[2] = 249.483245;

    pos2[0] = 400.473053;  // 5 lx:400.473053, ly:-11.825159,lz:299.483245
    pos2[1] = -11.825159;
    pos2[2] = 299.483245;

    pos3[0] = 400.473053;  // 7 lx:400.473053, ly:88.174841,lz:299.483245
    pos3[1] = 88.174841;
    pos3[2] = 299.483245;

    pos4[0] = 313.870513;  // 3 lx:313.870513, ly:88.174841,lz:249.483245
    pos4[1] = 88.174841;
    pos4[2] = 249.483245;

    pos5[0] = 213.870513;  // 4 lx:213.870513, ly:88.174841,lz:422.688325
    pos5[1] = 88.174841;
    pos5[2] = 422.688325;

    pos6[0] = 213.870513;  // 2 lx:213.870513, ly:-11.825159,lz:422.688325
    pos6[1] = -11.825159;
    pos6[2] = 422.688325;

    pos7[0] = 300.473053;  // 6 lx:300.473053, ly:-11.825159,lz:472.688325
    pos7[1] = -11.825159;
    pos7[2] = 472.688325;

    pos8[0] = 300.473053;  // 8 lx:300.473053, ly:88.174841,lz:472.688325,
    pos8[1] = 88.174841;
    pos8[2] = 472.688325;

    std::cout << "pose init: " << pos_init[0] << ", " << pos_init[1] << ", " << pos_init[2] << std::endl;
    std::cout << "pose init deg: " << pos_init[3] / (M_PI / 180) << ", " << pos_init[4] / (M_PI / 180) << ", "
              << pos_init[5] / (M_PI / 180) << std::endl;

    std::cout << "pose1: " << pos1[0] << ", " << pos1[1] << ", " << pos1[2] << std::endl;
    std::cout << "pose2: " << pos2[0] << ", " << pos2[1] << ", " << pos2[2] << std::endl;
    std::cout << "pose3: " << pos3[0] << ", " << pos3[1] << ", " << pos3[2] << std::endl;
    std::cout << "pose4: " << pos4[0] << ", " << pos4[1] << ", " << pos4[2] << std::endl;
    std::cout << "pose5: " << pos5[0] << ", " << pos5[1] << ", " << pos5[2] << std::endl;
    std::cout << "pose6: " << pos6[0] << ", " << pos6[1] << ", " << pos6[2] << std::endl;
    std::cout << "pose7: " << pos7[0] << ", " << pos7[1] << ", " << pos7[2] << std::endl;
    std::cout << "pose8: " << pos8[0] << ", " << pos8[1] << ", " << pos8[2] << std::endl;
    uint8_t ubotstatus_right;
    // while (true) {

    ret = ubot->moveto_cartesian_line(pos1, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 1 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_cartesian_line(pos2, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 2 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_cartesian_line(pos3, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 3 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_cartesian_line(pos4, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 4 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_cartesian_line(pos5, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 5 done!" << std::endl;

    sleep(10);
    ret = ubot->moveto_cartesian_line(pos6, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 6 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_cartesian_line(pos7, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos3, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 7 done!" << std::endl;

    sleep(10);
    ret = ubot->moveto_cartesian_line(pos8, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos3, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 8 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_joint_p2p(posjointL, speed, acc, 0);
    printf("moveto_cartesian_p2p  : %d\n", ret);
    while (ubotstatus_left == 1) {
        std::this_thread::sleep_for(1000ms);
        ubot->get_motion_status(&ubotstatus_left);
        std::cout << "Moving to......" << std::endl;
    }
    std::cout << "Joint moving done!" << std::endl;
    sleep(2);



    printf("finish!");
    // }
    delete utra_report;
    return ;
}

void utraRobot::left_arm_workspace_thread()
{
    char ip[64] = "192.168.3.37";

    arm_report_status_t rx;
    UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
    // utra_report->get_data(&rx);
    float pos_init[6];
    float pos1[6], pos2[6], pos3[6], pos4[6], pos5[6], pos6[6], pos7[6], pos8[6];
    float posjointL[7] = {91.910320, 69.493276, -90.623914, -124.103321, -27.947194, 59.642553, 119.9};
    float posjointL2[7] = {85.1,78.1,-118.6,-93.3,-11.2,-8.1,61.7};
    for (size_t i = 0; i < 7; i++) {
        posjointL[i] = posjointL[i] * (M_PI / 180.0);
    }

    uint8_t ubotstatus_left = 1;
    int ret = ubot->moveto_joint_p2p(posjointL, speed, acc, 0);
    printf("moveto_cartesian_p2p  : %d\n", ret);
    ubotstatus_left = 1;
    while (ubotstatus_left == 1) {
        std::this_thread::sleep_for(1000ms);
        ubot->get_motion_status(&ubotstatus_left);
        std::cout << "Moving to......" << std::endl;
    }
    std::cout << "Joint moving done!" << std::endl;
    sleep(2);
    utra_report->get_data(&rx);
    for (int i = 0; i < 6; i++) {
        pos_init[i] = rx.pose[i];
    }
    for (int i = 0; i < 6; i++) {
        pos1[i] = pos_init[i];
        pos2[i] = pos_init[i];
        pos3[i] = pos_init[i];
        pos4[i] = pos_init[i];
        pos5[i] = pos_init[i];
        pos6[i] = pos_init[i];
        pos7[i] = pos_init[i];
        pos8[i] = pos_init[i];
    }
    pos1[0] = 309.540386;  // 1 lx:313.870513, ly:-11.825159,lz:249.483245
    pos1[1] = -16.825159;
    pos1[2] = 246.983245;

    pos2[0] = 404.80318;  // 5 lx:400.473053, ly:-11.825159,lz:299.483245
    pos2[1] = -16.825159;
    pos2[2] = 296.983245;

    pos3[0] = 404.80318;  // 7 lx:400.473053, ly:88.174841,lz:299.483245
    pos3[1] = 93.174841;
    pos3[2] = 296.983245;

    pos4[0] = 309.540386;  // 3 lx:313.870513, ly:88.174841,lz:249.483245
    pos4[1] = 93.174841;
    pos4[2] = 246.983245;

    pos5[0] = 209.540386;  // 4 lx:213.870513, ly:88.174841,lz:422.688325
    pos5[1] = 93.174841;
    pos5[2] = 425.188325;

    pos6[0] = 209.540386;  // 2 lx:213.870513, ly:-11.825159,lz:422.688325
    pos6[1] = -16.825159;
    pos6[2] = 425.188325;

    pos7[0] = 304.80318;  // 6 lx:300.473053, ly:-11.825159,lz:472.688325
    pos7[1] = -16.825159;
    pos7[2] = 475.188325;

    pos8[0] = 304.80318;  // 8 lx:300.473053, ly:88.174841,lz:472.688325,
    pos8[1] = 93.174841;
    pos8[2] = 475.188325;

    std::cout << "pose init: " << pos_init[0] << ", " << pos_init[1] << ", " << pos_init[2] << std::endl;
    std::cout << "pose init deg: " << pos_init[3] / (M_PI / 180) << ", " << pos_init[4] / (M_PI / 180) << ", "
              << pos_init[5] / (M_PI / 180) << std::endl;

    std::cout << "pose1: " << pos1[0] << ", " << pos1[1] << ", " << pos1[2] << std::endl;
    std::cout << "pose2: " << pos2[0] << ", " << pos2[1] << ", " << pos2[2] << std::endl;
    std::cout << "pose3: " << pos3[0] << ", " << pos3[1] << ", " << pos3[2] << std::endl;
    std::cout << "pose4: " << pos4[0] << ", " << pos4[1] << ", " << pos4[2] << std::endl;
    std::cout << "pose5: " << pos5[0] << ", " << pos5[1] << ", " << pos5[2] << std::endl;
    std::cout << "pose6: " << pos6[0] << ", " << pos6[1] << ", " << pos6[2] << std::endl;
    std::cout << "pose7: " << pos7[0] << ", " << pos7[1] << ", " << pos7[2] << std::endl;
    std::cout << "pose8: " << pos8[0] << ", " << pos8[1] << ", " << pos8[2] << std::endl;
    uint8_t ubotstatus_right;
    // while (true) {

    ret = ubot->moveto_cartesian_line(pos1, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 1 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_cartesian_line(pos2, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 2 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_cartesian_line(pos3, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 3 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_cartesian_line(pos4, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 4 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_cartesian_line(pos5, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 5 done!" << std::endl;

    sleep(10);
    ret = ubot->moveto_cartesian_line(pos6, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 6 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_cartesian_line(pos7, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos3, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 7 done!" << std::endl;

    sleep(10);
    ret = ubot->moveto_cartesian_line(pos8, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos3, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 8 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_joint_p2p(posjointL, speed, acc, 0);
    printf("moveto_cartesian_p2p  : %d\n", ret);
    while (ubotstatus_left == 1) {
        std::this_thread::sleep_for(1000ms);
        ubot->get_motion_status(&ubotstatus_left);
        std::cout << "Moving to......" << std::endl;
    }
    std::cout << "Joint moving done!" << std::endl;
    sleep(2);



    printf("finish!");
    // }
    delete utra_report;
    return ;
}
void utraRobot::left_arm_reppeat_thread()
{
    char ip[64] = "192.168.3.37";

    arm_report_status_t rx;
    UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
    // utra_report->get_data(&rx);
    float pos_init[6];
    float pos1[6], pos2[6], pos3[6], pos4[6], pos5[6], pos6[6], pos7[6], pos8[6];
    float posjointL[7] = {91.910320, 69.493276, -90.623914, -124.103321, -27.947194, 59.642553, 119.9};
    float posjointL2[7] = {85.1,78.1,-118.6,-93.3,-11.2,-8.1,61.7};
    for (size_t i = 0; i < 7; i++) {
        posjointL[i] = posjointL[i] * (M_PI / 180.0);
    }
    for (size_t i = 0; i < 7; i++) {
        posjointL2[i] = posjointL2[i] * (M_PI / 180.0);
    }
    uint8_t ubotstatus_left = 1;
    int ret = ubot->moveto_joint_p2p(posjointL, speed, acc, 0);
    printf("moveto_cartesian_p2p  : %d\n", ret);
    ubotstatus_left = 1;
    while (ubotstatus_left == 1) {
        std::this_thread::sleep_for(1000ms);
        ubot->get_motion_status(&ubotstatus_left);
        std::cout << "Moving to......" << std::endl;
    }
    std::cout << "Joint moving done!" << std::endl;
    sleep(2);
    utra_report->get_data(&rx);
    for (int i = 0; i < 6; i++) {
        pos_init[i] = rx.pose[i];
    }
    for (int i = 0; i < 6; i++) {
        pos1[i] = pos_init[i];
        pos2[i] = pos_init[i];
        pos3[i] = pos_init[i];
        pos4[i] = pos_init[i];
        pos5[i] = pos_init[i];
        pos6[i] = pos_init[i];
        pos7[i] = pos_init[i];
        pos8[i] = pos_init[i];
    }
    pos1[0] = 313.870513;  // 1 lx:313.870513, ly:-11.825159,lz:249.483245
    pos1[1] = -11.825159;
    pos1[2] = 249.483245;

    pos2[0] = 400.473053;  // 5 lx:400.473053, ly:-11.825159,lz:299.483245
    pos2[1] = -11.825159;
    pos2[2] = 299.483245;

    pos3[0] = 400.473053;  // 7 lx:400.473053, ly:88.174841,lz:299.483245
    pos3[1] = 88.174841;
    pos3[2] = 299.483245;

    pos4[0] = 313.870513;  // 3 lx:313.870513, ly:88.174841,lz:249.483245
    pos4[1] = 88.174841;
    pos4[2] = 249.483245;

    pos5[0] = 213.870513;  // 4 lx:213.870513, ly:88.174841,lz:422.688325
    pos5[1] = 88.174841;
    pos5[2] = 422.688325;

    pos6[0] = 213.870513;  // 2 lx:213.870513, ly:-11.825159,lz:422.688325
    pos6[1] = -11.825159;
    pos6[2] = 422.688325;

    pos7[0] = 300.473053;  // 6 lx:300.473053, ly:-11.825159,lz:472.688325
    pos7[1] = -11.825159;
    pos7[2] = 472.688325;

    pos8[0] = 300.473053;  // 8 lx:300.473053, ly:88.174841,lz:472.688325,
    pos8[1] = 88.174841;
    pos8[2] = 472.688325;

    std::cout << "pose init: " << pos_init[0] << ", " << pos_init[1] << ", " << pos_init[2] << std::endl;
    std::cout << "pose init deg: " << pos_init[3] / (M_PI / 180) << ", " << pos_init[4] / (M_PI / 180) << ", "
              << pos_init[5] / (M_PI / 180) << std::endl;

    std::cout << "pose1: " << pos1[0] << ", " << pos1[1] << ", " << pos1[2] << std::endl;
    std::cout << "pose2: " << pos2[0] << ", " << pos2[1] << ", " << pos2[2] << std::endl;
    std::cout << "pose3: " << pos3[0] << ", " << pos3[1] << ", " << pos3[2] << std::endl;
    std::cout << "pose4: " << pos4[0] << ", " << pos4[1] << ", " << pos4[2] << std::endl;
    std::cout << "pose5: " << pos5[0] << ", " << pos5[1] << ", " << pos5[2] << std::endl;
    std::cout << "pose6: " << pos6[0] << ", " << pos6[1] << ", " << pos6[2] << std::endl;
    std::cout << "pose7: " << pos7[0] << ", " << pos7[1] << ", " << pos7[2] << std::endl;
    std::cout << "pose8: " << pos8[0] << ", " << pos8[1] << ", " << pos8[2] << std::endl;
    uint8_t ubotstatus_right;
    // while (true) {
    for (int i = 0; i < 5; ++i) {
        ret = ubot->moveto_cartesian_line(pos2, mvvelo, mvacc, 0);
        // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
        printf("moveto_cartesian_line: %d\n", ret);
        if (ret != 0) {
            uint8_t error_code[2];
            ret = ubot->get_error_code(error_code);
            std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                      << int(error_code[1]) << std::endl;
            ret = ubot->reset_err();
            printf("reset_err   : %d\n", ret);
        }
        ubotstatus_right = 1;
        while (ubotstatus_right == 1) {
            std::this_thread::sleep_for(100ms);
            ubot->get_motion_status(&ubotstatus_right);
            // std::cout << "Moving to......" << std::endl;
        }
        std::cout << "pose 2 done!" << std::endl;

        sleep(10);

        ret = ubot->moveto_cartesian_line(pos7, mvvelo, mvacc, 0);
        // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
        printf("moveto_cartesian_line: %d\n", ret);
        if (ret != 0) {
            uint8_t error_code[2];
            ret = ubot->get_error_code(error_code);
            std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                      << int(error_code[1]) << std::endl;
            ret = ubot->reset_err();
            printf("reset_err   : %d\n", ret);
        }
        ubotstatus_right = 1;
        while (ubotstatus_right == 1) {
            std::this_thread::sleep_for(100ms);
            ubot->get_motion_status(&ubotstatus_right);
            // std::cout << "Moving to......" << std::endl;
        }
        std::cout << "pose 7 done!" << std::endl;

        sleep(10);
        ret = ubot->moveto_joint_p2p(posjointL, speed, acc, 0);
        printf("moveto_cartesian_p2p  : %d\n", ret);
        while (ubotstatus_left == 1) {
            std::this_thread::sleep_for(1000ms);
            ubot->get_motion_status(&ubotstatus_left);
            std::cout << "Moving to......" << std::endl;
        }
        std::cout << "Joint moving done!" << std::endl;
        sleep(2);
    }
    ret = ubot->moveto_joint_p2p(posjointL2, speed, acc, 0);
    printf("moveto_cartesian_p2p  : %d\n", ret);
    while (ubotstatus_left == 1) {
        std::this_thread::sleep_for(1000ms);
        ubot->get_motion_status(&ubotstatus_left);
        std::cout << "Moving to......" << std::endl;
    }
    std::cout << "Joint moving done!" << std::endl;
    sleep(1);
    printf("finish!");
    // }
    delete utra_report;
    return ;
}

void utraRobot::right_arm_accuracy_thread()
{
    char ip[64] = "192.168.5.39";
    arm_report_status_t rx;
    UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
    // utra_report->get_data(&rx);
    float pos_init[6];
    float pos1[6], pos2[6], pos3[6], pos4[6], pos5[6], pos6[6], pos7[6], pos8[6];

    float posjointR[7] = {-115.913370, -85.959911, 125.752660, 120.587977, 17.698261, -77.364478, -69.737722};
    float posjointR2[7] ={-86.9,-78.4,118,84.4,13.5,2.6,-69.5};
    for (size_t i = 0; i < 7; i++) {
        posjointR[i] = posjointR[i] * (M_PI / 180.0);
    }
    for (size_t i = 0; i < 7; i++) {
        posjointR2[i] = posjointR2[i] * (M_PI / 180.0);
    }
    uint8_t ubotstatus_left = 1;
    int ret = ubot->moveto_joint_p2p(posjointR, speed, acc, 0);
    printf("moveto_cartesian_p2p  : %d\n", ret);
    ubotstatus_left = 1;
    while (ubotstatus_left == 1) {
        std::this_thread::sleep_for(1000ms);
        ubot->get_motion_status(&ubotstatus_left);
        std::cout << "Moving to......" << std::endl;
    }
    std::cout << "Joint moving done!" << std::endl;
    sleep(2);
    utra_report->get_data(&rx);
    for (int i = 0; i < 6; i++) {
        pos_init[i] = rx.pose[i];
    }
    for (int i = 0; i < 6; i++) {
        pos1[i] = pos_init[i];
        pos2[i] = pos_init[i];
        pos3[i] = pos_init[i];
        pos4[i] = pos_init[i];
        pos5[i] = pos_init[i];
        pos6[i] = pos_init[i];
        pos7[i] = pos_init[i];
        pos8[i] = pos_init[i];
    }
    pos1[0] = 158.300209;  // 1 rx:158.300209, ry:-188.025544,rz:373.062064
    pos1[1] = -188.025544;
    pos1[2] = 373.062064;

    pos2[0] = 244.902749;  // 5 rx:244.902749, ry:-188.025544,rz:423.062064
    pos2[1] = -188.025544;
    pos2[2] = 423.062064;

    pos3[0] = 244.902749;  // 7 rx:244.902749, ry:-88.025544,rz:423.062064
    pos3[1] = -88.025544;
    pos3[2] = 423.062064;

    pos4[0] = 158.300209;  // 3 rx:158.300209, ry:-88.025544,rz:373.062064
    pos4[1] = -88.025544;
    pos4[2] = 373.062064;

    pos5[0] = 58.300209;  // 4 rx:58.300209, ry:-88.025544,rz:546.267144
    pos5[1] = -88.025544;
    pos5[2] = 546.267144;

    pos6[0] = 58.300209;  // 2 rx:58.300209, ry:-188.025544,rz:546.267144
    pos6[1] = -188.025544;
    pos6[2] = 546.267144;

    pos7[0] = 144.902749;  // 6 rx:144.902749, ry:-188.025544,rz:596.267144
    pos7[1] = -188.025544;
    pos7[2] = 596.267144;

    pos8[0] = 144.902749;  // 8 rx:144.902749, ry:-88.025544,rz:596.267144
    pos8[1] = -88.025544;
    pos8[2] = 596.267144;

    std::cout << "pose init: " << pos_init[0] << ", " << pos_init[1] << ", " << pos_init[2] << std::endl;
    std::cout << "pose init deg: " << pos_init[3] / (M_PI / 180) << ", " << pos_init[4] / (M_PI / 180) << ", "
              << pos_init[5] / (M_PI / 180) << std::endl;

    std::cout << "pose1: " << pos1[0] << ", " << pos1[1] << ", " << pos1[2] << std::endl;
    std::cout << "pose2: " << pos2[0] << ", " << pos2[1] << ", " << pos2[2] << std::endl;
    std::cout << "pose3: " << pos3[0] << ", " << pos3[1] << ", " << pos3[2] << std::endl;
    std::cout << "pose4: " << pos4[0] << ", " << pos4[1] << ", " << pos4[2] << std::endl;
    std::cout << "pose5: " << pos5[0] << ", " << pos5[1] << ", " << pos5[2] << std::endl;
    std::cout << "pose6: " << pos6[0] << ", " << pos6[1] << ", " << pos6[2] << std::endl;
    std::cout << "pose7: " << pos7[0] << ", " << pos7[1] << ", " << pos7[2] << std::endl;
    std::cout << "pose8: " << pos8[0] << ", " << pos8[1] << ", " << pos8[2] << std::endl;
    uint8_t ubotstatus_right;
    // while (true) {

    ret = ubot->moveto_cartesian_line(pos1, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 1 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_cartesian_line(pos2, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 2 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_cartesian_line(pos3, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos3, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 3 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_cartesian_line(pos4, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 4 done!" << std::endl;

    sleep(10);
    ret = ubot->moveto_cartesian_line(pos5, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 5 done!" << std::endl;

    sleep(10);
    ret = ubot->moveto_cartesian_line(pos6, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 6 done!" << std::endl;

    sleep(10);
    ret = ubot->moveto_cartesian_line(pos7, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 7 done!" << std::endl;

    sleep(10);
    ret = ubot->moveto_cartesian_line(pos8, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 8 done!" << std::endl;

    sleep(10);
    ret = ubot->moveto_joint_p2p(posjointR, speed, acc, 0);
    printf("moveto_joint_p2p  : %d\n", ret);
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
    }
    sleep(2);
    // ret = ubot->moveto_joint_p2p(posjointR2, speed, acc, 0);
    // printf("moveto_joint_p2p  : %d\n", ret);
    // ubotstatus_right = 1;
    // while (ubotstatus_right == 1) {
    //   std::this_thread::sleep_for(100ms);
    //   ubot->get_motion_status(&ubotstatus_right);

    //   // std::cout << "Moving to......" << std::endl;
    // }
    // sleep(1);

    printf("finish!");
    // }
    delete utra_report;
    return ;
}

void utraRobot::right_arm_workspace_thread()
{
    char ip[64] = "192.168.5.39";
    arm_report_status_t rx;
    UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
    // utra_report->get_data(&rx);
    float pos_init[6];
    float pos1[6], pos2[6], pos3[6], pos4[6], pos5[6], pos6[6], pos7[6], pos8[6];

    float posjointR[7] = {-115.913370, -85.959911, 125.752660, 120.587977, 17.698261, -77.364478, -69.737722};
    float posjointR2[7] ={-86.9,-78.4,118,84.4,13.5,2.6,-69.5};
    for (size_t i = 0; i < 7; i++) {
        posjointR[i] = posjointR[i] * (M_PI / 180.0);
    }
    for (size_t i = 0; i < 7; i++) {
        posjointR2[i] = posjointR2[i] * (M_PI / 180.0);
    }
    uint8_t ubotstatus_left = 1;
    int ret = ubot->moveto_joint_p2p(posjointR, speed, acc, 0);
    printf("moveto_cartesian_p2p  : %d\n", ret);
    ubotstatus_left = 1;
    while (ubotstatus_left == 1) {
        std::this_thread::sleep_for(1000ms);
        ubot->get_motion_status(&ubotstatus_left);
        std::cout << "Moving to......" << std::endl;
    }
    std::cout << "Joint moving done!" << std::endl;
    sleep(2);
    utra_report->get_data(&rx);
    for (int i = 0; i < 6; i++) {
        pos_init[i] = rx.pose[i];
    }
    for (int i = 0; i < 6; i++) {
        pos1[i] = pos_init[i];
        pos2[i] = pos_init[i];
        pos3[i] = pos_init[i];
        pos4[i] = pos_init[i];
        pos5[i] = pos_init[i];
        pos6[i] = pos_init[i];
        pos7[i] = pos_init[i];
        pos8[i] = pos_init[i];
    }
    pos1[0] = 158.300209-4.330127;  // 1 rx:158.300209, ry:-188.025544,rz:373.062064
    pos1[1] = -188.025544-5;
    pos1[2] = 373.062064-2.5;

    pos2[0] = 244.902749+4.330127;  // 5 rx:244.902749, ry:-188.025544,rz:423.062064
    pos2[1] = -188.025544-5;
    pos2[2] = 423.062064-2.5;

    pos3[0] = 244.902749+4.330127;  // 7 rx:244.902749, ry:-88.025544,rz:423.062064
    pos3[1] = -88.025544+5;
    pos3[2] = 423.062064-5;

    pos4[0] = 158.300209-4.330127;  // 3 rx:158.300209, ry:-88.025544,rz:373.062064
    pos4[1] = -88.025544+5;
    pos4[2] = 373.062064-2.5;

    pos5[0] = 58.300209-4.330127;  // 4 rx:58.300209, ry:-88.025544,rz:546.267144
    pos5[1] = -88.025544+5;
    pos5[2] = 546.267144+2.5;

    pos6[0] = 58.300209-4.330127;  // 2 rx:58.300209, ry:-188.025544,rz:546.267144
    pos6[1] = -188.025544-5;
    pos6[2] = 546.267144+2.5;

    pos7[0] = 144.902749+4.330127;  // 6 rx:144.902749, ry:-188.025544,rz:596.267144
    pos7[1] = -188.025544-5;
    pos7[2] = 596.267144+2.5;

    pos8[0] = 144.902749+4.330127;  // 8 rx:144.902749, ry:-88.025544,rz:596.267144
    pos8[1] = -88.025544+5;
    pos8[2] = 596.267144+2.5;

    std::cout << "pose init: " << pos_init[0] << ", " << pos_init[1] << ", " << pos_init[2] << std::endl;
    std::cout << "pose init deg: " << pos_init[3] / (M_PI / 180) << ", " << pos_init[4] / (M_PI / 180) << ", "
              << pos_init[5] / (M_PI / 180) << std::endl;

    std::cout << "pose1: " << pos1[0] << ", " << pos1[1] << ", " << pos1[2] << std::endl;
    std::cout << "pose2: " << pos2[0] << ", " << pos2[1] << ", " << pos2[2] << std::endl;
    std::cout << "pose3: " << pos3[0] << ", " << pos3[1] << ", " << pos3[2] << std::endl;
    std::cout << "pose4: " << pos4[0] << ", " << pos4[1] << ", " << pos4[2] << std::endl;
    std::cout << "pose5: " << pos5[0] << ", " << pos5[1] << ", " << pos5[2] << std::endl;
    std::cout << "pose6: " << pos6[0] << ", " << pos6[1] << ", " << pos6[2] << std::endl;
    std::cout << "pose7: " << pos7[0] << ", " << pos7[1] << ", " << pos7[2] << std::endl;
    std::cout << "pose8: " << pos8[0] << ", " << pos8[1] << ", " << pos8[2] << std::endl;
    uint8_t ubotstatus_right;
    // while (true) {

    ret = ubot->moveto_cartesian_line(pos1, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 1 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_cartesian_line(pos2, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 2 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_cartesian_line(pos3, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos3, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 3 done!" << std::endl;

    sleep(10);

    ret = ubot->moveto_cartesian_line(pos4, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 4 done!" << std::endl;

    sleep(10);
    ret = ubot->moveto_cartesian_line(pos5, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 5 done!" << std::endl;

    sleep(10);
    ret = ubot->moveto_cartesian_line(pos6, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 6 done!" << std::endl;

    sleep(10);
    ret = ubot->moveto_cartesian_line(pos7, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 7 done!" << std::endl;

    sleep(10);
    ret = ubot->moveto_cartesian_line(pos8, mvvelo, mvacc, 0);
    // ret = ubot->moveto_cartesian_p2p(pos4, speed, acc, 0);
    printf("moveto_cartesian_line: %d\n", ret);
    if (ret != 0) {
        uint8_t error_code[2];
        ret = ubot->get_error_code(error_code);
        std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                  << int(error_code[1]) << std::endl;
        ret = ubot->reset_err();
        printf("reset_err   : %d\n", ret);
    }
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    std::cout << "pose 8 done!" << std::endl;

    sleep(10);
    ret = ubot->moveto_joint_p2p(posjointR, speed, acc, 0);
    printf("moveto_joint_p2p  : %d\n", ret);
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
    }
    sleep(2);
    // ret = ubot->moveto_joint_p2p(posjointR2, speed, acc, 0);
    // printf("moveto_joint_p2p  : %d\n", ret);
    // ubotstatus_right = 1;
    // while (ubotstatus_right == 1) {
    //   std::this_thread::sleep_for(100ms);
    //   ubot->get_motion_status(&ubotstatus_right);

    //   // std::cout << "Moving to......" << std::endl;
    // }
    // sleep(1);

    printf("finish!");
    // }
    delete utra_report;
    return ;
}

void utraRobot::right_arm_repeat_thread()
{
    char ip[64] = "192.168.5.39";
    arm_report_status_t rx;
    UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
    // utra_report->get_data(&rx);
    float pos_init[6];
    float pos1[6], pos2[6], pos3[6], pos4[6], pos5[6], pos6[6], pos7[6], pos8[6];

    float posjointR[7] = {-115.913370, -85.959911, 125.752660, 120.587977, 17.698261, -77.364478, -69.737722};
    float posjointR2[7] ={-86.9,-78.4,118,84.4,13.5,2.6,-69.5};
    for (size_t i = 0; i < 7; i++) {
        posjointR[i] = posjointR[i] * (M_PI / 180.0);
    }
    for (size_t i = 0; i < 7; i++) {
        posjointR2[i] = posjointR2[i] * (M_PI / 180.0);
    }
    uint8_t ubotstatus_left = 1;
    int ret = ubot->moveto_joint_p2p(posjointR, speed, acc, 0);
    printf("moveto_cartesian_p2p  : %d\n", ret);
    ubotstatus_left = 1;
    while (ubotstatus_left == 1) {
        std::this_thread::sleep_for(1000ms);
        ubot->get_motion_status(&ubotstatus_left);
        std::cout << "Moving to......" << std::endl;
    }
    std::cout << "Joint moving done!" << std::endl;
    sleep(2);
    utra_report->get_data(&rx);
    for (int i = 0; i < 6; i++) {
        pos_init[i] = rx.pose[i];
    }
    for (int i = 0; i < 6; i++) {
        pos1[i] = pos_init[i];
        pos2[i] = pos_init[i];
        pos3[i] = pos_init[i];
        pos4[i] = pos_init[i];
        pos5[i] = pos_init[i];
        pos6[i] = pos_init[i];
        pos7[i] = pos_init[i];
        pos8[i] = pos_init[i];
    }
    pos1[0] = 158.300209;  // 1 rx:158.300209, ry:-188.025544,rz:373.062064
    pos1[1] = -188.025544;
    pos1[2] = 373.062064;

    pos2[0] = 244.902749;  // 5 rx:244.902749, ry:-188.025544,rz:423.062064
    pos2[1] = -188.025544;
    pos2[2] = 423.062064;

    pos3[0] = 244.902749;  // 7 rx:244.902749, ry:-88.025544,rz:423.062064
    pos3[1] = -88.025544;
    pos3[2] = 423.062064;

    pos4[0] = 158.300209;  // 3 rx:158.300209, ry:-88.025544,rz:373.062064
    pos4[1] = -88.025544;
    pos4[2] = 373.062064;

    pos5[0] = 58.300209;  // 4 rx:58.300209, ry:-88.025544,rz:546.267144
    pos5[1] = -88.025544;
    pos5[2] = 546.267144;

    pos6[0] = 58.300209;  // 2 rx:58.300209, ry:-188.025544,rz:546.267144
    pos6[1] = -188.025544;
    pos6[2] = 546.267144;

    pos7[0] = 144.902749;  // 6 rx:144.902749, ry:-188.025544,rz:596.267144
    pos7[1] = -188.025544;
    pos7[2] = 596.267144;

    pos8[0] = 144.902749;  // 8 rx:144.902749, ry:-88.025544,rz:596.267144
    pos8[1] = -88.025544;
    pos8[2] = 596.267144;

    std::cout << "pose init: " << pos_init[0] << ", " << pos_init[1] << ", " << pos_init[2] << std::endl;
    std::cout << "pose init deg: " << pos_init[3] / (M_PI / 180) << ", " << pos_init[4] / (M_PI / 180) << ", "
              << pos_init[5] / (M_PI / 180) << std::endl;

    std::cout << "pose1: " << pos1[0] << ", " << pos1[1] << ", " << pos1[2] << std::endl;
    std::cout << "pose2: " << pos2[0] << ", " << pos2[1] << ", " << pos2[2] << std::endl;
    std::cout << "pose3: " << pos3[0] << ", " << pos3[1] << ", " << pos3[2] << std::endl;
    std::cout << "pose4: " << pos4[0] << ", " << pos4[1] << ", " << pos4[2] << std::endl;
    std::cout << "pose5: " << pos5[0] << ", " << pos5[1] << ", " << pos5[2] << std::endl;
    std::cout << "pose6: " << pos6[0] << ", " << pos6[1] << ", " << pos6[2] << std::endl;
    std::cout << "pose7: " << pos7[0] << ", " << pos7[1] << ", " << pos7[2] << std::endl;
    std::cout << "pose8: " << pos8[0] << ", " << pos8[1] << ", " << pos8[2] << std::endl;
    uint8_t ubotstatus_right;
    // while (true) {
    for (int i = 0; i < 5; ++i) {
        ret = ubot->moveto_cartesian_line(pos2, mvvelo, mvacc, 0);
        // ret = ubot->moveto_cartesian_p2p(pos1, speed, acc, 0);
        printf("moveto_cartesian_line: %d\n", ret);
        if (ret != 0) {
            uint8_t error_code[2];
            ret = ubot->get_error_code(error_code);
            std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                      << int(error_code[1]) << std::endl;
            ret = ubot->reset_err();
            printf("reset_err   : %d\n", ret);
        }
        ubotstatus_right = 1;
        while (ubotstatus_right == 1) {
            std::this_thread::sleep_for(100ms);
            ubot->get_motion_status(&ubotstatus_right);
            // std::cout << "Moving to......" << std::endl;
        }
        std::cout << "pose 2 done!" << std::endl;

        sleep(10);

        ret = ubot->moveto_cartesian_line(pos7, mvvelo, mvacc, 0);
        // ret = ubot->moveto_cartesian_p2p(pos2, speed, acc, 0);
        printf("moveto_cartesian_line: %d\n", ret);
        if (ret != 0) {
            uint8_t error_code[2];
            ret = ubot->get_error_code(error_code);
            std::cout << "get_error_code ret is " << ret << ",errorcode1 is " << int(error_code[0]) << ",errorcode2 is "
                      << int(error_code[1]) << std::endl;
            ret = ubot->reset_err();
            printf("reset_err   : %d\n", ret);
        }
        ubotstatus_right = 1;
        while (ubotstatus_right == 1) {
            std::this_thread::sleep_for(100ms);
            ubot->get_motion_status(&ubotstatus_right);
            // std::cout << "Moving to......" << std::endl;
        }
        std::cout << "pose 7 done!" << std::endl;

        sleep(10);
        ret = ubot->moveto_joint_p2p(posjointR, speed, acc, 0);
        printf("moveto_joint_p2p  : %d\n", ret);
        ubotstatus_right = 1;
        while (ubotstatus_right == 1) {
            std::this_thread::sleep_for(100ms);
            ubot->get_motion_status(&ubotstatus_right);
            // std::cout << "Moving to......" << std::endl;
        }
        sleep(1);
    }
    ret = ubot->moveto_joint_p2p(posjointR2, speed, acc, 0);
    printf("moveto_joint_p2p  : %d\n", ret);
    ubotstatus_right = 1;
    while (ubotstatus_right == 1) {
        std::this_thread::sleep_for(100ms);
        ubot->get_motion_status(&ubotstatus_right);
        // std::cout << "Moving to......" << std::endl;
    }
    sleep(1);
    printf("finish!");
    // }
    delete utra_report;
    return;
}

void utraRobot::set_left_tcp_thread()
{
    char ip[64] = "192.168.3.37";

    //arm_report_status_t rx;
    // UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
    float configure_tcp[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    int ret = ubot->set_tcp_offset(configure_tcp);
    std::this_thread::sleep_for(50ms);
    ret = ubot->saved_parm();
    std::this_thread::sleep_for(50ms);
    // delete utra_report;
    return;
}

void utraRobot::set_right_tcp_thread()
{
    char ip[64] = "192.168.5.39";

    //arm_report_status_t rx;
    // UtraReportStatus100Hz* utra_report = new UtraReportStatus100Hz(ip, 7);
    float configure_tcp[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    int ret = ubot->set_tcp_offset(configure_tcp);
    std::this_thread::sleep_for(50ms);
    ret = ubot->saved_parm();
    std::this_thread::sleep_for(50ms);
    // delete utra_report;
    return;
}
