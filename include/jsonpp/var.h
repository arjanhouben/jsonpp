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
#include <jsonpp/misc.h>
#include <jsonpp/basic_var_data.h>
#include <jsonpp/register_type.h>

namespace json
{
	template < template< class > class CopyBehaviour, class Char >
	struct basic_binary;

	template < template< class > class CopyBehaviour, class Char >
	struct basic_var
	{
		typedef Char character_type;

		typedef basic_var_data< CopyBehaviour, Char > basic_data;

		typedef std::basic_string< Char > string_type;

		typedef long double number_type;

		typedef key_value< string_type, basic_var > value_type;

		typedef std::vector< value_type > array_type;

		typedef typename array_type::iterator iterator;

		typedef typename array_type::const_iterator const_iterator;

		typedef CopyBehaviour< basic_data > data_pointer;

		const Types type;

			basic_var() :
				type( Undefined ),
				_data( basic_data() ) { }

			basic_var( Types type ) :
				type( type ),
				_data( basic_data() ) { }

			template < class InputType >
			basic_var( const InputType &type ) :
				type( register_type< basic_var, InputType >::type( type ) ),
				_data( register_type< basic_var, InputType >::to_json( type ) ) { }

			basic_var& operator = ( const basic_var &rhs )
			{
				if ( this != &rhs )
				{
					const_cast< Types& >( type ) = rhs.type;
					_data = rhs._data;
				}

				return *this;
			}

			string_type toString() const
			{
				switch ( type )
				{
					case Array:
						return convert_string< Char >( "Array" );
					case Object:
						return convert_string< Char >( "Object" );
					case String:
						return _data->_string;
					case Number:
					{
						std::basic_stringstream< Char > stream;
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
						return ( toBool() ? convert_string< Char >( "true" ) : convert_string< Char >( "false" ) );
					case Null:
						return convert_string< Char >( "null" );
					case Undefined:
					default:
						return string_type();
				}
			}

			long double toNumber() const
			{
				if ( isNaN( _data->_number ) )
				{
					std::basic_stringstream< Char > stream( _data->_string );
					long double result;
					stream >> result;
					return result;
				}
				return _data->_number;
			}

			long long toInteger() const { return toNumber(); }

			bool toBool() const
			{
				switch ( type )
				{
					case TypeCount:
					case Null:
					case Undefined:
						return false;
					case Array:
					case Object:
						return true;
					case String:
						return !toString().empty();
					case Number:
						break;
					case Bool:
						break;
				}

				return toNumber();
			}

			template < class DesiredType >
			DesiredType to() const
			{
				return register_type< basic_var, DesiredType >::from_json( *this );
			}

			const_iterator find_key( const string_type &key, const_iterator from = const_iterator() ) const
			{
				if ( from == const_iterator() ) from = begin();
				return std::find( from, end(), key );
			}

			bool has_key( const string_type &key, const_iterator from = const_iterator() ) const
			{
				return find_key( key, from ) != end();
			}

			basic_var& operator[]( const basic_var &key )
			{
				switch ( type )
				{
					case Number:
						return operator[]( key._data->_number );
					default:
						return operator[]( key.toString() );
				}
			}

			const basic_var& operator[]( const basic_var &key ) const
			{
				switch ( type )
				{
					case Number:
						return operator[]( key._data->_number );
					default:
						return operator[]( key.toString() );
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
						if ( markup & HumanReadable && markup & IndentFirstItem ) return tabs + convert_string< Char >( "null" );
						return convert_string< Char >( "null" );
					case Number:
					case Bool:
						if ( markup & HumanReadable && markup & IndentFirstItem ) return tabs + toString();
						return toString();
					case String:
					{
						const string_type tak = static_cast< Char >( '\"' ) + utf8Decode( _data->_string ) + static_cast< Char >( '\"' );
						if ( markup & HumanReadable && markup & IndentFirstItem ) return tabs + tak;
						return tak;
					}
					case Object:
					case Array:
					case TypeCount:
						break;
				}

				std::basic_stringstream< Char > result;

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

			size_t size() const
			{
				switch ( type )
				{
					default:
						return 0;
					case String:
						return _data->_string.size();
					case Array:
					case Object:
						return _data->_array.size();
				}
			}

			bool empty() const
			{
				switch ( type )
				{
					default:
						return true;
					case String:
						return _data->_string.empty();
					case Array:
					case Object:
						return _data->_array.empty();
				}
			}

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

		friend class basic_binary< CopyOnWrite, Char >;
	};

	template < template< class > class A, class B, class C >
	bool operator == ( const basic_var< A, B > &lhs, const C &rhs )
	{
		return lhs == basic_var< A, B >( rhs );
	}

	template < template< class > class A, class B, class C >
	bool operator == ( const C &lhs, const basic_var< A, B > &rhs )
	{
		return rhs == basic_var< A, B >( lhs );
	}

	template < template< class > class A, class B >
	inline std::ostream& operator << ( std::ostream &stream, const basic_var< A, B > &value )
	{
		return stream << value.serialize();
	}

	typedef basic_var< CopyOnWrite, char > var;
	typedef basic_var< CopyOnWrite, wchar_t > wvar;
}
