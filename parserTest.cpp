
#include <stdio.h>
#include <iostream>
#include <sstream>
#include "parserLang.h"

int castStringToInt( std::string val );

int main( int argc , char** argv )
{	
	if ( argc < 3 )
	{
		std::cout << "Error: Please provide two arguments; Requires parse " <<
			"ruleset file and source\n\tfile to parse." << std::endl;
		return 1;
	}
	ParseLang parse( argv[1] , argv[2] );
	//parse.printPassOne();
	return 0;
}
