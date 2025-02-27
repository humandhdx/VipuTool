#include "ut_robot_wrapper/robot_config.hpp"
#include <math.h>

#define METER_TO_MILLIMETER 1000.0

UtRobotConfig::TestConfig UtRobotConfig::TestConfig_RobotLeft = {
    "192.168.3.37",
    {},//identity uuid, sw_version, hw_version
    /* Original Mdh Models for 7 joints robot */
    {
        {
            //alpha,        a(mm),  d(mm),  theta
            {0.0f,          0.0f,   288.5f, 0.0f},
            {-M_PI / 2.0f,  0.0f,   0.0f,   0.0f},
            {M_PI / 2.0f,   0.0f,   300.0f, 0.0f},
            {M_PI / 2.0f,   55.0f,  0.0f,   0.0f},
            {-M_PI / 2.0f,  -55.0f, 273.5f, 0.0f},
            {-M_PI / 2.0f,  0.0f,   0.0f,   0.0f},
            {M_PI / 2.0f,   0.0f,   142.5f, 0.0f}
        }
    },
    {},// Mdh Models after calib
    /* repeat test */
    {
        //initial joint pos
        {91.910320, 69.493276,  -90.623914, -124.103321,    -27.947194, 59.642553,  119.9},
        //mid cartesian poses
        {
            {313.870513, -11.825159, 249.483245},   // 1
            {400.473053, -11.825159, 299.483245},   // 5
            {400.473053, 88.174841, 299.483245},    // 7
            {313.870513, 88.174841, 249.483245},    // 3
            {213.870513, 88.174841, 422.688325},    // 4
            {213.870513, -11.825159, 422.688325},   // 2
            {300.473053, -11.825159, 472.688325},   // 6
            {300.473053, 88.174841, 472.688325},    // 8
        },
        //finiah joint pos
        {85.1,      78.1,       -118.6,     -93.3,          -11.2,      -8.1,       61.7}
    },
    /* accracy test */
    {
        //initial joint pos
        {91.910320, 69.493276,  -90.623914, -124.103321,    -27.947194, 59.642553,  119.9},
        //mid cartesian poses
        {
            {313.870513, -11.825159, 249.483245},   // 1
            {400.473053, -11.825159, 299.483245},   // 5
            {400.473053, 88.174841, 299.483245},    // 7
            {313.870513, 88.174841, 249.483245},    // 3
            {213.870513, 88.174841, 422.688325},    // 4
            {213.870513, -11.825159, 422.688325},   // 2
            {300.473053, -11.825159, 472.688325},   // 6
            {300.473053, 88.174841, 472.688325},    // 8
        },
        //finiah joint pos
        {85.1,      78.1,       -118.6,     -93.3,          -11.2,      -8.1,       61.7}
    },
    /* workspace test */
    {
        //initial joint pos
        {91.910320, 69.493276,  -90.623914, -124.103321,    -27.947194, 59.642553,  119.9},
        //mid cartesian poses
        {
            {313.870513, -11.825159, 249.483245},   // 1
            {400.473053, -11.825159, 299.483245},   // 5
            {400.473053, 88.174841, 299.483245},    // 7
            {313.870513, 88.174841, 249.483245},    // 3
            {213.870513, 88.174841, 422.688325},    // 4
            {213.870513, -11.825159, 422.688325},   // 2
            {300.473053, -11.825159, 472.688325},   // 6
            {300.473053, 88.174841, 472.688325},    // 8
        },
        //finiah joint pos
        {85.1,      78.1,       -118.6,     -93.3,          -11.2,      -8.1,       61.7}
    },
    /* gravity test */
    {{91.910320, 69.493276, -90.623914, -124.103321, -27.947194, 59.642553, 119.9}}
};

