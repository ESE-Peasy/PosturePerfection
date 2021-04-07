find_package (Boost COMPONENTS system filesystem unit_test_framework REQUIRED)

set(libraries       ${CMAKE_THREAD_LIBS_INIT} PosturePerfection_static)# PosturePerfection_notify)
set(test_libraries  ${libraries} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

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
include_directories ("${TEST_SOURCE_DIR}/../src"
                     ${Boost_INCLUDE_DIRS})
include_directories ("${TEST_SOURCE_DIR}/../src/notifications"
                     ${Boost_INCLUDE_DIRS})
# Taken from https://github.com/WillBrennan/boost-test-example
function(create_test    test_path)
  set(test_name ${test_path})

  string(REPLACE "/" ";" test_name_list ${test_name})
  list(GET test_name_list -1 test_name)
  string(REPLACE "." ";" test_name_list ${test_name})
  list(GET test_name_list 0 test_name)

  message("-- adding test: ${test_name}")

  add_executable(${test_name}         "${TEST_SOURCE_DIR}/test/${test_path}")
  target_link_libraries(${test_name}  ${ARGN})

  target_compile_definitions(${test_name} PRIVATE BOOST_TEST_MAIN BOOST_TEST_DYN_LINK BOOST_TEST_MODULE=${test_name})

  add_test(${test_name} ${test_name})
endfunction()

add_definitions (-DBOOST_TEST_DYN_LINK)

create_test(test_iir ${test_libraries})
create_test(test_post_processor ${test_libraries})
create_test(test_pre_processor ${test_libraries} ${OpenCV_LIBS})
create_test(test_posture_estimator ${test_libraries} ${OpenCV_LIBS})
create_test(test_notifications ${test_libraries})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

