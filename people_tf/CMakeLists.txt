cmake_minimum_required(VERSION 2.8.3)
project(people_tf)

find_package(catkin REQUIRED COMPONENTS
  people_msgs
  roscpp
  tf
  visualization_msgs
)

include_directories(
  ${catkin_INCLUDE_DIRS}
)

catkin_package()

add_executable(tf_to_person src/tf_to_person.cpp)

add_dependencies(tf_to_person people_msgs_gencpp visualization_msgs_gencpp)

target_link_libraries(tf_to_person
  ${catkin_LIBRARIES}
)