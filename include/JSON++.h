#ifndef JSONPP_H
#define JSONPP_H

#include <vector>
#include <numeric>
#include <cmath>
#include <sstream>
#include <limits>
#include <iostream>
#include <algorithm>

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

	enum Markup
	{
		Compact = 1 << 0,
		HumanReadable = 1 << 1,
		CountArrayValues = 1 << 2,
		IndentFirstItem = 1 << 2
	};

	template< class Key, class Value >
	struct KeyValue
	{
		KeyValue() : key(), value() { }

		KeyValue( const Value &v ) : key(), value( v ) { }

		KeyValue( const Key &k ) : key( k ), value() { }
		bool operator == ( const KeyValue< Key, Value > &rhs ) const
		{
			return key == rhs.key && value == rhs.value;
		}

		bool operator != ( const KeyValue< Key, Value > &rhs ) const
		{
			return key != rhs.key || value != rhs.value;
		}

		Key key;
		Value value;

		struct findKey
		{
			findKey( const Key &k ) : key ( k ) { }

			bool operator()( const KeyValue< Key, Value > &keyValue ) const
			{
				return keyValue.key == key;
			}

			const Key key;
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
					return operator <<  ( "unknown type" ) << (int)t;
			}
		}
		~Debug() { std::cerr << std::endl; }
	};

	inline std::string utf8Encode( int unicode )
	{
		std::string output;
		output.reserve( 6 );

		// 0xxxxxxx
		if ( unicode < 0x80 )
		{
			output.push_back ( static_cast< char >( unicode ) );
		}
		// 110xxxxx 10xxxxxx
		else if ( unicode < 0x800 )
		{
			output.push_back ( static_cast< char >( Upper2Bits | unicode >> 6 ) );
			output.push_back ( static_cast< char >( UpperBit | unicode & LowerSixBits ) );
		}
		// 1110xxxx 10xxxxxx 10xxxxxx
		else if ( unicode < 0x10000 )
		{
			output.push_back ( static_cast< char >( Upper3Bits | unicode >> 12 ) );
			output.push_back ( static_cast< char >( UpperBit | unicode >> 6 & LowerSixBits ) );
			output.push_back ( static_cast< char >( UpperBit | unicode & LowerSixBits ) );
		}
		// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		else if ( unicode < 0x200000 )
		{
		   output.push_back( static_cast< char >( Upper4Bits | unicode >> 18 ) );
		   output.push_back( static_cast< char >( UpperBit | unicode >> 12 & LowerSixBits ) );
		   output.push_back( static_cast< char >( UpperBit | unicode >> 6 & LowerSixBits ) );
		   output.push_back( static_cast< char >( UpperBit | unicode & LowerSixBits ) );
		}
		// 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		else if ( unicode < 0x4000000 )
		{
		   output.push_back( static_cast< char >( Upper5Bits | unicode >> 24 ) );
		   output.push_back( static_cast< char >( UpperBit | unicode >> 18 & LowerSixBits ) );
		   output.push_back( static_cast< char >( UpperBit | unicode >> 12 & LowerSixBits ) );
		   output.push_back( static_cast< char >( UpperBit | unicode >> 6 & LowerSixBits ) );
		   output.push_back( static_cast< char >( UpperBit | unicode & LowerSixBits ) );
		}
		// 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		else if ( unicode < 0x8000000 )
		{
		   output.push_back( static_cast< char >( Upper6Bits | unicode >> 30 ) );
		   output.push_back( static_cast< char >( UpperBit | unicode >> 18 & LowerSixBits ) );
		   output.push_back( static_cast< char >( UpperBit | unicode >> 12 & LowerSixBits ) );
		   output.push_back( static_cast< char >( UpperBit | unicode >> 6 & LowerSixBits ) );
		   output.push_back( static_cast< char >( UpperBit | unicode & LowerSixBits ) );
		}

		return output;
	}

	inline std::string utf8Decode( const std::string &string )
	{
		if ( string.empty() ) return string;

		std::stringstream stream;

		std::string::const_iterator input = string.begin();
		const std::string::const_iterator end = string.end();

		while ( input != end )
		{
			// 0xxxxxxx
			if( static_cast< unsigned char >( *input ) < UpperBit )
			{
				switch ( *input )
				{
					case '"':
					case '\\':
					case '/':
					case '\b':
					case '\f':
					case '\n':
					case '\r':
					case '\t':
						stream << '\\';
						break;
					default:
						break;
				}

				stream << *input++;
			}
			else
			{
				int unicode = 0;

				// 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
				if ( ( *input & Upper6Bits ) == Upper6Bits )
				{
				   unicode = ( ( *input & 0x01 ) << 30 ) | ( ( *( input + 1 ) & LowerSixBits ) << 24 )
						| ( ( *( input + 2 ) & LowerSixBits ) << 18 ) | ( ( *( input + 3 )
								  & LowerSixBits ) << 12 )
						| ( ( *( input + 4 ) & LowerSixBits ) << 6 ) | ( *( input + 5 ) & LowerSixBits );
				   input += 6;
				}
				// 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
				else if ( ( *input & Upper5Bits ) == Upper5Bits )
				{
				   unicode = ( ( *input & 0x03 ) << 24 ) | ( ( *( input + 1 )
						  & LowerSixBits ) << 18 )
						| ( ( *( input + 2 ) & LowerSixBits ) << 12 ) | ( ( *( input + 3 )
							& LowerSixBits ) << 6 )
						| ( *( input + 4 ) & LowerSixBits );
				   input += 5;
				}
				// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
				else if ( ( *input & Upper4Bits ) == Upper4Bits )
				{
				   unicode = ( ( *input & 0x07 ) << 18 ) | ( ( *( input + 1 )
						  & LowerSixBits ) << 12 )
						| ( ( *( input + 2 ) & LowerSixBits ) << 6 ) | ( *( input + 3 ) & LowerSixBits );
				   input += 4;
				}
				// 1110xxxx 10xxxxxx 10xxxxxx
				else if ( ( *input & Upper3Bits ) == Upper3Bits )
				{
				   unicode = ( ( *input & 0x0F) << 12 ) | ( ( *( input + 1 ) & LowerSixBits ) << 6 )
						| ( *( input + 2 ) & LowerSixBits );
				   input += 3;
				}
				// 110xxxxx 10xxxxxx
				else if ( ( *input & Upper2Bits ) == Upper2Bits )
				{
				   unicode = ( ( *input & 0x1F) << 6 ) | ( *( input + 1 ) & LowerSixBits );
				   input += 2;
				}

				stream << std::hex << "\\u" << unicode;
			}
		}

		return stream.str();
	}

	struct Value
	{
		const Types type;

		Value() :
			type( Undefined ),
			_string(),
			_number( std::numeric_limits< long double >::quiet_NaN() ),
			_array() { }

		Value( Types type ) :
			type( type ),
			_string(),
			_number( std::numeric_limits< long double >::quiet_NaN() ),
			_array() { }

		Value( const std::string &string ) :
			type( String ),
			_string( string ),
			_number( std::numeric_limits< long double >::quiet_NaN() ),
			_array() { }

		Value( const char *string ) :
			type( String ),
			_string( string, string + strlen( string ) ),
			_number( std::numeric_limits< long double >::quiet_NaN() ),
			_array() { }

		Value( char character ) :
			type( String ),
			_string( 1, character ),
			_number( std::numeric_limits< long double >::quiet_NaN() ),
			_array() { }

		Value( unsigned char character ) :
			type( String ),
			_string( 1, character ),
			_number( std::numeric_limits< long double >::quiet_NaN() ),
			_array() { }

		Value( bool boolean ) :
			type( Bool ),
			_string(),
			_number( boolean ),
			_array() { }

		Value( short number ) :
			type( Number ),
			_string(),
			_number( number ),
			_array() { }

		Value( unsigned short number ) :
			type( Number ),
			_string(),
			_number( number ),
			_array() { }

		Value( int number ) :
			type( Number ),
			_string(),
			_number( number ),
			_array() { }

		Value( unsigned int number ) :
			type( Number ),
			_string(),
			_number( number ),
			_array() { }

		Value( long number ) :
			type( Number ),
			_string(),
			_number( number ),
			_array() { }

		Value( unsigned long number ) :
			type( Number ),
			_string(),
			_number( number ),
			_array() { }

		Value( long long number ) :
			type( Number ),
			_string(),
			_number( number ),
			_array() { }

		Value( unsigned long long number ) :
			type( Number ),
			_string(),
			_number( number ),
			_array() { }

		Value( float number ) :
			type( Number ),
			_string(),
			_number( number ),
			_array() { }

		Value( double number ) :
			type( Number ),
			_string(),
			_number( number ),
			_array() { }

		Value( long double number ) :
			type( Number ),
			_string(),
			_number( number ),
			_array() { }

		Value( const Value &rhs ) :
			type( rhs.type ),
			_string( rhs._string ),
			_number( rhs._number ),
			_array( rhs._array ) { }

		Value& operator = ( const Value &rhs )
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
				case Array:
					return false;
					return false;
				case Object:
					return true;
				case String:
					return !_string.empty();
				case Number:
					break;
				case Bool:
					break;
			}
			return _number && ( !std::isnan( _number ) );
		}

		operator std::string() const
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
			if ( std::isnan( _number ) )
			{
				std::stringstream stream( _string );
				long double result;
				stream >> result;
				return result;
			}
			return _number;
		}

		operator short() const { return operator long double(); }

		operator unsigned short() const { return operator long double(); }

		operator int() const { return operator long double(); }

		operator unsigned int() const { return operator long double(); }

		operator long() const { return operator long double(); }

		operator unsigned long() const { return operator long double(); }

		operator long long() const { return operator long double(); }

		operator unsigned long long() const { return operator long double(); }

		operator float() const { return operator long double(); }

		operator double() const { return operator long double(); }

		std::string toString() const { return operator std::string(); }

		long double toNumber() const { return operator long double(); }

		Value& operator[]( const Value &key )
		{
			switch ( type )
			{
				case Number:
					return operator[]( key._number );
				case Undefined:
				case Null:
				case Bool:
				case String:
				case Array:
				case Object:
					return operator[]( key.toString() );
			}
		}

		Value& operator[]( const char key[] ) { return operator []( std::string( key ) ); }

		Value& operator[]( const std::string &key )
		{
			if ( type != Object )
			{
				const_cast< Types& >( type ) = Object;
				_array.clear();
			}
			ArrayType::iterator i = std::find_if( _array.begin(), _array.end(), Data::findKey( key ) );
			if ( i == _array.end() )
			{
				_array.push_back( key );

				return _array.back().value;
			}
			return i->value;
		}

		Value operator[]( const char key[] ) const { return operator []( std::string( key ) ); }

		Value operator[]( const std::string &key ) const
		{
			ArrayType::const_iterator i = std::find_if( _array.begin(), _array.end(), Data::findKey( key ) );
			if ( i == _array.end() ) return Value();
			return i->value;
		}

		Value& operator[]( char index ) { return operator []( std::string( 1, index ) ); }

		Value& operator[]( unsigned char index ) { return operator []( std::string( 1, index ) ); }

		Value& operator[]( short index ) { return operator []( static_cast< size_t >( index ) ); }

		Value& operator[]( unsigned short index ) { return operator []( static_cast< size_t >( index ) ); }

		Value& operator[]( int index ) { return operator []( static_cast< size_t >( index ) ); }

		Value& operator[]( unsigned int index ) { return operator []( static_cast< size_t >( index ) ); }

		Value& operator[]( double index ) { return operator []( static_cast< size_t >( index ) ); }

		Value& operator[]( long double index ) { return operator []( static_cast< size_t >( index ) ); }

		Value& operator[]( size_t index )
		{
			if ( type != Array )
			{
				const_cast< Types& >( type ) = Array;
				_array.clear();
			}
			if ( index >= _array.size() ) _array.resize( index + 1 );
			return _array[ index ].value;
		}

		Value operator[]( char index ) const { return operator []( std::string( 1, index ) ); }

		Value operator[]( unsigned char index ) const { return operator []( std::string( 1, index ) ); }

		Value operator[]( short index ) const { return operator []( static_cast< size_t >( index ) ); }

		Value operator[]( unsigned short index ) const { return operator []( static_cast< size_t >( index ) ); }

		Value operator[]( int index ) const { return operator []( static_cast< size_t >( index ) ); }

		Value operator[]( unsigned int index ) const { return operator []( static_cast< size_t >( index ) ); }

		Value operator[]( double index ) const { return operator []( static_cast< size_t >( index ) ); }

		Value operator[]( long double index ) const { return operator []( static_cast< size_t >( index ) ); }

		Value operator[]( size_t index ) const
		{
			if ( index >= _array.size() ) return Value();
			return _array[ index ].value;
		}

		bool operator == ( const Value &rhs ) const
		{
			if ( type != rhs.type ) return false;

			if ( std::isnan( _number ) != std::isnan( rhs._number ) ) return false;

			if ( !std::isnan( _number ) )
			{
				if ( _number != rhs._number ) return false;
			}

			if ( _string != rhs._string ) return false;

			if ( _array != rhs._array ) return false;

			return true;
		}

		bool operator != ( const Value &rhs ) const
		{
			return !operator == ( rhs );
		}

		void push( const Value &value )
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

		void merge( const Value &rhs )
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
					for ( ArrayType::const_iterator i = rhs._array.begin(); i != rhs._array.end(); ++i )
					{
						operator []( i->key ).merge( i->value );
					}
					break;
				case Array:
					int c = 0;
					for ( ArrayType::const_iterator i = rhs._array.begin(); i != rhs._array.end(); ++i )
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
					const std::string tak = "\"" + utf8Decode( _string ) + "\"";
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
				result << "[";

				for ( ArrayType::const_iterator i = _array.begin(); i != _array.end(); ++i )
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

				result << "]";
			}
			else
			{
				result << "{";

				for ( ArrayType::const_iterator i = _array.begin(); i != _array.end(); ++i )
				{
					if ( i != _array.begin() ) result << ',';

					if ( markup & HumanReadable ) result << "\n\t" + tabs;

					result << "\"" + i->key + "\":";

					result << i->value.serialize( markup & ~IndentFirstItem, level + 1 );
				}

				if ( markup & HumanReadable ) result << "\n" + tabs;

				result << "}";
			}

			return result.str();
		}

		Value& front()
		{
			if ( _array.empty() ) return *this;
			return _array.front().value;
		}

		Value front() const
		{
			if ( _array.empty() ) return Value();
			return _array.front().value;
		}

		Value& back()
		{
			if ( _array.empty() ) return *this;
			return _array.back().value;
		}

		Value back() const
		{
			if ( _array.empty() ) return Value();
			return _array.back().value;
		}

		size_t size() const { return _array.size(); }

		private:

			typedef KeyValue< std::string, Value > Data;
			typedef std::vector< Data > ArrayType;

			std::string _string;
			long double _number;
			ArrayType _array;
	};

	class Parse
	{
		void add( std::vector< Value *> &destinations, std::string &string ) const
		{
			if ( destinations.empty() ) return;

			std::string::iterator start = string.begin(), end = string.end();

			/* strip whitespace */
			while ( start != end )
			{
				switch ( *start )
				{
					case ' ':
					case '\t':
					case '\r':
					case '\n':
						++start;
						continue;
				}

				switch ( *(end-1 ) )
				{
					case ' ':
					case '\t':
					case '\r':
					case '\n':
						--end;
						continue;
				}

				break;
			}

			if ( start != end )
			{
				unsigned int validNumber = 1;

				/* determine type of string */

				std::string::const_iterator i = start;

				while ( i != end )
				{
					switch ( validNumber )
					{
						case 0:
							i = end;
							continue;
						case 1:
							switch ( *i )
							{
								case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
									validNumber = 2;
									break;
								default:
									validNumber = 0;
									break;
							}
							break;
						case 2:
							switch ( *i )
							{
								case '.':
									validNumber = 3;
									break;
								case 'e':
								case 'E':
									validNumber = 4;
									break;
								case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
									break;
								default:
									validNumber = 0;
									break;
							}
							break;
						case 3:
							switch ( *i )
							{
								case 'e':
								case 'E':
									validNumber = 4;
									break;
								case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
									break;
								default:
									validNumber = 0;
									break;
							}
							break;
						case 4:
							switch ( *i )
							{
								case '+': case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
									validNumber = 5;
									break;
								default:
									validNumber = 0;
									break;
							}
							break;
						case 5:
							switch ( *i )
							{
								case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
									break;
								default:
									validNumber = 0;
									break;
							}
							break;
					}

					++i;
				}

				std::string string( start, end );

				if ( validNumber )
				{
					long double v = 0 ;
					std::stringstream stream( string );
					stream >> v;
					add( destinations, v );
				}
				else if ( string == "null" )
				{
					add( destinations, json::Null );
				}
				else if ( string == "true" )
				{
					add( destinations, Value( true ) );
				}
				else if ( string == "false" )
				{
					add( destinations, Value( false ) );
				}
				else if ( string == "NaN" )
				{
					add( destinations, Value( std::numeric_limits< long double >::quiet_NaN() ) );
				}
				else
				{
					// skip quotes
					if ( *start == '"' || *start == '\'' ) ++start;

					// skip quotes
					if ( *(end-1 ) == '"' || *(end-1 ) == '\'' ) --end;

					std::string result;
					result.reserve( end - start );

					int mode = 0;
					while ( start != end )
					{
						switch ( mode )
						{
							case 0:
								if ( *start == '\\' )
								{
									mode = 1;
									++start;
									continue;
								}
								break;
							case 1:
								switch ( *start )
								{
									case '"':
										result.push_back( '"' );
										mode = 0;
										++start;
										continue;
									case '\'':
										result.push_back( '\'' );
										mode = 0;
										++start;
										continue;
									case '\\':
										result.push_back( '\\' );
										mode = 0;
										++start;
										continue;
									case '/':
										result.push_back( '/' );
										mode = 0;
										++start;
										continue;
									case 'b':
										result.push_back( '\b' );
										mode = 0;
										++start;
										continue;
									case 'f':
										result.push_back( '\f' );
										mode = 0;
										++start;
										continue;
									case 'n':
										result.push_back( '\n' );
										mode = 0;
										++start;
										continue;
									case 'r':
										result.push_back( '\r' );
										mode = 0;
										++start;
										continue;
									case 't':
										result.push_back( '\t' );
										mode = 0;
										++start;
										continue;
									case 'u':
										mode = 2;
										++start;
										continue;
								}
								break;
							case 2:
							case 3:
							case 4:
							case 5:
								switch ( *start )
								{
									case '0':
									case '1':
									case '2':
									case '3':
									case '4':
									case '5':
									case '6':
									case '7':
									case '8':
									case '9':
									case 'a':
									case 'b':
									case 'c':
									case 'd':
									case 'e':
									case 'f':
									case 'A':
									case 'B':
									case 'C':
									case 'D':
									case 'E':
									case 'F':
										if ( ++mode > 5 )
										{
											std::stringstream stream( std::string( start - 3, start + 1 ) );

											int value = 0;

											stream >> std::hex >> value;

											result += utf8Encode( value );

											mode = 0;
										}
										++start;
										continue;
									default:
										break;
								}
								break;
						}

						result.push_back( *start++ );
					}

					add( destinations, Value( result ) );
				}
			}
		}

		void add( std::vector< Value *> &destinations, const Value &item ) const
		{
			if ( destinations.empty() ) return;

			Value &destination( *destinations.back() );

			switch ( destination.type )
			{
				case String:
				case Number:
				case Bool:
				case Null:
				case Undefined:
					destination = item;
					if ( item.type != Array && item.type != Object )
					{
						if ( destinations.empty() ) throw "empty array";
						destinations.pop_back();
					}
					break;
				case Object:
					destinations.push_back( &destination[ item.operator std::string() ] );
					break;
				case Array:
					destination.push( item );
					if ( item.type == Array || item.type == Object ) destinations.push_back( &destination.back() );
					break;
			}
		}

		public:

			Value operator()( const char string[] ) const
			{
				return operator ()( std::string( string ) );
			}

			Value operator()( const std::string &string ) const
			{
				const std::string::const_iterator &end = string.end();
				std::string::const_iterator start = string.begin();

				std::vector< Value* > destinations;
				destinations.reserve( 1024 );

				Value data;

				destinations.push_back( &data );

				enum Store
				{
					Save = 0,
					Skip = 1 << 0,
					Clear = 1 << 1
				};

				bool doubleString = false, singleString = false;
				int escape = 0;

				std::string literal;
				literal.reserve( 1024 );

				while ( start != end )
				{
					int store = Save;

					switch ( *start )
					{
						case '{': // start object
							if ( singleString || doubleString ) break;
							// add object
							add( destinations, Object );
							store = Skip | Clear;
							break;
						case '}': // close object
							if ( singleString || doubleString ) break;
							if ( !literal.empty() ) add( destinations, literal );
							store = Skip | Clear;
							if ( destinations.empty() ) throw "empty array";
							destinations.pop_back();
							break;
						case '[': // add array
							if ( singleString || doubleString ) break;
							add( destinations, Array );
							store = Skip | Clear;
							break;
						case ']': // close array
							if ( singleString || doubleString ) break;
							if ( !literal.empty() ) add( destinations, literal );
							store = Skip | Clear;
							if ( destinations.empty() ) throw "empty array";
							destinations.pop_back();
							break;
						case ':': // add property
							if ( singleString || doubleString ) break;
							add( destinations, literal );
							store = Skip | Clear;
							break;
						case ',': // add destination
							if ( singleString || doubleString ) break;
							if ( !literal.empty() ) add( destinations, literal );
							store = Skip | Clear;
							break;
						case ' ': case '\t': case '\r': case '\n':
							if ( singleString || doubleString ) break;
							// skip whitespace
							break;
						case '"':
							// handle string
							if ( escape || singleString ) break;
							doubleString = !doubleString;
							break;
						case '\'':
							// handle string
							if ( escape || doubleString ) break;
							singleString = !singleString;
							break;
						case '\\':
							// handle escape
							if ( escape ) break;
							escape = 2;
							break;
						default:
							break;
					}

					if ( escape ) --escape;

					if ( !store )
					{
						literal.push_back( *start );
					}
					else if ( store & Clear )
					{
						literal.clear();
					}

					++start;
				}

				/* if data remains, append it */
				if ( !literal.empty() ) add( destinations, literal );

				return data;
			}

	} static const parse = Parse();
}

#endif // JSONPP_H
