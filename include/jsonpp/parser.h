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
	template < class T >
	class basic_parser
	{
		public:

			basic_parser( const T str[] ) :
				_result( parse( str, str + strlen( str ) ) ) { }

			basic_parser( const std::basic_string< T > &string ) :
				_result( parse( string.begin(), string.end() ) ) { }

			basic_parser( std::basic_istream< T > &stream ) :
				_result( parse( std::istream_iterator< T >( stream ), std::istream_iterator< T >() ) ) { }

			operator const basic_var< T >&() const { return _result; }

			template < class I >
			static basic_var< T > parse( I start, const I &end )
			{
				std::vector< basic_var< T >* > destinations;
				destinations.reserve( 1024 );

				basic_var< T > data;

				destinations.push_back( &data );

				enum Store
				{
					Save = 0,
					Skip = 1 << 0,
					Clear = 1 << 1
				};

				bool doubleString = false, singleString = false;
				int escape = 0;

				std::basic_string< T > buffer;

				while ( start != end )
				{
					int store = Save;

					switch ( *start )
					{
						case '{': // start object
							if ( singleString || doubleString ) break;
							// add object
							add_item( destinations, basic_var< T >( Object ) );
							store = Skip | Clear;
							break;
						case '}': // close object
							if ( singleString || doubleString ) break;
							if ( !buffer.empty() ) add_string( destinations, buffer );
							store = Skip | Clear;
							if ( destinations.empty() ) throw "empty array";
							destinations.pop_back();
							break;
						case '[': // add array
							if ( singleString || doubleString ) break;
							add_item( destinations, basic_var< T >( Array ) );
							store = Skip | Clear;
							break;
						case ']': // close array
							if ( singleString || doubleString ) break;
							if ( !buffer.empty() ) add_string( destinations, buffer );
							store = Skip | Clear;
							if ( destinations.empty() ) throw "empty array";
							destinations.pop_back();
							break;
						case ':': // add property
							if ( singleString || doubleString ) break;
							add_string( destinations, buffer );
							store = Skip | Clear;
							break;
						case ',': // add destination
							if ( singleString || doubleString ) break;
							if ( !buffer.empty() ) add_string( destinations, buffer );
							store = Skip | Clear;
							break;
						case ' ': case '\t': case '\r': case '\n':
							if ( buffer.empty() )
							{
								store = Skip;
							}
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
						buffer.push_back( *start );
					}
					else if ( store & Clear )
					{
						buffer.clear();
					}

					++start;
				}

				/* if data remains, append it */
				if ( !buffer.empty() ) add_string( destinations, buffer );

				return data;
			}

		private:

			static void strip_whitespace( typename std::basic_string< T > &string )
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

			static std::basic_string< T > handle_escapes( typename std::basic_string< T > &result )
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

				typename std::basic_string< T >::iterator start = result.begin();
				const typename std::basic_string< T >::const_iterator &end = result.end();
				typename std::basic_string< T >::iterator unicodeStart;

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
										std::stringstream stream( std::basic_string< T >( start - 4, start ) );

										int value = 0;

										stream >> std::hex >> value;

										const std::basic_string< T > &utf8( utf8Encode< T >( value ) );

										typename std::basic_string< T >::const_iterator s = utf8.begin();
										const typename std::basic_string< T >::const_iterator &e = utf8.end();

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

//					result.push_back( *start++ );
				}

				return result;
			}

			static bool check_for_number( const std::basic_string< T > &string )
			{
				typename std::basic_string< T >::const_iterator start = string.begin();
				const typename std::basic_string< T >::const_iterator &end = string.end();

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

			static void add_string( std::vector< basic_var< T > *> &destinations, std::basic_string< T > &string )
			{
				if ( destinations.empty() ) return;

				strip_whitespace( string );

				if ( check_for_number( string) )
				{
#ifdef _MSC_VER
					std::basic_string< T >stream stream( std::basic_string< T >( start, end ) );
					long double temp = 0;
					stream >> temp;
					add_item( destinations, basic_var< T >( temp ) );
#else
					add_item( destinations, basic_var< T >( strtold( &string[ 0 ], 0 ) ) );
#endif
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
								add_item( destinations, basic_var< T >( std::numeric_limits< long double >::quiet_NaN() ) );
								return;
							}
							break;
						case 4:
							if( string.at( 0 ) == 't' &&
								string.at( 1 ) == 'r' &&
								string.at( 2 ) == 'u' &&
								string.at( 3 ) == 'e' )
							{
								add_item( destinations, basic_var< T >( true ) );
								return;
							}
							else if( string.at( 0 ) == 'n' &&
									 string.at( 1 ) == 'u' &&
									 string.at( 2 ) == 'l' &&
									 string.at( 3 ) == 'l' )
							{
								add_item( destinations, basic_var< T >( Null ) );
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
								add_item( destinations, basic_var< T >( false ) );
								return;
							}
							break;
					}

					add_item( destinations, basic_var< T >( handle_escapes( string ) ) );
				}
			}

			static void add_item( std::vector< basic_var< T > *> &destinations, const basic_var< T > &item )
			{
				if ( destinations.empty() ) return;

				basic_var< T > &destination( *destinations.back() );

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
						destinations.push_back( &destination[ item.operator std::basic_string< T >() ] );
						break;
					case Array:
						destination.push( item );
						if ( item.type == Array || item.type == Object ) destinations.push_back( &destination.back() );
						break;
				}
			}

			const basic_var< T > _result;
	};

	typedef basic_parser< char > parser;
	typedef basic_parser< wchar_t > wparser;
}
