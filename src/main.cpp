#include <json++>
#include <iostream>
#include <fstream>

enum RunRoundTrip
{
	NoRoundTrip,
	RoundTrip
};

void Test( const std::string &input, const json::var &expected, unsigned int line, RunRoundTrip roundTrip  )
{
	json::var result = json::parser( input );

	if ( expected != result )
	{
		json::Debug() << "Failed test on line " << line << ":\n" << result.serialize() << "\nexpected:\n" << expected.serialize() << "\n";
	}
	else if ( roundTrip == RoundTrip )
	{
		json::var sanity = json::parser( result.serialize() );

		if ( sanity != result )
		{
			json::Debug() << "Failed test on line " << line << ", initial parsing ok, but result did not survive round trip.\n"
						 << result.serialize() << "\nsecond parse:\n" << sanity.serialize() << "\n";
		}
		else
		{
			std::cout << "Test " << line << ": ok. " << std::string( input.begin(), input.end() ) << " -> " << result.serialize() << std::endl;
		}
	}
}

template < class A, class B >
void Test2( const A &value, const json::var &input, const B &original, unsigned int line )
{
	if ( !( value == input && value == original ) )
	{
		json::Debug() << "Failed test on line " << line << ":\nexpected " << original << " instead got " << input << " and " << value;
	}
}

