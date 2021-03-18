set(CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.16) # Required by TFL

project (TEST)
add_subdirectory(src)
if (ENABLE_TESTING)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()
