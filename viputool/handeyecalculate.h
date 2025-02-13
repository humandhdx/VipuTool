#ifndef HANDEYECALCULATE_H
#define HANDEYECALCULATE_H
#include <QObject>
#include <string>
#include <vector>
#include <fstream>

#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/eigen.hpp>

#include "Handeye/HandEye.hpp"
#include "Handeye/stereo_calibration_mat.hpp"
#include "libcbdetect/detect_cb.h"
struct CaliFrameInfo
{
    bool valid;
    cv::Point3d arm_translation;
    cv::Point3d arm_rotation;
    cv::Point3d cam_translation;
    cv::Point3d cam_rotation;
};
class handeyecalculate:public QObject
{
    Q_OBJECT
public:
    handeyecalculate(QObject *parent = nullptr);

public slots:
    bool runCalibration();

private:
    std::vector<std::vector<double>> readArmPose(const std::string &path);
    bool handEyeCalibration(const std::vector<CaliFrameInfo> &frame_list, std::vector<double> &pose, int min_num);
};

#endif // HANDEYECALCULATE_H
