
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
	${json++_SOURCE_DIR}/include/jsonpp/basic_var_data.h
	${json++_SOURCE_DIR}/include/jsonpp/register_type.h
	${json++_SOURCE_DIR}/include/jsonpp/base64.h
	${json++_SOURCE_DIR}/include/json++
)

add_executable( encode
	${json++_SOURCE_DIR}/src/encode.cpp
)

add_executable( decode
	${json++_SOURCE_DIR}/src/decode.cpp
)


target_link_libraries( test
)

install( DIRECTORY ${json++_SOURCE_DIR}/include/ DESTINATION include )
