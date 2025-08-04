cmake_minimum_required(VERSION 3.10)
project(PCA_Anomaly)

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

add_library(pca_detector PCA_AnomalyDetector.cpp)
add_executable(DetectAnomalyLogByPCAWithoutLable main.cpp)
target_link_libraries(DetectAnomalyLogByPCAWithoutLable pca_detector)
