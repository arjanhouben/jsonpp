#pragma once

#include <jsonpp/misc.h>

namespace json
{
	template < class T >
	Types register_type( T )
	{
		return Undefined;
	}

	template <>
	Types register_type( int )
	{
		return Number;
	}

	template <>
	Types register_type( const char* )
	{
		return String;
	}
}