int main( int, char *[] )
{
	json::var expected;

	// Support Empty Object
	expected = json::Object;
	Test( "{}", expected, __LINE__, RoundTrip );

	// Support simple Object String value
	expected[ "v" ] = "1";
	Test( "{ \"v\":\"1\"}", expected, __LINE__, RoundTrip );

	// Space Tester
	expected[ "v" ] = "1";
	Test( "{ \"v\":\"1\"\r\n}", expected, __LINE__, RoundTrip );

	// Support simple Object Int value
	expected[ "v" ] = 1;
	Test( "{ \"v\":1}", expected, __LINE__, RoundTrip );

	// Support simple Quote in String
	expected[ "v" ] = "ab'c";
	Test( "{ \"v\":\"ab'c\"}", expected, __LINE__, RoundTrip );

	// Support simple Object Float value
	expected[ "v" ] = 3.141E-10L;
	Test( "{ \"v\":3.141E-10}", expected, __LINE__, RoundTrip );

	// Support lowcase float value
	expected[ "v" ] = 3.141E-10L;
	Test( "{ \"v\":3.141e-10}", expected, __LINE__, RoundTrip );

	// Long number support
	expected[ "v" ] = 12345123456789;
	Test( "{ \"v\":12345123456789}", expected, __LINE__, RoundTrip );

	std::stringstream aap( "123456789123456789123456789" );
	long double flep = 0;
	aap >> flep;
	std::cerr << flep - 123456789123456789123456789e0l << std::endl;

	// Bigint number support
	expected[ "v" ] = 123456789123456789123456789e0L;
	Test( "{ \"v\":123456789123456789123456789}", expected, __LINE__, RoundTrip );

	// Support simple digit array
	expected[ 0 ] = 1;
	expected[ 1 ] = 2;
	expected[ 2 ] = 3;
	expected[ 3 ] = 4;
	Test( "[ 1,2,3,4]", expected, __LINE__, RoundTrip );

	// Support simple string array
	expected[ 0 ] = "1";
	expected[ 1 ] = "2";
	expected[ 2 ] = "3";
	expected[ 3 ] = "4";
	Test( "[ \"1\",\"2\",\"3\",\"4\"]", expected, __LINE__, RoundTrip );

	// Array of empty Object
	expected = json::Array;
	expected.push( json::Object );
	expected.push( json::Object );
	expected.push( json::Array );
	Test( "[ { }, { },[]]", expected, __LINE__, RoundTrip );

	// Support lowercase Unicode Text
	expected = json::Object;
	expected[ "v" ] = "\u2000\u20ff";
	Test( "{ \"v\":\"\u2000\u20ff\"}", expected, __LINE__, RoundTrip );

	// Support uppercase Unicode Text
	expected = json::Object;
	expected[ "v" ] = "\u2000\u20FF";
	Test( "{ \"v\":\"\u2000\u20FF\"}", expected, __LINE__, RoundTrip );

	// Support non protected / text
	expected = json::Object;
	expected[ "a" ] = "hp://foo";
	Test( "{ \"a\":\"hp://foo\"}", expected, __LINE__, RoundTrip );

	// Support null
	expected = json::Object;
	expected[ "a" ] = json::Null;
	Test( "{ \"a\":null}", expected, __LINE__, RoundTrip );

	// Support boolean
	expected = json::Object;
	expected[ "a" ] = true;
	Test( "{ \"a\":true}", expected, __LINE__, RoundTrip );

	// Support non trimed data
	expected = json::Object;
	expected[ "a" ] = false;
	Test( "{ \"a\" : false }", expected, __LINE__, RoundTrip );

	// Double precision floating point
	expected = json::Object;
	expected[ "v" ] = 1.7976931348623157e308L;
	Test( "{ \"v\":1.7976931348623157E308}", expected, __LINE__, RoundTrip );

	// trucated value
	expected = json::Object;
	expected[ "X" ] = 's';
	Test( "{'X':'s", expected, __LINE__, RoundTrip );

	// trucated key
	expected = json::Object;
	expected[ "X" ];
	Test( "{'X", expected, __LINE__, NoRoundTrip );

	// NON RFC 4627 Tests

	// Support Double Quote in Simple Quoted Text
	expected = json::Object;
	expected[ "v" ] = "ab\"c";
	Test( "{ \"v\":'ab\"c'}", expected, __LINE__, RoundTrip );

	// Support non protected String value
	expected = json::Object;
	expected[ "v" ] = "str";
	Test( "{ \"v\":str}", expected, __LINE__, RoundTrip );

	// Support simple quote in non protected string value
	expected = json::Object;
	expected[ "v" ] = "It's'Work";
	Test( "{ \"v\":It's'Work}", expected, __LINE__, RoundTrip );

	// Support non protected keys
	expected = json::Object;
	expected[ "a" ] = 1234;
	Test( "{ \"a\":1234}", expected, __LINE__, RoundTrip );

	// Support non protected array value
	expected = json::Array;
	expected.push( "a" );
	expected.push( "bc" );
	Test( "[ a,bc]", expected, __LINE__, RoundTrip );

	// Support non protected String value with space
	expected = json::Object;
	expected[ "v" ] = "s1 s2";
	Test( "{ \"v\":s1 s2}", expected, __LINE__, RoundTrip );

	// Support non protected String value having special spaces
	expected = json::Object;
	expected[ "v" ] = "s1	s2";
	Test( "{ \"v\":s1	s2	}", expected, __LINE__, RoundTrip );

	// Support gardbage tailling comment
	expected = json::Object;
	expected[ "a" ] = "foo.bar";
	Test( "{ \"a\":\"foo.bar\"}#toto", expected, __LINE__, RoundTrip );

	// Support Simple Quote Stings
	expected = json::Object;
	expected[ "value" ] = "string";
	Test( "{ 'value':'string'}", expected, __LINE__, RoundTrip );

	// Support non protected start like text value
	expected = json::Object;
	expected[ "v" ] = "15-55";
	Test( "{v:15-55}", expected, __LINE__, RoundTrip );

	// Support non protected start like text value
	expected = json::Object;
	expected[ "v" ] = "15%";
	Test( "{v:15%}", expected, __LINE__, RoundTrip );

	// Support non protected start like text value
	expected = json::Object;
	expected[ "v" ] = "15.06%";
	Test( "{v:15.06%}", expected, __LINE__, RoundTrip );

	// json++

	// Support slash at end of string
	expected = json::Object;
	expected[ "v" ] = "aap\\";
	Test( "{\"v\":\"aap\\\\\"}", expected, __LINE__, RoundTrip );

	// Support inserting of items
	expected = json::Array;
	expected[ 0 ] = "a";
	expected[ 1 ] = "c";
	expected[ 2 ] = "d";
	expected.splice( 1, 0, "b" );
	Test( "[\"a\",\"b\",\"c\",\"d\"]", expected, __LINE__, RoundTrip );

	// Support removing of items
	expected = json::Array;
	expected[ 0 ] = "a";
	expected[ 1 ] = "b";
	expected[ 2 ] = "b";
	expected[ 3 ] = "c";
	expected.splice( 2, 1 );
	Test( "[\"a\",\"b\",\"c\"]", expected, __LINE__, RoundTrip );

	// Support inserting / removing of items
	expected = json::Array;
	expected[ 0 ] = "a";
	expected[ 1 ] = "b";
	expected[ 2 ] = "b";
	expected[ 3 ] = "d";
	expected.splice( 2, 1, "c" );
	Test( "[\"a\",\"b\",\"c\",\"d\"]", expected, __LINE__, RoundTrip );

	// conversions
	// char
	char c = 'c';
	expected = c;
	c = expected.toString()[ 0 ];
	Test2( c, expected, 'c', __LINE__ );
	// short
	short s = 1;
	expected = s;
	s = expected.toNumber();
	Test2( s, expected, 1, __LINE__ );
	// unsigned short
	unsigned short us = 2;
	expected = us;
	us = expected.toNumber();
	Test2( us, expected, 2, __LINE__ );
	// int
	int i = 3;
	expected = i;
	i = expected.toNumber();
	Test2( i, expected, 3, __LINE__ );
	// unsigned int
	unsigned int ui = 4u;
	expected = ui;
	ui = expected.toNumber();
	Test2( ui, expected, 4u, __LINE__ );
	// long
	long l = 5;
	expected = l;
	l = expected.toNumber();
	Test2( l, expected, 5, __LINE__ );
	// unsigned long
	unsigned long ul = 6u;
	expected = ul;
	ul = expected.toNumber();
	Test2( ul, expected, 6u, __LINE__ );
	// long long
	long long ll = 7;
	expected = ll;
	ll = expected.toNumber();
	Test2( ll, expected, 7, __LINE__ );
	// unsigned long long
	unsigned long long ull = 8u;
	expected = ull;
	ull = expected.toNumber();
	Test2( ull, expected, 8u, __LINE__ );
	// float
	float f = 9;
	expected = f;
	f = expected.toNumber();
	Test2( f, expected, 9, __LINE__ );
	// double
	double d = 10;
	expected = d;
	d = expected.toNumber();
	Test2( d, expected, 10, __LINE__ );
	// bool
	bool b = true;
	expected = b;
	b = expected.toBool();
	Test2( b, expected, true, __LINE__ );
	b = false;
	expected = b;
	b = expected.toBool();
	Test2( b, expected, false, __LINE__ );
	// string
	std::string str = "string";
	expected = str;
	str = expected.toString();
	Test2( str, expected, "string", __LINE__ );

	return 0;
}
