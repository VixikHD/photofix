cmake_minimum_required(VERSION 3.25)
project(photofix)

set(CMAKE_CXX_STANDARD 17)

set(OpenCV_DIR "/usr/share/OpenCV")
find_package(OpenCV REQUIRED)
message(STATUS "Found OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "Found OpenCV includes: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "Using compiler ${CMAKE_CXX_COMPILER_ID}")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

include_directories(/usr/include/libheif)
include_directories(/usr/include/libpng)
include_directories(/usr/include/opencv4/)

link_directories(/usr/lib/x86_64-linux-gnu)

add_executable(photofix src/main.cpp src/format/HeifFileHolder.h src/format/FileHolder.h src/FormatConverter.h src/format/OpenCVFileHolder.h)

target_link_libraries(photofix png)
target_link_libraries(photofix heif)
target_link_libraries(photofix opencv_core;opencv_imgcodecs;opencv_imgproc)
