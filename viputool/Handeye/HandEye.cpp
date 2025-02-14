#include "HandEye.hpp"

//R和T转RT矩阵
cv::Mat R_T2RT(cv::Mat &R, cv::Mat &T)
{
    cv::Mat RT;
    cv::Mat_<double> R1 = (cv::Mat_<double>(4, 3) << R.at<double>(0, 0),
                           R.at<double>(0, 1),
                           R.at<double>(0, 2),
                           R.at<double>(1, 0),
                           R.at<double>(1, 1),
                           R.at<double>(1, 2),
                           R.at<double>(2, 0),
                           R.at<double>(2, 1),
                           R.at<double>(2, 2),
                           0.0,
                           0.0,
                           0.0);
    cv::Mat_<double> T1 =
        (cv::Mat_<double>(4, 1) << T.at<double>(0, 0), T.at<double>(1, 0), T.at<double>(2, 0), 1.0);

    cv::hconcat(R1, T1, RT);
    return RT;
}

//RT转R和T矩阵
void RT2R_T(cv::Mat &RT, cv::Mat &R, cv::Mat &T)
{
    cv::Rect R_rect(0, 0, 3, 3);
    cv::Rect T_rect(3, 0, 1, 3);
    R = RT(R_rect);
    T = RT(T_rect);
}

//判断是否为旋转矩阵
bool isRotationMatrix(const cv::Mat &R)
{
    cv::Mat tmp33 = R({0, 0, 3, 3});
    cv::Mat shouldBeIdentity;

    shouldBeIdentity = tmp33.t() * tmp33;

    cv::Mat I = cv::Mat::eye(3, 3, shouldBeIdentity.type());

    return cv::norm(I, shouldBeIdentity) < 1e-6;
}

/** @brief 欧拉角 -> 3*3 的R
 *	@param 	eulerAngle		角度值
 *	@param 	seq				指定欧拉角xyz的排列顺序如："xyz" "zyx"
 */
cv::Mat eulerAngleToRotatedMatrix(const cv::Mat &eulerAngle, const std::string &seq)
{
    CV_Assert(eulerAngle.rows == 1 && eulerAngle.cols == 3);

    eulerAngle /= 180. / CV_PI;
    cv::Matx13d m(eulerAngle);
    auto rx = m(0, 0), ry = m(0, 1), rz = m(0, 2);
    auto xs = std::sin(rx), xc = std::cos(rx);
    auto ys = std::sin(ry), yc = std::cos(ry);
    auto zs = std::sin(rz), zc = std::cos(rz);

    cv::Mat rotX = (cv::Mat_<double>(3, 3) << 1, 0, 0, 0, xc, -xs, 0, xs, xc);
    cv::Mat rotY = (cv::Mat_<double>(3, 3) << yc, 0, ys, 0, 1, 0, -ys, 0, yc);
    cv::Mat rotZ = (cv::Mat_<double>(3, 3) << zc, -zs, 0, zs, zc, 0, 0, 0, 1);

    cv::Mat rotMat;

    if (seq == "zyx")
        rotMat = rotX * rotY * rotZ;
    else if (seq == "yzx")
        rotMat = rotX * rotZ * rotY;
    else if (seq == "zxy")
        rotMat = rotY * rotX * rotZ;
    else if (seq == "xzy")
        rotMat = rotY * rotZ * rotX;
    else if (seq == "yxz")
        rotMat = rotZ * rotX * rotY;
    else if (seq == "xyz")
        rotMat = rotZ * rotY * rotX;
    else {
        cv::error(cv::Error::StsAssert,
                  "Euler angle sequence string is wrong.",
                  __FUNCTION__,
                  __FILE__,
                  __LINE__);
    }

    if (!isRotationMatrix(rotMat)) {
        cv::error(cv::Error::StsAssert,
                  "Euler angle can not convert to rotated matrix",
                  __FUNCTION__,
                  __FILE__,
                  __LINE__);
    }

    return rotMat;
}

/** @brief 四元数转旋转矩阵
 *	@note  数据类型double； 四元数定义 q = w + x*i + y*j + z*k
 *	@param q 四元数输入{w,x,y,z}向量
 *	@return 返回旋转矩阵3*3
 */
