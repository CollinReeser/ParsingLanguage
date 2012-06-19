#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include "SimpleTextUtil.h"

std::vector<std::string> SimpleTextUtil::fileToLines( std::string fileName )
	throw(std::string)
{
	std::vector<std::string> textFile;

	std::ifstream ifs( fileName.c_str() );
	if ( ifs.fail() )
	{
		std::string error;
		error += "File open failed on lie-file: ";
		error += fileName;
		error += "\nAre you sure you have the correct filename?";
		throw error;
	}
	std::string temp;
	while( std::getline( ifs, temp ) )
	{
		// The " " is to ensure the tokenizer does not get confused about tokens
		// on separate lines, ie, thinking that the token on the end of a line
		// is part of the token on the beginning of the next line, because there
		// was no whitespace besides a newline between them
		textFile.push_back( temp + " " );
	}
	ifs.close();
	return textFile;
}

void SimpleTextUtil::printFile( std::vector<std::string> lines )
{
	for ( int i = 0; i < lines.size(); i++ )
	{
		std::cout << lines.at( i ) << std::endl;
	}
	return;
}
