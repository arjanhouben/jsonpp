#ifndef JSONPP_PARSER_H
#define JSONPP_PARSER_H

#include <string>

#include <jsonpp/value.h>
#include <jsonpp/string.h>

namespace json
{
	class parser
	{
		public:

			parser() :
				_result() { }

			parser( const char string[] ) :
				_result( parse( string, string + strlen( string ) ) ) { }

			parser( const std::string &string ) :
				_result( parse( string.begin(), string.end() ) ) { }

			parser( std::istream &stream ) :
				_result( parse( std::istream_iterator< char >( stream ), std::istream_iterator< char >() ) ) { }

			operator const var&() const { return _result; }

			template < class T >
			static var parse( T start, const T end )
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

				string literal;

				while ( start != end )
				{
					int store = Save;

					switch ( *start )
					{
						case '{': // start object
							if ( singleString || doubleString ) break;
							// add object
							add( destinations, var( Object ) );
							store = Skip | Clear;
							break;
						case '}': // close object
							if ( singleString || doubleString ) break;
							if ( !literal.empty() ) add( destinations, literal );
							store = Skip | Clear;
							if ( destinations.empty() ) throw "empty array";
							destinations.pop_back();
							break;
						case '[': // add array
							if ( singleString || doubleString ) break;
							add( destinations, var( Array ) );
							store = Skip | Clear;
							break;
						case ']': // close array
							if ( singleString || doubleString ) break;
							if ( !literal.empty() ) add( destinations, literal );
							store = Skip | Clear;
							if ( destinations.empty() ) throw "empty array";
							destinations.pop_back();
							break;
						case ':': // add property
							if ( singleString || doubleString ) break;
							add( destinations, literal );
							store = Skip | Clear;
							break;
						case ',': // add destination
							if ( singleString || doubleString ) break;
							if ( !literal.empty() ) add( destinations, literal );
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
						literal.push_back( *start );
					}
					else if ( store & Clear )
					{
						literal.clear();
					}

					++start;
				}

				/* if data remains, append it */
				if ( !literal.empty() ) add( destinations, literal );

				return data;
			}

		private:

			static void add( std::vector< var *> &destinations, const string &str )
			{
				if ( destinations.empty() ) return;

				string::const_iterator start = str.begin(), end = str.end();

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

					switch ( *(end-1 ) )
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

					string::const_iterator i = start;

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

					if ( validNumber )
					{
						long double v = 0 ;
						std::stringstream stream( std::string( start, end ) );
						stream >> v;
						add( destinations, var( v ) );
					}
					else
					{
						switch ( end - start )
						{
							case 3:
								if ( start[ 0 ] == 'N' && start[ 1 ] == 'a' && start[ 2 ] == 'N' )
								{
									add( destinations, var( std::numeric_limits< long double >::quiet_NaN() ) );
									return;
								}
								break;
							case 4:
								switch ( *start )
								{
									case 'n':
										if ( start[ 1 ] == 'u' && start[ 2 ] == 'l' && start[ 3 ] == 'l' )
										{
											add( destinations, var( Null ) );
											return;
										}
									case 't':
										if ( start[ 1 ] == 'r' && start[ 2 ] == 'u' && start[ 3 ] == 'e' )
										{
											add( destinations, var( true ) );
											return;
										}
								}
								break;
							case 5:
								if ( start[ 0 ] == 'f' && start[ 1 ] == 'a' && start[ 2 ] == 'l' && start[ 3 ] == 's' && start[ 4 ] == 'e' )
								{
									add( destinations, var( false ) );
									return;
								}
								break;
						}

						// skip quotes
						if ( *start == '"' || *start == '\'' ) ++start;

						// skip quotes
						if ( *(end-1 ) == '"' || *(end-1 ) == '\'' ) --end;

						std::string result;
						result.reserve( end - start );

						int mode = 0;
						while ( start != end )
						{
							switch ( mode )
							{
								case 0:
									if ( *start == '\\' )
									{
										mode = 1;
										++start;
										continue;
									}
									break;
								case 1:
									switch ( *start )
									{
										case '"':
											result.push_back( '"' );
											mode = 0;
											++start;
											continue;
										case '\'':
											result.push_back( '\'' );
											mode = 0;
											++start;
											continue;
										case '\\':
											result.push_back( '\\' );
											mode = 0;
											++start;
											continue;
										case '/':
											result.push_back( '/' );
											mode = 0;
											++start;
											continue;
										case 'b':
											result.push_back( '\b' );
											mode = 0;
											++start;
											continue;
										case 'f':
											result.push_back( '\f' );
											mode = 0;
											++start;
											continue;
										case 'n':
											result.push_back( '\n' );
											mode = 0;
											++start;
											continue;
										case 'r':
											result.push_back( '\r' );
											mode = 0;
											++start;
											continue;
										case 't':
											result.push_back( '\t' );
											mode = 0;
											++start;
											continue;
										case 'u':
											mode = 2;
											++start;
											continue;
									}
									break;
								case 2:
								case 3:
								case 4:
								case 5:
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
											if ( ++mode > 5 )
											{
												std::stringstream stream( std::string( start - 3, start + 1 ) );

												int value = 0;

												stream >> std::hex >> value;

												result += utf8Encode( value );

												mode = 0;
											}
											++start;
											continue;
										default:
											break;
									}
									break;
							}

							result.push_back( *start++ );
						}

						add( destinations, var( result ) );
					}
				}
			}

			static void add( std::vector< var *> &destinations, const var &item )
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
						destinations.push_back( &destination[ item.operator string() ] );
						break;
					case Array:
						destination.push( item );
						if ( item.type == Array || item.type == Object ) destinations.push_back( &destination.back() );
						break;
				}
			}

			const var _result;
	};
}

#endif // JSONPP_PARSER_H
