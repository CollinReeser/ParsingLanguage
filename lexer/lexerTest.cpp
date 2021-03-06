

#include <iostream>
#include <vector>
#include <string>
#include "lexer.h"

int main( int argc , char** argv )
{
	if ( argc == 2 )
	{
		std::vector<std::string> tokens;
		try
		{
			tokens = tokenizeFile2( argv[1] );
		}
		catch ( std::string msg )
		{
			std::cout << msg << std::endl;
		}
		for ( int i = 0; i < tokens.size(); i++ )
		{
			std::cout << tokens.at(i) << std::endl;
		}
	}
	else
	{
		std::cout << "Please provide a single filename argument." << std::endl;
	}
	return 0;
}
