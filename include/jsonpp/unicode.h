#pragma once

#include <string>
#include <sstream>

namespace json
{
	enum Bitmasks
	{
		UpperBit = 0x80,
		Upper2Bits = 0xC0,
		Upper3Bits = 0xE0,
		Upper4Bits = 0xF0,
		Upper5Bits = 0xF8,
		Upper6Bits = 0xFC,
		LowerSixBits = 0x3F
	};

	template < class T >
	inline std::basic_string< T > utf8Encode( int unicode )
	{
		std::basic_string< T > output;
		output.reserve( 6 );

		// 0xxxxxxx
		if ( unicode < 0x80 )
		{
			output.push_back ( static_cast< char >( unicode ) );
		}
		// 110xxxxx 10xxxxxx
		else if ( unicode < 0x800 )
		{
			output.push_back ( static_cast< char >( Upper2Bits | ( unicode >> 6 ) ) );
			output.push_back ( static_cast< char >( UpperBit | ( unicode & LowerSixBits ) ) );
		}
		// 1110xxxx 10xxxxxx 10xxxxxx
		else if ( unicode < 0x10000 )
		{
			output.push_back ( static_cast< char >( Upper3Bits | ( unicode >> 12 ) ) );
			output.push_back ( static_cast< char >( UpperBit | ( ( unicode >> 6 ) & LowerSixBits ) ) );
			output.push_back ( static_cast< char >( UpperBit | ( unicode & LowerSixBits ) ) );
		}
		// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		else if ( unicode < 0x200000 )
		{
			output.push_back( static_cast< char >( Upper4Bits | ( unicode >> 18 ) ) );
			output.push_back( static_cast< char >( UpperBit | ( ( unicode >> 12 ) & LowerSixBits ) ) );
			output.push_back( static_cast< char >( UpperBit | ( ( unicode >> 6 ) & LowerSixBits ) ) );
			output.push_back( static_cast< char >( UpperBit | ( unicode & LowerSixBits ) ) );
		}
		// 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		else if ( unicode < 0x4000000 )
		{
			output.push_back( static_cast< char >( Upper5Bits | ( unicode >> 24 ) ) );
			output.push_back( static_cast< char >( UpperBit | ( ( unicode >> 18 ) & LowerSixBits ) ) );
			output.push_back( static_cast< char >( UpperBit | ( ( unicode >> 12 ) & LowerSixBits ) ) );
			output.push_back( static_cast< char >( UpperBit | ( ( unicode >> 6 ) & LowerSixBits ) ) );
			output.push_back( static_cast< char >( UpperBit | ( unicode & LowerSixBits ) ) );
		}
		// 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		else if ( unicode < 0x8000000 )
		{
			output.push_back( static_cast< char >( Upper6Bits | ( unicode >> 30 ) ) );
			output.push_back( static_cast< char >( UpperBit | ( ( unicode >> 18 ) & LowerSixBits ) ) );
			output.push_back( static_cast< char >( UpperBit | ( ( unicode >> 12 ) & LowerSixBits ) ) );
			output.push_back( static_cast< char >( UpperBit | ( ( unicode >> 6 ) & LowerSixBits ) ) );
			output.push_back( static_cast< char >( UpperBit | ( unicode & LowerSixBits ) ) );
		}

		return output;
	}

	template < class T >
	inline std::basic_string< T > utf8Decode( const std::basic_string< T > &string )
	{
		if ( string.empty() ) return string;

		std::stringstream stream;

		typename std::basic_string< T >::const_iterator input = string.begin();
		const typename std::basic_string< T >::const_iterator &end = string.end();

		size_t diff = 0;

		while ( ( diff = end - input ) )
		{
			int unicode( static_cast< const int >( *input ) );

			// 0xxxxxxx
			if( unicode < UpperBit )
			{
				switch ( *input )
				{
					case '"':
						stream << '\\' << '"';
						break;
					case '\\':
						stream << '\\' << '\\';
						break;
					case '/':
						stream << '\\' << '/';
						break;
					case '\b':
						stream << '\\' << 'b';
						break;
					case '\f':
						stream << '\\' << 'f';
						break;
					case '\n':
						stream << '\\' << 'n';
						break;
					case '\r':
						stream << '\\' << 'r';
						break;
					case '\t':
						stream << '\\' << 't';
						break;
					default:
						stream << *input;
						break;
				}

				++input;

				continue;
			}
			// 110xxxxx 10xxxxxx
			else if ( unicode < Upper2Bits )
			{
				if ( diff < 2 ) break;
				unicode = ( ( *input & 0x1F ) << 6 ) | ( *( input + 1 ) & LowerSixBits );
				input += 2;
			}
			// 1110xxxx 10xxxxxx 10xxxxxx
			else if ( unicode < Upper3Bits )
			{
				if ( diff < 3 ) break;
				unicode = ( ( *input & 0x0F ) << 12 ) | ( ( *( input + 1 ) & LowerSixBits ) << 6 )
						| ( *( input + 2 ) & LowerSixBits );
				input += 3;
			}
			// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
			else if ( unicode < Upper4Bits )
			{
				if ( diff < 4 ) break;
				unicode = ( ( *input & 0x07 ) << 18 ) | ( ( *( input + 1 ) & LowerSixBits ) << 12 )
					   | ( ( *( input + 2 ) & LowerSixBits ) << 6 ) | ( *( input + 3 ) & LowerSixBits );
				input += 4;
			}
			// 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
			else if ( unicode < Upper5Bits )
			{
				if ( diff < 5 ) break;
				unicode = ( ( *input & 0x03 ) << 24 ) | ( ( *( input + 1 ) & LowerSixBits ) << 18 )
					   | ( ( *( input + 2 ) & LowerSixBits ) << 12 ) | ( ( *( input + 3 ) & LowerSixBits ) << 6 )
					   | ( *( input + 4 ) & LowerSixBits );
			   input += 5;
			}
			// 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
			else if ( unicode < Upper6Bits )
			{
				if ( diff < 6 ) break;
				unicode = ( ( *input & 0x01 ) << 30 ) | ( ( *( input + 1 ) & LowerSixBits ) << 24 )
					| ( ( *( input + 2 ) & LowerSixBits ) << 18 ) | ( ( *( input + 3 ) & LowerSixBits ) << 12 )
					| ( ( *( input + 4 ) & LowerSixBits ) << 6 ) | ( *( input + 5 ) & LowerSixBits );
				input += 6;
			}

			stream << std::hex << "\\u" << unicode;
		}

		return stream.str();
	}
}
