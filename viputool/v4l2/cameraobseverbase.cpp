#include "cameraobseverbase.h"

void CameraObseverBase::TestGetImage()
{
    //BinocularRecord record;
    //loop_get_local_binocular_image_real_time(record);
}

CameraObseverBase::CameraObseverBase() {}

CameraObseverBase::~CameraObseverBase()
{
    m_flag = false;
}

void CameraObseverBase::SetLocalDiffThreshold(std::chrono::milliseconds::rep LocalDiffThreshold)
{
    LocalDiffThreshold_ = LocalDiffThreshold;
}

void CameraObseverBase::SetGlocalDiffThreshold(std::chrono::milliseconds::rep GlobalDiffThreshold)
{
    GlobalDiffThreshold_ = GlobalDiffThreshold;
}

void CameraObseverBase::SetLocalGetImageDuration(
    std::chrono::milliseconds::rep LocalGetImageDuration)
{
    LocalGetImageDuration_ = LocalGetImageDuration;
}

void CameraObseverBase::SetGlobalGetImageDuration(
    std::chrono::milliseconds::rep GlobalGetImageDuration)
{
    GlobalGetImageDuration_ = GlobalGetImageDuration;
}

GetImageCode CameraObseverBase::loop_get_local_binocular_image_real_time(BinocularRecord &record)
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    while (m_flag) {
        auto dur1 = std::chrono::duration_cast<std::chrono::milliseconds>(
                        timepoint_right_.time_since_epoch() - timepoint_left_.time_since_epoch())
                        .count();
        if (abs(dur1) > LocalDiffThreshold_) continue;

        {
            std::shared_lock<std::shared_mutex> lck(mtx_image_left_);
            auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(
                           now - timepoint_left_.time_since_epoch())
                           .count();
            if (mat_left_.rows == 0 || dur > LocalGetImageDuration_) continue;
            record.left_image = mat_left_.clone();
        }

        {
            std::shared_lock<std::shared_mutex> lck(mtx_image_right_);
            auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(
                           now - timepoint_right_.time_since_epoch())
                           .count();
            if (mat_right_.rows == 0 || dur > LocalGetImageDuration_) continue;
            record.right_image = mat_right_.clone();
        }

        record.timepoint = timepoint_left_ > timepoint_right_ ? timepoint_right_ : timepoint_left_;
        //auto dur2 = std::chrono::duration_cast<std::chrono::milliseconds>(now -
        //record.timepoint.time_since_epoch()).count();
        //auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(now -
        //now_time_last_).count(); std::cout << "time get dur:" << dur2 << " left time compare right
        //time: " << dur1 << "funtion get time:" << dur << std::endl;
        now_time_last_ = now;
        return GetImageCode_Suc;
    }
}

GetImageCode CameraObseverBase::loop_get_global_binocular_image_real_time(BinocularRecord &record)
{
    while (m_flag) {
        //RCLCPP_INFO(kLogger, "loop_get_global_binocular_image_real_time 1");
        auto dur1 = std::chrono::duration_cast<std::chrono::milliseconds>(
                        timepoint_right_.time_since_epoch() - timepoint_left_.time_since_epoch())
                        .count();
        if (abs(dur1) > GlobalDiffThreshold_) continue;
        //RCLCPP_INFO(kLogger, "loop_get_global_binocular_image_real_time 2");
        auto now = std::chrono::steady_clock::now().time_since_epoch();
        {
            std::shared_lock<std::shared_mutex> lck(mtx_image_left_);
            auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(
                           now - timepoint_left_.time_since_epoch())
                           .count();
            if (mat_left_.rows == 0 || dur > GlobalGetImageDuration_) continue;
            record.left_image = mat_left_.clone();
        }
        //RCLCPP_INFO(kLogger, "loop_get_global_binocular_image_real_time 3");
        {
            std::shared_lock<std::shared_mutex> lck(mtx_image_right_);
            auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(
                           now - timepoint_right_.time_since_epoch())
                           .count();
            if (mat_right_.rows == 0 || dur > GlobalGetImageDuration_) continue;
            record.right_image = mat_right_.clone();
        }
        //RCLCPP_INFO(kLogger, "loop_get_global_binocular_image_real_time 4");
        record.timepoint = timepoint_left_ > timepoint_right_ ? timepoint_right_ : timepoint_left_;

        //auto dur2 = std::chrono::duration_cast<std::chrono::milliseconds>(now -
        //record.timepoint.time_since_epoch()).count(); std::cout << "time get dur:" << dur2 << "
        //left time compare right time " << dur1 << std::endl;
        now_time_last_ = now;
        return GetImageCode_Suc;
    }
}

GetImageCode CameraObseverBase::loop_get_binocular_image_raw(BinocularRecord &record)
{
    while (m_flag) {
        {
            std::shared_lock<std::shared_mutex> lck(mtx_image_left_);
            if (mat_left_.rows == 0) continue;
            record.left_image = mat_left_.clone();
        }

        {
            std::shared_lock<std::shared_mutex> lck(mtx_image_right_);
            if (mat_right_.rows == 0) continue;
            record.right_image = mat_right_.clone();
        }

        record.timepoint = timepoint_left_ > timepoint_right_ ? timepoint_right_ : timepoint_left_;
        return GetImageCode_Suc;
    }
}

