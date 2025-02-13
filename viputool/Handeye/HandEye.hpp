#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

cv::Mat R_T2RT(cv::Mat &R, cv::Mat &T);
void RT2R_T(cv::Mat &RT, cv::Mat &R, cv::Mat &T);
bool isRotationMatrix(const cv::Mat &R);
bool isRotationMatrix(const cv::Mat &R);
cv::Mat eulerAngleToRotatedMatrix(const cv::Mat &eulerAngle, const std::string &seq);
cv::Mat quaternionToRotatedMatrix(const cv::Vec4d &q);
cv::Mat attitudeVectorToMatrix(cv::Mat m, bool useQuaternion, const std::string &seq);
std::vector<double> handEye(const cv::Mat_<double> &CalPose, const cv::Mat_<double> &ToolPose);
void getQuaternion(const cv::Mat &R, double Q[]);

// Y-Z-X order
cv::Mat rot2euler(const cv::Mat & rotationMatrix);
// xyzrpy in Y-Z-X order or [t,r]
std::vector<double> calcHandEyeError(const std::vector<cv::Mat> &vecHg,  const std::vector<cv::Mat> &vecHc, const cv::Mat &Hcg);
