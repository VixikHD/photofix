cmake_minimum_required(VERSION 3.25)
project(photofix)

set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/include/libheif)
include_directories(/usr/include/opencv4/)

link_directories(/usr/lib/x86_64-linux-gnu)

add_executable(photofix src/main.cpp src/format/HeifFileHolder.h src/format/FileHolder.h src/FormatConverter.h src/format/OpenCVFileHolder.h)

target_link_libraries(photofix heif)
target_link_libraries(photofix opencv_core;opencv_imgcodecs;opencv_imgproc)
