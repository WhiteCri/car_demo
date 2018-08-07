cmake_minimum_required(VERSION 2.8.3)
project(send_prius_msgs)
add_compile_options(-std=c++17)

find_package(catkin REQUIRED)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  prius_msgs
)

catkin_package(
     CATKIN_DEPENDS prius_msgs
)

include_directories(
  include ${catkin_INCLUDE_DIRS}

)


add_executable(fake_sender
  src/fake_sender.cpp
)
add_dependencies(fake_sender ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(fake_sender
  ${catkin_LIBRARIES}
)