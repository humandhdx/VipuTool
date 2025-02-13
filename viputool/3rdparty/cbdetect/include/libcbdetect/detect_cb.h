#pragma once
#ifndef LIBCBDETECT_DETECT_CB_H
#define LIBCBDETECT_DETECT_CB_H

#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

#include "libcbdetect/boards_from_corners.h"
#include "libcbdetect/config.h"
#include "libcbdetect/find_corners.h"
#include "libcbdetect/plot_boards.h"
#include "libcbdetect/plot_corners.h"


bool detectCB(const cv::Mat &img, std::vector<cv::Point2f> &points, const cv::Size &pattern_size);

#endif