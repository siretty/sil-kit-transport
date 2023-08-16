cmake_minimum_required(VERSION 3.26)
project(SilKitTransport)


set(CMAKE_CXX_STANDARD 14)


add_subdirectory(third_party EXCLUDE_FROM_ALL)
add_subdirectory(src EXCLUDE_FROM_ALL)


add_executable("${PROJECT_NAME}Run" main.cpp)

target_link_libraries("${PROJECT_NAME}Run" "${PROJECT_NAME}::${PROJECT_NAME}")
