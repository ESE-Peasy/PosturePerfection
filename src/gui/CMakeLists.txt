cmake_minimum_required(VERSION 3.7.0)

project(gui VERSION 1.0.0 LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets PrintSupport REQUIRED)
find_package(QCustomPlot)

add_executable(gui
  mainwindow.cpp
  main.cpp
)

target_link_libraries(gui
  Qt5::Widgets Qt5::PrintSupport ${QCustomPlot_LIBRARIES}
  )

install(TARGETS gui
        CONFIGURATIONS Release
        RUNTIME DESTINATION Release/bin) 

# add_library(PosturePerfection_static STATIC ${LIBSRC})

add_library(PosturePerfection_gui STATIC mainwindow.cpp)
target_link_libraries(PosturePerfection_gui
  Qt5::Widgets Qt5::PrintSupport ${QCustomPlot_LIBRARIES}
)