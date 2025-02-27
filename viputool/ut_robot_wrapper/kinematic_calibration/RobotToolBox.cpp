#include "ut_robot_wrapper/kinematic_calibration/RobotToolBox.hpp"
#include <Eigen/Dense>
#include <cmath>
#include <iostream>

Eigen::Isometry3d RobotToolBox::mdh_2_t(const Eigen::VectorXd &mdh) {
    Eigen::Isometry3d T = Eigen::Isometry3d::Identity();
    T.matrix() << cos(mdh(3)), -sin(mdh(3)), 0, mdh(1),
        sin(mdh(3)) * cos(mdh(0)), cos(mdh(3)) * cos(mdh(0)), -sin(mdh(0)),
        -sin(mdh(0)) * mdh(2), sin(mdh(3)) * sin(mdh(0)),
        cos(mdh(3)) * sin(mdh(0)), cos(mdh(0)), cos(mdh(0)) * mdh(2), 0, 0, 0,
        1;
    return std::move(T);
}

Eigen::MatrixXd RobotToolBox::mdh_2_urdf(const Eigen::MatrixXd &mdh) {
    int num = mdh.rows();
    Eigen::MatrixXd urdf(num, 6);
    Eigen::Isometry3d t;
    Eigen::Vector3d ypr;
    for (int i = 0; i < num; ++i) {
        t = mdh_2_t(mdh.row(i));
        double x = t(0, 3);
        double y = t(1, 3);
        double z = t(2, 3);
        ypr = t.rotation().eulerAngles(2, 1, 0);
        urdf(i, 0) = x;
        urdf(i, 1) = y;
        urdf(i, 2) = z;
        urdf(i, 0) = x / 1000.0;
        urdf(i, 1) = y / 1000.0;
        urdf(i, 2) = z / 1000.0;
    }
    return std::move(urdf);
}

Eigen::MatrixXd RobotToolBox::mdh_2_urdf_matlab(const Eigen::MatrixXd &mdh) {
    int num = mdh.rows();
    double eps = 2.2204e-16;
    Eigen::MatrixXd urdf(num, 6);
    Eigen::Isometry3d t;
    Eigen::Vector3d ypr;
    for (int i = 0; i < num; ++i) {
        t = mdh_2_t(mdh.row(i));
        double x = t(0, 3);
        double y = t(1, 3);
        double z = t(2, 3);

        if (abs(t(1, 1)) < eps && abs(t(2, 1)) < eps) {
            // singularity
            ypr(0) = 0;
            ypr(1) = atan2(-t(2, 0), t(0, 0)); // pitch
            ypr(2) = atan2(-t(1, 2), t(1, 1)); // yaw is difference yaw-roll
        } else {
            ypr(0) = atan2(t(1, 0), t(0, 0));
            double sp = sin(ypr(0));
            double cp = cos(ypr(0));
            ypr(1) = atan2(-t(2, 0), cp * t(0, 0) + sp * t(1, 0));
            ypr(2) =
                atan2(sp * t(0, 2) - cp * t(1, 2), cp * t(1, 1) - sp * t(0, 1));
        }
        urdf(i, 0) = x / 1000.0;
        urdf(i, 1) = y / 1000.0;
        urdf(i, 2) = z / 1000.0;
        urdf(i, 3) = ypr(2);
        urdf(i, 4) = ypr(1);
        urdf(i, 5) = ypr(0);
    }
    return std::move(urdf);
}

