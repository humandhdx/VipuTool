#pragma once

#include <string>
#include <vector>

#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <yaml-cpp/yaml.h>

class StereoCalibrationMat {
    public:
        StereoCalibrationMat();

        cv::Mat M1, M2, dist1, dist2, R, T;
        cv::Size dim;
        cv::Mat R1, R2, P1, P2, Q, left_mapx, left_mapy, right_mapx, right_mapy;

        StereoCalibrationMat(const std::string yamlFile);
        void rectify();
        cv::Mat remap_left(cv::Mat img_src);
        cv::Mat remap_right(cv::Mat img_src);
};


class MonoCameraMat{
    public:
        MonoCameraMat(const std::string yamlFile);
        cv::Mat M, dist;
};