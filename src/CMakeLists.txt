set(CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.16) # Required by TFL
project(PosturePerfection VERSION 0.1.0)


set(LIBSRC
  iir.cpp
  inference_core.cpp
  post_processor.cpp)


# Set up TFL path
if(NOT TENSORFLOW_SOURCE_DIR)
get_filename_component(TENSORFLOW_SOURCE_DIR
"${CMAKE_CURRENT_BINARY_DIR}/../../../tensorflow_src" ABSOLUTE)
endif()

# Ensure all necessary libraries are included
include_directories("${TENSORFLOW_SOURCE_DIR}")
include_directories("${TENSORFLOW_SOURCE_DIR}/tensorflow/lite")

# Add and build TFL
add_subdirectory(
  "${TENSORFLOW_SOURCE_DIR}/tensorflow/lite"
  "${CMAKE_CURRENT_BINARY_DIR}/tensorflow-lite" EXCLUDE_FROM_ALL)

  
add_library(PosturePerfection_static STATIC ${LIBSRC})
target_include_directories(PosturePerfection_static PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/../flatbuffers/include")

add_executable(PosturePerfection ${LIBSRC} main.cpp)
target_link_libraries(PosturePerfection tensorflow-lite ${CMAKE_DL_LIBS})
target_include_directories(PosturePerfection PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/../flatbuffers/include")
