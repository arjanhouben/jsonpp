#pragma once

#include <string>
#include <cstdlib>
#include <iterator>
#include <cstdlib>
#include <iterator>
#include <list>b

#include <jsonpp/var.h>

namespace json
{
	template < template< class > class CopyBehaviour, class T >
	class basic_parser
	{
		public:

			typedef std::basic_string< T > string_type;

			basic_parser( const T str[] ) :
				_string_value_buffer(),
				_string_value_whitespace_buffer(),
				_handle_escape_buffer(),
				_result( parse( str, str + strlen( str ) ) ) { }

			basic_parser( const string_type &string ) :
				_string_value_buffer(),
				_string_value_whitespace_buffer(),
				_handle_escape_buffer(),
				_result( parse( string.begin(), string.end() ) ) { }

			basic_parser( std::basic_istream< T > &stream ) :
				_string_value_buffer(),
				_string_value_whitespace_buffer(),
				_handle_escape_buffer(),
				_result( parse( std::istream_iterator< T >( stream ), std::istream_iterator< T >() ) ) { }

			operator const basic_var< CopyBehaviour, T >&() const { return _result; }

			template < class I >
			basic_var< CopyBehaviour, T > parse( I start, const I &end )
			{
				std::vector< basic_var< CopyBehaviour, T >* > destinations;

				basic_var< CopyBehaviour, T > root( Undefined );
				destinations.push_back( &root );

				while ( start != end )
				{
					switch ( *start )
					{
						case '{': // start object
							add_item( destinations, basic_var< CopyBehaviour, T >( Object ) );
							++start;
							break;
						case '[': // add array
							add_item( destinations, basic_var< CopyBehaviour, T >( Array ) );
							++start;
							break;
						case '}': // close object
						case ']': // close array
							if ( destinations.empty() ) throw "empty array";
							destinations.pop_back();
							++start;
							break;
						case ':': // add property
						case ',': // add destination
							++start;
							break;
						case ' ': case '\t': case '\r': case '\n':
							++start;
							break;
						case '"': // handle string
							start = string_value< '"' >( destinations, ++start, end );
							break;
						case '\'': // handle string
							start = string_value< '\'' >( destinations, ++start, end );
							break;
						default: // handle string/number/literal
							start = string_value( destinations, start, end );
							break;
					}
				}

				return root;
			}

		private:

			template < T EndChar, class I >
			I string_value( std::vector< basic_var< CopyBehaviour, T >* > &destination, const I &start, const I &end )
			{
				I i = start;

				_string_value_buffer.clear();

				while ( i != end )
				{
					switch ( *i )
					{
						case '\\':
							_string_value_buffer.append( handle_escape( ++i, end ) );
							break;
						case EndChar:
							add_item( destination, _string_value_buffer );
							return ++i;
						default:
							_string_value_buffer.push_back( *i );
					}

					++i;
				}

				add_item( destination, _string_value_buffer );

				return i;
			}

			template < class I >
			I string_value( std::vector< basic_var< CopyBehaviour, T >* > &destination, const I &start, const I &end )
			{
				I i = start;

				_string_value_whitespace_buffer.clear();
				_string_value_buffer.clear();

				while ( i != end )
				{
					switch ( *i )
					{
						case '\\':
							_string_value_buffer.append( handle_escape( ++i, end ) );
							break;
						case ',':
						case ':':
						case '}':
						case ']':
							if ( check_for_number( _string_value_buffer ) )
							{
								/* make sure the buffer is zero-delimited */
								_string_value_buffer.push_back( 0 );

								add_item( destination, dec_string_to_number< Buffer< T >, long double >( _string_value_buffer.begin(), _string_value_buffer.end() ) );
							}
							else
							{
								if ( _string_value_buffer == "null" )
								{
									add_item( destination, Null );
								}
								else if ( _string_value_buffer == "true" )
								{
									add_item( destination, true );
								}
								else if ( _string_value_buffer == "false" )
								{
									add_item( destination, false );
								}
								else
								{
									add_item( destination, _string_value_buffer );
								}
							}
							return i;
						case '\r':
						case '\n':
						case '\t':
						case ' ':
							_string_value_whitespace_buffer.push_back( *i++ );
							continue;
					}

					if ( !_string_value_whitespace_buffer.empty() )
					{
						_string_value_buffer.append( _string_value_whitespace_buffer.begin(), _string_value_whitespace_buffer.end() );
						_string_value_whitespace_buffer.clear();
					}

					_string_value_buffer.push_back( *i );

					++i;
				}

				add_item( destination, _string_value_buffer );

				return i;
			}

