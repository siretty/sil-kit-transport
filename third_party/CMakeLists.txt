
add_library(asio INTERFACE)
target_include_directories(asio INTERFACE asio/asio/include)

add_subdirectory(googletest EXCLUDE_FROM_ALL)
