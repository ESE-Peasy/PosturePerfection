set(CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.16) # Required by TFL
project(PosturePerfection VERSION 0.1.0)


set(LIBSRC
  inference_core.cpp
  posture_estimator.cpp)


# Set up TFL path
if(NOT TENSORFLOW_SOURCE_DIR)
get_filename_component(TENSORFLOW_SOURCE_DIR
"${CMAKE_CURRENT_BINARY_DIR}/../../../tensorflow_src" ABSOLUTE)
endif()

message("${TENSORFLOW_SOURCE_DIR}")

# Ensure all necessary libraries are included
include_directories("${TENSORFLOW_SOURCE_DIR}")
include_directories("${TENSORFLOW_SOURCE_DIR}/tensorflow/lite")
include_directories("${TENSORFLOW_SOURCE_DIR}/../tflite_build/flatbuffers/include/")

# Add and build TFL
add_subdirectory(
  "${TENSORFLOW_SOURCE_DIR}/tensorflow/lite"
  "${CMAKE_CURRENT_BINARY_DIR}/tensorflow-lite" EXCLUDE_FROM_ALL)

add_library(doubler doubler.cpp)

add_library(PosturePerfection_static STATIC ${LIBSRC})

add_executable(PosturePerfection inference_core.cpp main.cpp)
target_link_libraries(PosturePerfection tensorflow-lite doubler ${CMAKE_DL_LIBS})

