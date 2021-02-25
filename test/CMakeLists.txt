find_package (Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
include_directories ("${TEST_SOURCE_DIR}/../src"
                     ${Boost_INCLUDE_DIRS})

add_definitions (-DBOOST_TEST_DYN_LINK)
add_executable (Test test.cpp)
add_executable (IIRTests test_iir.cpp)
target_link_libraries (Test
                       doubler
                       PosturePerfection_static
                       ${Boost_FILESYSTEM_LIBRARY}
                       ${Boost_SYSTEM_LIBRARY}
                       ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test (NAME MyTest COMMAND Test)
add_test (NAME IIRTests COMMAND IIRTests)
