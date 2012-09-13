cmake_minimum_required(VERSION 2.8)
project(hns)

find_package(catkin)
catkin_stack()

find_package(Boost COMPONENTS system thread regex signals serialization)
find_package(llog)
find_package(darc_common)

include_directories(include)
include_directories(${darc_common_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${llog_INCLUDE_DIRS})

add_library(hsn
  src/dummy.cpp
)

target_link_libraries(hsn
  ${Boost_LIBRARIES}
  ${llog_LIBRARIES}
  ${darc_common_LIBRARIES}
  -lstdc++)

add_executable(hns_test
  src/test.cpp
)
target_link_libraries(hns_test hsn)

# Add test cpp file
catkin_add_gtest(hns_gtest test/hns_gtest.cpp)
target_link_libraries(hns_gtest hsn ${GTEST_BOTH_LIBRARIES})
