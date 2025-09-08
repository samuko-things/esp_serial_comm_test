cmake_minimum_required(VERSION 3.16)
project(test_cpp LANGUAGES CXX)

# include_directories(include)

add_executable( test_cpp examples/test.cpp )
target_link_libraries( test_cpp serial )

# add_executable( read_imu examples/read_imu.cpp )
# target_link_libraries( read_imu serial )