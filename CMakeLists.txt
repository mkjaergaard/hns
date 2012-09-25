cmake_minimum_required(VERSION 2.8)
project(hns)

find_package(catkin)
catkin_stack()

find_package(Boost COMPONENTS system thread regex signals serialization)
find_package(catkin COMPONENTS beam darc_common)

catkin_project(hns
  INCLUDE_DIRS include)

include_directories(include)
include_directories(${catkin_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

set(hns_LIBRARIES
  ${Boost_LIBRARIES}
  ${catkin_LIBRARIES}
  -lstdc++)

# Add test cpp file
catkin_add_gtest(hns_gtest test/hns_gtest.cpp)
target_link_libraries(hns_gtest ${hns_LIBRARIES} ${GTEST_BOTH_LIBRARIES})
