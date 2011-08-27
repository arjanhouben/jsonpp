
include_directories( ${JSONPP_SOURCE_DIR}/include/ )

file( GLOB headers ${JSONPP_SOURCE_DIR}/include/* )

set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -ggdb\ -Wall\ -O0\ -Wextra\ -Weffc++ )

add_executable( test
	${headers}
	${JSONPP_SOURCE_DIR}/src/main.cpp
)
