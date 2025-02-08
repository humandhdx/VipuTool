#include "FTSensorDriver.hpp"
//#include <chrono>
using namespace std::literals::chrono_literals;
void FTSensorDriver::set_speed(int fd, int speed)
{
    int i;
    int status;
    int speed_arr[] = {B460800, B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300};
    int name_arr[] = {460800, 115200, 38400, 19200, 9600, 4800, 2400, 1200, 300};
    struct termios Opt;
    tcgetattr(fd, &Opt);
    for (i = 0; i < (int)(sizeof(speed_arr) / sizeof(int)); i++) {
        if (speed == name_arr[i]) {
            /**
             * tcflush函数刷清(抛弃)输入缓存(终端驱动程序已接收到，但用户程序尚未读)或输出缓存(用户程序已经写，但尚未发送)。queue参数应是下列三个常数之一：
             * TCIFLUSH刷清输入队列。
             * TCOFLUSH刷清输出m_SensorData队列。
             * TCIOFLUSH刷清输入、输出队列。
             */
            tcflush(fd, TCIOFLUSH);  //设置前flush
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            //通过tcsetattr函数把新的属性设置到串口上。
            //tcsetattr(串口描述符，立即使用或者其他标示，指向termios的指针)
            status = tcsetattr(fd, TCSANOW, &Opt);
            if (status != 0) {
                perror("tcsetattr fd1");
                return;
            }
            tcflush(fd, TCIOFLUSH);  //设置后flush
        }
    }
}

/**
 *@brief   设置串口数据位，停止位和效验位
 *@param  fd     类型  int  打开的串口文件句柄
 *@param  databits 类型  int 数据位   取值 为 7 或者8
 *@param  stopbits 类型  int 停止位   取值为 1 或者2
 *@param  parity  类型  int  效验类型 取值为N,E,O,,S
 */
int FTSensorDriver::set_Parity(int fd, int databits, int stopbits, int parity)
{
    struct termios options;
    if (tcgetattr(fd, &options) != 0) {
        perror("SetupSerial 1");
        return false;
    }
    options.c_cflag &= ~CSIZE;
    switch (databits)
    /*设置数据位数*/
    {
    case 7: options.c_cflag |= CS7; break;
    case 8: options.c_cflag |= CS8; break;
    default: fprintf(stderr, "Unsupported data size\n"); return false;
    }
    switch (parity) {
    case 'n':
    case 'N':
        options.c_cflag &= ~PARENB; /* Clear parity enable */
        options.c_iflag &= ~INPCK;  /* Enable parity checking */
        break;
    case 'o':
    case 'O':
        options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
        options.c_iflag |= INPCK;             /* Disnable parity checking */
        break;
    case 'e':
    case 'E':
        options.c_cflag |= PARENB;  /* Enable parity */
        options.c_cflag &= ~PARODD; /* 转换为偶效验*/
        options.c_iflag |= INPCK;   /* Disnable parity checking */
        break;
    case 'S':
    case 's': /*as no parity*/
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default: fprintf(stderr, "Unsupported parity\n"); return false;
    }
    /* 设置停止位*/
    switch (stopbits) {
    case 1: options.c_cflag &= ~CSTOPB; break;
    case 2: options.c_cflag |= CSTOPB; break;
    default: fprintf(stderr, "Unsupported stop bits\n"); return false;
    }
    /* Set input parity option */
    if (parity != 'n') options.c_iflag |= INPCK;
    tcflush(fd, TCIFLUSH);
    options.c_cc[VTIME] = 5;  //150; /* 设置超时15 seconds*/
    options.c_cc[VMIN] = 0;   //0; /* Update the options and do it NOW */
    options.c_cflag &= ~HUPCL;
    options.c_iflag &= ~INPCK;
    options.c_iflag |= IGNBRK;
    options.c_iflag &= ~ICRNL;
    options.c_iflag &= ~IXON;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /*Input*/
    options.c_oflag &= ~OPOST;                          /*Output*/
    /*options.c_lflag &= ~IEXTEN;
    options.c_lflag &= ~ECHOK;
    options.c_lflag &= ~ECHOCTL;
    options.c_lflag &= ~ECHOKE;
    options.c_oflag &= ~ONLCR;
    */
    //options.c_oflag = ~ICANON;
    tcflush(fd, TCIFLUSH); /* Update the options and do it NOW */
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        perror("SetupSerial 3");
        return false;
    }
    return true;
}

