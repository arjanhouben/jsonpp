#pragma once

#include <jsonpp/misc.h>
#include <jsonpp/basic_var_data.h>

namespace json
{
	template < class JSON, class T >
	typename JSON::string_type register_string( const T& )
	{
		return typename JSON::string_type();
	}

	template < class JSON, class T >
	typename JSON::number_type register_number( const T& )
	{
		return typename JSON::number_type( std::numeric_limits< typename JSON::number_type >::quiet_NaN() );
	}
}
