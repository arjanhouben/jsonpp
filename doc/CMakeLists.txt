find_package( Doxygen )

if( DOXYGEN_FOUND )

	configure_file( ${json++_SOURCE_DIR}/doc/doxygen.pre-config doxygen.config )

	add_custom_target( doc ${DOXYGEN_EXECUTABLE} doxygen.config )

endif()
