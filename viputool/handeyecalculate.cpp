#include "handeyecalculate.h"
#include <QtConcurrent>
handeyecalculate::handeyecalculate(QObject *parent):QObject{parent}
{

}

handeyecalculate::~handeyecalculate()
{

}

void handeyecalculate::startCalibration()
{
    // 使用 QPointer 防止对象在后台线程中被意外销毁
    QPointer<handeyecalculate> self(this);
    QtConcurrent::run([self]() {
        if (self)
            self->runCalibration();
    });
}

bool handeyecalculate::runCalibration()
{
    //YAML::Node cfg = YAML::LoadFile(configPath);
    int min_hand_eye_num = 30;//cfg["min_hand_eye_num"].as<int>();
    //std::string pattern_folder = cfg["image_folder"].as<std::string>();
    //std::string camera_file = cfg["camera_model_file"].as<std::string>();
    //std::string arm_pose_file = cfg["arm_pose_file"].as<std::string>();
    QString pattern_folder="/home/vipu/VipuTool/viputool/VipuTool/viputool/HandEyeImages";
    QString camera_file="/home/vipu/VipuTool/viputool/VipuTool/viputool/cali_mat.yaml";
    QString arm_pose_file="/home/vipu/VipuTool/viputool/VipuTool/viputool/arm_pose.txt";

    MonoCameraMat camera_model(camera_file.toStdString());

    std::vector<cv::String> image_files;
    cv::glob(pattern_folder.toStdString(), image_files);

    float cb_length = 2.0;
    cv::Size pattern_size = cv::Size(11 ,8);

    std::vector<cv::Point3f> world_points;
    for (int i = 0; i < pattern_size.width * pattern_size.height; i++) {
        int row = i / pattern_size.width;
        int col = i % pattern_size.width;
        cv::Point3f pt(cb_length * row, cb_length * col, 0); // swap x and y cororidinate
        world_points.push_back(pt);
    }

    std::vector<std::vector<double>> arm_pose = readArmPose(arm_pose_file.toStdString());
    qDebug()<< "获取机械臂姿态成功 pattern size: " << arm_pose.size();
    //std::cout << "pattern size: " << arm_pose.size() << std::endl;
    if(arm_pose.size() != image_files.size()){
        //std::cout << "image size != arm pose size! Hand eye fail!" << std::endl;
        qDebug() << "image size != arm pose size! Hand eye fail!";
        return 0;
    }

    std::vector<CaliFrameInfo> frame_list;

    for (int i = 0 ; i < image_files.size() ; i++){
        auto file = image_files[i];
        //std::cout << file << std::endl;
        CaliFrameInfo frame_info;
        cv::Mat image = cv::imread(file);

        std::vector<cv::Point2f> pt;
        bool state = detectCB(image, pt, pattern_size);

        if(state){
            qDebug().noquote()<< "获取当前图片数据成功:\n"+QString::fromStdString(file);
            cv::Vec3d rvec;
            cv::Vec3d tvec;
            cv::solvePnP(world_points, pt, camera_model.M, camera_model.dist, rvec, tvec);
            frame_info.valid = true;
            frame_info.cam_translation = cv::Point3d(tvec[0], tvec[1], tvec[2]);
            frame_info.cam_rotation = cv::Point3d(rvec[0], rvec[1], rvec[2]);

            frame_info.arm_translation = cv::Point3d(arm_pose[i][0], arm_pose[i][1], arm_pose[i][2]);
            frame_info.arm_rotation = cv::Point3d(arm_pose[i][3], arm_pose[i][4], arm_pose[i][5]);

            // cv::drawFrameAxes(image, camera_model.M, camera_model.dist, rvec, tvec, 5);
            // cv::resize(image, image, cv::Size(1000,750));
            // cv::imshow("test", image);
            // cv::waitKey();
        }
        else{
            qDebug().noquote()<< "获取当前图片数据失败\n"+QString::fromStdString(file);
            frame_info.valid = false;
        }

        frame_list.push_back(frame_info);
    }

    std::vector<double> pose;
    bool rt = handEyeCalibration(frame_list, pose, min_hand_eye_num);

    if(rt){
        //qDebug() << "手眼标定成功";
        //std::cout << "Hand eye success!" << std::endl;
        Eigen::Quaterniond q(pose[6], pose[3], pose[4], pose[5]);
        Eigen::Isometry3d T = Eigen::Isometry3d::Identity();
        T.rotate(q);
        T.pretranslate(Eigen::Vector3d(pose[0], pose[1], pose[2]));
        //std::cout << std::setprecision(10);
        //std::cout << "hand eye result is: " << std::endl;
        //std::cout << T.matrix() << std::endl;
        std::ostringstream oss;
        oss << T.matrix().format(Eigen::IOFormat(10));
        qDebug() << "手眼标定结果: ";
        qDebug().noquote() << QString::fromStdString(oss.str());
    }
    else{
        qDebug() << "没有足够的有效图片，手眼标定失败";
    }
    QMetaObject::invokeMethod(this, [this]() {
            qDebug()<<"手眼标定结束";
            emit calculateSucess();
        }, Qt::QueuedConnection);
    return true;
}

