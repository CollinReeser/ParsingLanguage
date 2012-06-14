
#include <stdio.h>
#include <iostream>
#include <sstream>
#include "parserLang.h"

int castStringToInt( std::string val );

int main( int argc , char** argv )
{	
	if ( argc < 2 )
	{
		std::cout << "Error: Please provide one or two arguments:" << std::endl;
		std::cout << "\tVerify mode requires parse ruleset file." << 
			std::endl;
		std::cout << "\tParsing mode requires parse ruleset file and source "
			<< "file to parse." << std::endl;
		return 1;
	}
	if ( argc == 2 )
	{
		ParseLang parse( argv[1] );
	}
	if ( argc >= 3 )
	{
		ParseLang parse( argv[1] , argv[2] );
	}
	//parse.printPassOne();
	return 0;
}
