#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#ifdef _MSC_VER
#include <memory>
#else
#include <tr1/memory>
#endif
#include <limits>

#include <jsonpp/unicode.h>
#include <jsonpp/basic_var_data.h>
#include <jsonpp/misc.h>
#include <jsonpp/register_type.h>
#include <jsonpp/register_basic_var.h>

namespace json
{
	template < template< class > class CopyBehaviour, class T >
	struct basic_var
	{
		typedef basic_var_data< CopyBehaviour, T > basic_var_data;

		typedef std::basic_string< T > string_type;

		typedef long double number_type;

		typedef key_value< string_type, basic_var > value_type;

		typedef std::vector< value_type > array_type;

		typedef typename array_type::iterator iterator;

		typedef typename array_type::const_iterator const_iterator;

		typedef CopyBehaviour< basic_var_data > data_pointer;

		const Types type;

			basic_var() :
				type( Undefined ),
				_data( basic_var_data() ) { }

			template < class InputType >
			basic_var( const InputType &type ) :
				type( register_type( type ) ),
				_data( basic_var_data( register_string< basic_var< CopyOnWrite, T > >( type ),
									   register_number< basic_var< CopyOnWrite, T > >( type ) ) ) { }

//			basic_var( const Buffer< T > &string ) :
//				type( String ),
//				_data( basic_var_data( string ) ) { }

//			basic_var( const string_type &string ) :
//				type( String ),
//				_data( basic_var_data( string ) ) { }

//			basic_var( const char *string ) :
//				type( String ),
//				_data( basic_var_data( string ) ) { }

//			basic_var( char character ) :
//				type( String ),
//				_data( basic_var_data( string_type( 1, character ) ) ) { }

//			basic_var( unsigned char character ) :
//				type( String ),
//				_data( basic_var_data( string_type( 1, character ) ) ) { }

//			basic_var( bool boolean ) :
//				type( Bool ),
//				_data( basic_var_data( string_type(), boolean ) ) { }

//			basic_var( short number ) :
//				type( Number ),
//				_data( basic_var_data( string_type(), number ) ) { }

//			basic_var( unsigned short number ) :
//				type( Number ),
//				_data( basic_var_data( string_type(), number ) ) { }

//			basic_var( int number ) :
//				type( Number ),
//				_data( basic_var_data( string_type(), number ) ) { }

//			basic_var( unsigned int number ) :
//				type( Number ),
//				_data( basic_var_data( string_type(), number ) ) { }

//			basic_var( long number ) :
//				type( Number ),
//				_data( basic_var_data( string_type(), number ) ) { }

//			basic_var( unsigned long number ) :
//				type( Number ),
//				_data( basic_var_data( string_type(), number ) ) { }

//			basic_var( long long number ) :
//				type( Number ),
//				_data( basic_var_data( string_type(), static_cast< long double >( number ) ) ) { }

//			basic_var( unsigned long long number ) :
//				type( Number ),
//				_data( basic_var_data( string_type(), static_cast< long double >( number ) ) ) { }

//			basic_var( float number ) :
//				type( Number ),
//				_data( basic_var_data( string_type(), number ) ) { }

//			basic_var( double number ) :
//				type( Number ),
//				_data( basic_var_data( string_type(), number ) ) { }

//			basic_var( long double number ) :
//				type( Number ),
//				_data( basic_var_data( string_type(), number ) ) { }

