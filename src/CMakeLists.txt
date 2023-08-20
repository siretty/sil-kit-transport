
add_library("${PROJECT_NAME}"

    utilities/buffersequence.cpp
    utilities/byteorder.cpp
    utilities/exceptions.cpp

    interfaces/ibufferedbytestream.cpp
    interfaces/imessagestream.cpp
    interfaces/iunbufferedbytestream.cpp

    streams/asiogenericunbufferedbytestream.cpp
    streams/bufferedbytestream.cpp
    streams/messagestream.cpp
)

target_link_libraries("${PROJECT_NAME}"
    PUBLIC asio
)

target_include_directories("${PROJECT_NAME}"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/interfaces"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/streams"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/utilities"
)


add_executable("${PROJECT_NAME}-TestRunner"
    streams/asiogenericunbufferedbytestream-test.cpp
    streams/bufferedbytestream-test.cpp
)

target_link_libraries("${PROJECT_NAME}-TestRunner"
    PRIVATE "${PROJECT_NAME}"
    PRIVATE GTest::gtest_main
    PRIVATE GTest::gmock
)


add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")
