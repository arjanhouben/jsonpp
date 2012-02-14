#include <json++>
#include <fstream>

int main()
{
	std::string buf;
	char c;
	while( std::cin.get( c ) )
	{
		buf.push_back( c );
	}

	std::cout << json::base64::encode( buf.c_str(), buf.size() );
}
