cmake_minimum_required(VERSION 3.7.0)

project(gui VERSION 1.0.0 LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

# Set up OpenCV package
set(OpenCV_DIR "${CMAKE_CURRENT_BINARY_DIR}/../../../../opencv_build")
find_package(OpenCV REQUIRED)
include_directories("${OpenCV_INCLUDE_DIRS}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets PrintSupport REQUIRED)

# Set up TFL path
if(NOT TENSORFLOW_SOURCE_DIR)
get_filename_component(TENSORFLOW_SOURCE_DIR
"${CMAKE_CURRENT_BINARY_DIR}/../../../../tensorflow_src" ABSOLUTE)
endif()

# Ensure all necessary libraries are included
include_directories("${TENSORFLOW_SOURCE_DIR}")
include_directories("${TENSORFLOW_SOURCE_DIR}/tensorflow/lite")
include_directories("${OpenCV_INCLUDE_DIRS}")

add_library(PosturePerfection_gui STATIC mainwindow.cpp ../posture_estimator.cpp ../intermediate_structures.cpp ../pipeline.cpp)
target_link_libraries(PosturePerfection_gui
  Qt5::Widgets Qt5::PrintSupport
)

target_include_directories(PosturePerfection_gui PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/../../flatbuffers/include")
target_link_libraries(PosturePerfection_gui ${OpenCV_LIBS})
