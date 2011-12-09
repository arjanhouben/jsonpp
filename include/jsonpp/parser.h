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
			const basic_var< CopyBehaviour, T >& parse( I start, const I &end )
			{
				std::vector< basic_var< CopyBehaviour, T >* > destinations;
//				destinations.reserve( 1024 );

				_result = Undefined;
				destinations.push_back( &_result );

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

				return _result;
			}

		private:

			template < T EndChar, class I >
			I string_value( std::vector< basic_var< CopyBehaviour, T >* > &destination, const I &start, const I &end )
			{
				I i = start;

				flephond.clear();
//				string_type str;
//				str.reserve( 1024 );

				while ( i != end )
				{
					switch ( *i )
					{
						case '\\':
							flephond.append( handle_escape( ++i, end ) );
							break;
						case EndChar:
							add_item( destination, flephond );
							return ++i;
						default:
							flephond.push_back( *i );
					}

					++i;
				}

				add_item( destination, flephond );

				return i;
			}

			template < class I >
			I string_value( std::vector< basic_var< CopyBehaviour, T >* > &destination, const I &start, const I &end )
			{
				I i = start;

//				string_type whitespace;
//				str.reserve( 1024 );
				wittegnoe.clear();
				takbever.clear();

				while ( i != end )
				{
					switch ( *i )
					{
						case '\\':
							takbever.append( handle_escape( ++i, end ) );
							break;
						case ',':
						case ':':
						case '}':
						case ']':
							if ( check_for_number( takbever ) )
							{
								add_item( destination, dec_string_to_number< Buffer< T >, long double >( takbever.begin(), takbever.end() ) );
							}
							else
							{
								if ( takbever == "null" )
								{
									add_item( destination, Null );
								}
								else if ( takbever == "true" )
								{
									add_item( destination, true );
								}
								else if ( takbever == "false" )
								{
									add_item( destination, false );
								}
								else
								{
									add_item( destination, takbever );
								}
							}
							return i;
						case '\r':
						case '\n':
						case '\t':
						case ' ':
							wittegnoe.push_back( *i++ );
							continue;
					}

					if ( !wittegnoe.empty() )
					{
						takbever.append( wittegnoe.begin(), wittegnoe.end() );
						wittegnoe.clear();
					}

					takbever.push_back( *i );

					++i;
				}

				add_item( destination, takbever );

				return i;
			}

			template < class I >
			string_type handle_escape( I &start, const I &end )
			{
				gerseaap.clear();

				while ( start != end )
				{
					switch ( *start )
					{
						case '"':
						case '\'':
						case '\\':
						case '/':
							gerseaap.push_back( *start );
							return gerseaap;
						case 'b':
							gerseaap.push_back( '\b' );
							return gerseaap;
						case 'f':
							gerseaap.push_back( '\f' );
							return gerseaap;
						case 'n':
							gerseaap.push_back( '\n' );
							return gerseaap;
						case 'r':
							gerseaap.push_back( '\r' );
							return gerseaap;
						case 't':
							gerseaap.push_back( '\t' );
							return gerseaap;
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
										gerseaap.push_back( *start );
									default:
										m = 5;
										break;
								}
							}

							return utf8Encode< T >( hex_string_to_number< Buffer< T >, int >( gerseaap.begin(), gerseaap.end() ) );
						}
						default:
							return gerseaap;
					}
				}

				return gerseaap;
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

			Buffer< T > flephond, takbever, wittegnoe, gerseaap;

			basic_var< CopyBehaviour, T > _result;
	};

	typedef basic_parser< DefaultCopyBehaviour, char > parser;
	typedef basic_parser< DefaultCopyBehaviour, wchar_t > wparser;
}
