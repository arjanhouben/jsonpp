#pragma once

#include <jsonpp/misc.h>

namespace json
{
	template < class JSON, class T >
	typename JSON::number_type register_number( const T& )
	{
		return typename JSON::number_type( std::numeric_limits< typename JSON::number_type >::quiet_NaN() );
	}

	template < class JSON >
	typename JSON::number_type register_number( int number )
	{
		return typename JSON::number_type( number );
	}
}
