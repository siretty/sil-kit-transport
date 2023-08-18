
add_library("${PROJECT_NAME}"

    buffersequence.cpp
    byteorder.cpp
    exceptions.cpp

    ibufferedbytestream.cpp
    imessagestream.cpp
    iunbufferedbytestream.cpp

    asiogenericunbufferedbytestream.cpp
    bufferedbytestream.cpp
    messagestream.cpp
)

target_link_libraries("${PROJECT_NAME}"
    PUBLIC asio
)

target_include_directories("${PROJECT_NAME}"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)


add_executable("${PROJECT_NAME}-TestRunner"
    asiogenericunbufferedbytestream-test.cpp
    bufferedbytestream-test.cpp
)

target_link_libraries("${PROJECT_NAME}-TestRunner"
    PRIVATE "${PROJECT_NAME}"
    PRIVATE GTest::gtest_main
    PRIVATE GTest::gmock
)


add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")
