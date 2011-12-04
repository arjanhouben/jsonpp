#pragma once

#include <string>
#include <cstdlib>
#include <iterator>
#include <cstdlib>
#include <iterator>


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

			operator const var&() const { return _result; }

			template < class I >
			static var parse( I start, const I &end )
			{
				std::vector< var* > destinations;
				destinations.reserve( 1024 );

				var data;

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
							add_item( destinations, var( Object ) );
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
							add_item( destinations, var( Array ) );
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
				}
			}

			static std::string handle_escapes( typename std::basic_string< T > &result )
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

				typename std::basic_string< T >::const_iterator start = result.begin();
				const typename std::basic_string< T >::const_iterator &end = result.end();

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
									++start;
								case '"':
								case '\'':
									/* skip quotes */
									continue;
							}
							break;
						case 1:
							switch ( *start )
							{
								case '"':
									result.push_back( '"' );
									mode = Start;
									++start;
									continue;
								case '\'':
									result.push_back( '\'' );
									mode = Start;
									++start;
									continue;
								case '\\':
									result.push_back( '\\' );
									mode = Start;
									++start;
									continue;
								case '/':
									result.push_back( '/' );
									mode = Start;
									++start;
									continue;
								case 'b':
									result.push_back( '\b' );
									mode = Start;
									++start;
									continue;
								case 'f':
									result.push_back( '\f' );
									mode = Start;
									++start;
									continue;
								case 'n':
									result.push_back( '\n' );
									mode = Start;
									++start;
									continue;
								case 'r':
									result.push_back( '\r' );
									mode = Start;
									++start;
									continue;
								case 't':
									result.push_back( '\t' );
									mode = Start;
									++start;
									continue;
								case 'u':
									mode = EscapeUnicode_0;
									++start;
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
										std::stringstream stream( std::string( start - 4, start ) );

										int value = 0;

										stream >> std::hex >> value;

										result += utf8Encode( value );

										mode = Start;
									}
									++start;
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

					result.push_back( *start++ );
				}

				return result;
			}

			static bool check_for_number( typename std::basic_string< T > &string )
			{
#if 0
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
#endif
				return true;
			}

			static void add_string( std::vector< var *> &destinations, std::basic_string< T > &string )
			{
				if ( destinations.empty() ) return;

				strip_whitespace( string );

				if ( string.empty() ) return;
				// is this okay?

				if ( check_for_number( string) )
				{
#ifdef _MSC_VER
					std::stringstream stream( std::string( start, end ) );
					long double temp = 0;
					stream >> temp;
					add_item( destinations, var( temp ) );
#else
					add_item( destinations, var( strtold( &string[ 0 ], 0 ) ) );
#endif
				}
				else
				{
					/* check for NaN / null / true / false */
//					switch ( *s )
//					{
//						case 'N':
//							if ( ++s == end ) break;
//							if ( *s != 'a' ) break;
//							if ( ++s == end ) break;
//							if ( *s != 'N' ) break;
//							add_item( destinations, var( std::numeric_limits< long double >::quiet_NaN() ) );
//							return;
//						case 'n':
//							if ( ++s == end ) break;
//							if ( *s != 'u' ) break;
//							if ( ++s == end ) break;
//							if ( *s != 'l' ) break;
//							if ( ++s == end ) break;
//							if ( *s != 'l' ) break;
//							add_item( destinations, var( Null ) );
//							return;
//						case 't':
//							if ( ++s == end ) break;
//							if ( *s != 'r' ) break;
//							if ( ++s == end ) break;
//							if ( *s != 'u' ) break;
//							if ( ++s == end ) break;
//							if ( *s != 'e' ) break;
//							add_item( destinations, var( true ) );
//							return;
//						case 'f':
//							if ( ++s == end ) break;
//							if ( *s != 'a' ) break;
//							if ( ++s == end ) break;
//							if ( *s != 'l' ) break;
//							if ( ++s == end ) break;
//							if ( *s != 's' ) break;
//							if ( ++s == end ) break;
//							if ( *s != 'e' ) break;
//							add_item( destinations, var( false ) );
//							return;
//					}

					const std::string &result( handle_escapes( string ) );

					add_item( destinations, var( result ) );
				}
			}

			static void add_item( std::vector< var *> &destinations, const var &item )
			{
				if ( destinations.empty() ) return;

				var &destination( *destinations.back() );

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

			const var _result;
	};

	typedef basic_parser< char > parser;
	typedef basic_parser< wchar_t > wparser;
}