cv::Mat quaternionToRotatedMatrix(const cv::Vec4d &q)
{
    double w = q[0], x = q[1], y = q[2], z = q[3];

    double x2 = x * x, y2 = y * y, z2 = z * z;
    double xy = x * y, xz = x * z, yz = y * z;
    double wx = w * x, wy = w * y, wz = w * z;

    cv::Matx33d res{
        1 - 2 * (y2 + z2),
        2 * (xy - wz),
        2 * (xz + wy),
        2 * (xy + wz),
        1 - 2 * (x2 + z2),
        2 * (yz - wx),
        2 * (xz - wy),
        2 * (yz + wx),
        1 - 2 * (x2 + y2),
    };
    return cv::Mat(res);
}

/** @brief ((四元数||欧拉角||旋转向量) && 转移向量) -> 4*4 的Rt
 *	@param 	m				1*6 || 1*10的矩阵  -> 6  {x,y,z, rx,ry,rz}   10 {x,y,z, qw,qx,qy,qz,
 *rx,ry,rz}
 *	@param 	useQuaternion	如果是1*10的矩阵，判断是否使用四元数计算旋转矩阵
 *	@param 	seq				如果通过欧拉角计算旋转矩阵，需要指定欧拉角xyz的排列顺序如："xyz" "zyx"
 *为空表示旋转向量
 */
cv::Mat attitudeVectorToMatrix(cv::Mat m, bool useQuaternion, const std::string &seq)
{
    CV_Assert(m.total() == 6 || m.total() == 10);
    if (m.cols == 1) m = m.t();
    cv::Mat tmp = cv::Mat::eye(4, 4, CV_64FC1);

    //如果使用四元数转换成旋转矩阵则读取m矩阵的第第四个成员，读4个数据
    if (useQuaternion)  //normalized vector, its norm should be 1.
    {
        cv::Vec4d quaternionVec = m({3, 0, 4, 1});
        quaternionToRotatedMatrix(quaternionVec).copyTo(tmp({0, 0, 3, 3}));
        //cout << norm(quaternionVec) << endl;
    }
    else {
        cv::Mat rotVec;
        if (m.total() == 6)
            rotVec = m({3, 0, 3, 1});  //6
        else
            rotVec = m({7, 0, 3, 1});  //10

        //如果seq为空表示传入的是旋转向量，否则"xyz"的组合表示欧拉角
        if (0 == seq.compare(""))
            cv::Rodrigues(rotVec, tmp({0, 0, 3, 3}));
        else
            eulerAngleToRotatedMatrix(rotVec, seq).copyTo(tmp({0, 0, 3, 3}));
    }
    tmp({3, 0, 1, 3}) = m({0, 0, 3, 1}).t();
    return tmp;
}

void getQuaternion(const cv::Mat &R, double Q[])
{
    double trace = R.at<double>(0, 0) + R.at<double>(1, 1) + R.at<double>(2, 2);

    if (trace > 0.0) {
        double s = sqrt(trace + 1.0);
        Q[3] = (s * 0.5);
        s = 0.5 / s;
        Q[0] = ((R.at<double>(2, 1) - R.at<double>(1, 2)) * s);
        Q[1] = ((R.at<double>(0, 2) - R.at<double>(2, 0)) * s);
        Q[2] = ((R.at<double>(1, 0) - R.at<double>(0, 1)) * s);
    }

    else {
        int i = R.at<double>(0, 0) < R.at<double>(1, 1)
                    ? (R.at<double>(1, 1) < R.at<double>(2, 2) ? 2 : 1)
                    : (R.at<double>(0, 0) < R.at<double>(2, 2) ? 2 : 0);
        int j = (i + 1) % 3;
        int k = (i + 2) % 3;

        double s = sqrt(R.at<double>(i, i) - R.at<double>(j, j) - R.at<double>(k, k) + 1.0);
        Q[i] = s * 0.5;
        s = 0.5 / s;

        Q[3] = (R.at<double>(k, j) - R.at<double>(j, k)) * s;
        Q[j] = (R.at<double>(j, i) + R.at<double>(i, j)) * s;
        Q[k] = (R.at<double>(k, i) + R.at<double>(i, k)) * s;
    }
}



