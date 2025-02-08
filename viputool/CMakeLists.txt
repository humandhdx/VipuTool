cmake_minimum_required(VERSION 3.16)

project(VipuTool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Concurrent Quick Multimedia)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -fPIC)
endif()
qt_add_executable(${PROJECT_NAME}
    main.cpp
    resource.qrc
    sshmanager.h sshmanager.cpp
    cameramanager.h cameramanager.cpp
)
include_directories(/usr/include)
link_directories("/usr/lib/x86_64-linux-gnu/libssh.so")
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
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
