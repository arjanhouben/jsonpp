#pragma once

#include <iostream>
#include <sstream>

namespace json
{
	enum Types
	{
		Undefined = 0,
		Null = 1 << 0,
		Bool = 1 << 1,
		Number = 1 << 2,
		String = 1 << 3,
		Array = 1 << 4,
		Object = 1 << 5,
		TypeCount
	};

	std::ostream& operator << ( std::ostream &stream, Types type )
	{
		switch ( type )
		{
			case Undefined:
				return stream << "Undefined";
			case Null:
				return stream << "Null";
			case Bool:
				return stream << "Bool";
			case Number:
				return stream << "Number";
			case String:
				return stream << "String";
			case Array:
				return stream << "Array";
			case Object:
				return stream << "Object";
			default:
				return stream << "unknown type";
		}
	}

	enum Markup
	{
		Compact = 1 << 0,
		HumanReadable = 1 << 1,
		CountArrayValues = 1 << 2,
		IndentFirstItem = 1 << 3
	};

	template < class Char >
	std::basic_string< Char > convert_string( const std::string &string )
	{
		return std::basic_string< Char >( string.begin(), string.end() );
	}

	template< class Key, class Value >
	struct key_value
	{
		explicit key_value() : key(), value() { }

		explicit key_value( const Value &v ) : key(), value( v ) { }

		explicit key_value( const Key &k, const Value &v ) : key( k ), value( v ) { }

		explicit key_value( const Key &k ) : key( k ), value() { }

		key_value& operator = ( const key_value &rhs )
		{
			const_cast< Key& >( key ) = rhs.key;
			value = rhs.value;
			return *this;
		}

		bool operator == ( const Key &k ) const
		{
			return key == k;
		}

		bool operator == ( const key_value< Key, Value > &rhs ) const
		{
			return key == rhs.key && value == rhs.value;
		}

		Key key;
		Value value;
	};

	template < class T >
	class Buffer
	{
		public:

			typedef typename std::vector< T >::const_iterator const_iterator;

			typedef typename std::vector< T >::iterator iterator;

			typedef T value_type;

			Buffer() :
				_buffer(),
				_p( _buffer.begin() ) { }

			bool operator == ( const char *str ) const
			{
				const_iterator i = _buffer.begin();
				while ( *str && i != end() )
				{
					if ( *str != *i ) return false;
					++str;
					++i;
				}

				return true;
			}

			operator std::basic_string< T >() const
			{
				return std::basic_string< T >( _buffer.begin(), end() );
			}

			void push_back( const T &t )
			{
				if ( _p == _buffer.end() )
				{
					_buffer.push_back( t );
					_p = _buffer.end();
				}
				else
				{
					*_p++ = t;
				}
			}

			const_iterator begin() const
			{
				return _buffer.begin();
			}

			iterator begin()
			{
				return _buffer.begin();
			}

			const_iterator end() const
			{
				return _p;
			}

			iterator end()
			{
				return _p;
			}

			void clear()
			{
				_p = _buffer.begin();
			}

			void append( const std::basic_string< T > &str )
			{
				typename std::basic_string< T >::const_iterator i = str.begin();
				while ( i != str.end() )
				{
					push_back( *i );
					++i;
				}
			}

			void append( const_iterator s, const const_iterator &e )
			{
				while ( s != e )
				{
					push_back( *s );
					++s;
				}
			}

			bool empty() const
			{
				return _p == _buffer.begin();
			}

		private:

			std::vector< T > _buffer;
			iterator _p;
	};


	template < class Q, class Number >
	Number dec_string_to_number( typename Q::const_iterator start, const typename Q::const_iterator &end )
	{
		Number r = 0;
		std::basic_stringstream< typename Q::value_type > stream;
		while ( start != end ) stream << *start++;
		stream >> r;
		return r;
	}

	template <>
	inline long double dec_string_to_number< std::basic_string< char >, long double >( std::basic_string< char >::const_iterator start, const std::basic_string< char >::const_iterator & )
	{
		return strtold( &start[ 0 ], 0 );
	}

	template <>
	inline long double dec_string_to_number< Buffer< char >, long double >( Buffer< char >::const_iterator start, const Buffer< char >::const_iterator & )
	{
		return strtold( &start[ 0 ], 0 );
	}

	template < class Q, class Number >
	Number hex_string_to_number( typename Q::const_iterator start, const typename Q::const_iterator &end )
	{
		Number r = 0;
		std::basic_stringstream< typename Q::value_type > stream;
		while ( start != end ) stream << *start++;
		stream >> std::hex >> r;
		return r;
	}

	template <>
	inline int hex_string_to_number< std::basic_string< char >, int >( std::basic_string< char >::const_iterator start, const std::basic_string< char >::const_iterator & )
	{
		return strtol( &start[ 0 ], 0, 16 );
	}

	template <>
	inline int hex_string_to_number< Buffer< char >, int >( Buffer< char >::const_iterator start, const Buffer< char >::const_iterator & )
	{
		return strtol( &start[ 0 ], 0, 16 );
	}

	template < class T >
	class DefaultCopyBehaviour
	{
		public:

			explicit DefaultCopyBehaviour( const T &t ) : _t( t ) { }

			T* operator ->()
			{
				return &_t;
			}

			const T* operator ->() const
			{
				return &_t;
			}

		private:

			T _t;
	};

	template < class T >
	class CopyOnWrite
	{
		public:

			explicit CopyOnWrite( const T &t ) :
				_t( new T( t ) ) { }

			T* operator ->()
			{
				if ( !_t.unique() )
				{
					_t = std::tr1::shared_ptr< T >( new T( *_t.get() ) );
				}
				return _t.get();
			}

			const T* operator ->() const
			{
				return _t.get();
			}

		private:

			std::tr1::shared_ptr< T > _t;
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

	class exception : public std::exception
	{
		public:

			exception( const std::string &string = std::string() ) :
				_message( string ) { }

			virtual ~exception() throw() {}

			virtual const char* what() const throw()
			{
				return _message.c_str();
			}

			template < class T >
			exception& operator << ( const T &t )
			{
				std::string::size_type s = _message.size();
				if ( s-- && !std::isspace( _message.at( s ) ) )
				{
					_message.push_back( ' ' );
				}
				std::stringstream stream;
				stream << _message << t;
				_message = stream.str();
				return *this;
			}

		private:

			std::string _message;
	};
}
