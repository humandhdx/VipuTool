#include "cameramanager.h"
#include <QThread>
#include <QEventLoop>
#include <QDesktopServices>


cameraManager::cameraManager(QObject *parent): QObject{parent},
    left_id_(-1),
    right_id_(-1),
    middle_id_(-1),
    capture_left_flag_(false),
    capture_middle_flag_(false),
    capture_right_flag_(false),
    failed_count_left_(0),
    failed_count_middle_(0),
    failed_count_right_(0)
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
    matlab_process=new QProcess(this);
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
    if(matlab_process){
        if (matlab_process->state() != QProcess::NotRunning) {
            matlab_process->kill();  // 强制终止进程
        }
        delete matlab_process;
        matlab_process=nullptr;
    }
}

void cameraManager::stopCamera()
{
    is_left_fouc=false;
    is_right_fouc=false;
    setCur_foc(0);
    setMax_foc(0);
    setMin_foc(0);
    shutdown_leftcapture();
    shutdown_rightcapture();
    shutdown_middlecapture();
}

bool cameraManager::start_camera_capture(const QString &path, int type, int count)
{
    QEventLoop spinner;
    bool executionResult=false;
    std::string pathStr =path.toStdString();
    std::future<bool> fut=std::async(std::launch::async ,[&spinner,&executionResult,&pathStr,&type,&count,this](){
        executionResult=this->captureImage(pathStr,type,count);
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    return executionResult;
}

bool cameraManager::deleteFisterCaptureImage(QString path)
{
    QDir dir(path);
    QStringList filters;
    QFileInfoList fileList;
    // 确保目录存在
    if (!dir.exists()) {
        qWarning() << "全局相机图片目录不存在：" << path;
        return false;
    }
    // 设置文件筛选器，删除 ".jpg" 文件
    filters << "*.jpg" << "*.jpeg"; // 支持 jpg 和 jpeg
    dir.setNameFilters(filters);
    // 获取所有匹配的文件并按照修改时间排序
    fileList = dir.entryInfoList(QDir::Files, QDir::Time | QDir::Reversed);
    if (!fileList.isEmpty()) {
        // 删除最新的文件（按修改日期排序的第一个文件）
        QFileInfo latestFile = fileList.last();
        QString latestFilePath = latestFile.absoluteFilePath();

        if (QFile::remove(latestFilePath)) {
            qDebug() << "已删除最新拍照的图片：" << latestFilePath;
        } else {
            qWarning() << "删除失败：" << latestFilePath;
            return false;
        }
    }
    else {
        qDebug() << "当前目录下没有符合条件的图片文件：" << path;
        return false;
    }
    return true;
    qDebug() << "删除最后一次采集图片成功";
}

bool cameraManager::captureImage(const std::string &path, int type, int count)
{
    bool left_saved=false;
    bool right_saved=false;
    switch (type) {
    case 0:
        if (left_image_0_.empty()) {
            qDebug() << "保存图像失败";
            return false;
        }
        return saveImage(path, left_image_0_, "Global_Left_Image", count);
    case 1:
        if (right_image_0_.empty()) {
            qDebug() << "保存图像失败";
            return false;
        }
        return saveImage(path, right_image_0_, "Global_Right_Image", count);
    case 2:
        if (left_image_0_.empty() || right_image_0_.empty()) {
            qDebug() << "保存图像失败";
            return false;
        }
        left_saved = saveImage(path, left_image_0_, "Global_Left_Image", count);
        right_saved = saveImage(path, right_image_0_, "Global_Right_Image", count);
        return left_saved && right_saved;

    case 3:
        if (middle_image_0_.empty()) {
            qDebug() << "保存图像失败";
            return false;
        }
        return saveImage(path, middle_image_0_, "Middle_Image", count);
    case 4:
        if (middle_image_0_.empty()) {
            qDebug() << "保存图像失败";
            return false;
        }
        return saveImage(path, middle_image_0_, "Middle_Image", count);
    case 5:
        if (middle_image_0_.empty()) {
            qDebug() << "保存图像失败";
            return false;
        }
        return saveImage(path, middle_image_0_, "Middle_Image", count);

    default:
        qDebug() << "未知的校准类型";
        return false;
    }
}


bool cameraManager::clearCaptureCount(QString path)
{
    QDir dir(path);
    QStringList filters;
    QFileInfoList fileList;
    // 确保目录存在
    if (!dir.exists()) {
        //qWarning() << "清空目录不存在：" << path;
        if (!dir.mkpath(".")) {
            qWarning() << "无法创建目录：" << path;
            return false;
        }
        return false;
    }
    // 设置文件筛选器，删除所有 ".jpg" 文件
    filters << "*.jpg" << "*.jpeg"; // 支持 jpg 和 jpeg
    dir.setNameFilters(filters);
    // 获取所有匹配的文件
    fileList = dir.entryInfoList(QDir::Files);

    // 删除文件
    for (const QFileInfo &fileInfo : fileList) {
        QString filePath = fileInfo.absoluteFilePath();
        if (QFile::remove(filePath)) {
            //qDebug() << "已删除：" << filePath;
        } else {
            //qWarning() << "删除失败：" << filePath;
        }
    }
    qDebug() << "重置采集图片数量成功";
    return true;
}

void cameraManager::open_path(QString path)
{
    if (path.isEmpty()) {
        qWarning() << "传入的路径为空";
        return;
    }

    // 将路径转换为 QUrl
    QUrl url = QUrl::fromLocalFile(path);

    // 使用桌面服务打开路径
    if (QDesktopServices::openUrl(url)) {
        qDebug() << "已打开路径：" << path;
    } else {
        qWarning() << "打开路径失败：" << path;
    }
}

void cameraManager::openMalLab()
{
    if (matlab_process->state() == QProcess::Running) {
        qWarning()<<"matlab 运行中，请勿重复启动";
        return;
    }
    QString matlabPath = "/home/vipu/testmatlab/bin/matlab";  // 如果 PATH 中有 matlab，否则用完整路径
    QStringList arguments;
    arguments << "-desktop";
    matlab_process->start(matlabPath, arguments);
}

void cameraManager::startFouc(int l_r)
{
    if(l_r==0){
        is_left_fouc=true;
    }
    else{
        is_right_fouc=true;
    }
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

        int dev_num_cur =atoi(name.substr(10).c_str());  //10 = len for "/dev/video", to get the num behind

        if (strncmp((const char *)cap.card, "FueCamLeft", 10) == 0) {
            left_id_ = dev_num_cur;
        }
        else if (strncmp((const char *)cap.card, "FueCamRight", 11) == 0) {
            right_id_ = dev_num_cur;
        }
        else if (strncmp((const char *)cap.card, "USB 2.0 Camera:", 15) == 0) {
            middle_id_=dev_num_cur;
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
            qWarning()<<"不存在相机设备请检查相机设备";
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
            qWarning()<<"不存在相机设备请检查相机设备";
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
            qWarning()<<"不存在相机设备请检查相机设备";
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
                qWarning() << "设备异常";
                emit signalDeviceErro();
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
                if(is_left_fouc)EOG(left_image_0_);
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
                qWarning() << "设备异常";
                emit signalDeviceErro();
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
                if(is_right_fouc)EOG(right_image_0_);
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
        qWarning() << "isReadable erro";
        ++failed_count_right_;
        if (failed_count_right_ >= 10) {
            capture_right_flag_ = false;
            //RCLCPP_INFO(kLogger, "cvcamera restart left 5");
        }
    }
}

void cameraManager::capture_middle()
{
    if (!capture_middle_flag_) return;
    std::lock_guard<std::mutex> lck{mtx_restart_middle_};
    if (camera_middle_ == nullptr) {
        failed_count_middle_++;
        if (failed_count_middle_ >= 10) {
            capture_middle_flag_ = false;
            //RCLCPP_INFO(kLogger, "cvcamera restart left 1");
        }
        return;
    }

    tv_middle_.tv_sec = 1;
    tv_middle_.tv_usec = 0;
    if (camera_middle_->isReadable(&tv_middle_)) {
        memset(buff_middle_, 0, 1920 * 1080);
        int rsize = camera_middle_->read((char *)buff_middle_, 1920 * 1080 - 1);
        if (rsize == -1) {
            ++failed_count_middle_;
            if (failed_count_middle_ >= 10) {
                qWarning() << "设备异常";
                emit signalDeviceErro();
                capture_middle_flag_ = false;
                //RCLCPP_INFO(kLogger, "cvcamera restart left 2");
            }
            return;
        }
        else if (rsize == 0) {
            ++failed_count_middle_;
            if (failed_count_middle_ >= 10) {
                capture_middle_flag_ = false;
                //RCLCPP_INFO(kLogger, "cvcamera restart left 3");
            }
            return;
        }
        else {
            failed_count_middle_ = 0;
            restart_failed_middle_ = 0;
            suc_capture_middle_flag_ = true;
            if (camera_middle_->getFormat() == V4L2_PIX_FMT_MJPEG) {
                std::lock_guard<std::mutex> lck{mtx_camera_left_};
                vec_buff_middle_->clear();
                vec_buff_middle_->resize(rsize + 1);
                vec_buff_middle_->assign(buff_middle_, buff_middle_ + rsize);
                try {
                    // 使用 cv::imdecode 解码 MJPEG 数据为 cv::Mat
                    middle_image_0_ = cv::imdecode(*vec_buff_middle_, cv::IMREAD_COLOR);
                } catch (const std::exception &e) {
                    qWarning() << "cv::imdecode 异常:" << e.what();
                    return;
                }
                if(middle_image_0_.empty())return;
                QImage image(middle_image_0_.data,
                             middle_image_0_.cols,
                             middle_image_0_.rows,
                             static_cast<int>(middle_image_0_.step),
                             QImage::Format_BGR888);
                emit signalSendMiddleImage(image);
            }
            else {
                ++failed_count_middle_;
                if (failed_count_middle_ >= 10) {
                    capture_middle_flag_ = false;
                    //RCLCPP_INFO(kLogger, "cvcamera restart left 4");
                }
            }
        }
    }
    else {
        qWarning() << "isReadable erro";
        ++failed_count_middle_;
        if (failed_count_middle_ >= 10) {
            capture_middle_flag_ = false;
            //RCLCPP_INFO(kLogger, "cvcamera restart left 5");
        }
    }

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

void cameraManager::EOG(const cv::Mat &mat)
{
    cv::Mat gray;
    cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
    cv::Mat kernely = (cv::Mat_<char>(2, 1) << -1, 1);
    cv::Mat kernelx = (cv::Mat_<char>(1, 2) << -1, 1);
    cv::Mat engx, engy;
    filter2D(gray, engx, CV_32F, kernelx);
    filter2D(gray, engy, CV_32F, kernely);

    cv::Mat result = engx.mul(engx) + engy.mul(engy);
    double focus_metric = cv::mean(result)[0];
    // 初始化 min_foc 和 max_foc（如果是第一次调用）
    if (min_foc() == 0 && max_foc() == 0) {
        setMin_foc(focus_metric);
        setMax_foc(focus_metric);
        qDebug() << "First run: Initialized min_foc and max_foc to: " << focus_metric;
        return;
    }

    // 更新最小和最大清晰度值
    if (focus_metric < min_foc()) {
        setMin_foc(focus_metric);
    }

    if (focus_metric > max_foc()) {
        setMax_foc(focus_metric);
    }
    setCur_foc(focus_metric);

    // 输出当前的清晰度信息
    // qDebug() << "当前图像清晰度为:" << focus_metric;
    // qDebug() << "最小清晰度为:" << min_foc;
    // qDebug() << "最大清晰度为:" << max_foc;
}

bool cameraManager::saveImage(const std::string &path, const cv::Mat &image, const std::string &prefix, int count)
{
    std::ostringstream oss;
    oss << path << "/" << prefix << "_"
        << std::setw(3) << std::setfill('0') << count
        << ".jpg";
    std::string save_path = oss.str();
    bool result = cv::imwrite(save_path, image);
    if (result) {
        qDebug() << "图像已成功保存到: " << QString::fromStdString(save_path);
    } else {
        qDebug() << "保存图像失败";
    }
    return result;
}


bool cameraManager::startCamera(const int l_r)
{
    if(l_r>3||l_r<0){
        return false;
    }
    switch (l_r) {
    case 0:
        return start_left_capture();
    case 1:
        return start_right_capture();
    case 2:
        return  start_left_capture()&&start_right_capture();
    case 3:
        return start_middle_capture();
    default:
        return false;
        break;
    }
    return false;
}

double cameraManager::max_foc() const
{
    return m_max_foc;
}

void cameraManager::setMax_foc(double newMax_foc)
{
    if (qFuzzyCompare(m_max_foc, newMax_foc))
        return;
    m_max_foc = newMax_foc;
    emit max_focChanged();
}

double cameraManager::min_foc() const
{
    return m_min_foc;
}

void cameraManager::setMin_foc(double newMin_foc)
{
    if (qFuzzyCompare(m_min_foc, newMin_foc))
        return;
    m_min_foc = newMin_foc;
    emit min_focChanged();
}

double cameraManager::cur_foc() const
{
    return m_cur_foc;
}

void cameraManager::setCur_foc(double newCur_foc)
{
    if (qFuzzyCompare(m_cur_foc, newCur_foc))
        return;
    m_cur_foc = newCur_foc;
    emit cur_focChanged();
}
