#ifndef JSONPP_H
#define JSONPP_H

#include <vector>
#include <numeric>
#include <cmath>
#include <sstream>

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

	template< class Key, class Value >
	struct KeyValue
	{
		KeyValue() : key(), destination() { }

		KeyValue( const Value &v ) : key(), destination( v ) { }

		KeyValue( const Key &k ) : key( k ), destination() { }

		bool operator == ( const Key &rhs ) const
		{
			return key == rhs;
		}

		bool operator == ( const KeyValue< Key, Value > &rhs ) const
		{
			return key == rhs.key && destination == rhs.destination;
		}

		bool operator != ( const KeyValue< Key, Value > &rhs ) const
		{
			return key != rhs.key || destination != rhs.destination;
		}

		Key key;
		Value destination;
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
			_string( string ),
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
			_array() { }

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
					if ( std::modf( _number, &temp ) == 0.0 && _number <= std::numeric_limits< long long >::max() )
					{
						stream << static_cast< long long >( _number );
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

		Value& operator[]( const char key[] ) { return operator []( std::string( key ) ); }

		Value& operator[]( const std::string &key )
		{
			if ( type != Object )
			{
				const_cast< Types& >( type ) = Object;
				_array.clear();
			}
			ArrayType::iterator i = std::find( _array.begin(), _array.end(), key );
			if ( i == _array.end() )
			{
				_array.push_back( key );

				return _array.back().destination;
			}
			return i->destination;
		}

		Value operator[]( const char key[] ) const { return operator []( std::string( key ) ); }

		Value operator[]( const std::string &key ) const
		{
			ArrayType::const_iterator i = std::find( _array.begin(), _array.end(), key );
			if ( i == _array.end() ) return Value();
			return i->destination;
		}

		Value& operator[]( int index ) { return operator []( static_cast< size_t >( index ) ); }

		Value& operator[]( size_t index )
		{
			if ( type != Array )
			{
				const_cast< Types& >( type ) = Array;
				_array.clear();
			}
			if ( index >= _array.size() ) _array.resize( index + 1 );
			return _array[ index ].destination;
		}

		Value operator[]( int index ) const { return operator []( static_cast< size_t >( index ) ); }

		Value operator[]( size_t index ) const
		{
			if ( index >= _array.size() ) return Value();
			return _array[ index ].destination;
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

		void push( const Value &destination )
		{
			if ( type != Array )
			{
				const_cast< Types& >( type ) = Array;
				_array.clear();
			}
			_array.push_back( destination );
		}

		void clear()
		{
			const_cast< Types& >( type ) = Undefined;
			_string.clear();
			_array.clear();
			_number = std::numeric_limits< long double >::quiet_NaN();
		}

		std::string serialize() const
		{
			switch ( type )
			{
				case Null:
				case Undefined:
					return "null";
				case Number:
				case Bool:
					return *this;
				case String:
				{
					std::string str( _string );
					std::string::iterator s = str.begin();
					while ( s != str.end() )
					{
						if ( *s == '"' )
						{
							s = str.insert( s, '\\' );
							++s;
						}
						++s;
					}
					return "\"" + str + "\"";
				}
				case Object:
				case Array:
					break;
			}

			std::string result;

			if ( type == Array )
			{
				result += "[";

				for ( ArrayType::const_iterator i = _array.begin(); i != _array.end(); ++i )
				{
					if ( i != _array.begin() ) result += ',';

					result += i->destination.serialize();
				}

				result += "]";
			}
			else
			{
				result += "{";

				for ( ArrayType::const_iterator i = _array.begin(); i != _array.end(); ++i )
				{
					if ( i != _array.begin() ) result += ',';

					result += "\"" + i->key + "\":";

					result += i->destination.serialize();
				}

				result += "}";
			}

			return result;
		}

		Value& front()
		{
			if ( _array.empty() ) return *this;
			return _array.front().destination;
		}

		Value front() const
		{
			if ( _array.empty() ) return Value();
			return _array.front().destination;
		}

		Value& back()
		{
			if ( _array.empty() ) return *this;
			return _array.back().destination;
		}

		Value back() const
		{
			if ( _array.empty() ) return Value();
			return _array.back().destination;
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
		void add( std::vector< Value *> &destinations, std::string::iterator start, std::string::iterator end ) const
		{
			if ( destinations.empty() ) throw;

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

				switch ( *(end-1) )
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
					if ( *(end-1) == '"' || *(end-1) == '\'' ) --end;

					add( destinations, Value( std::string( start, end ) ) );
				}
			}
		}

		void add( std::vector< Value *> &destinations, const Value &item ) const
		{
			if ( destinations.empty() ) throw;

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
						if ( destinations.empty() ) throw;
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

			Value operator()( const std::string &string ) const
			{
				const std::string::const_iterator &end = string.end();
				std::string::const_iterator start = string.begin();

				std::vector< Value* > destinations;
//				Value *destinations[ 1024 ];
//				Value **destination = destinations;

				Value data;
//				*destination = 0;
//				*++destination = &data;

				destinations.push_back( &data );

				enum Store
				{
					Save = 0,
					Skip = 1 << 0,
					Clear = 1 << 1
				};

				bool doubleString = false, singleString = false;

				std::string literal( 1024, 0 );
				const std::string::iterator literalStart = literal.begin();
				std::string::iterator literalEnd = literalStart;

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
							if ( literalEnd != literalStart ) add( destinations, literalStart, literalEnd );
							store = Skip | Clear;
							if ( destinations.empty() ) throw;
							destinations.pop_back();
							break;
						case '[': // add array
							if ( singleString || doubleString ) break;
							add( destinations, Array );
							store = Skip | Clear;
							break;
						case ']': // close array
							if ( singleString || doubleString ) break;
							if ( literalEnd != literalStart ) add( destinations, literalStart, literalEnd );
							store = Skip | Clear;
							if ( destinations.empty() ) throw;
							destinations.pop_back();
							break;
						case ':': // add property
							if ( singleString || doubleString ) break;
							add( destinations, literalStart, literalEnd );
							store = Skip | Clear;
							break;
						case ',': // add destination
							if ( singleString || doubleString ) break;
							if ( literalEnd != literalStart ) add( destinations, literalStart, literalEnd );
							store = Skip | Clear;
							break;
						case ' ': case '\t': case '\r': case '\n':
							if ( singleString || doubleString ) break;
							// skip whitespace
							break;
						case '"':
							// handle string
							if ( singleString ) break;
							doubleString = !doubleString;
							break;
						case '\'':
							// handle string
							if ( doubleString ) break;
							singleString = !singleString;
							break;
						default:
							break;
					}

					if ( !store )
					{
						*literalEnd++ = *start;
					}
					else if ( store & Clear )
					{
						literalEnd = literalStart;
					}

					++start;
				}

				/* if data remains, append it */
				if ( literalEnd != literalStart && !destinations.empty() ) add( destinations, literalStart, literalEnd );

				return data;
			}

	} parse;
}

#endif // JSONPP_H
