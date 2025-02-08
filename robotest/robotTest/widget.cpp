#include "widget.h"
#include "./ui_widget.h"
#include "iostream"
#include "string"
#include <thread>
#include <chrono>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

//#include <QButtonGroup>
#include <QDebug>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_box = new QButtonGroup;
    m_box->setExclusive(true);
    m_box->addButton(ui->RbtnLeftArm);
    m_box->addButton(ui->RbtnRightArm);
    ui->RbtnLeftArm->setChecked(true);
    ui->RbtnLeftArm->setChecked(false);
    m_armChoose = ArmType::left;

    ui->EditJointPos1->setDisabled(true);
    ui->EditJointPos2->setDisabled(true);
    ui->EditJointPos3->setDisabled(true);
    ui->EditJointPos4->setDisabled(true);
    ui->EditJointPos5->setDisabled(true);
    ui->EditJointPos6->setDisabled(true);
    ui->EditJointPos7->setDisabled(true);
    ui->EditTotalPosNum->setDisabled(true);
    ui->EditCurrentPosNum->setDisabled(true);
    ui->EditRecordCounter->setDisabled(true);
    ui->EditDataPath->setDisabled(true);

    ui->BtnSetAdjust->setDisabled(true);
    ui->BtnHoldandRecord->setDisabled(false);
    ui->BtnSaveData->setDisabled(true);
    ui->BtnMoveToNextPos->setDisabled(true);
    ui->CkbRecordFTSensor->setEnabled(true);
   //QStringList items;
    // items << "回初始位" << "位置准确度1" << "位置准确度2"<<"位置准确度3"<<"位置准确度4"
    //     << "位置重复性1" << "位置重复性2"<<"位置重复性3"<<"位置重复性4"<<"位置重复性5"
    //       <<"位置重复性6"<< "位置重复性7"<<"位置重复性8"<<"位置重复性9"<<"位置重复性10";
    MachineSettings returnsetting = {0, 0, 1000, 0, 0, 0};
    ui->comboBox->addItem("回初始位",QVariant::fromValue(returnsetting));
    MachineSettings psettings1 = {4, -5, 1500, 0, 0, 0};
    MachineSettings psettings2 = {4, -10, 1500, 0, 0, 0};
    MachineSettings psettings3 = {4, -15, 1500, 0, 0, 0};
    MachineSettings psettings4 = {4, -20, 1500, 0, 0, 0};
    // 将实例存储在数组中
    MachineSettings psettingsArray[] = {psettings1, psettings2, psettings3, psettings4};

    MachineSettings rsettings1 = {4, 0, 1500, 0, 0, 0};
    MachineSettings rsettings2 = {4, -15, 1500, 0, 0, 0};
    MachineSettings rsettings3 = {4, 15, 1500, 0, 0, 0};
    MachineSettings rsettings4 = {4, -15, 1000, 0, 0, 0};
    MachineSettings rsettings5 = {4, 15, 1500, 0, 0, 0};
    MachineSettings rsettings6 = {4, -15, 1000, 0, 0, 0};
    MachineSettings rsettings7 = {4, 15, 1500, 0, 0, 0};
    MachineSettings rsettings8 = {4, -15, 1000, 0, 0, 0};
    MachineSettings rsettings9 = {4, 15, 1500, 0, 0, 0};
    MachineSettings rsettings10 = {4, -15, 1000, 0, 0, 0};
    // 将实例存储在数组中
    MachineSettings rsettingsArray[] = {rsettings1, rsettings2, rsettings3, rsettings4,
    rsettings5, rsettings6, rsettings7, rsettings8,rsettings9, rsettings10};

    for (int i = 0; i < 4; ++i) {
        ui->comboBox->addItem("位置准确度"+QString::number(i+1),QVariant::fromValue(psettingsArray[i]));
    }
    for (int j = 0; j < 10; ++j) {
        ui->comboBox->addItem("位置重复性"+QString::number(j+1),QVariant::fromValue(rsettingsArray[j]));
    }
    rbt = new utraRobot();
    m_posCounter = 0;
    ui->EditRecordCounter->setText(QString::number(m_posCounter));
    connect(ui->BtnConnectRobot,&QPushButton::clicked,this,&Widget::initArm);
    connect(ui->BtnSetAdjust,&QPushButton::clicked,this,&Widget::SetAdjustStatus);
    connect(ui->BtnHoldandRecord,&QPushButton::clicked,this,&Widget::setHoldStatusandRecord);
    connect(ui->BtnSaveData,&QPushButton::clicked,this,&Widget::savetcpdata);
    connect(ui->BtnIputData,&QPushButton::clicked,this,&Widget::inputdata);
    connect(ui->BtnMoveToNextPos,&QPushButton::clicked,this,&Widget::movetoNextPos);
    connect(ui->CkbRecordFTSensor,&QCheckBox::stateChanged,this,&Widget::setStateRecordFTSensorDataOrNot);

    //left btn
    connect(ui->leftbtn1,&QPushButton::clicked,this,&Widget::left_arm_gravity_test);
    connect(ui->leftbtn2,&QPushButton::clicked,this,&Widget::left_arm_accuracy_test);
    connect(ui->leftbtn3,&QPushButton::clicked,this,&Widget::left_arm_workspace_test);
    connect(ui->leftbtn4,&QPushButton::clicked,this,&Widget::left_arm_repeatabilitity_test);
    connect(ui->leftbtn5,&QPushButton::clicked,this,&Widget::set_left_tcp_offset);

    //right btn
    connect(ui->rightbtn1,&QPushButton::clicked,this,&Widget::right_arm_gravity_test);
    connect(ui->rightbtn2,&QPushButton::clicked,this,&Widget::right_arm_accuracy_test);
    connect(ui->rightbtn3,&QPushButton::clicked,this,&Widget::right_arm_workspace_test);
    connect(ui->rightbtn4,&QPushButton::clicked,this,&Widget::right_arm_repeatabilitity_test);
    connect(ui->rightbtn5,&QPushButton::clicked,this,&Widget::set_right_tcp_offset);


    //huatai btn

    connect(ui->huataibtn1,&QPushButton::clicked,this,&Widget::set_huatai_return_zero);
    connect(ui->huataibtn2,&QPushButton::clicked,this,&Widget::set_huatai_move);
    connect(ui->huataibtn3,&QPushButton::clicked,this,&Widget::set_needle_rotation);
    connect(ui->huataibtn4,&QPushButton::clicked,this,&Widget::set_needle_rotation_stop);


    m_PosRecord.clear();
    m_PosRecord2.clear();
    m_PosData.clear();
    m_TcpRecord.clear();
    m_posIndex = 0;
    m_NeedRecordFTSensorData = false;
    m_FTSensorData.clear();
    ui->EditCurrentPosNum->setText(QString::number(m_posIndex));

    m_box2 = new QButtonGroup;
    m_box2->setExclusive(true);
    m_box2->addButton(ui->RdbJoint);
    m_box2->addButton(ui->RdbCastesian);
    ui->RdbJoint->setChecked(true);
    ui->CkbRecordFTSensor->setChecked(false);



