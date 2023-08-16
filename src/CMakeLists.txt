
add_library("${PROJECT_NAME}"
    ibytestream.cpp
    imessagestream.cpp

    byteorder.cpp
    exceptions.cpp
    log.cpp

    asiogenericbytestream.cpp
    messagestream.cpp
    buffersequence.cpp
    buffersequence.hpp
    iunbufferedbytestream.cpp
    iunbufferedbytestream.hpp
    asiogenericunbufferedbytestream.cpp
    asiogenericunbufferedbytestream.hpp
)

target_link_libraries("${PROJECT_NAME}"
    PUBLIC asio
)

target_include_directories("${PROJECT_NAME}"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)


add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")
