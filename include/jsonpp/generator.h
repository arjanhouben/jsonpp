#pragma once

#include <cstdlib>

#include <jsonpp/var.h>

namespace json
{
	std::string generateString( unsigned int stringlength )
	{
		stringlength = 1 + ( rand() % stringlength );

		std::string result;
		result.reserve( stringlength );

		while ( stringlength-- )
		{
			result.push_back( char( ( rand() % 94 ) + 32 ) );
		}

		return result;
	}

	var generate( unsigned int treeDepth, unsigned int stringLength, unsigned iterations )
	{
		var v;

		switch( rand() & 3 )
		{
			case 0: // array
				if ( treeDepth )
				{
					for ( unsigned int i = 0; i < iterations; ++i )
					{
						v.push( generate( treeDepth - 1, stringLength, iterations ) );
					}
					break;
				}
			case 1: // integer
				v = rand();
				break;
			case 2: // object
				if ( treeDepth )
				{
					for ( unsigned int i = 0; i < iterations; ++i )
					{
						v[ generateString( stringLength ) ] = generate( treeDepth - 1, stringLength, iterations );
					}
					break;
				}
			case 3: // string
				v = generateString( stringLength );
				break;
		}

		return v;
	}
}
