cmake_minimum_required(VERSION 3.16)

project(VipuTool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Concurrent Quick Multimedia Widgets)
find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -fPIC)
endif()
add_subdirectory(3rdparty/cbdetect)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/utapi)

set(src_robot_wrapper
    ${CMAKE_CURRENT_SOURCE_DIR}/ut_robot_wrapper/robot_config.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ut_robot_wrapper/robot_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ut_robot_wrapper/utrrobotqwrapper.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ut_robot_wrapper/utrrobotqwrapper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ut_robot_wrapper/utrrobot.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ut_robot_wrapper/utrrobot.cpp
)

qt_add_executable(${PROJECT_NAME}
    main.cpp
    resource.qrc
    sshmanager.h sshmanager.cpp
    cameramanager.h cameramanager.cpp
    v4l2/Periodlc_rt2.hpp
    v4l2/Timer2.hpp
    v4l2/v4l2capture.h v4l2/v4l2capture.cpp
    v4l2/v4l2access.h v4l2/v4l2access.cpp
    v4l2/v4l2device.h v4l2/v4l2device.cpp
    v4l2/struct.h
    v4l2/cameraobseverbase.h v4l2/cameraobseverbase.cpp
    v4l2/v4l2mmapdevice.h v4l2/v4l2mmapdevice.cpp
    v4l2/v4l2readwritedevice.h v4l2/v4l2readwritedevice.cpp
    Handeye/HandEye.hpp Handeye/HandEye.cpp
    Handeye/stereo_calibration_mat.hpp Handeye/stereo_calibration_mat.cpp
    imageprovider.h imageprovider.cpp
    handeyecalculate.h handeyecalculate.cpp
    ${src_robot_wrapper}
    logger.h logger.cpp
)
include_directories(/usr/include)
include_directories("/usr/include/eigen3")
link_directories("/usr/lib/x86_64-linux-gnu/libssh.so")
link_directories("/usr/lib/x86_64-linux-gnu/libv4l2.so")
set_target_properties(${PROJECT_NAME} PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appVipuTool
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    $<TARGET_PROPERTY:utapi,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(${PROJECT_NAME} PUBLIC libssh.so)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt6::Quick
    Qt6::Concurrent
    Qt6::Multimedia
    Qt6::Widgets
    ${OpenCV_LIBS}
    yaml-cpp
    cbdetect
    utapi
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