//    char devName[] = "/dev/ttyUSB0";
//    m_PtrFTSensorDevice = new FTSensorDriver(devName);

}


Widget::~Widget()
{
    if(ui!=nullptr)
    {
        delete ui;
    }

    if(rbt!=nullptr)
     {
         delete rbt;
     }
    if(m_box2!=nullptr)
    {
        delete m_box2;
    }
    if(m_box!=nullptr)
    {
        delete m_box;
    }
//    if(m_PtrFTSensorDevice != nullptr)
//    {
//        delete m_PtrFTSensorDevice;
//    }
}

void Widget::left_arm_gravity_test()
{
    std::thread *t1 = new std::thread(&utraRobot::left_arm_gravity_thread,rbt);
  // std::unique_ptr<std::thread> *t1 =std::make_unique<std::thread>(&utraRobot::left_arm_gravity_thread,this);
     t1->detach();
    qDebug("左臂负载测试");
    // auto result= rbt->left_arm_gravity_test();
    // if(result){
    //     qDebug("left_arm_gravity_test_sucess");
    // }
    return;
}

void Widget::right_arm_gravity_test()
{
    std::thread *t1 = new std::thread(&utraRobot::right_arm_gravity_thread,rbt);
    t1->detach();
    qDebug("右臂负载测试");
    // auto result= rbt->right_arm_gravity_test();
    // if(result){
    //     qDebug("right_arm_gravity_test_sucess");
    // }
    return;
}

void Widget::left_arm_accuracy_test()
{
    std::thread *t1 = new std::thread(&utraRobot::left_arm_accuracy_thread,rbt);
    t1->detach();
    qDebug("左臂负载测试");
    // auto result= rbt->left_arm_accuracy_test();
    // if(result){
    //     qDebug("left_arm_accuracy_test_sucess");
    // }
    return;
}

