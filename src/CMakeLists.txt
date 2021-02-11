cmake_minimum_required(VERSION 3.16) # Required by TFL
project(PosturePerfection VERSION 0.1.0)

# include(CTest)
# enable_testing()

## From https://www.tensorflow.org/lite/guide/build_cmake#create_a_cmake_project_which_uses_tensorflow_lite

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)

# set(TENSORFLOW_SOURCE_DIR "" CACHE PATH
#   "Directory that contains the TensorFlow project" )
if(NOT TENSORFLOW_SOURCE_DIR)
  get_filename_component(TENSORFLOW_SOURCE_DIR
    "${CMAKE_CURRENT_LIST_DIR}/../../tensorflow_src" ABSOLUTE)
endif()

add_subdirectory(
  "${TENSORFLOW_SOURCE_DIR}/tensorflow/lite"
  "${CMAKE_CURRENT_BINARY_DIR}/tensorflow-lite" EXCLUDE_FROM_ALL)
  
# add_executable(minimal minimal.cc)
## End of TFL-related CMake

include_directories("${CMAKE_SOURCE_DIR}/../../tensorflow_src/tensorflow/lite")

add_executable(PosturePerfection main.cpp hello.cpp)
target_link_libraries(PosturePerfection tensorflow-lite ${CMAKE_DL_LIBS})
