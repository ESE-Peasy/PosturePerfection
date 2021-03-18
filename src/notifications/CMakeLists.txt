cmake_minimum_required(VERSION 3.10)

# set the project name
project(Notification)

set(NOTFYSRC
  server.cpp
  notify.cpp
  client.cpp
  )
# add the executable
add_executable(NotificationServer ${NOTFYSRC} server_main.cpp)
add_executable(NotificationClient ${NOTFYSRC} client_main.cpp)

add_library(PosturePerfection_notify STATIC ${NOTFYSRC})

set(CMAKE_CXX_STANDARD 11)



