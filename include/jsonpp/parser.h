#pragma once

#include <string>
#include <cstdlib>
#include <iterator>
#include <cstdlib>
#include <iterator>

#include <jsonpp/var.h>

namespace json
{
	namespace parse_options
	{
		enum Events
		{
			NextCharacter,
			UnquotedString,
			SingleQuotedString,
			EventCount
		};

		inline std::ostream& operator << ( std::ostream &stream, Events type )
		{
			switch ( type )
			{
				case NextCharacter:
					return stream << "NextCharacter";
				case UnquotedString:
					return stream << "UnquotedString";
				case SingleQuotedString:
					return stream << "SingleQuotedString";
				default:
					return stream << "unknown event";
			}
		}

		inline const var& standard( Events /*event*/, const var &value )
		{
			return value;
		}

		inline const wvar& wstandard( Events /*event*/, const wvar &value )
		{
			return value;
		}

		inline var strict( Events event, const var &value )
		{
			if ( event != NextCharacter )
			{
				std::cout << event << ':' << value <<  std::endl;
				return var( "error" );
			}
			return value;
		}
	}

	template < template< class > class CopyBehaviour, class Char >
	class basic_parser
	{
		public:

			typedef std::basic_string< Char > string_type;

			template < class Options >
			basic_parser( const Char str[], Options options = parse_options::standard ) :
				_string_value_buffer(),
				_string_value_whitespace_buffer(),
				_handle_escape_buffer(),
				_result( parse( str, str + strlen( str ), options ) ) { }

			template < class Options >
			basic_parser( const string_type &string, Options options = parse_options::standard ) :
				_string_value_buffer(),
				_string_value_whitespace_buffer(),
				_handle_escape_buffer(),
				_result( parse( string.begin(), string.end(), options ) ) { }

			template < class Options >
			basic_parser( std::basic_istream< Char > &stream, Options options = parse_options::standard ) :
				_string_value_buffer(),
				_string_value_whitespace_buffer(),
				_handle_escape_buffer(),
				_result( parse( std::istream_iterator< Char >( stream ), std::istream_iterator< Char >(), options ) ) { }

			operator const basic_var< CopyBehaviour, Char >&() const { return _result; }

			template < class I, class Options >
			basic_var< CopyBehaviour, Char > parse( I start, const I &end, Options options )
			{
				std::vector< basic_var< CopyBehaviour, Char >* > destinations;

				basic_var< CopyBehaviour, Char > root( Undefined );
				destinations.push_back( &root );

				while ( start != end )
				{
					switch ( *start )
					{
						case '{': // start object
							add_item( destinations, basic_var< CopyBehaviour, Char >( Object ) );
							increment( start, options );
							break;
						case '[': // add array
							add_item( destinations, basic_var< CopyBehaviour, Char >( Array ) );
							increment( start, options );
							break;
						case '}': // close object
						case ']': // close array
							if ( destinations.empty() ) throw "empty array";
							destinations.pop_back();
							increment( start, options );
							break;
						case ':': // add property
						case ',': // add destination
							increment( start, options );
							break;
						case ' ': case '\t': case '\r': case '\n':
							increment( start, options );
							break;
						case '"': // handle string
							start = string_value< '"' >( destinations, increment( start, options ), end, options );
							break;
						case '\'': // handle string
							start = string_value< '\'' >( destinations, increment( start, options ), end, options );
							break;
						default: // handle string/number/literal
							start = string_or_number_value( destinations, start, end, options );
							break;
					}
				}

				return root;
			}

		private:

			template < Char EndChar, class I, class Options >
			I string_value( std::vector< basic_var< CopyBehaviour, Char >* > &destination, const I &start, const I &end, Options options )
			{
				I i = start;

				_string_value_buffer.clear();

				while ( i != end )
				{
					switch ( *i )
					{
						case '\\':
							_string_value_buffer.append( handle_escape( ++i, end, options ) );
							break;
						case EndChar:
							if ( EndChar == '\'' )
							{
								add_item( destination, options( parse_options::SingleQuotedString, _string_value_buffer ) );
							}
							else
							{
								add_item( destination, _string_value_buffer );
							}
							return ++i;
						default:
							_string_value_buffer.push_back( *i );
					}

					++i;
				}

				add_item( destination, _string_value_buffer );

				return i;
			}

