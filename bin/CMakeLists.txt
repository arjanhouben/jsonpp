
include_directories( ${JSONPP_SOURCE_DIR}/include/ )

file( GLOB headers ${JSONPP_SOURCE_DIR}/include/* )

if( MSVC )
else()
    set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -ggdb\ -Wall\ -O0\ -Wextra\ -Weffc++ )
endif()

add_executable( test
        ${headers}
        ${JSONPP_SOURCE_DIR}/src/main.cpp
)
