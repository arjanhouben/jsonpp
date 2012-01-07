#pragma once

#include <jsonpp/misc.h>

namespace json
{
	template < class JSON, class T >
	typename JSON::string_type register_string( const T& )
	{
		return typename JSON::string_type();
	}

	template < class JSON >
	typename JSON::string_type register_string( const char *string )
	{
		return typename JSON::string_type( string );
	}
}
