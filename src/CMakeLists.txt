set(CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.16) # Required by TFL
project(PosturePerfection VERSION 0.1.0)

# Sets up to use TFL git repo
# So far only found to work with commit f32b4d37cc059771198484d453d5cf288cf7803b
include(ExternalProject)
ExternalProject_Add(TFL
  GIT_REPOSITORY https://github.com/tensorflow/tensorflow.git
  GIT_TAG f32b4d37cc059771198484d453d5cf288cf7803b
  GIT_PROGRESS true
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}/tensorflow"
  CONFIGURE_COMMAND ${CMAKE_COMMAND} "${CMAKE_CURRENT_BINARY_DIR}/tensorflow/src/TFL/tensorflow/lite"
)

# Ensure all necessary libraries are included
include_directories("${CMAKE_CURRENT_BINARY_DIR}/tensorflow/src/TFL")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/tensorflow/src/TFL/tensorflow/lite")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/tensorflow/src/TFL-build/flatbuffers/include")

# Set up some path variable
if(NOT TENSORFLOW_SOURCE_DIR)
  get_filename_component(TENSORFLOW_SOURCE_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/tensorflow/src/TFL/" ABSOLUTE)
endif()

# Necessary for linking to work
add_subdirectory(
  "${TENSORFLOW_SOURCE_DIR}/tensorflow/lite"
  "${CMAKE_CURRENT_BINARY_DIR}/tensorflow-lite" EXCLUDE_FROM_ALL)

include_directories("${CMAKE_CURRENT_BINARY_DIR}/tensorflow/src/TFL/tensorflow/lite")

add_executable(PosturePerfection inference_core.cpp main.cpp)
target_link_libraries(PosturePerfection tensorflow-lite ${CMAKE_DL_LIBS})

add_dependencies(PosturePerfection TFL)
