
#include <string>
#include <vector>
#include <iostream>
#include "lexer.h"
#include "SimpleTextUtil.h"

std::vector<std::string> tokenizeFile( std::string fileName )
	throw (std::string)
{
	SimpleTextUtil util;
	// Raw lines from a file
	std::vector<std::string> lines;
	// Tokens of lines
	std::vector<std::string> tokens;
	try
	{
		// Pull the lines from the file
		lines = util.fileToLines( fileName );
	}
	catch ( std::string message )
	{
		throw message;
	}
	// Strip comments from the resultant lines
	util.stripMultiComments(lines);
	util.stripLineComments(lines);
	std::string temp;
	// Tokenize the business
	// Loop over lines
	for ( int i = 0; i < lines.size(); i++ )
	{
		// Loop over characters of line
		for ( int j = 0; j < lines.at(i).size(); j++ )
		{
			if ( lines.at(i).at(j) == ' ' && temp.size() == 0 )
			{
				// Skip spaces when we aren't yet building a token
				continue;
			}
			// Space delimited
			if ( lines.at(i).at(j) == ' ' && temp.size() != 0 )
			{
				// Push the token received. Must be a token because space-
				// delimited
				tokens.push_back( temp );
				temp.clear();
				continue;
			}
			// Operator delimited
			if ( util.isOperatorT( lines.at(i).at(j) ) )
			{
				if ( temp.size() == 0 )
				{
					std::string str;
					str += (lines.at(i).at(j));
					tokens.push_back( str );
				}
				else
				{
					tokens.push_back( temp );
					std::string str;
					str += (lines.at(i).at(j));
					tokens.push_back( str );
					temp.clear();
				}
				continue;
			}
			else
			{
				// Start building the token then
				temp += lines.at(i).at(j);
			}
		}
	}
	// If the token was not flushed at loop-exit, flush
	if ( temp.size() != 0 )
	{
		tokens.push_back( temp );
		temp.clear();
	}
	// Remove whitespace from the tokens
	tokens = internalRemoveWhitespace( tokens);
	// Fix certain tokenizing errors, like two '<'s next to eachother not being
	// tokenized as "<<". Basically do some compaction or expansion as needed
	tokens = internalTokenizeFix( tokens );
	// Remove empty tokens ( size zero )
	tokens = internalRemoveNullTokens( tokens );
	// Return tokenized result
	return tokens;
}

// This function generates complex operators from adjacent simple ones
// TODO: Need to handle the case where "1234 + -56", which is tokenized to
// "1234","+","-","56" is correctly tokenized to "1234","+","-56"
std::vector<std::string> internalTokenizeFix( 
	std::vector<std::string> tokens )
{
	// Loop over the tokens and fix them
	for ( int i = 0; i < tokens.size(); i++ )
	{
		// Fix bitshifts
		if ( tokens.at( i ) == "<" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == "<" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = "<<";
		}
		else if ( tokens.at( i ) == ">" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == ">" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = ">>";
		}
		// Fix arithmetic equals
		else if ( tokens.at( i ) == "-" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == "=" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = "-=";
		}
		else if ( tokens.at( i ) == "+" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == "=" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = "+=";
		}
		else if ( tokens.at( i ) == "/" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == "=" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = "/=";
		}
		else if ( tokens.at( i ) == "*" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == "=" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = "*=";
		}
		else if ( tokens.at( i ) == "%" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == "=" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = "%=";
		}
		// Fix logical tests
		else if ( tokens.at( i ) == "!" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == "=" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = "!=";
		}
		else if ( tokens.at( i ) == "=" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == "=" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = "==";
		}
		// Fix increments and decrements
		else if ( tokens.at( i ) == "+" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == "+" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = "++";
		}
		else if ( tokens.at( i ) == "-" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == "-" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = "--";
		}
		// Fix bitwise equals
		else if ( tokens.at( i ) == "^" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == "=" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = "^=";
		}
		else if ( tokens.at( i ) == "&" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == "=" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = "&=";
		}
		else if ( tokens.at( i ) == "|" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == "=" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = "|=";
		}
		else if ( tokens.at( i ) == "~" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == "=" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = "~=";
		}
		// Fix logical operators
		else if ( tokens.at( i ) == "&" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == "&" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = "&&";
		}
		else if ( tokens.at( i ) == "|" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == "|" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = "||";
		}
		else if ( tokens.at( i ) == ":" && i + 1 != tokens.size() &&
			tokens.at(i + 1) == ":" )
		{
			tokens.erase( tokens.begin() + i + 1 );
			tokens[i] = "::";
		}
	}
	return tokens;
}

std::vector<std::string> internalRemoveWhitespace( 
	std::vector<std::string> tokens )
{
	SimpleTextUtil util;
	// For every token in the list, use the SimpleTextUtil function that strips
	// whitespace from the beginning and end of the token
	for ( int i = 0; i < tokens.size(); i++ )
	{
		tokens[i] = util.stripWhitespace( tokens.at(i) );
	}
	return tokens;
}

// Remove null token entries
std::vector<std::string> internalRemoveNullTokens( 
	std::vector<std::string> tokens )
{
	for ( int i = 0; i < tokens.size(); i++ )
	{
		if ( tokens.at(i).size() == 0 )
		{
			tokens.erase(tokens.begin() + i );
		}
	}
	return tokens;
}
