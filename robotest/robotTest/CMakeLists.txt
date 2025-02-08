cmake_minimum_required(VERSION 3.22)

project(robotTest LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(
  ../utapi/
   /opt/ros/humble/include/rclcpp
)

link_directories(
  ../utapi/build
  /opt/ros/humble/lib/
)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Qt6 COMPONENTS Widgets LinguistTools REQUIRED)

#set(TS_FILES robotTest_zh_CN.ts)
add_executable(robotTest
    main.cpp
    widget.cpp
    widget.h
    widget.ui
    utrarobot.h
    utrarobot.cpp
    FTSensorDriver.cpp
    FTSensorDriver.hpp
  #  ${TS_FILES}
)

target_link_libraries(robotTest PUBLIC utapi rt pthread)
target_link_libraries(robotTest PRIVATE Qt6::Widgets)