void Widget::left_arm_workspace_test()
{
    std::thread *t1 = new std::thread(&utraRobot::left_arm_workspace_thread,rbt);
    t1->detach();
    qDebug("左臂工作空间测试");
    // auto result= rbt->left_arm_workspace_test();
    // if(result){
    //     qDebug("left_arm_workspace_test_sucess");
    // }
    return;
}

void Widget::left_arm_repeatabilitity_test()
{
    std::thread *t1 = new std::thread(&utraRobot::left_arm_reppeat_thread,rbt);
    t1->detach();
    qDebug("左臂重复性测试");
     //auto result= rbt->left_arm_repeatabilitity_test();
    // if(result){
    //     qDebug("left_arm_repeatabilitity_test_sucess");
    // }
    return;
}

void Widget::right_arm_accuracy_test()
{
    std::thread *t1 = new std::thread(&utraRobot::right_arm_accuracy_thread,rbt);
    t1->detach();
    qDebug("右臂精度测试");
    // auto result= rbt->right_arm_accuracy_test();
    // if(result){
    //     qDebug("right_arm_accuracy_test_sucess");
    // }
    return;
}

void Widget::right_arm_workspace_test()
{
    std::thread *t1 = new std::thread(&utraRobot::right_arm_workspace_thread,rbt);
    t1->detach();
    qDebug("右臂工作空间测试");
    // auto result= rbt->right_arm_workspace_test();
    // if(result){
    //     qDebug("right_arm_workspace_test_sucess");
    // }
    return;
}

void Widget::right_arm_repeatabilitity_test()
{
    std::thread *t1 = new std::thread(&utraRobot::right_arm_repeat_thread,rbt);
    t1->detach();
     qDebug("右臂重复性测试");
    // auto result= rbt->right_arm_repeatabilitity_test();
    // if(result){
    //     qDebug("right_arm_repeatabilitity_test_sucess");
    // }
     return;
}

void Widget::set_left_tcp_offset()
{
    std::thread *t1 = new std::thread(&utraRobot::set_left_tcp_thread,rbt);
    t1->detach();
     qDebug("左臂设置tcp");
    // auto result= rbt->set_left_tcp_offset();
    // if(result){
    //     qDebug("set_left_tcp_offset_sucess");
    // }
     return;
}

void Widget::set_right_tcp_offset()
{
    std::thread *t1 = new std::thread(&utraRobot::set_right_tcp_thread,rbt);
    t1->detach();
    qDebug("右臂设置tcp");
    //uto result= rbt->set_right_tcp_offset();
    // if(result){
    //     qDebug("set_right_tcp_offset_sucess");
    // }
    return;
}

void Widget::set_huatai_return_zero()
{
    MachineSettings setting = {4, -15, 1500, 0, 0, 0}; // 初始化参数
    callPunchCommandService(setting); // 调用服务
}

void Widget::set_huatai_move()
{
    auto index = ui->comboBox->currentIndex();
    qDebug() << "index:" << index;

    QVariant var = ui->comboBox->itemData(index);

    // 检查是否可以转换
    if (var.canConvert<MachineSettings>()) {
        MachineSettings setting = var.value<MachineSettings>(); // 正确地获取 MachineSettings
        qDebug() << "当前选中项:"
                 << setting.work_mode
                 << setting.slide_depth
                 << setting.slide_velocity
                 << setting.spin_velocity
                 << setting.spin_revolution
                 << setting.hold_millisecond;

        // 调用服务函数
        if(0<index&&index<5){
            qDebug()<<"index:"<<index;
            QVariant var2 = ui->comboBox->itemData(0);
            MachineSettings return_setting = var2.value<MachineSettings>();
            callPunchCommandService(return_setting);
            sleep(5);
            callPunchCommandService(setting);
        }
        else{
            callPunchCommandService(setting);
        }
    } else {
        qDebug() << "无法转换为 MachineSettings";
    }



}

void Widget::set_needle_rotation()
{
    MachineSettings setting = {3, 0, 0, 3000, 0, 0};
    callPunchCommandService(setting); // 调用服务
}

void Widget::set_needle_rotation_stop()
{
    MachineSettings setting = {3, 0, 0, 0, 0, 0};
    callPunchCommandService(setting); // 调用服务
}

