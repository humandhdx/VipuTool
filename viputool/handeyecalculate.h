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
    Q_PROPERTY(int arm_pose_count READ arm_pose_count  NOTIFY arm_pose_countChanged FINAL)
public:
    handeyecalculate(QObject *parent = nullptr);
    ~handeyecalculate();

    int arm_pose_count() const;

public slots:
     bool startCalibration(QString patternfolder,QString armposefile,QString camerafile);
     bool recordArmPose(QVariantList armpose);
     QString saveArmPose();
     bool resetCalibration();

private:
    std::vector<std::vector<double>> readArmPose(const std::string &path);
    bool handEyeCalibration(const std::vector<CaliFrameInfo> &frame_list, std::vector<double> &pose, int min_num);
    bool runCalibration(QString patternfolder,QString armpose_file,QString camerafile);
    std::vector<std::vector<double> > arm_pose;

signals:
    void calculateError(const QString &message);
    void arm_pose_countChanged();
};

#endif // HANDEYECALCULATE_H
