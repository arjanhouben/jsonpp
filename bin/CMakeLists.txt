
include_directories(
	${json++_SOURCE_DIR}/include/
)

if( MSVC )

else()
	set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -ggdb\ -Wall\ -O0\ -Wextra\ -Weffc++ )
endif()

add_executable( test
	${json++_SOURCE_DIR}/src/main.cpp
	${json++_SOURCE_DIR}/include/jsonpp/parser.h
	${json++_SOURCE_DIR}/include/jsonpp/var.h
	${json++_SOURCE_DIR}/include/jsonpp/unicode.h
	${json++_SOURCE_DIR}/include/jsonpp/misc.h
	${json++_SOURCE_DIR}/include/jsonpp/generator.h
	${json++_SOURCE_DIR}/include/json++
)

target_link_libraries( test
)

install( DIRECTORY ${json++_SOURCE_DIR}/include/ DESTINATION include )
