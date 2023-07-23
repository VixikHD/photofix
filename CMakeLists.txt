cmake_minimum_required(VERSION 3.25)
project(photofix)

set(CMAKE_TOOLCHAIN_FILE "C:/CLionProjects/vcpkg/scripts/buildsystems/vcpkg.cmake")

set(CMAKE_CXX_STANDARD 17)

add_executable(photofix main.cpp HeifFile.cpp HeifFile.h)

# Manually set the libheif include directory and link against the library
target_include_directories(photofix PRIVATE "C:/CLionProjects/vcpkg/installed/x64-windows/include")
target_link_libraries(photofix PRIVATE "C:/CLionProjects/vcpkg/installed/x64-windows/lib/libpng16.lib")
target_link_libraries(photofix PRIVATE "C:/CLionProjects/vcpkg/installed/x64-windows/lib/heif.lib")
