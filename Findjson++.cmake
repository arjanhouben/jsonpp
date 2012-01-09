# This module defines
#  json++_INCLUDE_DIR, where to find png.h, etc.
#  json++_FOUND, If false, do not try to use json++.

find_path( json++_INCLUDE_DIR json++ )

if( json++_INCLUDE_DIR )

	set( json++_FOUND TRUE )

endif()
