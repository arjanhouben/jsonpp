#pragma once

#include <string>

#include <jsonpp/misc.h>

namespace json
{
	template < template< class > class CopyBehaviour, class T >
	struct basic_var;

	template < template< class > class CopyBehaviour, class T >
	struct basic_var_data
	{
		typedef std::basic_string< T > string_type;

		typedef key_value< string_type, basic_var< CopyBehaviour, T > > value_type;

		typedef std::vector< value_type > array_type;

		basic_var_data() :
			_string(),
			_number( std::numeric_limits< long double >::quiet_NaN() ),
			_array() { }

		basic_var_data( const string_type &s, long double n ) :
			_string( s ),
			_number( n ),
			_array() { }

		basic_var_data( const string_type &s ) :
			_string( s ),
			_number( std::numeric_limits< long double >::quiet_NaN() ),
			_array() { }

		basic_var_data( long double n ) :
			_string(),
			_number( n ),
			_array() { }

		string_type _string;
		long double _number;
		array_type _array;
	};
}