// Y-Z-X order
cv::Mat rot2euler(const cv::Mat & rotationMatrix)
{
    cv::Mat euler(3,1,CV_64F);

    double m00 = rotationMatrix.at<double>(0,0);
    double m02 = rotationMatrix.at<double>(0,2);
    double m10 = rotationMatrix.at<double>(1,0);
    double m11 = rotationMatrix.at<double>(1,1);
    double m12 = rotationMatrix.at<double>(1,2);
    double m20 = rotationMatrix.at<double>(2,0);
    double m22 = rotationMatrix.at<double>(2,2);

    double bank, attitude, heading;

    // Assuming the angles are in radians.
    if (m10 > 0.998) { // singularity at north pole
        bank = 0;
        attitude = CV_PI/2;
        heading = atan2(m02,m22);
    }
    else if (m10 < -0.998) { // singularity at south pole
        bank = 0;
        attitude = -CV_PI/2;
        heading = atan2(m02,m22);
    }
    else
    {
        bank = atan2(-m12,m11);
        attitude = asin(m10);
        heading = atan2(-m20,m00);
    }

    euler.at<double>(0) = bank;
    euler.at<double>(1) = attitude;
    euler.at<double>(2) = heading;

    return euler;
}

// xyzrpy in Y-Z-X order or [t,r]
std::vector<double> calcHandEyeError(const std::vector<cv::Mat> &vecHg,  const std::vector<cv::Mat> &vecHc, const cv::Mat &Hcg){
	int size = vecHg.size();
	cv::Mat r_mat(size, 3, CV_64FC1);
	cv::Mat t_mat(size, 3, CV_64FC1);
	

	for(int i = 0 ; i < size ; i++){
		cv::Mat T = vecHg[i] * Hcg * vecHc[i];
		cv::Mat R,t;
		RT2R_T(T, R, t);
		cv::Mat R_euler = rot2euler(R);
		// std::cout << R_euler.t() << std::endl;
		// std::cout << t.t() << std::endl;
		r_mat.row(i) = R_euler.t();
		t_mat.row(i) = t.t();
	}

	cv::Mat r_mean;
	cv::reduce(r_mat, r_mean, 0, cv::REDUCE_AVG);
	cv::Mat t_mean;
	cv::reduce(t_mat, t_mean, 0, cv::REDUCE_AVG);

	cv::Mat r_error(size, 3, CV_64FC1);
	cv::Mat t_error(size, 3, CV_64FC1);

	for(int i = 0; i < size ; i++){
		r_error.row(i) = r_mat.row(i) - r_mean;
		t_error.row(i) = t_mat.row(i) - t_mean;
	}

	cv::Mat r_error_abs = cv::abs(r_error);
	cv::Mat t_error_abs = cv::abs(t_error);

	// give the max error of each axi, eg: [x,y,z]
	// cv::Mat r_error_max, t_error_max;
	// cv::reduce(r_error_abs, r_error_max, 0, cv::REDUCE_MAX);
	// cv::reduce(t_error_abs, t_error_max, 0, cv::REDUCE_MAX);
	// return {r_error_max.at<double>(0,0) * 180 / CV_PI, r_error_max.at<double>(0,1) * 180 / CV_PI, r_error_max.at<double>(0,2) * 180 / CV_PI, t_error_max.at<double>(0,0), t_error_max.at<double>(0,1), t_error_max.at<double>(0,2)};

	// give the max error of all axis
	double r_max_error, t_max_error;
	cv::Point r_max_error_loc, t_max_error_loc;
	cv::minMaxLoc(r_error_abs, nullptr, &r_max_error, nullptr, &r_max_error_loc);
	cv::minMaxLoc(t_error_abs, nullptr, &t_max_error, nullptr, &t_max_error_loc);

	return {t_max_error, r_max_error * 180 / CV_PI};
}