Eigen::MatrixXd
RobotToolBox::mdh_2_urdf_matlab_without_theta(const Eigen::MatrixXd &mdh) {
    int num = mdh.rows();
    double eps = 2.2204e-16;
    Eigen::MatrixXd urdf(num, 6);
    Eigen::Isometry3d t;
    Eigen::Vector3d ypr;
    Eigen::MatrixXd mdh_without_theta(num, 4);
    mdh_without_theta = mdh;
    for (int i = 0; i < num; ++i) {
        mdh_without_theta(i, 3) = 0.0;
        t = mdh_2_t(mdh_without_theta.row(i));
        double x = t(0, 3);
        double y = t(1, 3);
        double z = t(2, 3);

        if (abs(t(1, 1)) < eps && abs(t(2, 1)) < eps) {
            // singularity
            ypr(0) = 0;
            ypr(1) = atan2(-t(2, 0), t(0, 0)); // pitch
            ypr(2) = atan2(-t(1, 2), t(1, 1)); // yaw is difference yaw-roll
        } else {
            ypr(0) = atan2(t(1, 0), t(0, 0));
            double sp = sin(ypr(0));
            double cp = cos(ypr(0));
            ypr(1) = atan2(-t(2, 0), cp * t(0, 0) + sp * t(1, 0));
            ypr(2) =
                atan2(sp * t(0, 2) - cp * t(1, 2), cp * t(1, 1) - sp * t(0, 1));
        }
        urdf(i, 0) = x / 1000.0;
        urdf(i, 1) = y / 1000.0;
        urdf(i, 2) = z / 1000.0;
        urdf(i, 3) = ypr(2);
        urdf(i, 4) = ypr(1);
        urdf(i, 5) = ypr(0);
    }
    return std::move(urdf);
}

Eigen::VectorXd
RobotToolBox::forward_kinematics_for_calibration(const Eigen::MatrixXd &mdh) {
    int num = mdh.rows();
    Eigen::Isometry3d T = Eigen::Isometry3d::Identity();
    for (int i = 0; i < num; ++i) {
        T = T * mdh_2_t(mdh.row(i));
    }
    Eigen::VectorXd d(6);
    d << T(0, 3), T(1, 3), T(2, 3), T(2, 1), T(0, 2), T(1, 0);
    return std::move(d);
}

Eigen::MatrixXd
RobotToolBox::jacobin_by_numerical_method(const Eigen::MatrixXd &mdh) {
    double delta = 1e-6;
    int num = mdh.rows();
    int down = 0;
    Eigen::MatrixXd J(6, 4 * num);
    Eigen::MatrixXd mdh1 = mdh;
    for (int i = 0; i < num; ++i) {
        for (int j = 0; j < 4; ++j) {
            Eigen::MatrixXd mdh2 = mdh1;
            mdh2(i, j) = mdh1(i, j) + delta;
            J.col(down) = (forward_kinematics_for_calibration(mdh2) -
                           forward_kinematics_for_calibration(mdh1)) /
                          delta;
            down++;
        }
    }
    return std::move(J);
}

Eigen::Isometry3d RobotToolBox::fixxyz_2_t(const Eigen::VectorXd &inp) {
    double x = inp(0);
    double y = inp(1);
    double z = inp(2);
    double rx = inp(3);
    double ry = inp(4);
    double rz = inp(5);
    Eigen::Isometry3d out = Eigen::Isometry3d::Identity();
    Eigen::Matrix3d rot_z =
        Eigen::AngleAxisd(rz, Eigen::Vector3d::UnitZ()).toRotationMatrix();
    Eigen::Matrix3d rot_y =
        Eigen::AngleAxisd(ry, Eigen::Vector3d::UnitY()).toRotationMatrix();
    Eigen::Matrix3d rot_x =
        Eigen::AngleAxisd(rx, Eigen::Vector3d::UnitX()).toRotationMatrix();
    Eigen::Matrix3d rot_combined = rot_z * rot_y * rot_x;
    out.matrix().block<3, 3>(0, 0) = rot_combined;
    out.matrix()(0, 3) = x;
    out.matrix()(1, 3) = y;
    out.matrix()(2, 3) = z;
    return std::move(out);
}

