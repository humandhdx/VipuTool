#include "cameramanager.h"
#include <QThread>

cameraManager::cameraManager(QObject *parent): QObject{parent},cali_type_(CalibritionType::Null),
    left_id_(-1),
    right_id_(-1),
    middle_id_(-1),
    capture_left_flag_(false),
    capture_middle_flag_(false),
    capture_right_flag_(false),
    failed_count_left_(0),
    failed_count_middle_(0),
    failed_count_right_(0),
    handeye_image_count_(0),
    global_left_image_count_(0),
    global_right_image_count_(0),
    middle_image_count_(0)
{
    buff_left_ = new uchar[4000 * 3000];
    buff_middle_=new uchar[1920 * 1080];
    buff_right_ = new uchar[4000 * 3000];
    memset(buff_left_, 0, 4000 * 3000);
    memset(buff_middle_, 0, 1920 * 1080);
    memset(buff_right_, 0, 4000 * 3000);
    vec_buff_left_ = new std::vector<uchar>();
    vec_buff_middle_ = new std::vector<uchar>();
    vec_buff_right_ = new std::vector<uchar>();
    vec_buff_left_->reserve(4000 * 3000);
    vec_buff_middle_->reserve(1920*1080);
    vec_buff_right_->reserve(4000 * 3000);
}

cameraManager::~cameraManager()
{
    stopCamera();
    if (buff_left_) {
        delete[] buff_left_;
        buff_left_ = nullptr;
    }
    if (buff_middle_) {
        delete[] buff_middle_;
        buff_middle_ = nullptr;
    }
    if (buff_right_) {
        delete[] buff_right_;
        buff_right_ = nullptr;
    }

    if (vec_buff_left_) {
        vec_buff_left_->clear();
        delete vec_buff_left_;
        vec_buff_left_ = nullptr;
    }
    if (vec_buff_middle_) {
        vec_buff_middle_->clear();
        delete vec_buff_middle_;
        vec_buff_middle_ = nullptr;
    }

    if (vec_buff_right_) {
        vec_buff_right_->clear();
        delete vec_buff_right_;
        vec_buff_right_ = nullptr;
    }
}

void cameraManager::stopCamera()
{
    shutdown_leftcapture();
    shutdown_rightcapture();
    shutdown_middlecapture();
}

