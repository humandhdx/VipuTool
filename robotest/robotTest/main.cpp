#include "widget.h"
#include <QApplication>
#include <memory>

//#include <rclcpp/rclcpp.hpp>
//#include <moveit/move_group_interface/move_group_interface.h>
int main(int argc, char *argv[])
{
//    char devName[] = "/dev/ttyUSB0";
//    FTSensorDriver m_PtrFTSensorDevice(devName);
//    float f_FTSensorData[6];

//    while(1)
//    {
//        m_PtrFTSensorDevice.getData(f_FTSensorData);
//        std::cout<<"FTSensorData is recorded: ";
//        for(int i = 0;i < 6;i++)
//        {
//            std::cout << f_FTSensorData[i] << ",";
//        }
//        std::cout << std::endl;
//        sleep(1);

//    }
    //start_ros2cli();
    //sleep(500);
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