bool RobotToolBox::mdh_calibration(
    const Eigen::MatrixXd &mdh0, const Eigen::MatrixXd &joint_theta,
    const std::vector<Eigen::Isometry3d> &flange_pose,
    Eigen::MatrixXd &mdh_cali, double allowable_deviation) {
    int itenum = 10;
    if (mdh0.rows() != joint_theta.cols()) {
        return false;
    }
    int ptnum = joint_theta.rows();
    if (ptnum != flange_pose.size()) {
        return false;
    }
    Eigen::MatrixXd mdh1 = mdh0;
    int degnum = mdh1.rows();
    Eigen::MatrixXd d(0, 1), J(0, 28);
    for (int i = 0; i < ptnum; ++i) {
        Eigen::VectorXd st = joint_theta.row(i);
        Eigen::MatrixXd mdh_act0(7, 4);
        mdh_act0 << 0, 0, 0, st(0), 0, 0, 0, st(1), 0, 0, 0, st(2), 0, 0, 0,
            st(3), 0, 0, 0, st(4), 0, 0, 0, st(5), 0, 0, 0, st(6);
        Eigen::MatrixXd mdh_act1 = mdh1 + mdh_act0;
        Eigen::Isometry3d T_sensor = flange_pose.at(i);
        Eigen::VectorXd par_sensor(6);
        par_sensor << T_sensor(0, 3), T_sensor(1, 3), T_sensor(2, 3),
            T_sensor(2, 1), T_sensor(0, 2), T_sensor(1, 0);
        d.conservativeResize(d.rows() + 6, Eigen::NoChange);
        d.block(6 * i, 0, 6, 1) =
            ((par_sensor)-forward_kinematics_for_calibration(mdh_act1));
        J.conservativeResize(J.rows() + 6, Eigen::NoChange);
        Eigen::MatrixXd Jmatrix(6, 28);
        Jmatrix = jacobin_by_numerical_method(mdh_act1);
        J.block(6 * i, 0, 6, 28) = Jmatrix;
    }

    Eigen::VectorXd Err = (J.transpose() * J).ldlt().solve(J.transpose() * d);
    // std::cout <<"Err:" << std::endl;
    // std::cout << Err << std::endl;
    for (int ii = 0; ii < itenum; ++ii) {
        int down = 0;
        for (int j = 0; j < degnum; ++j) {
            for (int k = 0; k < 4; ++k) {
                if (std::abs(Err(down)) < 20) {
                    mdh1(j, k) += Err(down);
                }
                down++;
            }
        }
        Eigen::MatrixXd dd(0, 1), JJ(0, 28);
        for (int i = 0; i < ptnum; ++i) {
            Eigen::VectorXd st = joint_theta.row(i);
            Eigen::MatrixXd mdh_act0(7, 4);
            mdh_act0 << 0, 0, 0, st(0), 0, 0, 0, st(1), 0, 0, 0, st(2), 0, 0, 0,
                st(3), 0, 0, 0, st(4), 0, 0, 0, st(5), 0, 0, 0, st(6);
            Eigen::MatrixXd mdh_act1 = mdh1 + mdh_act0;
            Eigen::Isometry3d T_sensor = flange_pose.at(i);
            Eigen::VectorXd par_sensor(6);
            par_sensor << T_sensor(0, 3), T_sensor(1, 3), T_sensor(2, 3),
                T_sensor(2, 1), T_sensor(0, 2), T_sensor(1, 0);
            dd.conservativeResize(dd.rows() + 6, Eigen::NoChange);
            dd.block(6 * i, 0, 6, 1) =
                ((par_sensor)-forward_kinematics_for_calibration(mdh_act1));
            JJ.conservativeResize(JJ.rows() + 6, Eigen::NoChange);
            Eigen::MatrixXd Jmatrix(6, 28);
            Jmatrix = jacobin_by_numerical_method(mdh_act1);
            JJ.block(6 * i, 0, 6, 28) = Jmatrix;
        }
        Err = (JJ.transpose() * JJ).ldlt().solve(JJ.transpose() * dd);
    }
    Eigen::VectorXd distance(ptnum);
    Eigen::MatrixXd ddd(0, 1);
    for (int i = 0; i < ptnum; ++i) {
        Eigen::VectorXd st = joint_theta.row(i);
        Eigen::MatrixXd mdh_act0(7, 4);
        mdh_act0 << 0, 0, 0, st(0), 0, 0, 0, st(1), 0, 0, 0, st(2), 0, 0, 0,
            st(3), 0, 0, 0, st(4), 0, 0, 0, st(5), 0, 0, 0, st(6);
        Eigen::MatrixXd mdh_act1 = mdh1 + mdh_act0;
        Eigen::Isometry3d T_sensor = flange_pose.at(i);
        Eigen::VectorXd par_sensor(6);
        par_sensor << T_sensor(0, 3), T_sensor(1, 3), T_sensor(2, 3),
            T_sensor(2, 1), T_sensor(0, 2), T_sensor(1, 0);
        ddd.conservativeResize(ddd.rows() + 6, Eigen::NoChange);
        ddd.block(6 * i, 0, 6, 1) =
            ((par_sensor)-forward_kinematics_for_calibration(mdh_act1));
        distance(i) =
            sqrt(ddd(i * 6) * ddd(i * 6) + ddd(i * 6 + 1) * ddd(i * 6 + 1) +
                 ddd(i * 6 + 2) * ddd(i * 6 + 2));
    }
    // std::cout <<"distance" << std::endl;
    // std::cout <<distance << std::endl;
    //  Eigen::VectorXd maxRowValues = distance.maxCoeff();
    double distances_max = distance.maxCoeff();
    // std::cout << "max distance: " << maxRowValues.transpose() << std::endl;
    if (distances_max > allowable_deviation) // mm
    {
        std::cout << "calibration failed!\ndeviation is too big, deviation is "
                  << distances_max << std::endl;
        return false;
    } else {
        std::cout << "max calibration deviation is: " << distances_max
                  << std::endl;
    }

    mdh_cali = mdh1;
    // std::cout <<"mdh_cali" << std::endl;
    // std::cout <<mdh_cali << std::endl;
    // std::cout << "calculation finish!" << std::endl;
    return true;
}