std::vector<std::vector<double> > handeyecalculate::readArmPose(const std::string &path)
{
    std::vector<std::vector<double>> pose;
    std::ifstream inputFile(path);

    if(!inputFile.is_open()){
        return pose;
    }

    double tx, ty, tz, rx, ry, rz;

    while (inputFile >> tx >> ty >> tz >> rx >> ry >> rz) {
        rx *= 180.0 / CV_PI;
        ry *= 180.0 / CV_PI;
        rz *= 180.0 / CV_PI;
        std::vector<double> tmp{tx, ty, tz, rx, ry, rz};
        pose.push_back(tmp);
    }

    // Close the file
    inputFile.close();
    return pose;
}

bool handeyecalculate::handEyeCalibration(const std::vector<CaliFrameInfo> &frame_list, std::vector<double> &pose, int min_num)
{
    int num = 0;
    for (const auto &frame : frame_list) {
        if (frame.valid) {
            num++;
        }
    }
    //std::cout << "valid pattern size = " << num << std::endl;
    qDebug()<<"采集图片数量: "<<num;
    qDebug()<<"最小采集图片数量: "<<min_num;
    if(num < min_num){
        qDebug()<<"手眼标定失败：采集图片数量少于最小采集数量";
        return false;
    }
    qDebug()<<"有效图片数量: "<<num;
    //std::cout << "valid frames: " << num << std::endl;

    cv::Mat_<double> cam_pose(num, 6);
    cv::Mat_<double> arm_pose(num, 6);

    int idx = 0;
    for (size_t i = 0; i < frame_list.size(); i++) {
        if (!frame_list[i].valid) {
            continue;
        }

        cam_pose.at<double>(idx, 0) = frame_list[i].cam_translation.x;
        cam_pose.at<double>(idx, 1) = frame_list[i].cam_translation.y;
        cam_pose.at<double>(idx, 2) = frame_list[i].cam_translation.z;
        cam_pose.at<double>(idx, 3) = frame_list[i].cam_rotation.x;
        cam_pose.at<double>(idx, 4) = frame_list[i].cam_rotation.y;
        cam_pose.at<double>(idx, 5) = frame_list[i].cam_rotation.z;
        arm_pose.at<double>(idx, 0) = frame_list[i].arm_translation.x;
        arm_pose.at<double>(idx, 1) = frame_list[i].arm_translation.y;
        arm_pose.at<double>(idx, 2) = frame_list[i].arm_translation.z;
        arm_pose.at<double>(idx, 3) = frame_list[i].arm_rotation.x;
        arm_pose.at<double>(idx, 4) = frame_list[i].arm_rotation.y;
        arm_pose.at<double>(idx, 5) = frame_list[i].arm_rotation.z;

        idx++;
    }
    pose = handEye(cam_pose, arm_pose);

    return true;
}