int FTSensorDriver::OpenDev(char* Dev)
{
    //Dev 就是设备，设备就是文件，就是给出该设备文件的路径
    int fd = open(Dev, O_RDWR | O_NONBLOCK | O_NOCTTY);  //| O_NOCTTY | O_NDELAY
    if (-1 == fd) {
        perror("Can't Open Serial Port");
        return -1;
    }
    else
        return fd;
}

int FTSensorDriver::serial_init(struct serial_pcb &serial, char* devname)
{
    // static char devname[] = "/dev/ttyUSB0";

    memset(&serial, 0, sizeof(struct serial_pcb));

    serial.dev_name = devname;
    serial.bourd = 460800;
    serial.fd = OpenDev(m_serial.dev_name);

    set_speed(m_serial.fd, m_serial.bourd);

    if (set_Parity(m_serial.fd, 8, 1, 'N') == false) {
        printf("Set Parity Error\n");
        return -1;
    }

    return 0;
}

int FTSensorDriver::serial_write(const char* data, int size, char* devname)
{
    int fd = m_serial.fd;
    serial_cb_t dfn = NULL;
    if (fd == -1) {
        printf("serial doesn't exist\r\n");
    }
    int res;
    res = write(fd, data, size);
    if (res == -1) {
        close(fd);
        printf("serial doesn't exist\r\n");
        sleep(2);
        if (m_serial.cb != NULL) {
            dfn = m_serial.cb;
        }
        serial_init(m_serial,devname);
        m_serial.cb = dfn;
    }
    return res;
}

int FTSensorDriver::get_data(float* result, unsigned char* sourceBuff)
{
    int start_index_temp;
    unsigned char temp[60];
    unsigned char targetBuff[30];

    memcpy(temp, sourceBuff, 60);

    for (int index = 0; index <= 56; index++) {
        if (temp[index] == 0x48 && temp[index + 1] == 0xAA) {
            start_index_temp = index;
            break;
        }
    }

    int index_cnter_temp = 0;
    while (index_cnter_temp < 30) {
        targetBuff[index_cnter_temp] = temp[start_index_temp + index_cnter_temp];
        index_cnter_temp++;
    }

    if (targetBuff[26] != 13 || targetBuff[27] != 10) {
        return 0;
    }

    //deal with 6 floats next
    unsigned tempValue;
    for (int index = 0; index <= 5; index++) {
        tempValue = (targetBuff[2 + index * 4 + 3] << 24) + (targetBuff[2 + index * 4 + 2] << 16) +
                    (targetBuff[2 + index * 4 + 1] << 8) + targetBuff[2 + index * 4];

        memcpy(&result[index], &tempValue, 4);
    }
    return 1;
}

void FTSensorDriver::init_serial_port(struct serial_pcb & serial, char *devname)

{
    serial_init(serial, devname);

    serial_write("\x48\xAA\x0D\x0A", 4, devname);

    return;
}

void FTSensorDriver::read_serial_data()
{
    unsigned char sourceBuff[60];
    int nread = -1;
    while(m_Scanning){
        nread = read(m_serial.fd, sourceBuff, 60);
        if (!get_data(m_SensorData, sourceBuff)) {
            nread = read(m_serial.fd, sourceBuff, 60);
            if (!get_data(m_SensorData, sourceBuff)) {
                nread = read(m_serial.fd, sourceBuff, 60);
                if (!get_data(m_SensorData, sourceBuff)) {
                    printf("error occur\n");
                }
            }
        }
        tcflush(m_serial.fd,TCIOFLUSH);
        std::this_thread::sleep_for(20ms);
    }
}

FTSensorDriver::FTSensorDriver(char* devname)
{
    for(int i = 0;i < 6;i++)
    {
        m_SensorData[i] = 0;
    }
    init_serial_port(m_serial,devname);
    m_Scanning = true;
    m_scanningThread = new std::thread(&FTSensorDriver::read_serial_data, this);
}
FTSensorDriver::~FTSensorDriver()
{
    m_Scanning = false;
    if(m_scanningThread != nullptr)
    {
        m_scanningThread->join();
        delete m_scanningThread;
    }
}
bool FTSensorDriver::getData(float *data)
{
    mtx.lock();
    for(int i = 0;i < 6;i++)
    {
        data[i] = m_SensorData[i];
    }
    mtx.unlock();
    return true;
}
