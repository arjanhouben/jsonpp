#pragma once

#include <cstdlib>

#include <jsonpp/var.h>

namespace json
{
	template < class T >
	std::basic_string< T > generateString( unsigned int stringlength )
	{
		stringlength = rand() % stringlength;

		std::basic_string< T > result;
		result.reserve( stringlength );

		while ( stringlength-- )
		{
			result.push_back( char( ( rand() % 94 ) + 32 ) );
		}

		return result;
	}

	template < template< class > class CopyBehaviour, class T >
	basic_var< CopyBehaviour, T > generate( unsigned int treeDepth, unsigned int stringLength, unsigned iterations )
	{
		basic_var< CopyBehaviour, T > v;

		if ( treeDepth )
		{
			if ( rand() & 1 )
			{
				// array
				for ( unsigned int i = 0; i < iterations; ++i )
				{
					v.push( generate< CopyBehaviour, T >( treeDepth - 1, stringLength, iterations ) );
				}
			}
			else
			{
				// object
				for ( unsigned int i = 0; i < iterations; ++i )
				{
					v[ generateString< T >( stringLength ) ] = generate< CopyBehaviour, T >( treeDepth - 1, stringLength, iterations );
				}
			}
		}
		else
		{
			switch ( rand() % 5 )
			{
				case 0: // Undefined
					v = Undefined;
					break;
				case 1: // Null
					v = Null;
					break;
				case 2: // Bool
					v = bool( rand() & 1 );
					break;
				case 3: // Number
					v = rand();
					break;
				case 4: // String
					v = generateString< T >( stringLength );
					break;
			}
		}

		return v;
	}
}
