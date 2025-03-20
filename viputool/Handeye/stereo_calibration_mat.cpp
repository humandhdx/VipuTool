#include "stereo_calibration_mat.hpp"

StereoCalibrationMat::StereoCalibrationMat(){
    
};

StereoCalibrationMat::StereoCalibrationMat(const std::string yamlFile) {
    YAML::Node cfg =  YAML::LoadFile(yamlFile);

    std::vector<double> m1_vec = cfg["M1"].as<std::vector<double>>();
    std::vector<double> d1_vec = cfg["d1"].as<std::vector<double>>();
    std::vector<double> m2_vec = cfg["M2"].as<std::vector<double>>();
    std::vector<double> d2_vec = cfg["d2"].as<std::vector<double>>();
    std::vector<double> dim_vec = cfg["dim"].as<std::vector<double>>();
    std::vector<double> R_vec = cfg["R"].as<std::vector<double>>();
    std::vector<double> T_vec = cfg["T"].as<std::vector<double>>();
    
    M1 = cv::Mat(3,3,CV_64F);
    for (int row=0; row<3; row++) {
        for (int col=0; col<3; col++) {
            M1.at<double>(row, col) = m1_vec[col+row*3];
        }
    }
    M2 = cv::Mat(3,3,CV_64F);
    for (int row=0; row<3; row++) {
        for (int col=0; col<3; col++) {
            M2.at<double>(row, col) = m2_vec[col+row*3];
        }
    }
    R = cv::Mat(3,3,CV_64F);
    for (int row=0; row<3; row++) {
        for (int col=0; col<3; col++) {
            R.at<double>(row, col) = R_vec[col+row*3];
        }
    }
    dist1 = cv::Mat(5,1,CV_64F);
    for (int col=0; col<5; col++) {
        dist1.at<double>(0, col) = d1_vec[col];
    }
    dist2 = cv::Mat(5,1,CV_64F);
    for (int col=0; col<5; col++) {
        dist2.at<double>(0, col) = d2_vec[col];
    }    
    T = cv::Mat(3,1,CV_64F);
    for (int col=0; col<3; col++) {
        T.at<double>(0, col) = T_vec[col];
    }    
    dim = cv::Size((int)dim_vec[0], (int)dim_vec[1]);
    rectify();
   
}

void StereoCalibrationMat::rectify() {
    cv::stereoRectify(M1, dist1, M2, dist2, dim, R, T, R1, R2, P1, P2, Q, 0, -1);
    cv::initUndistortRectifyMap(M1, dist1, R1, P1, dim, CV_32FC1, left_mapx, left_mapy);
    cv::initUndistortRectifyMap(M2, dist2, R2, P2, dim, CV_32FC1, right_mapx, right_mapy);
}

cv::Mat StereoCalibrationMat::remap_left(cv::Mat img_src) {
    cv::Mat img_dst;
    cv::remap(img_src, img_dst, left_mapx, left_mapy, cv::INTER_LINEAR, cv::BORDER_CONSTANT);
    return img_dst;
}

cv::Mat StereoCalibrationMat::remap_right(cv::Mat img_src) {
    cv::Mat img_dst;
    cv::remap(img_src, img_dst, right_mapx, right_mapy, cv::INTER_LINEAR, cv::BORDER_CONSTANT);
    return img_dst;
}


MonoCameraMat::MonoCameraMat(const std::string yamlFile) {
    YAML::Node cfg =  YAML::LoadFile(yamlFile);

    std::vector<double> m_vec = cfg["M1"].as<std::vector<double>>();
    std::vector<double> d_vec = cfg["d1"].as<std::vector<double>>();
    
    M = cv::Mat(3,3,CV_64F);
    for (int row=0; row<3; row++) {
        for (int col=0; col<3; col++) {
            M.at<double>(row, col) = m_vec[col+row*3];
        }
    }
   
    dist = cv::Mat(5,1,CV_64F);
    for (int col=0; col<5; col++) {
        dist.at<double>(0, col) = d_vec[col];
    }
}