void Widget::callPunchCommandService(const MachineSettings &setting)
{
    auto pid = fork();
    if (pid == -1) {
        std::cout << "Failed to start script" << std::endl;
        return; // 不需要返回 -1，因为返回类型是 void
    } else if (pid == 0) { // 子进程
        // 构建 JSON 字符串
        std::string jsonParams = "{work_mode: " + std::to_string(setting.work_mode) +
                                 ", slide_depth: " + std::to_string(setting.slide_depth) +
                                 ", slide_velocity: " + std::to_string(setting.slide_velocity) +
                                 ", spin_velocity: " + std::to_string(setting.spin_velocity) +
                                 ", spin_revolution: " + std::to_string(setting.spin_revolution) +
                                 ", hold_millisecond: " + std::to_string(setting.hold_millisecond) + "}";

        // 使用 execl 调用 ROS 2 服务
        execl("/opt/ros/humble/bin/ros2",
              "ros2", "service", "call", "/punch_command",
              "fue_interfaces/srv/PunchCommand",
              jsonParams.c_str(), // 转换为 C 字符串
              (char *)nullptr); // 最后一个参数必须是 nullptr

        // 如果 execl 返回，表示出错
        std::cerr << "Failed to execute ros2 command" << std::endl;
        exit(1); // 退出子进程
    } else { // 父进程
        std::cout << "Child process id: " << pid << std::endl;
        return; // 不需要返回 pid，因为返回类型是 void
    }
}

void Widget::initArm()
{
    bool left = ui->RbtnLeftArm->isChecked();
      bool res;
      if(left == true)
      {
          m_armChoose = ArmType::left;
           std::cout<<"connect left arm..."<<std::endl;
           res = rbt->initArm(ArmType::left);
      }else{
          m_armChoose = ArmType::right;
           std::cout<<"connect right arm..."<<std::endl;
          res = rbt->initArm(ArmType::right);
      }
      if(res == true)
      {
          ui->EditArmStatus->setText("连接成功!");
          ui->BtnSetAdjust->setDisabled(false);
          ui->BtnHoldandRecord->setDisabled(false);
          ui->BtnSaveData->setDisabled(false);
      }else{
          ui->EditArmStatus->setText("连接失败,请重试!");
          ui->BtnSetAdjust->setDisabled(true);
          ui->BtnHoldandRecord->setDisabled(true);
          ui->BtnSaveData->setDisabled(true);
      }
    return;
}
bool Widget::SetAdjustStatus()
{
    ArmStatus st;
    st = ArmStatus::ADJUST;
    rbt->SetStatus(st);

    ui->EditJointPos1->setDisabled(false);
    ui->EditJointPos2->setDisabled(false);
    ui->EditJointPos3->setDisabled(false);
    ui->EditJointPos4->setDisabled(false);
    ui->EditJointPos5->setDisabled(false);
    ui->EditJointPos6->setDisabled(false);
    ui->EditJointPos7->setDisabled(false);
    ui->BtnHoldandRecord->setDisabled(false);
    ui->BtnSetAdjust->setDisabled(true);
    return true;
}
bool Widget::setHoldStatusandRecord()
{
    ArmStatus st;
    st = ArmStatus::HOLD;
    rbt->SetStatus(st);
    sleep(1);
    JointPos robotPos;
    rbt->getJointPos(robotPos);
//    m_PosRecord.push_back(robotPos);

    TcpPose tcp_pose;
    rbt->getTcpPos(tcp_pose);
    m_TcpRecord.push_back(tcp_pose);

    m_posCounter += 1;
    ui->EditJointPos1->setText(QString::number(robotPos.data[0]));
    ui->EditJointPos2->setText(QString::number(robotPos.data[1]));
    ui->EditJointPos3->setText(QString::number(robotPos.data[2]));
    ui->EditJointPos4->setText(QString::number(robotPos.data[3]));
    ui->EditJointPos5->setText(QString::number(robotPos.data[4]));
    ui->EditJointPos6->setText(QString::number(robotPos.data[5]));
    ui->EditJointPos7->setText(QString::number(robotPos.data[6]));
    ui->EditRecordCounter->setText(QString::number(m_posCounter));
    ui->BtnHoldandRecord->setDisabled(true);//true
    ui->BtnSetAdjust->setDisabled(false);
    return true;
}
bool Widget::GetStatus(ArmStatus &status)
{
    rbt->GetStatus(status);
    return true;
}

