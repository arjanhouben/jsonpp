#pragma once

#include <cstdlib>

#include <jsonpp/var.h>

namespace json
{
	std::string generateString( unsigned int stringlength )
	{
		stringlength = rand() % stringlength;

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

		if ( treeDepth )
		{
			if ( rand() & 1 )
			{
				// array
				for ( unsigned int i = 0; i < iterations; ++i )
				{
					v.push( generate( treeDepth - 1, stringLength, iterations ) );
				}
			}
			else
			{
				// object
				for ( unsigned int i = 0; i < iterations; ++i )
				{
					v[ generateString( stringLength ) ] = generate( treeDepth - 1, stringLength, iterations );
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
					v = generateString( stringLength );
					break;
			}
		}

		return v;
	}
}