std::vector<double> handEye(const cv::Mat_<double> &CalPose, const cv::Mat_<double> &ToolPose)
{
    std::vector<cv::Mat> R_gripper2base;
    std::vector<cv::Mat> t_gripper2base;
    std::vector<cv::Mat> R_target2cam;
    std::vector<cv::Mat> t_target2cam;
    cv::Mat R_cam2gripper = (cv::Mat_<double>(3, 3));
    cv::Mat t_cam2gripper = (cv::Mat_<double>(3, 1));

    std::vector<cv::Mat> images;
    size_t num_images = CalPose.rows;

    //读取末端，标定板的姿态矩阵 4*4
    std::vector<cv::Mat> vecHg, vecHc;
    cv::Mat Hcg;  //定义相机camera到末端grab的位姿矩阵
    cv::Mat tempR, tempT;

    for (size_t i = 0; i < num_images; i++)  //计算标定板位姿
    {
        //cv::Mat row_ = CalPose.row(i);
        cv::Mat tmp = attitudeVectorToMatrix(CalPose.row(i), false, "");  //转移向量转旋转矩阵
        vecHc.push_back(tmp);
        RT2R_T(tmp, tempR, tempT);

        R_target2cam.push_back(tempR);
        t_target2cam.push_back(tempT);
    }

    for (size_t i = 0; i < num_images; i++)  //计算机械臂位姿
    {
        cv::Mat tmp =
            attitudeVectorToMatrix(ToolPose.row(i), false, "xyz");  //机械臂位姿为欧拉角转旋转矩阵
        vecHg.push_back(tmp);
        RT2R_T(tmp, tempR, tempT);

        R_gripper2base.push_back(tempR);
        t_gripper2base.push_back(tempT);
    }
    //手眼标定，CALIB_HAND_EYE_TSAI法为11ms，最快
    calibrateHandEye(R_gripper2base,
                     t_gripper2base,
                     R_target2cam,
                     t_target2cam,
                     R_cam2gripper,
                     t_cam2gripper,
                     cv::CALIB_HAND_EYE_TSAI);  //TSAI, PARK

    Hcg = R_T2RT(R_cam2gripper, t_cam2gripper);
    cv::Mat R, t;
    RT2R_T(Hcg, R, t);

    //test
    // Tool_In_Base*Hcg*Cal_In_Cam，用第一组和第二组进行对比验证
    std::cout << "第一组和第二组手眼数据验证：" << std::endl;

    std::cout << vecHg[0] * Hcg * vecHc[0] << std::endl
              << vecHg[1] * Hcg * vecHc[1] << std::endl
              << std::endl;  //.inv()
    std::cout << "标定板在相机中的位姿：" << std::endl;
    std::cout << vecHc[1] << std::endl;
    std::cout << "手眼系统反演的位姿为：" << std::endl;
    //用手眼系统预测第一组数据中标定板相对相机的位姿，是否与vecHc[1]相同
    std::cout << Hcg.inv() * vecHg[1].inv() * vecHg[0] * Hcg * vecHc[0] << std::endl << std::endl;

    std::cout << "----手眼系统测试----" << std::endl;
    std::cout << "机械臂下标定板XYZ为: " << std::endl;
    for (size_t i = 0; i < vecHc.size(); ++i) {
        cv::Mat cheesePos{0.0, 0.0, 0.0, 1.0};  //4*1矩阵，单独求机械臂下，标定板的xyz
        cv::Mat worldPos = vecHg[i] * Hcg * vecHc[i] * cheesePos;
        std::cout << i << ": " << worldPos.t() << std::endl;
    }
    //test

    double q[4];  //xyzw
    getQuaternion(R, q);

    std::vector<double> pose_q(9); // x,y,z,rx,ry,rz,w,translation error, rotation error
    pose_q[0] = t.at<double>(0);
    pose_q[1] = t.at<double>(1);
    pose_q[2] = t.at<double>(2);
    pose_q[3] = q[0];
    pose_q[4] = q[1];
    pose_q[5] = q[2];
    pose_q[6] = q[3];

    std::vector<double> error = calcHandEyeError(vecHg, vecHc, Hcg);
    pose_q[7] = error[0];
    pose_q[8] = error[1];

    return pose_q;
}
