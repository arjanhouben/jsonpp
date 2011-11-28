#ifndef JSONPP_STRING_H
#define JSONPP_STRING_H

#include <string>
#include <algorithm>

#include <tr1/memory>

namespace json
{

	typedef std::tr1::shared_ptr< std::string > data;

	bool operator<( const data &a, const std::string &b )
	{
		return *a.get() < b;
	}

	class string
	{
		public:

			typedef std::tr1::shared_ptr< std::string > data;
			typedef std::string::iterator iterator;
			typedef std::string::const_iterator const_iterator;

			string() :
				_string( new std::string() ),
				_begin( _string->begin() ),
				_end( _string->end() ) { }

			string( unsigned int i, char c ) :
				_string( new std::string( i, c ) ),
				_begin( _string->begin() ),
				_end( _string->end() )  { }

			string( const char *str ) :
				_string( new std::string( str ) ),
				_begin( _string->begin() ),
				_end( _string->end() )  { }

			string( char str ) :
				_string( new std::string( 1, str ) ),
				_begin( _string->begin() ),
				_end( _string->end() )  { }

			string( const std::string &str ) :
				_string( new std::string( str ) ) ,
				_begin( _string->begin() ),
				_end( _string->end() ) { }

			operator const std::string&() const
			{
				return *_string.get();
			}

			string& operator += ( const string &str )
			{
				*_string.get() += *str._string.get();
				setIterators();
				return *this;
			}

			bool empty() const
			{
				return _string->empty();
			}

			bool operator == ( const string &str ) const
			{
				return _string == str._string;
			}

			bool operator != ( const string &str ) const
			{
				return !operator == ( str );
			}

			void clear()
			{
				_string->clear();
				setIterators();
			}

			void push_back( const char &c )
			{
				_string->push_back( c );
				setIterators();
			}

			iterator begin() { return _begin; }

			iterator end() { return _end; }

			const_iterator begin() const { return _begin; }

			const_iterator end() const { return _end; }

		private:

			void setIterators()
			{
				_begin = _string->begin();
				_end = _string->end();
			}

			data _string;
			iterator _begin, _end;
	};

	string operator + ( const std::string &a, const string &b )
	{
		return string( a ) += b;
	}
}

#endif // JSONPP_STRING_H
