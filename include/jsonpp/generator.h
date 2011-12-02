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

		for ( unsigned int i = 0; i < iterations; ++i )
		{
			if ( treeDepth )
			{
				switch ( rand() & 1 )
				{
					case 0:
						if ( v != json::Undefined ) continue;
						v = rand();
						break;
					case 1:
					{
						v[ generateString( stringLength ) ] = generate( treeDepth - 1, stringLength, iterations );
						break;
					}
				}
			}
			else
			{
				if ( v != json::Undefined ) continue;
				v = rand();
			}
		}

		return v;
	}
}
