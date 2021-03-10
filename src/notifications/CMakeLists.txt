cmake_minimum_required(VERSION 3.10)

# set the project name
project(Notification)

set(LIBSRC
  server.cpp
  notify.cpp
  )
# add the executable
add_executable(Notification ${LIBSRC} server_main.cpp)

set(CMAKE_CXX_STANDARD 11)



