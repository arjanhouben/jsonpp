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

	json::base64::data_type data = json::base64::decode( buf );
	std::cout << std::string( data.begin(), data.end() );
}