UtRobotConfig::TestConfig UtRobotConfig::TestConfig_RobotRight = {
    "192.168.5.39",
    {},//identity uuid, sw_version, hw_version
    /* original Mdh Models for 7 joints robot */
    {
        {
            //alpha,        a(mm),  d(mm),  theta
            {0.0f,          0.0f,   288.5f, 0.0f},
            {M_PI / 2.0f,   0.0f,   0.0f,   0.0f},
            {-M_PI / 2.0f,  0.0f,   300.0f, 0.0f},
            {-M_PI / 2.0f,  55.0f,  0.0f,   0.0f},
            {M_PI / 2.0f,   -55.0f, 273.5f, 0.0f},
            {M_PI / 2.0f,   0.0f,   0.0f,   0.0f},
            {-M_PI / 2.0f,  0.0f,   142.5f, 0.0f}
        }
    },
    {},// Mdh Models after calib
    /* repeat test */
    {
        //initial joint pos
        {-115.91337,-85.959911, 125.752660, 120.587977, 17.698261, -77.364478, -69.737722},
        //mid cartesian poses
        {
            {158.300209, -188.025544, 373.062064},   // 1
            {244.902749, -188.025544, 423.062064},   // 5
            {244.902749, -88.025544, 423.062064},    // 7
            {158.300209, -88.025544, 373.062064},    // 3
            {58.300209,  -88.025544, 546.267144},    // 4
            {58.300209, -188.025544, 546.267144},   // 2
            {144.902749, -188.025544, 596.267144},   // 6
            {144.902749, -88.025544, 596.267144},    // 8
        },
        //finiah joint pos
        {-86.9,     -78.4,      118,        84.4,           13.5,       2.6,        -69.5}
    },
    /* accracy test */
    {
        //initial joint pos
        {-115.91337,-85.959911, 125.752660, 120.587977, 17.698261, -77.364478, -69.737722},
        //mid cartesian poses
        {
            {158.300209, -188.025544, 373.062064},   // 1
            {244.902749, -188.025544, 423.062064},   // 5
            {244.902749, -88.025544, 423.062064},    // 7
            {158.300209, -88.025544, 373.062064},    // 3
            {58.300209,  -88.025544, 546.267144},    // 4
            {58.300209, -188.025544, 546.267144},   // 2
            {144.902749, -188.025544, 596.267144},   // 6
            {144.902749, -88.025544, 596.267144},    // 8
        },
        //finiah joint pos
        {-86.9,     -78.4,      118,        84.4,           13.5,       2.6,        -69.5}
    },
    /* workspace test */
    {
        //initial joint pos
        {-115.91337,-85.959911, 125.752660, 120.587977, 17.698261, -77.364478, -69.737722},
        //mid cartesian poses
        {
            {158.300209, -188.025544, 373.062064},   // 1
            {244.902749, -188.025544, 423.062064},   // 5
            {244.902749, -88.025544, 423.062064},    // 7
            {158.300209, -88.025544, 373.062064},    // 3
            {58.300209,  -88.025544, 546.267144},    // 4
            {58.300209, -188.025544, 546.267144},   // 2
            {144.902749, -188.025544, 596.267144},   // 6
            {144.902749, -88.025544, 596.267144},    // 8
        },
        //finiah joint pos
        {-86.9,     -78.4,      118,        84.4,           13.5,       2.6,        -69.5}
    },
    /* gravity test */
    {{91.910320, 69.493276, -90.623914, -124.103321, -27.947194, 59.642553, 119.9}}
};

struct MDH_Param_Arr{
    float   theta_Rot;//radian
    float   d_Trans;//mm
    float   alpha_Rot;//radian
    float   a_Trans;//mm
};
struct MDH_Param{
    float   alpha_Rot;//radian
    float   a_Trans;//mm
    float   d_Trans;//mm
    float   theta_Rot;//radian
};

void UtRobotConfig::Convertion_RobotDhModel_Array_To_Struct(std::array<std::array<float,4>,7> &mdh_arr, RobotDhModel_UtraRobot &mdh_model)
{
    for(int i=0; i<7; i++)
    {
        mdh_model[i].alpha_Rot    = mdh_arr[i][2];
        mdh_model[i].a_Trans      = mdh_arr[i][3] * METER_TO_MILLIMETER;
        mdh_model[i].d_Trans      = mdh_arr[i][1] * METER_TO_MILLIMETER;
        mdh_model[i].theta_Rot    = mdh_arr[i][0];
    }
}

void UtRobotConfig::Convertion_RobotDhModel_Struct_To_Array(RobotDhModel_UtraRobot &mdh_model, std::array<std::array<float,4>,7> &mdh_arr)
{
    for(int i=0; i<7; i++)
    {
        mdh_arr[i][0]   = mdh_model[i].theta_Rot;
        mdh_arr[i][1]   = mdh_model[i].d_Trans / METER_TO_MILLIMETER;
        mdh_arr[i][2]   = mdh_model[i].alpha_Rot;
        mdh_arr[i][3]   = mdh_model[i].a_Trans / METER_TO_MILLIMETER;
    }
}

// UtRobotConfig::Identity_Info UtRobotConfig::Id_INFO_left;
// UtRobotConfig::Identity_Info UtRobotConfig::Id_INFO_right;

// void UtRobotConfig::update_robot_identity_info_left(std::string& uuid, std::string& version_sw, std::string& version_hw)
// {
//     Id_INFO_left.UUID = uuid;
//     Id_INFO_left.VERSION_SW = version_sw;
//     Id_INFO_left.VERSION_HW = version_hw;
// }

// void UtRobotConfig::update_robot_identity_info_right(std::string &uuid, std::string &version_sw, std::string &version_hw)
// {
//     Id_INFO_right.UUID = uuid;
//     Id_INFO_right.VERSION_SW = version_sw;
//     Id_INFO_right.VERSION_HW = version_hw;
// }