			template < class I >
			string_type handle_escape( I &start, const I &end )
			{
				_handle_escape_buffer.clear();

				while ( start != end )
				{
					switch ( *start )
					{
						case '"':
						case '\'':
						case '\\':
						case '/':
							_handle_escape_buffer.push_back( *start );
							return _handle_escape_buffer;
						case 'b':
							_handle_escape_buffer.push_back( '\b' );
							return _handle_escape_buffer;
						case 'f':
							_handle_escape_buffer.push_back( '\f' );
							return _handle_escape_buffer;
						case 'n':
							_handle_escape_buffer.push_back( '\n' );
							return _handle_escape_buffer;
						case 'r':
							_handle_escape_buffer.push_back( '\r' );
							return _handle_escape_buffer;
						case 't':
							_handle_escape_buffer.push_back( '\t' );
							return _handle_escape_buffer;
						case 'u':
						{
							++start;
							int m = 0;
							while ( start != end && ++m < 5 )
							{
								switch ( *start )
								{
									case '0':	case 'a':	case 'A':
									case '1':	case 'b':	case 'B':
									case '2':	case 'c':	case 'C':
									case '3':	case 'd':	case 'D':
									case '4':	case 'e':	case 'E':
									case '5':	case 'f':	case 'F':
									case '6':
									case '7':
									case '8':
									case '9':
										++start;
										_handle_escape_buffer.push_back( *start );
									default:
										m = 5;
										break;
								}
							}

							return utf8Encode< T >( hex_string_to_number< Buffer< T >, int >( _handle_escape_buffer.begin(), _handle_escape_buffer.end() ) );
						}
						default:
							return _handle_escape_buffer;
					}
				}

				return _handle_escape_buffer;
			}

			template < class Q >
			bool check_for_number( const Q &string )
			{
				typename Q::const_iterator start = string.begin();
				const typename Q::const_iterator &end = string.end();

				unsigned int validNumber = 1;

				/* determine type of string */
				while ( start != end )
				{
					switch ( validNumber )
					{
						case 1:
							switch ( *start )
							{
								case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
									validNumber = 2;
									break;
								default:
									return false;
							}
							break;
						case 2:
							switch ( *start )
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
									return false;
							}
							break;
						case 3:
							switch ( *start )
							{
								case 'e':
								case 'E':
									validNumber = 4;
									break;
								case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
									break;
								default:
									return false;
							}
							break;
						case 4:
							switch ( *start )
							{
								case '+': case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
									validNumber = 5;
									break;
								default:
									return false;
							}
							break;
						case 5:
							switch ( *start )
							{
								case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
									break;
								default:
									return false;
							}
							break;
					}

					++start;
				}

				return true;
			}

			void add_item( std::vector< basic_var< CopyBehaviour, T > *> &destinations, const basic_var< CopyBehaviour, T > &item )
			{
				if ( destinations.empty() ) return;

				basic_var< CopyBehaviour, T > &destination( *destinations.back() );

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
						destinations.push_back( &destination[ item.operator string_type() ] );
						break;
					case Array:
						destination.push( item );
						if ( item.type == Array || item.type == Object ) destinations.push_back( &destination.back() );
						break;
				}
			}

			Buffer< T > _string_value_buffer, _string_value_whitespace_buffer, _handle_escape_buffer;

			const basic_var< CopyBehaviour, T > _result;
	};

	typedef basic_parser< CopyOnWrite, char > parser;
	typedef basic_parser< CopyOnWrite, wchar_t > wparser;
}
