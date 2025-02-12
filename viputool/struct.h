#pragma once

#include <array>
#include <string>
#include <vector>

// #include <Eigen/Dense>
#include <opencv2/opencv.hpp>

// #include "NvInfer.h"

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

struct CImageTime
{
    cv::Mat *pImage;
    TimePoint time;

    CImageTime() { pImage = nullptr; }
};

enum class CameraTypeEnum
{
    local_camera,
    global_carmera,
    follow_camera,
};

struct BinocularRecord
{
    cv::Mat left_image;
    cv::Mat right_image;
    TimePoint timepoint;
    BinocularRecord();
    void clear();
};

// struct Binding
// {
//     size_t size = 1;
//     size_t dsize = 1;
//     nvinfer1::Dims dims;
//     std::string name;
// };

struct Object
{
    cv::Rect_<float> rect;
    int label = 0;
    float prob = 0.0;
    float cx, cy, w, h, angle, area;
    std::vector<cv::Point2f> vertices;
};

struct PreParam
{
    float ratio = 1.0f;
    float dw = 0.0f;
    float dh = 0.0f;
    float height = 0;
    float width = 0;
};

struct FollicleUnit
{
    cv::Rect rectangle_;
    std::vector<std::array<int, 4>> hair_list_;
    float mean_thickness_;
    std::vector<cv::Point> contours;
};

// struct FollicleUnitInfo
// {
//     bool is_valid = true;
//     cv::Rect left_rect;
//     cv::Rect right_rect;

//     std::vector<std::array<int, 4>> left_hair_list;
//     std::vector<std::array<int, 4>> right_hair_list;
//     std::vector<std::array<int, 4>> left_hairs_matched;
//     std::vector<std::array<int, 4>> right_hairs_matched;
//     std::vector<bool> hairs_matched_check;
//     std::vector<std::array<int, 4>> left_hairs_matched_afer_check;
//     std::vector<std::array<int, 4>> right_hairs_matched_afer_check;

//     std::vector<cv::Point2f> left_top_2d;
//     std::vector<cv::Point2f> left_root_2d;
//     std::vector<cv::Point2f> right_top_2d;
//     std::vector<cv::Point2f> right_root_2d;

//     Eigen::MatrixX3d top_3D;
//     Eigen::MatrixX3d root_3D;

//     Eigen::Vector3d dirVect;
//     Eigen::Vector3d startpt;
//     Eigen::Vector3d endpt;

//     cv::Point left_top_reproject;
//     cv::Point left_root_reproject;
//     cv::Point right_top_reproject;
//     cv::Point right_root_reproject;
// };

struct HairMatchFilterOptions{
    float min_angle_x;
    float min_overlap_percent;
    float max_root_differ;
    float max_angle_differ;
};
