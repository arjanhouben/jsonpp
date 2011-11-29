#pragma once

#include <iostream>

namespace json
{
	enum Types
	{
		Undefined = 0,
		Null,
		Bool,
		Number,
		String,
		Array,
		Object
	};

	enum Markup
	{
		Compact = 1 << 0,
		HumanReadable = 1 << 1,
		CountArrayValues = 1 << 2,
		IndentFirstItem = 1 << 3
	};

	template< class Key, class Value >
	struct key_value
	{
		key_value() : key(), value() { }

		key_value( const Value &v ) : key(), value( v ) { }

		key_value( const Key &k, const Value &v ) : key( k ), value( v ) { }

		key_value( const Key &k ) : key( k ), value() { }

		bool operator == ( const key_value< Key, Value > &rhs ) const
		{
			return key == rhs.key && value == rhs.value;
		}

		bool operator != ( const key_value< Key, Value > &rhs ) const
		{
			return key != rhs.key || value != rhs.value;
		}

		Key key;
		Value value;

		struct findKey
		{
			findKey( const std::string *k ) : key ( k ) { }

			bool operator()( const key_value< Key, Value > &keyValue ) const
			{
				return keyValue.key == *key;
			}

			const std::string *key;
		};
	};

	struct Debug
	{
		template < class T > Debug& operator << ( T t ) { std::cerr << t << ' '; return *this; }

		Debug& operator << ( const Types &t )
		{
			switch ( t )
			{
				case Undefined:
					return operator << ( "Undefined" );
				case Null:
					return operator << ( "Null" );
				case Bool:
					return operator << ( "Bool" );
				case Number:
					return operator << ( "Number" );
				case String:
					return operator << ( "String" );
				case Array:
					return operator << ( "Array" );
				case Object:
					return operator << ( "Object" );
				default:
					return operator <<  ( "unknown type" ) << static_cast< int >( t );
			}
		}
		~Debug() { std::cerr << std::endl; }
	};

	inline bool isNaN( long double t )
	{
		return t != t;
	}
}
