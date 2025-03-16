cmake_minimum_required(VERSION 3.10)
project(InfluxDBMetrics)

set(CMAKE_CXX_STANDARD 11)

# Find the curl package
find_package(CURL REQUIRED)

# Add the executable
add_executable(process_metrics process_metrics.cpp)

# Link with libcurl
target_link_libraries(process_metrics CURL::libcurl)

