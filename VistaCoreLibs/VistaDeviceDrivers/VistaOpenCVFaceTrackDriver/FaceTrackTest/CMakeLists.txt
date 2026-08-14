cmake_minimum_required(VERSION 3.28)

project(OpenCVFaceTrackDemo)

find_package(Vista  REQUIRED COMPONENTS VistaKernel)
find_package(OpenCV REQUIRED)

add_executable(OpenCVFaceTrackDemo main.cpp configfiles/vista.ini configfiles/ucp_facetrack.xml configfiles/keyboard.xml)
target_link_libraries(OpenCVFaceTrackDemo
  PRIVATE
    Vista::Kernel
    VistaOpenCVFaceTrackDriver
)
