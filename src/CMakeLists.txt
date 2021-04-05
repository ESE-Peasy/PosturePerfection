set(CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.16) # Required by TFL
project(PosturePerfection VERSION 0.3.0)

if(APPLE)
  set(CMAKE_PREFIX_PATH "/usr/local/Cellar/")
endif()

if(UNIX AND NOT APPLE)
  set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -latomic")
endif()

# Include the GUI files
add_subdirectory(gui ENABLE_EXPORTS)
include_directories("${CMAKE_CURRENT_BINARY_DIR}/../../src/gui")

set(NTFYSRC
    notifications/notify.cpp
    notifications/broadcaster.cpp
    notifications/receiver.cpp
    )

set(LIBSRC
  iir.cpp
  inference_core.cpp
  framerate_settings.cpp
  post_processor.cpp
  pre_processor.cpp
  posture_estimator.cpp
  pipeline.cpp)

# Set up OpenCV package
set(OpenCV_DIR "${CMAKE_CURRENT_BINARY_DIR}/../../../opencv_build")
find_package(OpenCV REQUIRED)

# Set up TFL path
if(NOT TENSORFLOW_SOURCE_DIR)
get_filename_component(TENSORFLOW_SOURCE_DIR
"${CMAKE_CURRENT_BINARY_DIR}/../../../tensorflow_src" ABSOLUTE)
endif()

# Ensure all necessary libraries are included
include_directories("${TENSORFLOW_SOURCE_DIR}")
include_directories("${TENSORFLOW_SOURCE_DIR}/tensorflow/lite")
include_directories("${OpenCV_INCLUDE_DIRS}")

# Add and build TFL
add_subdirectory(
  "${TENSORFLOW_SOURCE_DIR}/tensorflow/lite"
  "${CMAKE_CURRENT_BINARY_DIR}/tensorflow-lite" EXCLUDE_FROM_ALL)
  
add_library(PosturePerfection_static STATIC ${LIBSRC})
add_library(PosturePerfection_notify STATIC ${NTFYSRC})
target_include_directories(PosturePerfection_static PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/../flatbuffers/include")

add_executable(PosturePerfection ${LIBSRC} main.cpp)
target_link_libraries(PosturePerfection tensorflow-lite PosturePerfection_gui ${CMAKE_DL_LIBS})
target_link_libraries(PosturePerfection ${OpenCV_LIBS})
target_include_directories(PosturePerfection PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/../flatbuffers/include")
