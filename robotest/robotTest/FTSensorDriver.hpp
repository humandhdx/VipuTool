#ifndef FTSENSORDRIVER_HPP
#    define FTSENSORDRIVER_HPP
#    include "linux/serial.h"
#    include <iostream>

#    include <errno.h>     /*错误号定义*/
#    include <fcntl.h>     /*文件控制定义*/
#    include <pthread.h>
#    include <stdio.h>     /*标准输入输出定义*/
#    include <stdlib.h>    /*标准函数库定义*/
#    include <string.h>
#    include <sys/stat.h>  /*定义了一些返回值的结构，没看明白*/
#    include <sys/types.h> /*数据类型，比如一些XXX_t的那种*/
#    include <termios.h>   /*PPSIX 终端控制定义*/
#    include <unistd.h>    /*Unix 标准函数定义*/
#    include <thread>
#    include <mutex>
class FTSensorDriver {
    typedef void (*serial_cb_t)(struct serial_pcb* serial, unsigned char* data, int size);

    struct serial_pcb
    {
        char* dev_name;
        int fd;
        int bourd;
        serial_cb_t cb;
    };

public:
    FTSensorDriver(char* devname);
    ~FTSensorDriver();
    bool getData(float *data);

private:
    void init_serial_port(struct serial_pcb & serial, char* devname);
    int serial_init(struct serial_pcb & serial, char* devname);
    void set_speed(int fd, int speed);
    int set_Parity(int fd, int databits, int stopbits, int parity);
    int OpenDev(char* Dev);
    int serial_write(const char* data, int size, char* devname);
    int get_data(float* result, unsigned char* sourceBuff);
    void read_serial_data();

    std::thread* m_scanningThread;
    std::mutex mtx;
    float m_SensorData[6];
    serial_pcb m_serial;
    bool m_Scanning;
};
#endif  //FTSENSORDRIVER_HPP

        //int main(int argc, char **argv)
//{
//struct serial_pcb serial;
//float result[6];

//init_serial_port(&serial);
//sleep(1);

//while (1) {
//    read_serial_data(&serial, result);

//printf("%f %f %f %f %f %f\n",
//       result[0],
//       result[1],
//       result[2],
//       result[3],
//       result[4],
//       result[5]);
//}
//return 1;
//}
