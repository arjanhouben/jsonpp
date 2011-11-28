
include_directories(
		${json++_SOURCE_DIR}/include/
#	/Users/arjan/Downloads/jsoncpp-src-0.5.0/include/json
		/Users/arjan/Downloads/jsoncpp/trunk/jsoncpp/include/json
)

if( MSVC )
else()
		set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -ggdb\ -Wall\ -O3\ -Wextra\ -Weffc++ )
endif()

add_executable( test
	${json++_SOURCE_DIR}/src/main.cpp
	${json++_SOURCE_DIR}/include/jsonpp/parser.h
	${json++_SOURCE_DIR}/include/jsonpp/value.h
	${json++_SOURCE_DIR}/include/jsonpp/unicode.h
	${json++_SOURCE_DIR}/include/jsonpp/string.h
)

target_link_libraries( test
#	/Users/arjan/Downloads/jsoncpp-src-0.5.0/libs/linux-gcc-4.2.1/libjson_linux-gcc-4.2.1_libmt.a
		/Users/arjan/Downloads/jsoncpp/trunk/jsoncpp/libs/linux-gcc-4.2.1/libjson_linux-gcc-4.2.1_libmt.a
)
