cmake_minimum_required(VERSION 3.28)

project(KinectDemo)

find_package(Vista REQUIRED COMPONENTS VistaKernel)
find_package(OpenNI REQUIRED)

add_executable(KinectDemo main.cpp KinectHandler.h KinectHandler.cpp configfiles/vista.ini configfiles/ucp_kinect.xml configfiles/spatial_kinect.xml)

target_link_libraries(KinectDemo
    Vista::Kernel
    Vista::OpenNIDriver
)
