#pragma once

#include <jsonpp/var.h>

namespace json
{
	template < class Char >
	class basic_base64
	{
		public:

			typedef std::basic_string< Char > string_type;

			typedef Char character_type;

			typedef std::vector< char > data_type;

			template < class T >
			static T decode( const string_type &string )
			{
				std::vector< char > result( decode_raw( string ) );

				if ( result.size() != sizeof( T ) ) throw exception( "size mismatch" );

				return *reinterpret_cast< T* >( &result.front() );
			}

			static data_type decode( const string_type &string )
			{
				return decode_raw( string );
			}

			template < class POD >
			static string_type encode( const POD &pod )
			{
				return encode( reinterpret_cast< const char* >( &pod ), sizeof( pod ) );
			}

			static string_type encode( const char *start, size_t count )
			{
				string_type result = block_encode( start, count );

				count %= 3;

				if ( count )
				{
					char buf[] = { 0, 0, 0 };

					for ( unsigned int i = 0; i < count; ++i )
					{
						buf[ i ] = *start;
						++start;
					}

					start = buf;
					result.append( block_encode( start, 3 ) );

					count = 3 - count;
					while ( count )
					{
						result[ result.size() - count ] = '=';
						--count;
					}
				}

				return result;
			}

		private:

			static std::vector< char > decode_raw( const string_type &string )
			{
				typename string_type::const_iterator start = string.begin();
				const typename string_type::const_iterator end = string.end();

				std::vector< char > result = block_decode( start, end );

				size_t count = ( end - start ) % 4;

				if ( count )
				{
					unsigned char buf[] = { 0, 0, 0, 0 };

					for ( unsigned int i = 0; i < count; ++i )
					{
						buf[ i ] = *start;
						++start;
					}

					string_type temp( buf, buf + 4 );
					typename string_type::const_iterator s = temp.begin(), e = temp.end();
					std::vector< char > extra( block_decode( s, e ) );

					std::vector< char >::const_iterator c = extra.begin();

					while ( count-- )
					{
						result.push_back( *c++ );
					}
				}

				return result;
			}

			static string_type block_encode( const char *&start, size_t count )
			{
				string_type result( count * 4 / 3, 0 );

				typename string_type::iterator output = result.begin();

				unsigned char buf = 0;
				while ( count > 2 )
				{
					buf = ( *start >> 2 ) & 0x3F;
					// 0 0 1 2 3 4 5 6
					*output++ = b64e( buf );

					buf = ( *start & 0x03 ) << 4;
					// 0 0 7 8 _ _ _ _
					++start;
					buf |= ( *start >> 4 ) & 0x0F;
					// 0 0 7 8 1 2 3 4
					*output++ = b64e( buf );

					buf = ( *start & 0x0F ) << 2;
					// 0 0 5 6 7 8 _ _
					++start;
					buf |= ( *start >> 6 ) & 0x03;
					// 0 0 5 6 7 8 1 2
					*output++ = b64e( buf );

					buf = *start & 0x3F;
					// 0 0 3 4 5 6 7 8
					*output++ = b64e( buf );
					++start;

					count -= 3;
				}

				result.erase( output, result.end() );

				return result;
			}

			static std::vector< char > block_decode( typename string_type::const_iterator &start, const typename string_type::const_iterator &end )
			{
				std::vector< char > result( ( end - start ) * 3 / 4, 0 );
				std::vector< char >::iterator output = result.begin();

				unsigned char buf = 0;
				while ( end - start > 3 )
				{
					unsigned char i = b64d( *start );
					buf = i << 2;
					i = b64d( *++start );
					if ( *start == '=' )
					{
						start += 3;
						break;
					}
					buf |= i >> 4;
					*output++ = buf;

					buf = i << 4;
					i = b64d( *++start );
					if ( *start == '=' )
					{
						start += 2;
						break;
					}
					buf |= i >> 2;
					*output++ = buf;

					buf = ( i << 6 ) & 0xC0;
					i = b64d( *++start );
					if ( *start == '=' )
					{
						start += 1;
						break;
					}
					buf |= i;
					*output++ = buf;

					++start;
				}

				result.erase( output, result.end() );

				return result;
			}

			inline static char b64e( unsigned char c )
			{
				static const char *chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
				return chars[ c ];
			}

			inline static char b64d( unsigned char c )
			{
				if ( c >= 'A' && c <= 'Z' )
				{
					return c - 'A';
				}

				if ( c >= 'a' && c <= 'z' )
				{
					return ( c - 'a' ) + 26;
				}

				if ( c >= '0' && c <= '9' )
				{
					return ( c - '0' ) + 52;
				}

				switch ( c )
				{
					case '+':
						return 62;
					case '/':
						return 63;
				}

				return 0;
			}
	};

	typedef basic_base64< char > base64;
	typedef basic_base64< wchar_t > wbase64;
}
