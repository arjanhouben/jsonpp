#pragma once

#include <vector>
#include <cmath>
#include <tr1/memory>

#include <jsonpp/unicode.h>
#include <jsonpp/misc.h>

namespace json
{
	struct var
	{
		typedef key_value< std::string, var > value_type;

		typedef std::vector< value_type > array_type;

		typedef array_type::iterator iterator;

		typedef array_type::const_iterator const_iterator;

		struct var_data
		{
			var_data( const std::string &s = std::string(), long double n = std::numeric_limits< long double >::quiet_NaN() ) :
				_string( s ),
				_number( n ),
				_array() { }
			std::string _string;
			long double _number;
			array_type _array;
		};

		typedef std::tr1::shared_ptr< var_data > data_pointer;

		const Types type;

			var() :
				type( Undefined ),
				_data( new var_data() ) { }

			var( Types type ) :
				type( type ),
				_data( new var_data() ) { }

			var( const std::string &string ) :
				type( String ),
				_data( new var_data( string ) ) { }

			var( const char *string ) :
				type( String ),
				_data( new var_data( string ) ) { }

			var( char character ) :
				type( String ),
				_data( new var_data( std::string( 1, character ) ) ) { }

			var( unsigned char character ) :
				type( String ),
				_data( new var_data( std::string( 1, character ) ) ) { }

			var( bool boolean ) :
				type( Bool ),
				_data( new var_data( std::string(), boolean ) ) { }

			var( short number ) :
				type( Number ),
				_data( new var_data( std::string(), number ) ) { }

			var( unsigned short number ) :
				type( Number ),
				_data( new var_data( std::string(), number ) ) { }

			var( int number ) :
				type( Number ),
				_data( new var_data( std::string(), number ) ) { }

			var( unsigned int number ) :
				type( Number ),
				_data( new var_data( std::string(), number ) ) { }

			var( long number ) :
				type( Number ),
				_data( new var_data( std::string(), number ) ) { }

			var( unsigned long number ) :
				type( Number ),
				_data( new var_data( std::string(), number ) ) { }

			var( long long number ) :
				type( Number ),
				_data( new var_data( std::string(), number ) ) { }

			var( unsigned long long number ) :
				type( Number ),
				_data( new var_data( std::string(), number ) ) { }

			var( float number ) :
				type( Number ),
				_data( new var_data( std::string(), number ) ) { }

			var( double number ) :
				type( Number ),
				_data( new var_data( std::string(), number ) ) { }

			var( long double number ) :
				type( Number ),
				_data( new var_data( std::string(), number ) ) { }

			var( const var &rhs ) :
				type( rhs.type ),
				_data( rhs._data ) { }