GetImageCode CameraObseverBase::get_local_binocular_image_real_time(BinocularRecord &record)
{

    auto dur1 = std::chrono::duration_cast<std::chrono::milliseconds>(
                    timepoint_right_.time_since_epoch() - timepoint_left_.time_since_epoch())
                    .count();
    if (abs(dur1) > LocalDiffThreshold_) return GetImageCode_Fail;
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    {
        std::shared_lock<std::shared_mutex> lck(mtx_image_left_);
        auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(
                       now - timepoint_left_.time_since_epoch())
                       .count();
        if (mat_left_.rows == 0 || dur > LocalGetImageDuration_) return GetImageCode_Fail;
        record.left_image = mat_left_.clone();
    }

    {
        std::shared_lock<std::shared_mutex> lck(mtx_image_right_);
        auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(
                       now - timepoint_right_.time_since_epoch())
                       .count();
        if (mat_right_.rows == 0 || dur > LocalGetImageDuration_) return GetImageCode_Fail;
        record.right_image = mat_right_.clone();
    }

    record.timepoint = timepoint_left_ > timepoint_right_ ? timepoint_right_ : timepoint_left_;
    //auto dur2 = std::chrono::duration_cast<std::chrono::milliseconds>(now -
    //record.timepoint.time_since_epoch()).count();
    //std::cout << "time get dur:" << dur2 << " left time compare right time " << dur1 << std::endl;
    now_time_last_ = now;
    return GetImageCode_Suc;
}

GetImageCode CameraObseverBase::get_global_binocular_image_real_time(BinocularRecord &record)
{

    auto dur1 = std::chrono::duration_cast<std::chrono::milliseconds>(
                    timepoint_right_.time_since_epoch() - timepoint_left_.time_since_epoch())
                    .count();
    if (abs(dur1) > GlobalDiffThreshold_) return GetImageCode_Fail;
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    {
        std::shared_lock<std::shared_mutex> lck(mtx_image_left_);
        auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(
                       now - timepoint_left_.time_since_epoch())
                       .count();
        if (mat_left_.rows == 0 || dur > GlobalGetImageDuration_) return GetImageCode_Fail;
        record.left_image = mat_left_.clone();
    }

    {
        std::shared_lock<std::shared_mutex> lck(mtx_image_right_);
        auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(
                       now - timepoint_right_.time_since_epoch())
                       .count();
        if (mat_right_.rows == 0 || dur > GlobalGetImageDuration_) return GetImageCode_Fail;
        record.right_image = mat_right_.clone();
    }

    record.timepoint = timepoint_left_ > timepoint_right_ ? timepoint_right_ : timepoint_left_;

    //auto mil = std::chrono::duration_cast<std::chrono::milliseconds>(now).count() % 1000;
    //auto sec = std::chrono::duration_cast<std::chrono::seconds>(now).count();
    //std::time_t t_sys = std::chrono::duration_cast<std::chrono::seconds>(
    //                        std::chrono::system_clock::now().time_since_epoch())
    //                        .count();
    //std::time_t t_show = t_sys - t_sys % 86400 + sec;

    //auto dur2 = std::chrono::duration_cast<std::chrono::milliseconds>(now -
    //record.timepoint.time_since_epoch()).count();
    //std::cout << "time get dur:" << dur2 << " left time compare right time " << dur1 << std::endl;
    now_time_last_ = now;
    return GetImageCode_Suc;
}

GetImageCode CameraObseverBase::get_binocular_image_raw(BinocularRecord &record)
{
    {
        std::shared_lock<std::shared_mutex> lck(mtx_image_left_);
        if (mat_left_.rows == 0) return GetImageCode_Fail;
        record.left_image = mat_left_.clone();
    }

    {
        std::shared_lock<std::shared_mutex> lck(mtx_image_right_);
        if (mat_right_.rows == 0) return GetImageCode_Fail;
        record.right_image = mat_right_.clone();
    }

    record.timepoint = timepoint_left_ > timepoint_right_ ? timepoint_right_ : timepoint_left_;
    return GetImageCode_Suc;
}

void CameraObseverBase::UpdateLeftImage(cv::Mat *pImage, TimePoint time)
{
    std::unique_lock<std::shared_mutex> lck(mtx_image_left_);
    if (pImage == nullptr || pImage->rows == 0) return;
    if (timepoint_left_ >= time) return;

    mat_left_ = pImage->clone();
    timepoint_left_ = time;
}

void CameraObseverBase::UpdateRightImage(cv::Mat *pImage, TimePoint time)
{
    std::unique_lock<std::shared_mutex> lck(mtx_image_right_);
    if (pImage == nullptr || pImage->rows == 0) return;

    if (timepoint_right_ >= time) return;

    mat_right_ = pImage->clone();
    timepoint_right_ = time;
}

void CameraObseverBase::NotifyLeftCameraFailed()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
}

void CameraObseverBase::NotifyRightCameraFailed()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
}

void CameraObseverBase::NotifyLeftCameraSucced()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
}

void CameraObseverBase::NotifyRightCameraSucced()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
}
