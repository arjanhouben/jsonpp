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
				_result( parse( str, str + strlen( str ) ) ) { }

			basic_parser( const string_type &string ) :
				_result( parse( string.begin(), string.end() ) ) { }

			basic_parser( std::basic_istream< T > &stream ) :
				_result( parse( std::istream_iterator< T >( stream ), std::istream_iterator< T >() ) ) { }

			operator const basic_var< CopyBehaviour, T >&() const { return _result; }

			template < class I >
			static basic_var< CopyBehaviour, T > parse( I start, const I &end )
			{
				std::vector< basic_var< CopyBehaviour, T >* > destinations;
				destinations.reserve( 1024 );

				basic_var< CopyBehaviour, T > data;

				destinations.push_back( &data );

				while ( start != end )
				{
					switch ( *start )
					{
						case '{': // start object
							// add object
							add_item( destinations, basic_var< CopyBehaviour, T >( Object ) );
							++start;
							break;
						case '}': // close object
//							if ( !buffer.empty() ) add_string( destinations, buffer );
							if ( destinations.empty() ) throw "empty array";
							destinations.pop_back();
							++start;
							break;
						case '[': // add array
							add_item( destinations, basic_var< CopyBehaviour, T >( Array ) );
							++start;
							break;
						case ']': // close array
//							if ( !buffer.empty() ) add_string( destinations, buffer );
							if ( destinations.empty() ) throw "empty array";
							destinations.pop_back();
							++start;
							break;
						case ':': // add property
//							add_string( destinations, buffer );
							++start;
							break;
						case ',': // add destination
//							if ( !buffer.empty() ) add_string( destinations, buffer );
							++start;
							break;
						case ' ': case '\t': case '\r': case '\n':
							++start;
							break;
						case '"':
							// handle string
							start = string_value< '"' >( destinations, ++start, end );
							break;
						case '\'':
							// handle string
							start = string_value< '\'' >( destinations, ++start, end );
							break;
						case '\\':
							// handle escape
							start = string_value( destinations, start, end );
							break;
						default:
							start = string_value( destinations, start, end );
							break;
					}
				}

				/* if data remains, append it */
//				if ( !buffer.empty() ) add_string( destinations, buffer );
				return data;
			}

		private:

			template < T EndChar, class I >
			static I string_value( std::vector< basic_var< CopyBehaviour, T >* > &destination, const I &start, const I &end )
			{
				I i = start;

				string_type str;
				str.reserve( 1024 );

				while ( i != end )
				{
					switch ( *i )
					{
						case '\\':
							str.append( handle_escape( ++i, end ) );
							break;
						case EndChar:
							add_item( destination, str );
							return ++i;
						default:
							str.push_back( *i );
					}

					++i;
				}

				add_item( destination, str );

				return i;
			}

			template < class I >
			static I string_value( std::vector< basic_var< CopyBehaviour, T >* > &destination, const I &start, const I &end )
			{
				I i = start;

				string_type str, whitespace;
				str.reserve( 1024 );

				while ( i != end )
				{
					switch ( *i )
					{
						case '\\':
							str.append( handle_escape( ++i, end ) );
							break;
						case ',':
						case ':':
						case '}':
						case ']':
							if ( check_for_number( str ) )
							{
								add_item( destination, dec_string_to_number< T, long double >( str.begin(), str.end() ) );
							}
							else
							{
								if ( str == "null" )
								{
									add_item( destination, Null );
								}
								else if ( str == "true" )
								{
									add_item( destination, true );
								}
								else if ( str == "false" )
								{
									add_item( destination, false );
								}
								else
								{
									add_item( destination, str );
								}
							}
							return ++i;
						case '\r':
						case '\n':
						case '\t':
						case ' ':
							whitespace.push_back( *i++ );
							continue;
					}

					if ( !whitespace.empty() )
					{
						str.append( whitespace.begin(), whitespace.end() );
						whitespace.clear();
					}

					str.push_back( *i );

					++i;
				}

				add_item( destination, str );

				return i;
			}

			template < class I >
			static string_type handle_escape( I &start, const I &end )
			{
				string_type str;

				while ( start != end )
				{
					switch ( *start )
					{
						case '"':
						case '\'':
						case '\\':
						case '/':
							str.push_back( *start );
							return str;
						case 'b':
							str.push_back( '\b' );
							return str;
						case 'f':
							str.push_back( '\f' );
							return str;
						case 'n':
							str.push_back( '\n' );
							return str;
						case 'r':
							str.push_back( '\r' );
							return str;
						case 't':
							str.push_back( '\t' );
							return str;
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
										str.push_back( *start );
									default:
										m = 5;
										break;
								}
							}

							return utf8Encode< T >( hex_string_to_number< T, int >( str.begin(), str.end() ) );
						}
						default:
							return str;
					}
				}

				return str;
			}

			static void strip_whitespace( string_type &string )
			{
				while ( !string.empty() )
				{
					switch ( string.at( 0 ) )
					{
						case ' ':
						case '\t':
						case '\r':
						case '\n':
							string.erase( string.begin() );
							continue;
					}

					switch ( *( string.end() - 1 ) )
					{
						case ' ':
						case '\t':
						case '\r':
						case '\n':
							string.erase( string.end() - 1 );
							continue;
					}

					break;
				}
			}

			static string_type handle_escapes( string_type &result )
			{
				enum Mode
				{
					Start,
					StartEscape,
					EscapeUnicode_0,
					EscapeUnicode_1,
					EscapeUnicode_2,
					EscapeUnicode_3,
					EscapeUnicodeDone
				};

				const unsigned int SingleQuote = 1;
				const unsigned int DoubleQuote = 2;

				typename string_type::iterator start = result.begin();
				const typename string_type::const_iterator &end = result.end();
				typename string_type::iterator unicodeStart;

				unsigned int quote = SingleQuote | DoubleQuote;
				Mode mode = Start;
				while ( start != end )
				{
					switch ( mode )
					{
						case Start:
							switch ( *start )
							{
								case '\\':
									mode = StartEscape;
									start = result.erase( start );
									unicodeStart = start;
									break;
								case '"':
									if ( quote & DoubleQuote )
									{
										start = result.erase( start );
										quote &= DoubleQuote;
									}
									else
									{
										++start;
									}
									break;
								case '\'':
									if ( quote & SingleQuote )
									{
										start = result.erase( start );
										quote &= SingleQuote;
									}
									else
									{
										++start;
									}
									break;
								default:
									if ( quote == ( SingleQuote | DoubleQuote ) )
									{
										quote = 0;
									}
									// fall through
								case ' ':
									++start;
									continue;
							}
							break;
						case StartEscape:
							switch ( *start )
							{
								case '"':
								case '\'':
								case '\\':
								case '/':
									mode = Start;
									++start;
									continue;
								case 'b':
									*start = '\b';
									mode = Start;
									++start;
									continue;
								case 'f':
									*start = '\f';
									mode = Start;
									++start;
									continue;
								case 'n':
									*start = '\n';
									mode = Start;
									++start;
									continue;
								case 'r':
									*start = '\r';
									mode = Start;
									++start;
									continue;
								case 't':
									*start = '\t';
									mode = Start;
									++start;
									continue;
								case 'u':
									mode = EscapeUnicode_0;
									continue;
							}
							break;
						case EscapeUnicode_0:
						case EscapeUnicode_1:
						case EscapeUnicode_2:
						case EscapeUnicode_3:
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
									mode = static_cast< Mode >( static_cast< int >( mode ) + 1 );
									if ( mode == EscapeUnicodeDone )
									{
										int value = hex_string_to_number< T, int >( start - 4, start );

										const string_type &utf8( utf8Encode< T >( value ) );

										typename string_type::const_iterator s = utf8.begin();
										const typename string_type::const_iterator &e = utf8.end();

										while ( unicodeStart != start && s != e )
										{
											*unicodeStart++ = *s++;
										}

										if ( unicodeStart == start )
										{
											result.insert( start, s, e );
										}
										else if ( s == e )
										{
											result.erase( unicodeStart, start );
										}

										mode = Start;
									}
									else
									{
										start = result.erase( start );
									}
									continue;
								default:
									/* invalid unicode escape */
									mode = Start;
									break;
							}
							break;
						case EscapeUnicodeDone:
							break;
					}
				}

				return result;
			}

			static bool check_for_number( const string_type &string )
			{
				typename string_type::const_iterator start = string.begin();
				const typename string_type::const_iterator &end = string.end();

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

			static void add_string( std::vector< basic_var< CopyBehaviour, T >* > &destinations, string_type &string )
			{
				if ( destinations.empty() ) return;

				strip_whitespace( string );

				if ( check_for_number( string ) )
				{
					add_item( destinations, basic_var< CopyBehaviour, T >( dec_string_to_number< T, long double >( string.begin(), string.end() ) ) );
				}
				else
				{
					/* check for NaN / null / true / false */
					switch ( string.size() )
					{
						case 3:
							if( string.at( 0 ) == 'N' &&
								string.at( 1 ) == 'a' &&
								string.at( 2 ) == 'N' )
							{
								add_item( destinations, basic_var< CopyBehaviour, T >( std::numeric_limits< long double >::quiet_NaN() ) );
								return;
							}
							break;
						case 4:
							if( string.at( 0 ) == 't' &&
								string.at( 1 ) == 'r' &&
								string.at( 2 ) == 'u' &&
								string.at( 3 ) == 'e' )
							{
								add_item( destinations, basic_var< CopyBehaviour, T >( true ) );
								return;
							}
							else if( string.at( 0 ) == 'n' &&
									 string.at( 1 ) == 'u' &&
									 string.at( 2 ) == 'l' &&
									 string.at( 3 ) == 'l' )
							{
								add_item( destinations, basic_var< CopyBehaviour, T >( Null ) );
								return;
							}
							break;
						case 5:
							if( string.at( 0 ) == 'f' &&
								string.at( 1 ) == 'a' &&
								string.at( 2 ) == 'l' &&
								string.at( 3 ) == 's' &&
								string.at( 4 ) == 'e' )
							{
								add_item( destinations, basic_var< CopyBehaviour, T >( false ) );
								return;
							}
							break;
					}

					add_item( destinations, basic_var< CopyBehaviour, T >( handle_escapes( string ) ) );
				}
			}

			static void add_item( std::vector< basic_var< CopyBehaviour, T > *> &destinations, const basic_var< CopyBehaviour, T > &item )
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

			const basic_var< CopyBehaviour, T > _result;
	};

	typedef basic_parser< DefaultCopyBehaviour, char > parser;
	typedef basic_parser< DefaultCopyBehaviour, wchar_t > wparser;
}
