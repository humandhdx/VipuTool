cmake_minimum_required(VERSION 3.16)

project(VipuTool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Concurrent Quick Multimedia Widgets)
find_package(OpenCV REQUIRED)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -fPIC)
endif()
qt_add_executable(${PROJECT_NAME}
    main.cpp
    resource.qrc
    sshmanager.h sshmanager.cpp
    cameramanager.h cameramanager.cpp
    Periodlc_rt2.hpp
    Timer2.hpp
    v4l2capture.h v4l2capture.cpp
    v4l2access.h v4l2access.cpp
    v4l2device.h v4l2device.cpp
    struct.h
    cameraobseverbase.h cameraobseverbase.cpp
    imageprovider.h imageprovider.cpp
    v4l2mmapdevice.h v4l2mmapdevice.cpp
    v4l2readwritedevice.h v4l2readwritedevice.cpp
)
include_directories(/usr/include)
link_directories("/usr/lib/x86_64-linux-gnu/libssh.so")
link_directories("/usr/lib/x86_64-linux-gnu/libv4l2.so")
set_target_properties(${PROJECT_NAME} PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appVipuTool
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
target_link_libraries(${PROJECT_NAME} PUBLIC libssh.so)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt6::Quick
    Qt6::Concurrent
    Qt6::Multimedia
    Qt6::Widgets
    ${OpenCV_LIBS}
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