			template < class I, class Options >
			I string_or_number_value( std::vector< basic_var< CopyBehaviour, Char >* > &destination, const I &start, const I &end, Options options )
			{
				I i = start;

				_string_value_whitespace_buffer.clear();
				_string_value_buffer.clear();

				while ( i != end )
				{
					switch ( *i )
					{
						case '\\':
							_string_value_buffer.append( handle_escape( ++i, end, options ) );
							break;
						case ',':
						case ':':
						case '}':
						case ']':
							goto NUMBER_FOUND;
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

NUMBER_FOUND:
				if ( check_for_number( _string_value_buffer, options ) )
				{
					/* make sure the buffer is zero-delimited */
					_string_value_buffer.push_back( 0 );

					add_item( destination, dec_string_to_number< Buffer< Char >, long double >( _string_value_buffer.begin(), _string_value_buffer.end() ) );
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
						add_item( destination, options( parse_options::UnquotedString, _string_value_buffer ) );
					}
				}

				return i;
			}

			template < class I, class Options >
			string_type handle_escape( I &start, const I &end, Options options )
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
							increment( start, options );
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
										increment( start, options );
										_handle_escape_buffer.push_back( *start );
									default:
										m = 5;
										break;
								}
							}

							return utf8Encode< Char >( hex_string_to_number< Buffer< Char >, int >( _handle_escape_buffer.begin(), _handle_escape_buffer.end() ) );
						}
						default:
							return _handle_escape_buffer;
					}
				}

				return _handle_escape_buffer;
			}

			template < class Q, class Options >
			bool is_binary( const Q &string, Options options )
			{
				typename Q::const_iterator start = string.begin();

				if ( std::distance( start, string.end() ) > 2 )
				{
					if ( *start == '0' )
					{
						increment( start, options );
						if ( *start == 'x' )
						{
							increment( start, options );
							return *start == 'b';
						}
					}
				}

				return false;
			}

			template < class Q, class Options >
			bool check_for_number( const Q &string, Options options )
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

					increment( start, options );
				}

				return true;
			}

			void add_item( std::vector< basic_var< CopyBehaviour, Char > *> &destinations, const basic_var< CopyBehaviour, Char > &item )
			{
				if ( destinations.empty() ) return;

				basic_var< CopyBehaviour, Char > &destination( *destinations.back() );

				switch ( destination.type )
				{
					case String:
					case Number:
					case Bool:
					case Null:
					case Undefined:
					case TypeCount:
						destination = item;
						if ( item.type != Array && item.type != Object )
						{
							if ( destinations.empty() ) throw "empty array";
							destinations.pop_back();
						}
						break;
					case Object:
						destinations.push_back( &destination[ item.toString() ] );
						break;
					case Array:
						destination.push( item );
						if ( item.type == Array || item.type == Object ) destinations.push_back( &destination.back() );
						break;
				}
			}

			template < class iterator_type, class Options >
			iterator_type& increment( iterator_type &it, Options options )
			{
				options( parse_options::NextCharacter, '*' );
				return ++it;
			}

			Buffer< Char > _string_value_buffer, _string_value_whitespace_buffer, _handle_escape_buffer;

			const basic_var< CopyBehaviour, Char > _result;
	};

	template < class Options >
	inline var parser( const var::string_type &string, Options options )
	{
		return basic_parser< CopyOnWrite, char >( string, options ).operator const var&();
	}

	inline var parser( const var::string_type &string )
	{
		return basic_parser< CopyOnWrite, char >( string, parse_options::standard ).operator const var&();
	}

	template < class Options >
	inline wvar wparser( const wvar::string_type &string, Options options )
	{
		return basic_parser< CopyOnWrite, wchar_t >( string, options ).operator const wvar&();;
	}

	inline wvar wparser( const wvar::string_type &string )
	{
		return basic_parser< CopyOnWrite, wchar_t >( string, parse_options::wstandard ).operator const wvar&();;
	}
}
