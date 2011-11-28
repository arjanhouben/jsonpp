#ifndef JSONPP_VALUE_H
#define JSONPP_VALUE_H

#include <vector>
#include <numeric>
#include <cmath>
#include <sstream>
#include <limits>
#include <iostream>
#include <algorithm>
#include <set>

#include <jsonpp/unicode.h>
#include <jsonpp/string.h>

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

	template< class Key, class var >
	struct KeyValue
	{
		KeyValue() : key(), value() { }

		KeyValue( const var &v ) : key(), value( v ) { }

		KeyValue( const Key &k, const var &v ) : key( k ), value( v ) { }

		KeyValue( const Key &k ) : key( k ), value() { }
		bool operator == ( const KeyValue< Key, var > &rhs ) const
		{
			return key == rhs.key && value == rhs.value;
		}

		bool operator != ( const KeyValue< Key, var > &rhs ) const
		{
			return key != rhs.key || value != rhs.value;
		}

		Key key;
		var value;

		struct findKey
		{
			findKey( const string &k ) : key ( k ) { }

			bool operator()( const KeyValue< Key, var > &keyValue ) const
			{
				return keyValue.key == key;
			}

			const string key;
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

	struct var
	{
		typedef KeyValue< string, var > value_type;

		typedef std::vector< value_type > array_type;

		typedef array_type::iterator iterator;

		typedef array_type::const_iterator const_iterator;

		const Types type;

			var() :
				type( Undefined ),
				_string(),
				_number( std::numeric_limits< long double >::quiet_NaN() ),
				_array() { }

			var( Types type ) :
				type( type ),
				_string(),
				_number( std::numeric_limits< long double >::quiet_NaN() ),
				_array() { }

			var( const string &string ) :
				type( String ),
				_string( string ),
				_number( std::numeric_limits< long double >::quiet_NaN() ),
				_array() { }

			var( const char *string ) :
				type( String ),
				_string( string ),
				_number( std::numeric_limits< long double >::quiet_NaN() ),
				_array() { }

			var( char character ) :
				type( String ),
				_string( 1, character ),
				_number( std::numeric_limits< long double >::quiet_NaN() ),
				_array() { }

			var( unsigned char character ) :
				type( String ),
				_string( 1, character ),
				_number( std::numeric_limits< long double >::quiet_NaN() ),
				_array() { }

			var( bool boolean ) :
				type( Bool ),
				_string(),
				_number( boolean ),
				_array() { }

			var( short number ) :
				type( Number ),
				_string(),
				_number( number ),
				_array() { }

			var( unsigned short number ) :
				type( Number ),
				_string(),
				_number( number ),
				_array() { }

			var( int number ) :
				type( Number ),
				_string(),
				_number( number ),
				_array() { }

			var( unsigned int number ) :
				type( Number ),
				_string(),
				_number( number ),
				_array() { }

			var( long number ) :
				type( Number ),
				_string(),
				_number( number ),
				_array() { }

			var( unsigned long number ) :
				type( Number ),
				_string(),
				_number( number ),
				_array() { }

			var( long long number ) :
				type( Number ),
				_string(),
				_number( static_cast< long double >( number ) ),
				_array() { }

			var( unsigned long long number ) :
				type( Number ),
				_string(),
				_number( static_cast< long double >( number ) ),
				_array() { }

			var( float number ) :
				type( Number ),
				_string(),
				_number( number ),
				_array() { }

			var( double number ) :
				type( Number ),
				_string(),
				_number( number ),
				_array() { }

			var( long double number ) :
				type( Number ),
				_string(),
				_number( number ),
				_array() { }

			var( const var &rhs ) :
				type( rhs.type ),
				_string( rhs._string ),
				_number( rhs._number ),
				_array( rhs._array ) { }

			var& operator = ( const var &rhs )
			{
				if ( this != &rhs )
				{
					const_cast< Types& >( type ) = rhs.type;
					_string = rhs._string;
					_number = rhs._number;
					_array = rhs._array;
				}
				return *this;
			}

			operator bool() const
			{
				switch ( type )
				{
					case Null:
					case Undefined:
						return false;
					case Array:
					case Object:
						return true;
					case String:
						return !_string.empty();
					case Number:
						break;
					case Bool:
						break;
				}
				return _number && ( !isNaN( _number ) );
			}

			operator string() const
			{
				switch ( type )
				{
					case Array:
						return "Array";
					case Object:
						return "Object";
					case String:
						return _string;
					case Number:
					{
						std::stringstream stream;
						long double temp = 0;
						if ( std::modf( _number, &temp ) == 0.0 )
						{
							stream.precision( 30 );
							stream << _number;
						}
						else
						{
							stream << _number;
						}
						return stream.str();
					}
					case Bool:
						return ( operator bool() ? "true" : "false" );
					case Null:
						return "null";
					case Undefined:
					default:
						return "";
				}
			}

			operator long double() const
			{
				if ( isNaN( _number ) )
				{
					std::stringstream stream( _string );
					long double result;
					stream >> result;
					return result;
				}
				return _number;
			}

			operator short() const { return static_cast< short >( operator long double() ); }

			operator unsigned short() const { return static_cast< unsigned short >( operator long double() ); }

			operator int() const { return static_cast< int >( operator long double() ); }

			operator unsigned int() const { return static_cast< unsigned int >( operator long double() ); }

			operator long() const { return static_cast< long >( operator long double() ); }

			operator unsigned long() const { return static_cast< unsigned long >( operator long double() ); }

			operator long long() const { return static_cast< long long >( operator long double() ); }

			operator unsigned long long() const { return static_cast< unsigned long long >( operator long double() ); }

			operator float() const { return static_cast< float >( operator long double() ); }

			operator double() const { return static_cast< double >( operator long double() ); }

			string toString() const { return operator string(); }

			long double toNumber() const { return operator long double(); }

			var& operator[]( const var &key )
			{
				switch ( type )
				{
					case Number:
						return operator[]( key._number );
					default:
						return operator[]( key.operator string() );
				}
			}

			var& operator[]( const char key[] ) { return operator []( string( key ) ); }

			var& operator[]( const string &key )
			{
				if ( type != Object )
				{
					const_cast< Types& >( type ) = Object;
					_array.clear();
				}
				iterator i = std::find_if( _array.begin(), _array.end(), value_type::findKey( key ) );
				if ( i == _array.end() )
				{
					_array.push_back( array_type::value_type( string( key ), Undefined ) );

					return _array.back().value;
				}
				return i->value;
			}

			const var& operator[]( const char key[] ) const { return operator []( string( key ) ); }

			const var& operator[]( const string &key ) const
			{
				const_iterator i = std::find_if( _array.begin(), _array.end(), value_type::findKey( key ) );
				if ( i == _array.end() )
				{
					static var undefined( Undefined );
					return undefined;
				}
				return i->value;
			}

			var& operator[]( char index ) { return operator []( string( 1, index ) ); }

			var& operator[]( unsigned char index ) { return operator []( string( 1, index ) ); }

			var& operator[]( short index ) { return operator []( static_cast< unsigned int >( index ) ); }

			var& operator[]( unsigned short index ) { return operator []( static_cast< unsigned int >( index ) ); }

			var& operator[]( int index ) { return operator []( static_cast< unsigned int >( index ) ); }

			var& operator[]( unsigned int index )
			{
				if ( type != Array )
				{
					const_cast< Types& >( type ) = Array;
					_array.clear();
				}
				if ( index >= _array.size() ) _array.resize( index + 1 );
				return _array[ index ].value;
			}

			var& operator[]( float index ) { return operator []( static_cast< unsigned int >( index ) ); }

			var& operator[]( double index ) { return operator []( static_cast< unsigned int >( index ) ); }

			var& operator[]( long double index ) { return operator []( static_cast< unsigned int >( index ) ); }

			var& operator[]( long index ) { return operator []( static_cast< unsigned int >( index ) ); }

			var& operator[]( unsigned long index ) { return operator []( static_cast< unsigned int >( index ) ); }


			const var& operator[]( char index ) const { return operator []( string( index ) ); }

			const var& operator[]( unsigned char index ) const { return operator []( string( index ) ); }

			const var& operator[]( short index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const var& operator[]( unsigned short index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const var& operator[]( int index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const var& operator[]( unsigned int index ) const
			{
				if ( index >= _array.size() )
				{
					static var undefined( Undefined );
					return undefined;
				}
				return _array[ index ].value;
			}

			const var& operator[]( float index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const var& operator[]( double index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const var& operator[]( long double index ) const { return operator []( static_cast< unsigned int >( index ) ); }


			bool operator == ( const var &rhs ) const
			{
				if ( type != rhs.type ) return false;

				if ( isNaN( _number ) != isNaN( rhs._number ) ) return false;

				if ( !isNaN( _number ) )
				{
					if ( _number != rhs._number ) return false;
				}

				if ( _string != rhs._string ) return false;

				if ( _array != rhs._array ) return false;

				return true;
			}

			bool operator == ( Types rhs ) const
			{
				return operator == ( json::var( rhs ) );
			}

			bool operator != ( const var &rhs ) const
			{
				return !operator == ( rhs );
			}

			bool operator != ( Types rhs ) const
			{
				return !operator == ( rhs );
			}

			var& operator += ( const var &rhs )
			{
				if ( type == Number && rhs.type == Number )
				{
					_number += rhs.toNumber();
				}
				else
				{
					*this = toString() + rhs.toString();
				}

				return *this;
			}

			var& splice( unsigned int index, unsigned int remove )
			{
				if ( remove )
				{
					if ( index < _array.size() && remove < _array.size() - index )
					{
						_array.erase( _array.begin() + index, _array.begin() + index + remove );
					}
				}

				return *this;
			}

			var splice( unsigned int index, unsigned int remove, const var &item )
			{
				var removed = Array;

				if ( remove )
				{
					for ( unsigned int i = 0; i < remove; ++i )
					{
						removed[ i ] = item;
					}

					splice( index, remove );
				}

				if ( index < _array.size() )
				{
					_array.insert( _array.begin() + index, item );
				}

				if ( remove )
				{
					return removed;
				}
				else
				{
					return *this;
				}
			}

			void push( const var &value )
			{
				if ( type != Array )
				{
					const_cast< Types& >( type ) = Array;
					_array.clear();
				}
				_array.push_back( value );
			}

			void clear()
			{
				const_cast< Types& >( type ) = Undefined;
				_string.clear();
				_array.clear();
				_number = std::numeric_limits< long double >::quiet_NaN();
			}

			void merge( const var &rhs )
			{
				switch ( type )
				{
					case Null:
					case Undefined:
					case Number:
					case Bool:
					case String:
						operator = ( rhs );
						break;
					case Object:
						for ( const_iterator i = rhs._array.begin(); i != rhs._array.end(); ++i )
						{
							operator []( i->key ).merge( i->value );
						}
						break;
					case Array:
						int c = 0;
						for ( const_iterator i = rhs._array.begin(); i != rhs._array.end(); ++i )
						{
							operator []( c++ ).merge( i->value );
						}
						break;
				}
			}

			std::string serialize( unsigned int markup = Compact, unsigned int level = 0 ) const
			{
				const std::string tabs( level, '\t' );

				switch ( type )
				{
					case Null:
					case Undefined:
						if ( markup & HumanReadable && markup & IndentFirstItem ) return tabs + "null";
						return "null";
					case Number:
					case Bool:
						if ( markup & HumanReadable && markup & IndentFirstItem ) return tabs + toString();
						return toString();
					case String:
					{
						const string tak = "\"" + utf8Decode( _string ) + "\"";
						if ( markup & HumanReadable && markup & IndentFirstItem ) return tabs + tak;
						return tak;
					}
					case Object:
					case Array:
						break;
				}

				std::stringstream result;

				if ( markup & HumanReadable && markup & IndentFirstItem ) result << tabs;

				if ( type == Array )
				{
					result << '[';

					for ( const_iterator i = _array.begin(); i != _array.end(); ++i )
					{
						if ( i != _array.begin() ) result << ',';

						if ( markup & HumanReadable )
						{
							result << "\n\t" << tabs;

							if ( markup & CountArrayValues )
							{
								result << i - _array.begin() << " => ";
							}
						}

						result << i->value.serialize( markup & ~IndentFirstItem, level + 1 );
					}

					if ( markup & HumanReadable ) result << "\n" + tabs;

					result << ']';
				}
				else
				{
					result << '{';

					for ( const_iterator i = _array.begin(); i != _array.end(); ++i )
					{
						if ( i != _array.begin() ) result << ',';

						if ( markup & HumanReadable ) result << "\n\t" + tabs;

						result << "\"" + utf8Decode( i->key ) + "\":";

						result << i->value.serialize( markup & ~IndentFirstItem, level + 1 );
					}

					if ( markup & HumanReadable ) result << "\n" + tabs;

					result << '}';
				}

				return result.str();
			}

			var& front()
			{
				if ( _array.empty() ) return *this;
				return _array.front().value;
			}

			var front() const
			{
				if ( _array.empty() ) return var();
				return _array.front().value;
			}

			var& back()
			{
				if ( _array.empty() ) return *this;
				return _array.back().value;
			}

			var back() const
			{
				if ( _array.empty() ) return var();
				return _array.back().value;
			}

			size_t size() const { return _array.size(); }

			iterator begin() { return _array.begin(); }

			const_iterator begin() const { return _array.begin(); }

			iterator end() { return _array.end(); }

			const_iterator end() const { return _array.end(); }

		private:

			string _string;
			long double _number;
			array_type _array;
	};
}

#endif // JSONPP_VALUE_H
