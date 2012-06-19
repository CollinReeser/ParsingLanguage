// This is the pure-functions form of the lexer from my compiler project


#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

// NOTE: DEPRECATED, use tokenizeFileV2
// This function opens the file of name fileName, and attempts to populate
// a vector of strings with the tokens, using a few private functions to
// clean up the result
std::vector<std::string> tokenizeFile( std::string fileName )
	throw (std::string);
// This function combines simple operators into complex ones when the
// operators are adjacent in the vector. For example, if two "<" strings
// are adjacent in the token vector, they are combined into "<<"
std::vector<std::string> internalTokenizeFix( 
	std::vector<std::string> tokens );
// This function removes tabs and whitespace from the beginning and end
// of tokens
std::vector<std::string> internalRemoveWhitespace( 
	std::vector<std::string> tokens );
// This function removes entries into the token vector if the entry is
// of size zero
std::vector<std::string> internalRemoveNullTokens( 
	std::vector<std::string> tokens );
#endif