			basic_var& operator = ( const basic_var &rhs )
			{
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

			operator string_type() const
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
						stream.precision( std::numeric_limits< long double >::digits10 );
						if ( std::modf( _data->_number, &temp ) != 0.0 )
						{
							// fraction
							stream << _data->_number;
						}
						else
						{
							// integer
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

			string_type toString() const { return operator string_type(); }

			long double toNumber() const { return operator long double(); }

			basic_var& operator[]( const basic_var &key )
			{
				switch ( type )
				{
					case Number:
						return operator[]( key._data->_number );
					default:
						return operator[]( key.operator string_type() );
				}
			}

			const basic_var& operator[]( const basic_var &key ) const
			{
				switch ( type )
				{
					case Number:
						return operator[]( key._data->_number );
					default:
						return operator[]( key.operator string_type() );
				}
			}

			basic_var& operator[]( const char key[] ) { return operator []( string_type( key ) ); }

			basic_var& operator[]( const string_type &key )
			{
				if ( type != Object )
				{
					const_cast< Types& >( type ) = Object;
					_data->_array.clear();
				}
				iterator i = std::find( _data->_array.begin(), _data->_array.end(), key );
				if ( i == _data->_array.end() )
				{
					_data->_array.push_back( value_type( key, Undefined ) );

					return _data->_array.back().value;
				}
				return i->value;
			}

			const basic_var& operator[]( const char key[] ) const { return operator []( string_type( key ) ); }

			const basic_var& operator[]( const string_type &key ) const
			{
				const_iterator i = std::find( _data->_array.begin(), _data->_array.end(), key );
				if ( i == _data->_array.end() )
				{
					static basic_var undefined( Undefined );
					return undefined;
				}
				return i->value;
			}

			basic_var& operator[]( char index ) { return operator []( string_type( 1, index ) ); }

			basic_var& operator[]( unsigned char index ) { return operator []( string_type( 1, index ) ); }

			basic_var& operator[]( short index ) { return operator []( static_cast< unsigned int >( index ) ); }

			basic_var& operator[]( unsigned short index ) { return operator []( static_cast< unsigned int >( index ) ); }

			basic_var& operator[]( int index ) { return operator []( static_cast< unsigned int >( index ) ); }

			basic_var& operator[]( unsigned int index )
			{
				if ( type != Array )
				{
					const_cast< Types& >( type ) = Array;
					_data->_array.clear();
				}
				if ( index >= _data->_array.size() ) _data->_array.resize( index + 1, value_type() );
				return _data->_array.operator[]( index ).value;
			}

			basic_var& operator[]( long index ) { return operator []( static_cast< unsigned int >( index ) ); }

			basic_var& operator[]( unsigned long index ) { return operator []( static_cast< unsigned int >( index ) ); }

			basic_var& operator[]( long long index ) { return operator []( static_cast< unsigned int >( index ) ); }

			basic_var& operator[]( unsigned long long index ) { return operator []( static_cast< unsigned int >( index ) ); }

			basic_var& operator[]( float index ) { return operator []( static_cast< unsigned int >( index ) ); }

			basic_var& operator[]( double index ) { return operator []( static_cast< unsigned int >( index ) ); }

			basic_var& operator[]( long double index ) { return operator []( static_cast< unsigned int >( index ) ); }


			const basic_var& operator[]( char index ) const { return operator []( string_type( 1, index ) ); }

			const basic_var& operator[]( unsigned char index ) const { return operator []( string_type( 1, index ) ); }

			const basic_var& operator[]( short index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const basic_var& operator[]( unsigned short index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const basic_var& operator[]( int index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const basic_var& operator[]( unsigned int index ) const
			{
				if ( index >= _data->_array.size() )
				{
					static basic_var undefined( Undefined );
					return undefined;
				}
				return _data->_array.operator[]( index ).value;
			}

			const basic_var& operator[]( long index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const basic_var& operator[]( unsigned long index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const basic_var& operator[]( long long index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const basic_var& operator[]( unsigned long long index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const basic_var& operator[]( float index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const basic_var& operator[]( double index ) const { return operator []( static_cast< unsigned int >( index ) ); }

			const basic_var& operator[]( long double index ) const { return operator []( static_cast< unsigned int >( index ) ); }


			bool operator == ( const basic_var &rhs ) const
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

			bool operator != ( const basic_var &rhs ) const
			{
				return !operator == ( rhs );
			}

			basic_var& operator += ( const basic_var &rhs )
			{
				if ( type == Number && rhs.type == Number )
				{
					_data->_number += rhs.toNumber();
				}
				else
				{
					*this = basic_var( toString() + rhs.toString() );
				}

				return *this;
			}

			basic_var& splice( unsigned int index, unsigned int remove )
			{
				if ( remove )
				{
					if ( index < _data->_array.size() && remove < _data->_array.size() - index )
					{
						_data->_array.erase( _data->_array.begin() + index, _data->_array.begin() + index + remove );
					}
				}

				return *this;
			}

			basic_var splice( unsigned int index, unsigned int remove, const basic_var &item )
			{
				basic_var removed = Array;

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
					_data->_array.insert( _data->_array.begin() + index, value_type( item ) );
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

			void push( const basic_var &value )
			{
				if ( type != Array )
				{
					const_cast< Types& >( type ) = Array;
					_data->_array.clear();
				}
				_data->_array.push_back( value_type( value ) );
			}

			void clear()
			{
				const_cast< Types& >( type ) = Undefined;
				_data->_string.clear();
				_data->_array.clear();
				_data->_number = std::numeric_limits< long double >::quiet_NaN();
			}

			void merge( const basic_var &rhs )
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

			string_type serialize( unsigned int markup = Compact, unsigned int level = 0 ) const
			{
				const string_type tabs( level, '\t' );

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
						const string_type tak = '\"' + utf8Decode( _data->_string ) + '\"';
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

			basic_var& front()
			{
				if ( _data->_array.empty() ) return *this;
				return _data->_array.front().value;
			}

			basic_var front() const
			{
				if ( _data->_array.empty() ) return basic_var();
				return _data->_array.front().value;
			}

			basic_var& back()
			{
				if ( _data->_array.empty() ) return *this;
				return _data->_array.back().value;
			}

			basic_var back() const
			{
				if ( _data->_array.empty() ) return basic_var();
				return _data->_array.back().value;
			}

			size_t size() const { return _data->_array.size(); }

			iterator begin()
			{
				return _data->_array.begin();
			}

			const_iterator begin() const { return _data->_array.begin(); }

			iterator end()
			{
				return _data->_array.end();
			}

			const_iterator end() const { return _data->_array.end(); }

		private:

			data_pointer _data;
	};

	template < template< class > class A, class B >
	inline std::ostream& operator << ( std::ostream &stream, const basic_var< A, B > &value )
	{
		return stream << value.serialize();
	}

	typedef basic_var< CopyOnWrite, char > var;
	typedef basic_var< CopyOnWrite, wchar_t > wvar;
}