			var& operator = ( const var &rhs )
			{
				make_unique();

				if ( this != &rhs )
				{
					const_cast< Types& >( type ) = rhs.type;
					_data = rhs._data;
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
						return !_data->_string.empty();
					case Number:
						break;
					case Bool:
						break;
				}
				return _data->_number && ( !isNaN( _data->_number ) );
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
						return _data->_string;
					case Number:
					{
						std::stringstream stream;
						long double temp = 0;
						if ( std::modf( _data->_number, &temp ) != 0.0 )
						{
							// fraction
							stream.precision( 18 );
							stream << _data->_number;
						}
						else
						{
							// integer
							stream.precision( 32 );
							stream << _data->_number;
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
				if ( isNaN( _data->_number ) )
				{
					std::stringstream stream( _data->_string );
					long double result;
					stream >> result;
					return result;
				}
				return _data->_number;
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

			std::string toString() const { return operator std::string(); }

			long double toNumber() const { return operator long double(); }

			var& operator[]( const var &key )
			{
				make_unique();

				switch ( type )
				{
					case Number:
						return operator[]( key._data->_number );
					default:
						return operator[]( key.operator std::string() );
				}
			}

			var& operator[]( const char key[] ) { return operator []( std::string( key ) ); }

			var& operator[]( const std::string &key )
			{
				make_unique();

				if ( type != Object )
				{
					const_cast< Types& >( type ) = Object;
					_data->_array.clear();
				}
				iterator i = std::find_if( _data->_array.begin(), _data->_array.end(), value_type::findKey( &key ) );
				if ( i == _data->_array.end() )
				{
					_data->_array.push_back( array_type::value_type( std::string( key ), Undefined ) );

					return _data->_array.back().value;
				}
				return i->value;
			}

			const var& operator[]( const char key[] ) const { return operator []( std::string( key ) ); }

			const var& operator[]( const std::string &key ) const
			{
				const_iterator i = std::find_if( _data->_array.begin(), _data->_array.end(), value_type::findKey( &key ) );
				if ( i == _data->_array.end() )
				{
					static var undefined( Undefined );
					return undefined;
				}
				return i->value;
			}

			var& operator[]( char index ) { return operator []( std::string( 1, index ) ); }

			var& operator[]( unsigned char index ) { return operator []( std::string( 1, index ) ); }

			var& operator[]( short index ) { return operator []( static_cast< unsigned int >( index ) ); }

			var& operator[]( unsigned short index ) { return operator []( static_cast< unsigned int >( index ) ); }

			var& operator[]( int index ) { return operator []( static_cast< unsigned int >( index ) ); }

			var& operator[]( unsigned int index )
			{
				make_unique();

				if ( type != Array )
				{
					const_cast< Types& >( type ) = Array;
					_data->_array.clear();
				}
				if ( index >= _data->_array.size() ) _data->_array.resize( index + 1 );
				return _data->_array.operator[]( index ).value;
			}

			var& operator[]( float index ) { return operator []( static_cast< unsigned int >( index ) ); }

			var& operator[]( double index ) { return operator []( static_cast< unsigned int >( index ) ); }

			var& operator[]( long double index ) { return operator []( static_cast< unsigned int >( index ) ); }

			var& operator[]( long index ) { return operator []( static_cast< unsigned int >( index ) ); }

			var& operator[]( unsigned long index ) { return operator []( static_cast< unsigned int >( index ) ); }


			const var& operator[]( char index ) const { return operator []( std::string( 1, index ) ); }

			const var& operator[]( unsigned char index ) const { return operator []( std::string( 1, index ) ); }

			const var& operator[]( short index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const var& operator[]( unsigned short index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const var& operator[]( int index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const var& operator[]( unsigned int index ) const
			{
				if ( index >= _data->_array.size() )
				{
					static var undefined( Undefined );
					return undefined;
				}
				return _data->_array.operator[]( index ).value;
			}

			const var& operator[]( float index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const var& operator[]( double index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const var& operator[]( long double index ) const { return operator []( static_cast< unsigned int >( index ) ); }


			bool operator == ( const var &rhs ) const
			{
				if ( type != rhs.type ) return false;

				if ( isNaN( _data->_number ) != isNaN( rhs._data->_number ) ) return false;

				if ( !isNaN( _data->_number ) )
				{
					if ( _data->_number != rhs._data->_number ) return false;
				}

				if ( _data->_string != rhs._data->_string ) return false;

				if ( _data->_array != rhs._data->_array ) return false;

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
				make_unique();

				if ( type == Number && rhs.type == Number )
				{
					_data->_number += rhs.toNumber();
				}
				else
				{
					*this = var( toString() + rhs.toString() );
				}

				return *this;
			}

			var& splice( unsigned int index, unsigned int remove )
			{
				make_unique();

				if ( remove )
				{
					if ( index < _data->_array.size() && remove < _data->_array.size() - index )
					{
						_data->_array.erase( _data->_array.begin() + index, _data->_array.begin() + index + remove );
					}
				}

				return *this;
			}

			var splice( unsigned int index, unsigned int remove, const var &item )
			{
				make_unique();

				var removed = Array;

				if ( remove )
				{
					for ( unsigned int i = 0; i < remove; ++i )
					{
						removed[ i ] = item;
					}

					splice( index, remove );
				}

				if ( index < _data->_array.size() )
				{
					_data->_array.insert( _data->_array.begin() + index, item );
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
				make_unique();

				if ( type != Array )
				{
					const_cast< Types& >( type ) = Array;
					_data->_array.clear();
				}
				_data->_array.push_back( value );
			}

			void clear()
			{
				make_unique();

				const_cast< Types& >( type ) = Undefined;
				_data->_string.clear();
				_data->_array.clear();
				_data->_number = std::numeric_limits< long double >::quiet_NaN();
			}

			void merge( const var &rhs )
			{
				make_unique();

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
						for ( const_iterator i = rhs._data->_array.begin(); i != rhs._data->_array.end(); ++i )
						{
							operator []( i->key ).merge( i->value );
						}
						break;
					case Array:
						int c = 0;
						for ( const_iterator i = rhs._data->_array.begin(); i != rhs._data->_array.end(); ++i )
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
						const std::string tak = '\"' + utf8Decode( _data->_string ) + '\"';
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

					for ( const_iterator i = _data->_array.begin(); i != _data->_array.end(); ++i )
					{
						if ( i != _data->_array.begin() ) result << ',';

						if ( markup & HumanReadable )
						{
							result << '\n' << '\t' << tabs;

							if ( markup & CountArrayValues )
							{
								result << i - _data->_array.begin() << " => ";
							}
						}

						result << i->value.serialize( markup & ~IndentFirstItem, level + 1 );
					}

					if ( markup & HumanReadable ) result << '\n' << tabs;

					result << ']';
				}
				else
				{
					result << '{';

					for ( const_iterator i = _data->_array.begin(); i != _data->_array.end(); ++i )
					{
						if ( i != _data->_array.begin() ) result << ',';

						if ( markup & HumanReadable ) result << '\n' << '\t' << tabs;

						result << '\"' << utf8Decode( i->key ) << '\"' << ':';

						result << i->value.serialize( markup & ~IndentFirstItem, level + 1 );
					}

					if ( markup & HumanReadable ) result << '\n' << tabs;

					result << '}';
				}

				return result.str();
			}

			var& front()
			{
				make_unique();

				if ( _data->_array.empty() ) return *this;
				return _data->_array.front().value;
			}

			var front() const
			{
				if ( _data->_array.empty() ) return var();
				return _data->_array.front().value;
			}

			var& back()
			{
				make_unique();

				if ( _data->_array.empty() ) return *this;
				return _data->_array.back().value;
			}

			var back() const
			{
				if ( _data->_array.empty() ) return var();
				return _data->_array.back().value;
			}

			size_t size() const { return _data->_array.size(); }

			iterator begin()
			{
				make_unique();

				return _data->_array.begin();
			}

			const_iterator begin() const { return _data->_array.begin(); }

			iterator end()
			{
				make_unique();

				return _data->_array.end();
			}

			const_iterator end() const { return _data->_array.end(); }

		private:

			void make_unique()
			{
				if ( _data.unique() ) return;

				_data = data_pointer( new var_data( *_data ) );
			}

			data_pointer _data;
	};

	inline std::ostream& operator << ( std::ostream &stream, const var &value )
	{
		return stream << value.serialize();
	}
}