QString cameraManager::getCaptureImageSavePath()
{
    QString savePath=QString();
    if (savePath.isEmpty()) {
        savePath = QFileDialog::getExistingDirectory(nullptr,
                                                     tr("选择保存图片的文件夹"),
                                                     QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

        if (savePath.isEmpty()) {
            qWarning() << "用户取消了保存路径选择";
            return QString();
        }
    }
    return savePath;
}

bool cameraManager::captureImage(QString path, CalibritionType type)
{
    bool result=false;
    std::string save_path="";
    std::ostringstream oss;
    switch (type) {
    case CalibritionType::HandEyeCalibrition:
        if(right_image_0_.empty()){
            return false;
        }
        handeye_image_count_++; // 编号自增
        oss << path.toStdString() << "/HandEye_Image"
            << std::setw(3) << std::setfill('0') << handeye_image_count_
            << ".jpg";
        save_path = oss.str();
        result = cv::imwrite(save_path, right_image_0_);
        if (result) {
            std::cout << "图像已成功保存到: " << save_path << std::endl;
        } else {
            std::cerr << "保存图像失败" << std::endl;
        }
        break;
    case CalibritionType::GlobalCalibrition:
        if(right_image_0_.empty()||left_image_0_.empty()){
            return false;
        }
         global_left_image_count_++;
         global_right_image_count_++;
        oss << path.toStdString() << "/Global_Left_Image"
            << std::setw(3) << std::setfill('0') << global_left_image_count_
            << ".jpg";
        save_path = oss.str();
        result = cv::imwrite(save_path, left_image_0_);
        if (result) {
            std::cout << "图像已成功保存到: " << save_path << std::endl;
        } else {
            std::cerr << "保存图像失败" << std::endl;
        }
        oss << path.toStdString() << "/Global_Right_Image"
            << std::setw(3) << std::setfill('0') << global_right_image_count_
            << ".jpg";
        save_path = oss.str();
        result = cv::imwrite(save_path, right_image_0_);
        if (result) {
            std::cout << "图像已成功保存到: " << path.toStdString() << std::endl;
        } else {
            std::cerr << "保存图像失败" << std::endl;
        }
        break;
    case CalibritionType::MiddleCalibrition:
        if(middle_image_0_.empty()){
            return false;
        }
        middle_image_count_++; // 编号自增
        oss << path.toStdString() << "/Middle_Image"
            << std::setw(3) << std::setfill('0') << middle_image_count_
            << ".jpg";
        save_path = oss.str();
        result = cv::imwrite(save_path, middle_image_0_);
        if (result) {
            std::cout << "图像已成功保存到: " << path.toStdString() << std::endl;
        } else {
            std::cerr << "保存图像失败" << std::endl;
        }
        break;
        break;
    default:
        break;
    }
    return result;
}

void cameraManager::resetCaptureCount()
{
     handeye_image_count_=0;
     global_left_image_count_=0;
     global_right_image_count_=0;
}

void cameraManager::init_cam()
{
    left_id_ = -1;
    right_id_ = -1;
    middle_id_=-1;
    //读取当前系统所有设备
    std::vector<std::string> devs{};
    {  //read carmera devs
        DIR *dfd;
        const std::string pathname = "/dev";
        dfd = opendir(pathname.c_str());
        dirent *dp = nullptr;
        while ((dp = readdir(dfd)) != nullptr) {
            if (strncmp(dp->d_name, ".", 1) == 0)
                continue; /* 跳过当前文件夹和上一层文件夹以及隐藏文件*/
            std::string name{dp->d_name};
            if (strncmp(dp->d_name, "video", 5) == 0 && strlen(dp->d_name) <= 7) {
                std::string dev = pathname + "/" + dp->d_name;
                devs.push_back(dev);
            }
        }
        closedir(dfd);
    }
    //获取相机的id
    v4l2_capability cap;
    for (const auto &name : devs) {
        int vfd = open(name.c_str(), O_RDWR);
        if (vfd < 0) {
            //need not close
            continue;
        }

        if (ioctl(vfd, VIDIOC_QUERYCAP, &cap) == -1) {
            close(vfd);
            continue;
        }
        if (!(V4L2_CAP_VIDEO_CAPTURE & cap.device_caps)) {
            close(vfd);
            continue;
        }
        if (!(V4L2_CAP_VIDEO_CAPTURE & cap.device_caps)) {
            close(vfd);
            continue;
        }

        int dev_num_cur =
            atoi(name.substr(10).c_str());  //10 = len for "/dev/video", to get the num behind

        if (cali_type_ == CalibritionType::GlobalCalibrition) {  //for global
            if (strncmp((const char *)cap.card, "FueCamLeft", 10) == 0) {
                left_id_ = dev_num_cur;
            }
            else if (strncmp((const char *)cap.card, "FueCamRight", 11) == 0) {
                right_id_ = dev_num_cur;
            }
        }

        if (cali_type_ == CalibritionType::HandEyeCalibrition) {  //for global
            qDebug()<<(const char *)cap.card;
            if (strncmp((const char *)cap.card, "FueCamRight", 11) == 0) {
                right_id_ = dev_num_cur;
            }
        }
        if (cali_type_ == CalibritionType::HandEyeCalibrition){
            if (strncmp((const char *)cap.card, "SPCA2100", 8) == 0) {
                middle_id_=dev_num_cur;
            }
        }
        close(vfd);
    }

}

bool cameraManager::start_left_capture()
{
    if (!capture_left_flag_) {
        shutdown_leftcapture();
        init_cam();
        if(left_id_==-1){
            return false;
        }
        restart_left_cam();
        //std::this_thread::sleep_for(50ms);
        QThread::sleep(0.1);
    }
    failed_count_left_ = 0;
    capture_left_flag_ = true;
    if (run_servo_task_left_ == nullptr) {
        int priority = 49;
        run_servo_task_left_ = new RtPeriodicMemberFun2<cameraManager>(capture_period_,
                                                                       "run_capture_left_task",
                                                                       1024 * 1024,
                                                                       priority,
                                                                       &cameraManager::capture_left,
                                                                       this);
        run_servo_task_left_->start();
    }
    return true;
}

bool cameraManager::start_right_capture()
{
    if (!capture_right_flag_) {
        shutdown_rightcapture();
        init_cam();
        if(right_id_==-1){
            return false;
        }
        restart_right_cam();
        //std::this_thread::sleep_for(50ms);
        QThread::sleep(0.1);
    }
    failed_count_right_ = 0;
    capture_right_flag_ = true;
    if (run_servo_task_right_ == nullptr) {
        int priority = 49;
        run_servo_task_right_ = new RtPeriodicMemberFun2<cameraManager>(capture_period_,
                                                                        "run_capture_right_task",
                                                                        1024 * 1024,
                                                                        priority,
                                                                        &cameraManager::capture_right,
                                                                        this);
        run_servo_task_right_->start();
    }
    return true;
}

bool cameraManager::start_middle_capture()
{
    if (!failed_count_middle_) {
        shutdown_middlecapture();
        init_cam();
        if(middle_id_==-1){
            return false;
        }
        restart_middle_cam();
        //std::this_thread::sleep_for(50ms);
        QThread::sleep(0.1);
    }
    failed_count_middle_ = 0;
    capture_middle_flag_ = true;
    if (run_servo_task_middle_ == nullptr) {
        int priority = 49;
        run_servo_task_middle_ = new RtPeriodicMemberFun2<cameraManager>(capture_period_,
                                                                         "run_capture_middle_task",
                                                                         1024 * 1024,
                                                                         priority,
                                                                         &cameraManager::capture_middle,
                                                                         this);
        run_servo_task_middle_->start();
    }
    return true;
}

void cameraManager::capture_left()
{
    if (!capture_left_flag_) return;
    std::lock_guard<std::mutex> lck{mtx_restart_left_};
    if (camera_left_ == nullptr) {
        failed_count_left_++;
        if (failed_count_left_ >= 10) {
            capture_left_flag_ = false;
            //RCLCPP_INFO(kLogger, "cvcamera restart left 1");
        }
        return;
    }

    tv_left_.tv_sec = 1;
    tv_left_.tv_usec = 0;
    if (camera_left_->isReadable(&tv_left_)) {
        memset(buff_left_, 0, 4000 * 3000);
        int rsize = camera_left_->read((char *)buff_left_, 4000 * 3000 - 1);
        if (rsize == -1) {
            ++failed_count_left_;
            if (failed_count_left_ >= 10) {
                capture_left_flag_ = false;
                //RCLCPP_INFO(kLogger, "cvcamera restart left 2");
            }
            return;
        }
        else if (rsize == 0) {
            ++failed_count_left_;
            if (failed_count_left_ >= 10) {
                capture_left_flag_ = false;
                //RCLCPP_INFO(kLogger, "cvcamera restart left 3");
            }
            return;
        }
        else {
            failed_count_left_ = 0;
            restart_failed_left_ = 0;
            // left_imgae_available_time_ = std::chrono::steady_clock::now();
            suc_capture_left_flag_ = true;
            if (camera_left_->getFormat() == V4L2_PIX_FMT_MJPEG) {
                // std::lock_guard<std::mutex> lck{mtx_camera_left_};
                vec_buff_left_->clear();
                vec_buff_left_->resize(rsize + 1);
                vec_buff_left_->assign(buff_left_, buff_left_ + rsize);
                try {
                    // 使用 cv::imdecode 解码 MJPEG 数据为 cv::Mat
                    left_image_0_ = cv::imdecode(*vec_buff_left_, cv::IMREAD_COLOR);
                } catch (const std::exception &e) {
                    qWarning() << "cv::imdecode 异常:" << e.what();
                    return;
                }
                //if (left_image_0_.empty()) return;
                // left_image_available_0_.pImage = &left_image_0_;
                // left_image_available_0_.time = left_imgae_available_time_;
                // UpdateLeftImage();
                if(left_image_0_.empty())return;
                QImage image(left_image_0_.data,
                             left_image_0_.cols,
                             left_image_0_.rows,
                             static_cast<int>(left_image_0_.step),
                             QImage::Format_BGR888);
                emit signalSendLeftImage(image);
            }
            else {
                ++failed_count_left_;
                if (failed_count_left_ >= 10) {
                    capture_left_flag_ = false;
                    //RCLCPP_INFO(kLogger, "cvcamera restart left 4");
                }
            }
        }
    }
    else {
        ++failed_count_left_;
        if (failed_count_left_ >= 10) {
            capture_left_flag_ = false;
            //RCLCPP_INFO(kLogger, "cvcamera restart left 5");
        }
    }
}

void cameraManager::capture_right()
{
    if (!capture_right_flag_) return;
    std::lock_guard<std::mutex> lck{mtx_restart_right_};
    if (camera_right_ == nullptr) {
        failed_count_right_++;
        if (failed_count_right_ >= 10) {
            capture_right_flag_ = false;
            //RCLCPP_INFO(kLogger, "cvcamera restart left 1");
        }
        return;
    }

    tv_right_.tv_sec = 1;
    tv_right_.tv_usec = 0;
    if (camera_right_->isReadable(&tv_right_)) {
        memset(buff_right_, 0, 4000 * 3000);
        int rsize = camera_right_->read((char *)buff_right_, 4000 * 3000 - 1);
        if (rsize == -1) {
            ++failed_count_right_;
            if (failed_count_right_ >= 10) {
                capture_right_flag_ = false;
                //RCLCPP_INFO(kLogger, "cvcamera restart left 2");
            }
            return;
        }
        else if (rsize == 0) {
            ++failed_count_right_;
            if (failed_count_right_ >= 10) {
                capture_right_flag_ = false;
                //RCLCPP_INFO(kLogger, "cvcamera restart left 3");
            }
            return;
        }
        else {
            failed_count_right_ = 0;
            restart_failed_right_ = 0;
            // left_imgae_available_time_ = std::chrono::steady_clock::now();
            suc_capture_right_flag_ = true;
            if (camera_right_->getFormat() == V4L2_PIX_FMT_MJPEG) {
                 std::lock_guard<std::mutex> lck{mtx_camera_left_};
                vec_buff_right_->clear();
                vec_buff_right_->resize(rsize + 1);
                vec_buff_right_->assign(buff_right_, buff_right_ + rsize);
                try {
                    // 使用 cv::imdecode 解码 MJPEG 数据为 cv::Mat
                    right_image_0_ = cv::imdecode(*vec_buff_right_, cv::IMREAD_COLOR);
                } catch (const std::exception &e) {
                    qWarning() << "cv::imdecode 异常:" << e.what();
                    return;
                }
                //if (left_image_0_.empty()) return;
                // left_image_available_0_.pImage = &left_image_0_;
                // left_image_available_0_.time = left_imgae_available_time_;
                // UpdateLeftImage();
                if(right_image_0_.empty())return;
                QImage image(right_image_0_.data,
                             right_image_0_.cols,
                             right_image_0_.rows,
                             static_cast<int>(right_image_0_.step),
                             QImage::Format_BGR888);
                emit signalSendRightImage(image);
            }
            else {
                ++failed_count_right_;
                if (failed_count_right_ >= 10) {
                    capture_right_flag_ = false;
                    //RCLCPP_INFO(kLogger, "cvcamera restart left 4");
                }
            }
        }
    }
    else {
        ++failed_count_right_;
        if (failed_count_right_ >= 10) {
            capture_right_flag_ = false;
            //RCLCPP_INFO(kLogger, "cvcamera restart left 5");
        }
    }
}

void cameraManager::capture_middle()
{

}

void cameraManager::restart_left_cam()
{
    std::lock_guard<std::mutex> lck{mtx_restart_left_};
    if (camera_left_) {
        delete camera_left_;
        camera_left_ = nullptr;
    }
    std::ostringstream oss_left;
    oss_left << "/dev/video" << left_id_;
    int verbose = 0;
    V4l2IoType ioTypeIn = IOTYPE_MMAP;
    width_ = 4000;
    height_ = 3000;
    int fps = 10;
    V4L2DeviceParameters param(
        oss_left.str().c_str(), format_, width_, height_, fps, ioTypeIn, verbose);
    camera_left_ = V4l2Capture::create(param);
}

void cameraManager::restart_right_cam()
{
    std::lock_guard<std::mutex> lck{mtx_restart_right_};
    if (camera_right_) {
        delete camera_right_;
        camera_right_ = nullptr;
    }
    std::ostringstream oss_right;
    oss_right << "/dev/video" << right_id_;
    int verbose = 0;
    V4l2IoType ioTypeIn = IOTYPE_MMAP;
    width_ = 4000;
    height_ = 3000;
    int fps = 10;
    // width_ = 1600;
    // height_ = 1200;
    // int fps = 30;
    V4L2DeviceParameters param(
        oss_right.str().c_str(), format_, width_, height_, fps, ioTypeIn, verbose);
    camera_right_ = V4l2Capture::create(param);
}

void cameraManager::restart_middle_cam()
{
    std::lock_guard<std::mutex> lck{mtx_restart_middle_};
    if (camera_middle_) {
        delete camera_middle_;
        camera_middle_ = nullptr;
    }
    std::ostringstream oss_middle;
    oss_middle << "/dev/video" << middle_id_;
    int verbose = 0;
    V4l2IoType ioTypeIn = IOTYPE_MMAP;
    width_ = 1920;
    height_ = 1080;
    int fps = 30;
    V4L2DeviceParameters param(
        oss_middle.str().c_str(), format_, width_, height_, fps, ioTypeIn, verbose);
    camera_middle_ = V4l2Capture::create(param);
}

void cameraManager::shutdown_leftcapture()
{
    capture_left_flag_ = false;
    std::lock_guard<std::mutex> lck{mtx_restart_left_};
    if (run_servo_task_left_) {
        run_servo_task_left_->stop();
        delete run_servo_task_left_;
        run_servo_task_left_ = nullptr;
    }

    if (camera_left_) {
        delete camera_left_;
        camera_left_ = nullptr;
    }
}

void cameraManager::shutdown_rightcapture()
{
    capture_right_flag_ = false;
    std::lock_guard<std::mutex> lck{mtx_restart_right_};
    if (run_servo_task_right_) {
        run_servo_task_right_->stop();
        delete run_servo_task_right_;
        run_servo_task_right_ = nullptr;
    }

    if (camera_right_) {
        delete camera_right_;
        camera_right_ = nullptr;
    }
}

void cameraManager::shutdown_middlecapture()
{
    capture_middle_flag_ = false;
    std::lock_guard<std::mutex> lck{mtx_restart_middle_};
    if (run_servo_task_middle_) {
        run_servo_task_middle_->stop();
        delete run_servo_task_middle_;
        run_servo_task_middle_ = nullptr;
    }

    if (camera_middle_) {
        delete camera_middle_;
        camera_middle_ = nullptr;
    }
}

QImage cameraManager::cvMatToQImage(cv::Mat &mat)
{

}


bool cameraManager::startCamera(const int l_r)
{
    if(l_r>3||l_r<1){
        return false;
    }
    switch (l_r) {
    case 1:
        cali_type_=CalibritionType::HandEyeCalibrition;
        return start_left_capture();
        break;
    case 2:
        cali_type_=CalibritionType::GlobalCalibrition;
        //start_left_capture()&&
        return  start_right_capture();
        break;
    case 3:
        cali_type_=CalibritionType::MiddleCalibrition;
        return start_middle_capture();
        break;
    default:
        cali_type_=CalibritionType::Null;
        return false;
        break;
    }
    return false;
}