void Widget::savetcpdata()
{
    if(m_TcpRecord.empty()){
        QMessageBox::warning(this,"警告","无数据");
    }else
    {
        QFileInfo path("..//..//data/");
        QString abpath = path.absolutePath();
        QDir dir;
        dir.mkpath(abpath);
        QString createTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss");
        QString fileName = abpath + "/" + "TcpPos-" + createTime + ".txt";
        //qDebug()<<fileName;
        QFile file(fileName);
        file.open(QFile::WriteOnly|QFile::Text|QIODevice::Append);
        std::vector<TcpPose>::iterator it = m_TcpRecord.begin();
        QString str;
        for (;it != m_TcpRecord.end();++it) {
            str = "";
            str = str + QString::number(it->data[0],'f',8);
            for (int i = 1;i < 6; i++) {
                str = str + " " + QString::number(it->data[i],'f',8);
            }
            str = str + "\n";
            QByteArray tmp = str.toUtf8();
            file.write(tmp);
        }
        file.close();
        m_TcpRecord.clear();
        m_posCounter = 0;
    }
    ui->EditJointPos1->setDisabled(true);
    ui->EditJointPos2->setDisabled(true);
    ui->EditJointPos3->setDisabled(true);
    ui->EditJointPos4->setDisabled(true);
    ui->EditJointPos5->setDisabled(true);
    ui->EditJointPos6->setDisabled(true);
    ui->EditJointPos7->setDisabled(true);
    ui->EditRecordCounter->setText(QString::number(m_posCounter));
}


//void Widget::savedata()
//{
//    if(m_PosRecord.empty()){
//        QMessageBox::warning(this,"警告","无数据");
//    }else
//    {
//        QFileInfo path("..//..//data/");
//        QString abpath = path.absolutePath();
//        QDir dir;
//        dir.mkpath(abpath);
//        QString createTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss");
//        QString fileName = abpath + "/" + "JointPos-" + createTime + ".txt";
//        //qDebug()<<fileName;
//        QFile file(fileName);
//        file.open(QFile::WriteOnly|QFile::Text|QIODevice::Append);
//        std::vector<JointPos>::iterator it = m_PosRecord.begin();
//        QString str;
//        for (;it != m_PosRecord.end();++it) {
//            str = "";
//            for (int i = 0;i < 7; i ++) {
//                str = str + QString::number(it->data[i],'f',8) + ",";
//            }
//            str = str + "\n";
//            QByteArray tmp = str.toUtf8();
//            file.write(tmp);
//        }
//        file.close();
//        m_PosRecord.clear();
//        m_posCounter = 0;
//    }
//    ui->EditJointPos1->setDisabled(true);
//    ui->EditJointPos2->setDisabled(true);
//    ui->EditJointPos3->setDisabled(true);
//    ui->EditJointPos4->setDisabled(true);
//    ui->EditJointPos5->setDisabled(true);
//    ui->EditJointPos6->setDisabled(true);
//    ui->EditJointPos7->setDisabled(true);
//    ui->EditRecordCounter->setText(QString::number(m_posCounter));
//}

//void Widget::savedataduringRecord()
//{
//    if(m_PosRecord2.empty()){
//        QMessageBox::warning(this,"警告","无数据");
//    }else
//    {
//        QFileInfo path("..//..//dataDuringRecord/");
//        QString abpath = path.absolutePath();
//        QDir dir;
//        dir.mkpath(abpath);
//        QString createTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss");
//        QString fileName = abpath + "/" + "JointPosDuringRecord-" + createTime + ".txt";
//        //qDebug()<<fileName;
//        QFile file(fileName);
//        file.open(QFile::WriteOnly|QFile::Text|QIODevice::Append);
//        std::vector<JointPos>::iterator it = m_PosRecord2.begin();
//        QString str;
//        for (;it != m_PosRecord2.end();++it) {
//            str = "";
//            for (int i = 0;i < 7; i ++) {
//                str = str + QString::number(it->data[i],'f',12) + ",";
//            }
//            str = str + "\n";
//            QByteArray tmp = str.toUtf8();
//            file.write(tmp);
//        }
//        file.close();
//        m_PosRecord2.clear();
//        m_posCounter = 0;
//    }
//    ui->EditJointPos1->setDisabled(true);
//    ui->EditJointPos2->setDisabled(true);
//    ui->EditJointPos3->setDisabled(true);
//    ui->EditJointPos4->setDisabled(true);
//    ui->EditJointPos5->setDisabled(true);
//    ui->EditJointPos6->setDisabled(true);
//    ui->EditJointPos7->setDisabled(true);
//    ui->EditRecordCounter->setText(QString::number(m_posCounter));
//    if(m_NeedRecordFTSensorData)
//    {
//        if(m_FTSensorData.empty()){
//            QMessageBox::warning(this,"警告","力传感器数据无数据");
//        }else
//        {
//            QFileInfo path("..//..//dataDuringRecord/");
//            QString abpath = path.absolutePath();
//            QDir dir;
//            dir.mkpath(abpath);
//            QString createTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss");
//            QString fileName = abpath + "/" + "FTSensorDataDuringRecord-" + createTime + ".txt";
//            //qDebug()<<fileName;
//            QFile file(fileName);
//            file.open(QFile::WriteOnly|QFile::Text|QIODevice::Append);
//            std::vector<FTSensorData>::iterator it = m_FTSensorData.begin();
//            QString str;
//            for (;it != m_FTSensorData.end();++it) {
//                str = "";
//                for (int i = 0;i < 6; i ++) {
//                    str = str + QString::number(it->data[i],'f',12) + ",";
//                }
//                str = str + "\n";
//                QByteArray tmp = str.toUtf8();
//                file.write(tmp);
//            }
//            file.close();
//            m_FTSensorData.clear();
//            m_posCounter = 0;
//        }
//    }
//}

