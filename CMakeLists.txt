cmake_minimum_required(VERSION 2.8)
project(hns)

find_package(catkin)
catkin_stack()

find_package(Boost COMPONENTS system thread regex signals serialization)
find_package(llog)

include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${llog_INCLUDE_DIRS})

add_library(hsn
  src/id.cpp
  src/distributed_manager.cpp
  src/distributed_base.cpp
)

target_link_libraries(hsn ${Boost_LIBRARIES} ${llog_LIBRARIES} -lstdc++)

add_executable(hns_test
  src/test.cpp
)
target_link_libraries(hns_test hsn)

add_executable(hns_test2
  src/test2.cpp
)
target_link_libraries(hns_test2 hsn)

#add_executable(hns_gtest test/hns_gtest.cpp)

# Add test cpp file
add_gtest(hns_gtest test/hns_gtest.cpp)
target_link_libraries(hns_gtest hsn ${GTEST_BOTH_LIBRARIES})
message(GTEST_BOTH_LIBRARIES ${GTEST_BOTH_LIBRARIES})

