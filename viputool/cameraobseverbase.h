/**
 * ***************************************
 * @file CameraObseverBase.hpp
 * @author : linxinshi@fuebiosciences.com
 * @brief :  相机观察者基类
 * @version 0.1
 * @date : 2024-09-03
 * @copyright 上海馥逸医疗科技有限公司
 *
 * ***************************************
 */
#ifndef CAMERA_OBSERVER_BASE_HPP_
#define CAMERA_OBSERVER_BASE_HPP_

#include <atomic>
#include <thread>
#include <chrono>
#include <mutex>
#include "Periodlc_rt2.hpp"
#include "v4l2capture.h"
#include <dirent.h>
#include "struct.h"
#include <shared_mutex>

enum GetImageCode
{
    GetImageCode_Suc,
    GetImageCode_Fail,
    GetImageCode_TimeOut,
};



class CameraObseverBase
{
public:
    CameraObseverBase();
    ~CameraObseverBase();
    /** get_binocular_image_raw_real_time 循环获取本地相机实时的图片
     *
     * @param 参数1， BinocularRecord 输出实时图片信息，包含左眼图片、右眼图片、以及两张图片最前的生成的时间,
     * @return  函数执行的结果  0 成功  1、没有图片  2 获取时间超时
     **/
    virtual GetImageCode loop_get_local_binocular_image_real_time(BinocularRecord &record);

    /** get_binocular_image_raw_real_time 循环获取全局相机实时的图片
     *
     * @param 参数1， BinocularRecord 输出实时图片信息，包含左眼图片、右眼图片、以及两张图片最前的生成的时间,
     * @return  函数执行的结果  0 成功  1、没有图片  2 获取时间超时
     **/
    virtual GetImageCode loop_get_global_binocular_image_real_time(BinocularRecord &record);

    /** loop_get_binocular_image_raw 循环获取图片
     *
     * @param 参数1，BinocularRecord  输出图片信息，包含左眼图片、右眼图片、以及两张图片最前的生成的时间,
     * @return  函数执行的结果  0 成功  1、没有图片  2 获取时间超时
     **/
    virtual GetImageCode loop_get_binocular_image_raw(BinocularRecord &record);

    /** get_binocular_image_raw_real_time 获取本地相机实时的图片
     *
     * @param 参数1， BinocularRecord 输出实时图片信息，包含左眼图片、右眼图片、以及两张图片最前的生成的时间,
     * @return  函数执行的结果  0 成功  1、没有图片  2 获取时间超时
     **/
    virtual GetImageCode get_local_binocular_image_real_time(BinocularRecord &record);

    /** get_binocular_image_raw_real_time 获取全局相机实时的图片
     *
     * @param 参数1， BinocularRecord 输出实时图片信息，包含左眼图片、右眼图片、以及两张图片最前的生成的时间,
     * @return  函数执行的结果  0 成功  1、没有图片  2 获取时间超时
     **/
    virtual GetImageCode get_global_binocular_image_real_time(BinocularRecord &record);

    /** get_binocular_image_raw_real_time 获取图片
     *
     * @param 参数1，BinocularRecord  输出图片信息，包含左眼图片、右眼图片、以及两张图片最前的生成的时间,
     * @return  函数执行的结果  0 成功  1、没有图片  2 获取时间超时
     **/
    virtual GetImageCode get_binocular_image_raw(BinocularRecord &record);

    /** UpdateLeftImage 更新左眼图片信息
     *
     * @param 参数1，  pImage 图片信息
     * @param 参数2，  time   获取图片的时间
     * @return  无
     **/
    virtual void UpdateLeftImage(cv::Mat* pImage, TimePoint time);

    /** UpdateLeftImage 更新右眼图片信息
     *
     * @param 参数1， pImage 图片信息
     * @param 参数2， time   获取图片的时间
     * @return
     **/
    virtual void UpdateRightImage(cv::Mat* pImage, TimePoint time);

    /** NotifyLeftCameraFailed 通知左侧相机获取图片失败
     *
     * @return  无
     **/
    virtual void NotifyLeftCameraFailed();

    /** NotifyRightCameraFailed 通知右侧相机获取图片失败
     *
     * @return 无
     **/
    virtual void NotifyRightCameraFailed();

    /** NotifyLeftCameraSucced 通知左侧相机获取图片成功
     *
     * @return  无
     **/
    virtual void NotifyLeftCameraSucced();

    /** NotifyRightCameraSucced 通知右侧相机获取图片成功
     *
     * @return 无
     **/
    virtual void NotifyRightCameraSucced();

    /** TestGetImage 测试获取图片信息
     *
     * @return 无
     **/
    void  TestGetImage();

    /** SetLocalDiffThreshold 设置局部相机左右图片时间阈值
     *
     * @return 无
     **/
    void SetLocalDiffThreshold(std::chrono::milliseconds::rep LocalDiffThreshold);

    /** SetGlocalDiffThreshold 设置全部相机左右图片时间阈值
     *
     * @return 无
     **/
    void SetGlocalDiffThreshold(std::chrono::milliseconds::rep GlobalDiffThreshold);

    /** SetLocalGetImageDuration 设置局部相机图片的时间跟当前的阈值
     *
     * @return 无
     **/
    void SetLocalGetImageDuration(std::chrono::milliseconds::rep LocalGetImageDuration);

    /** SetLocalGetImageDuration 设置全局相机图片的时间跟当前的阈值
     *
     * @return 无
     **/
    void SetGlobalGetImageDuration(std::chrono::milliseconds::rep GlobalGetImageDuration);


protected:
    mutable std::shared_mutex mtx_image_left_;
    mutable std::shared_mutex mtx_image_right_;
    cv::Mat mat_left_;
    cv::Mat mat_right_;
    TimePoint timepoint_left_;
    TimePoint timepoint_right_;
    std::chrono::duration<double> now_time_last_ ;
    bool m_flag = true;
    std::chrono::milliseconds::rep  LocalDiffThreshold_  = 1000 / 60 * 2 + 5;
    std::chrono::milliseconds::rep  GlobalDiffThreshold_  = 1000 / 10 * 2 + 5;
    std::chrono::milliseconds::rep  LocalGetImageDuration_ = 1000 / 60 * 2 + 5;
    std::chrono::milliseconds::rep  GlobalGetImageDuration_ = 1000 / 10 +3;
};

#endif // CAMERA_OBSERVER_BASE_HPP_