void Widget::inputdata()
{
    m_PosData.clear();
    QString filePath = QFileDialog::getOpenFileName(this,"打开文件","..//..//data/");
    if(filePath.isEmpty())
    {
        QMessageBox::warning(this,"警告","文件路径不能为空");
        return;
    }
    else
    {
        ui->EditDataPath->setText(filePath);
        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QByteArray array;
        while(!file.atEnd())
        {
            array = file.readLine();
            QString qstr = QString(array);
            QStringList strlist = qstr.split(",");
            JointPos tmpPos;
            QStringList::iterator ittmp;
            ittmp = strlist.begin();
            int num = 0;
            for (;ittmp!=strlist.end();ittmp ++) {
                if (num < 7)
                {
                    tmpPos.data[num] = ittmp->toDouble();
                }
                num ++;
            }
            m_PosData.push_back(tmpPos);
        }

    }
    m_it = m_PosData.begin();
    m_posIndex = 0;
    ui->EditCurrentPosNum->setText(QString::number(m_posIndex));
    ui->EditTotalPosNum->setText(QString::number(m_PosData.size()));
    ui->BtnMoveToNextPos->setDisabled(false);
}

void Widget::movetoNextPos()
{
    //std::thread *t1 = new std::thread(&Widget::movetoNextPosThread,this);
   // t1->join();
   // sleep(1);
   // delete t1;
   movetoNextPosThread();
    return;
}

void Widget::movetoNextPosThread()
{
    ui->BtnMoveToNextPos->setDisabled(true);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    JointPos targetPos;
    targetPos = *m_it;
    rbt->movetoNextPos(targetPos);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    JointPos robotPos;
    rbt->getJointPos(robotPos);
     std::cout<<"123456"<<std::endl;
    m_PosRecord2.push_back(robotPos);
    std::cout<<"1234567"<<std::endl;
    if(m_NeedRecordFTSensorData)
    {
        FTSensorData FTSensorDatatmp;
        float f_FTSensorData[6];
        //m_PtrFTSensorDevice->getData(f_FTSensorData);
        std::cout<<"FTSensorData is recorded: ";
        for(int i = 0;i < 6;i++)
        {
            FTSensorDatatmp.data[i] = double(f_FTSensorData[i]);
            std::cout << f_FTSensorData[i] << ",";
        }
        std::cout << std::endl;
        m_FTSensorData.push_back(FTSensorDatatmp);
    }
    std::cout<<"12345678"<<std::endl;
    m_it ++;
    ui->BtnMoveToNextPos->setDisabled(false);
    m_posIndex ++;
    ui->EditCurrentPosNum->setText(QString::number(m_posIndex));

    if(m_it == m_PosData.end())
    {
        ui->BtnMoveToNextPos->setDisabled(true);
        //savedataduringRecord();
    }
    return;
}

void Widget::setStateRecordFTSensorDataOrNot()
{
    if(ui->CkbRecordFTSensor->isChecked())
    {
        m_NeedRecordFTSensorData = true;
        std::cout<<"need record FTSensor data"<<std::endl;
    }
    else
    {
        m_NeedRecordFTSensorData = false;
        std::cout<<"need not record FTSensor data"<<std::endl;
    }

}


