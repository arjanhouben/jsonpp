#pragma once

#include <jsonpp/misc.h>

namespace json
{
	template < bool, class T = void > struct enable_if {};

	template < class T > struct enable_if< true, T > { typedef T type; };

	template < class JSON, class T >
	struct register_type;

	template < class JSON, class T >
	struct register_type
	{
			typedef T return_type;

			static Types type( const T& )
			{
				if ( std::tr1::is_arithmetic< T >::value ) return Number;
				return String;
			}

			static typename JSON::basic_data to_json( const T &t )
			{
				return t;
			}
	};

	template < class JSON >
	struct register_type< JSON, bool >
	{
			static Types type( const bool& ) { return Bool; }

			static typename JSON::basic_data to_json( bool boolean )
			{
				return boolean;
			}
	};

	template < class JSON >
	struct register_type< JSON, char >
	{
			static Types type( const char& ) { return String; }

			static typename JSON::string_type to_json( char string )
			{
				return typename JSON::string_type( 1, string );
			}
	};

	template < class JSON, int T >
	struct register_type< JSON, typename JSON::character_type[T] >
	{
			static Types type( const typename JSON::character_type[T] ) { return String; }

			static typename JSON::basic_data to_json( const typename JSON::character_type string[ T ] )
			{
				return std::basic_string< typename JSON::character_type >( string );
			}
	};

	template < class JSON >
	struct register_type< JSON, Buffer< typename JSON::character_type > >
	{
			static Types type( const Buffer< typename JSON::character_type >& ) { return String; }

			static typename JSON::basic_data to_json( const Buffer< typename JSON::character_type > &string )
			{
				return static_cast< std::basic_string< typename JSON::character_type > >( string );
			}

			static Buffer< typename JSON::character_type > from_json( const JSON &json )
			{
				std::basic_string< typename JSON::character_type > str( json.toString() );

				return Buffer< typename JSON::character_type >( str.begin(), str.end() );
			}
	};
}
