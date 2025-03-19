#include "handeyecalculate.h"
#include "json.hpp"
#include <QtConcurrent>
using json = nlohmann::json;
handeyecalculate::handeyecalculate(QObject *parent):QObject{parent}
{
    arm_pose.clear();
}

handeyecalculate::~handeyecalculate()
{

}

bool handeyecalculate::startCalibration(QString patternfolder,QString armposefile,QString camerafile)
{
    QEventLoop spinner;
    bool executionResult = false;
    std::future<bool> fut = std::async(std::launch::async, [&spinner, &executionResult,&patternfolder,&armposefile,&camerafile,this](){
        executionResult = this->runCalibration(patternfolder,armposefile,camerafile);
        spinner.exit();
        return executionResult;
    });
    spinner.exec();
    return executionResult;
}

bool handeyecalculate::recordArmPose(QVariantList armpose)
{
    // 预先分配足够的空间，避免多次扩容
    std::vector<double> vecArmpose;
    vecArmpose.reserve(armpose.size());
    for(auto a :armpose){
        vecArmpose.push_back(a.toDouble());
    }
    arm_pose.push_back(vecArmpose);
    qDebug()<<"记录当前机械臂姿态成功："<<arm_pose.size();
    emit arm_pose_countChanged();
    return true;
}

QString handeyecalculate::saveArmPose()
{
    QString filename=QDir::currentPath()+"/arm_pose.txt";
    std::ofstream ofs(filename.toStdString(),std::ios::out | std::ios::trunc);
    if (!ofs) {
        qDebug()<<"无法打开文件进行写入机械臂姿态";
        return QString();
    }
    // 设置固定格式和精度（8位小数，根据示例数据）
    ofs << std::fixed << std::setprecision(8);
    // 遍历外层 vector
    for (const auto &vec : arm_pose) {
        // 如果每个子集必须有6个数据，可以进行检查
        if (vec.size() != 6) {
            std::cerr << "警告：子向量的元素数量不等于6，跳过该行。" << std::endl;
            continue;
        }
        // 遍历子向量，将每个数据后跟一个空格（最后一个数据后不加空格）
        for (size_t i = 0; i < vec.size(); ++i) {
            ofs << vec[i];
            if (i < vec.size() - 1)
                ofs << " ";
        }
        ofs << "\n"; // 换行
    }
    ofs.close();
    qDebug()<<"保存机械臂姿态成功："<<filename;
    return filename;
}

bool handeyecalculate::resetCalibration()
{
    arm_pose.clear();
    currentGlobalPoseMatrix.clear();
    emit arm_pose_countChanged();
    qDebug()<<"重置机械臂姿态数量成功";
    return true;
}

bool handeyecalculate::saveCalibrationToFile(QString savepath,int type)
{
    if(type==0&&currentGlobalPoseMatrix.empty()){
        qDebug()<<"请先进行全局相机手眼标定后再保存";
        return false;
    }
    if(type==1&&currentCenterPoseMatrix.empty()){
        qDebug()<<"请先进行随动相机手眼标定后再保存";
        return false;
    }
    json jsonTemplate = {
        {"version", "0.0.0.0"},
        {"update_date", ""},
        {"eye_in_hand_cali", {
                                 {"local_camera_pose_matrix_mm", json::array()},
                                 {"global_camera_pose_matrix_mm", json::array()},
                                 {"center_camera_pose_matrix_mm", json::array()},
                                 {"punch_direction_vector", json::array()},
                                 {"punch_tip_vector_mm", json::array()}
                             }}
    };
    // 创建一个JSON对象，并使用模板填充数据
    json data = jsonTemplate;

    // 获取当前时间并填入JSON对象
    data["update_date"] = getCurrentTimeStr();

    // 填充JSON数据
    if(type==0){
       data["eye_in_hand_cali"]["global_camera_pose_matrix_mm"] = currentGlobalPoseMatrix;
    }
    else{
       data["eye_in_hand_cali"]["center_camera_pose_matrix_mm"] = currentCenterPoseMatrix;
    }

    // 将JSON对象保存到指定位置的文件中
    std::string filePath = savepath.toStdString()+(type == 0 ? "/global_hand_cali.json" : "/center_hand_cali.json"); // 更改这个路径为你的目标路径
    std::ofstream file(filePath);
    if (file.is_open()) {
        file << std::setw(4) << data << std::endl; // 使用std::setw(4)来设置缩进
        file.close();
        qDebug() << "全局相机手眼标定结果保存成功: "<<QString::fromStdString(filePath);
    } else {
        qWarning() << "全局相机手眼标定结果保存失败！";
    }
    return true;
}

bool handeyecalculate::runCalibration( QString patternfolder,QString armposefile,QString camerafile)
{
    int min_hand_eye_num = 5;
    // QString pattern_folder="/home/vipu/VipuTool/viputool/VipuTool/viputool/HandEyeImages";
    // QString arm_pose_file="/home/vipu/VipuTool/viputool/VipuTool/viputool/arm_pose.txt";
    // QString camera_file="/home/vipu/VipuTool/viputool/VipuTool/viputool/cali_mat.yaml";
    QString pattern_folder=armposefile;
    QString arm_pose_file=armposefile;
    QString camera_file=armposefile;
    if (!QFile::exists(arm_pose_file)) {
        qWarning()<< "图片保存路径不存在:" << pattern_folder;
        return false;
    }
    if (!QFile::exists(arm_pose_file)) {
        qWarning()<< "机械臂姿态文件不存在:" << pattern_folder;
        return false;
    }
    if (!QFile::exists(camera_file)) {
        qWarning()<< "全局相机参数文件不存在:" << camera_file;
        return false;
    }
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
    qDebug()<< "获取机械臂姿态数量: "<< arm_pose.size();
    //std::cout << "pattern size: " << arm_pose.size() << std::endl;
    if(arm_pose.size() != image_files.size()){
        qDebug() << "图片数量和机械臂姿态数量不匹配手眼标定失败!";
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
        currentGlobalPoseMatrix.clear();
        currentGlobalPoseMatrix={
            T(0,0),T(0,1),T(0,2),T(0,3),
            T(1,0),T(1,1),T(1,2),T(1,3),
            T(2,0),T(2,1),T(2,2),T(2,3),
            0.0,0.0,0.0,1.0
        };
        std::ostringstream oss;
        oss << T.matrix().format(Eigen::IOFormat(10));
        qDebug() << "手眼标定结果: ";
        qDebug().noquote() << QString::fromStdString(oss.str());
        qDebug().noquote()<< "translation error: " << pose[7] ;
    }
    else{
        qDebug() << "没有足够的有效图片，手眼标定失败";
        return false;
    }
    return true;
}

std::string handeyecalculate::getCurrentTimeStr()
{
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    auto tt = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&tt);

    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%Y-%m-%d-%H-%M-%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
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

int handeyecalculate::arm_pose_count() const
{
    return arm_pose.size();
}