bool RobotToolBox::residual(const Eigen::MatrixXd &mdh0,
                            const Eigen::MatrixXd &joint_theta,
                            const std::vector<Eigen::Isometry3d> &flange_pose,
                            Eigen::MatrixXd &residual) {
    int ptnum = joint_theta.rows();
    Eigen::MatrixXd mdh1 = mdh0;
    int degnum = mdh1.rows();
    Eigen::MatrixXd d(0, 1), J(0, 28), res(ptnum, 6);
    if (mdh0.rows() != joint_theta.cols()) {
        return false;
    }
    if (ptnum != flange_pose.size()) {
        return false;
    }
    for (int i = 0; i < ptnum; ++i) {
        Eigen::VectorXd st = joint_theta.row(i);
        Eigen::MatrixXd mdh_act0(7, 4);
        mdh_act0 << 0, 0, 0, st(0), 0, 0, 0, st(1), 0, 0, 0, st(2), 0, 0, 0,
            st(3), 0, 0, 0, st(4), 0, 0, 0, st(5), 0, 0, 0, st(6);
        Eigen::MatrixXd mdh_act1 = mdh1 + mdh_act0;
        Eigen::Isometry3d T_sensor = flange_pose.at(i);
        Eigen::VectorXd par_sensor(6);
        par_sensor << T_sensor(0, 3), T_sensor(1, 3), T_sensor(2, 3),
            T_sensor(2, 1), T_sensor(0, 2), T_sensor(1, 0);
        d.conservativeResize(d.rows() + 6, Eigen::NoChange);
        d.block(6 * i, 0, 6, 1) =
            ((par_sensor)-forward_kinematics_for_calibration(mdh_act1));
        J.conservativeResize(J.rows() + 6, Eigen::NoChange);
        Eigen::MatrixXd Jmatrix(6, 28);
        Jmatrix = jacobin_by_numerical_method(mdh_act1);
        J.block(6 * i, 0, 6, 28) = Jmatrix;
        res.block(i, 0, 1, 6) = d.block(6 * i, 0, 6, 1).transpose();
    }
    residual = res;
    return true;
}
