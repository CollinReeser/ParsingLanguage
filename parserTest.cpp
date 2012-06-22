
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <regex.h>
#include <stdlib.h>
#include "parserLang.h"
#include "lexer/lexer.h"

int castStringToInt( std::string val );
bool matchRegex( std::string regexString , std::string tokenString );
bool matchChar( std::string characters , std::string token );
bool matchOctothorpeString( std::string octostring , std::string token );

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
		//parse.printPassOne();
	}
	if ( argc >= 3 )
	{
		ParseLang parse( argv[1] , argv[2] );
		//parse.printPassOne();
	}
	/*
	std::cout << "\n\n";
	try
	{
		if ( matchRegex( "ca?b" , "cab" ) )
		{
			std::cout << "Reg Success: expects success" << std::endl;
		}
		else
		{
			std::cout << "Reg Failure: expects success" << std::endl;
		}
		// This is a demonstration of the regular expression that matches any
		// string literal
		if ( matchRegex( "^\".*\"$" , "\" helkj dsfkhjdfs ldfs lih dfs \"" ) )
		{
			std::cout << "Reg Success: expects success" << std::endl;
		}
		else
		{
			std::cout << "Reg Failure: expects success" << std::endl;
		}
	}
	catch ( std::string msg )
	{
		std::cout << msg << std::endl;
	}
	if ( matchChar( "0123456789" , "9876543210" ) )
	{
		std::cout << "Lit string success: expects success" << std::endl;
	}
	else
	{
		std::cout << "Lit string failure: expects success" << std::endl;
	}
	if ( matchChar( "0123456789" , "987654321k" ) )
	{
		std::cout << "Lit string success: expects failure" << std::endl;
	}
	else
	{
		std::cout << "Lit string failure: expects failure" << std::endl;
	}
	
	
	if ( matchOctothorpeString( "rca?b" , "cab" ) )
	{
		std::cout << "Reg string success: expects success" << std::endl;
	}
	else
	{
		std::cout << "Reg string failure: expects success" << std::endl;
	}
	if ( matchOctothorpeString( "\\r0123456789" , "9876r54321" ) )
	{
		std::cout << "Lit string success: expects success" << std::endl;
	}
	else
	{
		std::cout << "Lit string failure: expects success" << std::endl;
	}
	if ( matchOctothorpeString( "0123456789" , "9876543210" ) )
	{
		std::cout << "Lit string success: expects success" << std::endl;
	}
	else
	{
		std::cout << "Lit string failure: expects success" << std::endl;
	}
	*/
	return 0;
}

bool matchOctothorpeString( std::string octostring , std::string token )
{
	bool success;
	if ( octostring.at(0) == 'r' )
	{
		success = matchRegex( octostring.substr( 1 ) , token );
	}
	else if ( octostring.size() >= 2 && octostring.at(0) == '\\' && 
		octostring.at(1) == 'r' )
	{
		success = matchChar( octostring.substr( 1 ) , token );
	}
	else
	{
		success = matchChar( octostring , token );
	}
	return success;
}


bool matchChar( std::string characters , std::string token )
{
	bool success = true;
	for ( int i = 0; i < token.size(); i++ )
	{
		for ( int j = 0; j < characters.size(); j++ )
		{
			if ( token.at(i) == characters.at(j) )
			{
				break;
			}
			if ( j == characters.size() - 1 )
			{
				success = false;
			}
		}
		if ( !success )
		{
			break;
		}
	}
	return success;
}


bool matchRegex( std::string regexString , std::string tokenString )
{
	bool success = true;
	const int len = 256;
	regex_t regex;
	int ret;
	char* msgbuf = (char*)malloc( len );

	/* Compile regular expression */
	ret = regcomp( &regex , regexString.c_str() , REG_EXTENDED );
	if( ret )
	{
		regerror( ret , &regex , msgbuf , len );
		std::string errormsg = msgbuf;
		free( msgbuf );
		regfree( &regex );
		throw errormsg;
	}

	/* Execute regular expression */
	ret = regexec( &regex , tokenString.c_str() , 0 , 0 , 0 );
	if( !ret )
	{
		// Success
	}
	else if ( ret == REG_NOMATCH )
	{
		success = false;
	}
	else
	{
		regerror( ret , &regex , msgbuf , len );
		std::string errormsg = msgbuf;
		free( msgbuf );
		regfree( &regex );
		throw errormsg;
	}
	/* Free compiled regular expression if you want to use the regex_t again */
	regfree( &regex );
	free( msgbuf );
	return success;
}
