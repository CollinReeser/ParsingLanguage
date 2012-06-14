
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include "parserLang.h"
#include "lexer.h"

// Takes an int and returns a string representation
std::string castIntToString( int val )
{
	std::stringstream ss;
	ss << val;
	return ss.str();
}

// Takes a string and attempts to convert it to an int. On failure, returns 0
int castStringToInt( std::string val )
{
	std::stringstream ss(val);
	int num;
	ss >> num;
	return num;
}

// Constructor, which also happens to do all the work
ParseLang::ParseLang( std::string parseFile , std::string sourcefile )
{
	try
	{
		std::cout << "Beginning top-level absorbtion of rule-set... \t\t\t";
		parseDescription( parseFile );
		std::cout << "Absorbed.\nEnsuring no statements share a common " <<
			"identifier... \t\t";
		ensureNoRuleDuplicates();
		std::cout << "Verified.\nEnsuring no statements contain parentheses " <<
			"problems... \t\t";
		ensureParentheses();
		std::cout << "Verified.\nEnsuring statements start with valid " <<
			"tokens... \t\t\t";
		ensureValidStart();
		std::cout << "Verified.\nEnsuring proper operator usage... \t\t\t\t";
		ensureOperatorUsage();
		std::cout << "Verified\nEnsuring all symbols are separated by " <<
			"operators... \t\t";
		ensureSymbolsOperatorSeparated();
		std::cout << "Verified." << std::endl;
	}
	catch ( std::string msg )
	{
		std::cout << "\n" << msg << std::endl;
	}
}

void ParseLang::ensureSymbolsOperatorSeparated()
{
	for ( int i = 0; i < statements.size(); i++ )
	{
		// Get copy of the rule
		std::vector<std::string> rule = statements.at(i).getRule();
		for ( int j = 0; j < rule.size(); j++ )
		{
			if ( !isOperator( rule.at(j) ) && !isOperator( rule.at(j+1) ) &&
				rule.at(j).compare( "\"" ) != 0 && 
				rule.at(j+1).compare( "\"" ) != 0 )
			{
				std::string error = "Error in definition of ";
				error += statements.at(i).getName();
				error += ":\n\t";
				error += "Two symbols not separated by an operator found at";
				error += " token ";
				error += castIntToString(j);
				error += ". Two symbols\n\tmust have one or more operators ";
				error += "between them.\n\tFound as: [";
				error += rule.at( j );
				error += " ";
				error += rule.at( j + 1 );
				error += "].";
				throw error;
			}
			else if ( !isOperator( rule.at(j) ) && !isOperator( rule.at(j+1) ) 
				&& rule.at(j).compare( "\"" ) == 0 &&
				rule.at(j+1).compare( "\"" ) == 0 )
			{
				std::string error = "Error in definition of ";
				error += statements.at(i).getName();
				error += ":\n\t";
				error += "Two literal symbols not separated by an operator";
				error += " found at token ";
				error += castIntToString(j);
				error += ". Two literal symbols\n\tmust have one or more ";
				error += "operators between them.\n\tFound as: [";
				error += rule.at( j - 2 );
				error += " ";
				error += rule.at( j - 1 );
				error += " ";
				error += rule.at( j );
				error += " ";
				error += rule.at( j + 1 );
				error += " ";
				error += rule.at( j + 2 );
				error += " ";
				error += rule.at( j + 3 );
				error += "].";
				throw error;
			}
		}
	}
	return;
}

// Returns true if the passed string is considered an operator by the parser
bool ParseLang::isOperator( std::string op )
{
	if ( op.compare( "*" ) == 0 || op.compare( "|" ) == 0 )
	{
		return true;
	}
	else if ( op.compare( ">" ) == 0 || op.compare( "@" ) == 0 )
	{
		return true;
	}
	else if ( op.compare( "(" ) == 0 || op.compare( ")" ) == 0 )
	{
		return true;
	}
	else if ( op.compare( ";" ) == 0 )
	{
		return true;
	}
	return false;
}

void ParseLang::ensureOperatorUsage()
{
	for ( int i = 0; i < statements.size(); i++ )
	{
		// Get copy of the rule
		std::vector<std::string> rule = statements.at(i).getRule();
		for ( int j = 0; j < rule.size(); j++ )
		{
			if ( rule.at(j).compare( "*" ) == 0 )
			{
				// If "*" is attached to something other than a symbol or an
				// open-paren, fail
				if ( isOperator( rule.at( j + 1 ) ) && 
					rule.at(j+1).compare( "(" ) != 0 )
				{
					std::string error = "Error in definition of ";
					error += statements.at(i).getName();
					error += ":\n\t";
					error += "\"*\" operator misused at token ";
					error += castIntToString(j);
					error += ", \"*\" must be left-attached to a symbol\n\t";
					error += "or open-paren. Found left-attached to: \"";
					error += rule.at( j + 1 );
					error += "\".";
					throw error;
				}
			}
			else if ( rule.at(j).compare( "|" ) == 0 )
			{
				/*
				std::cout << rule.at( j - 1 ) << " " << rule.at( j ) <<
					" " << rule.at( j + 1 ) << std::endl;
				*/
				// If "*" is attached to something other than a symbol or an
				// open-paren, fail
				if ( j - 1 < 0 || ( ( isOperator( rule.at( j + 1 ) ) && 
					rule.at( j + 1 ).compare( "(" ) != 0 &&
					rule.at( j + 1 ).compare( "*" ) != 0 ) ||
					( isOperator( rule.at( j - 1 ) ) &&
					rule.at( j - 1 ).compare( ")" ) != 0 ) ) )
				{
					std::string error = "Error in definition of ";
					error += statements.at(i).getName();
					error += ":\n\t";
					error += "\"|\" operator misused at token ";
					error += castIntToString(j);
					error += ", \"|\" must be between symbols\n\t";
					error += "or parentheses [)|(]. Found attached to: \"";
					error += "\n\tFound as: [";
					error += rule.at( j - 1 );
					error += " ";
					error += rule.at( j );
					error += " ";
					error += rule.at( j + 1 );
					error += "].";
					throw error;
				}
			}
			else if ( rule.at(j).compare( ">" ) == 0 )
			{
				/*
				std::cout << rule.at( j - 1 ) << " " << rule.at( j ) <<
					" " << rule.at( j + 1 ) << std::endl;
				*/
				// If "*" is attached to something other than a symbol or an
				// open-paren, fail
				if ( j - 1 < 0 || ( ( isOperator( rule.at( j + 1 ) ) && 
					rule.at( j + 1 ).compare( "(" ) != 0 &&
					rule.at( j + 1 ).compare( "*" ) != 0 ) ||
					( isOperator( rule.at( j - 1 ) ) &&
					rule.at( j - 1 ).compare( ")" ) != 0 ) ) )
				{
					std::string error = "Error in definition of ";
					error += statements.at(i).getName();
					error += ":\n\t";
					error += "\">\" operator misused at token ";
					error += castIntToString(j);
					error += ", \">\" must be between symbols\n\t";
					error += "or parentheses [)>(]. Found attached to: \"";
					error += "\n\tFound as: [";
					error += rule.at( j - 1 );
					error += " ";
					error += rule.at( j );
					error += " ";
					error += rule.at( j + 1 );
					error += "].";
					throw error;
				}
			}
			else if ( rule.at(j).compare( "@" ) == 0 )
			{
				// If "*" is attached to something other than a symbol or an
				// open-paren, fail
				if ( j - 1 < 0 || ( rule.at(j-1).compare("symbol") != 0 &&
					rule.at(j-1).compare("newsym") != 0 ) ||
					castStringToInt( rule.at(j+1) ) <= 0 ||
					castStringToInt( rule.at(j+1) ) > 255 )
				{
					std::string error = "Error in definition of ";
					error += statements.at(i).getName();
					error += ":\n\t";
					error += "\"@\" operator misused at token ";
					error += castIntToString(j);
					error += ", \"@\" must follow \"symbol\" or \"newsym\"\n\t";
					error += "and must precede an integer, 1-255.";
					error += "\n\tFound as: [";
					error += rule.at( j - 1 );
					error += " ";
					error += rule.at( j );
					error += " ";
					error += rule.at( j + 1 );
					error += "].";
					throw error;
				}
			}
		}
	}
	return;
}

void ParseLang::ensureNoRuleDuplicates()
{
	for ( int i = 0; i < statements.size(); i++ )
	{
		for ( int j = i + 1; j < statements.size(); j++ )
		{
			if ( statements.at(i).getName().compare( 
				statements.at(j).getName() ) == 0 )
			{
				std::string error = "Error in definition of ";
				error += statements.at(i).getName();
				error += ":\n\t";
				error += "Multiple definitions, rules ";
				error += castIntToString(i);
				error += " and ";
				error += castIntToString(j);
				error += " (zero-indexed).";
				throw error;
			}
		}
	}
	return;
}

// This algorithm ensures that the first parentheses in a rule is "(", that
// there is exactly as many "(" as ")", and that there is at least one
// non-operator token between any set of parentheses
void ParseLang::ensureParentheses()
{
	// Loop over all statements
	for ( int i = 0; i < statements.size(); i++ )
	{
		// The num of open and close parentheses must match at exit of
		// statement analysis
		int openParen = 0;
		int closeParen = 0;
		// Get copy of the rule
		std::vector<std::string> rule = statements.at(i).getRule();
		for ( int j = 0; j < rule.size(); j++ )
		{
			if ( rule.at(j).compare( "(" ) == 0 )
			{
				openParen++;
				// This for loop is to detect empty parentheses: ()
				for ( int k = j; k < rule.size(); k++ )
				{
					// If we found a non-operator within parentheses, success,
					// the parentheses contain a symbolic token
					if ( !isOperator( rule.at(k) ) )
					{
						break;
					}
					// If we find a close parentheses after finding any number
					// of operators, without finding a symbolic token, then
					// the parentheses are empty
					else if ( rule.at(k).compare( ")" ) == 0 )
					{
						std::string error = "Error in definition of ";
						error += statements.at(i).getName();
						error += ":\n\t";
						error += "Found empty parentheses: \"()\" at tokens ";
						error += castIntToString( j );
						error += " and ";
						error += castIntToString( k );
						error += ".\n\tNote that parentheses containing only ";
						error += "operators (no symbolic tokens) is";
						error += "\n\tconsidered to be empty as well.";
						throw error;
					}
				}
			}
			else if ( rule.at(j).compare( ")" ) == 0 )
			{
				if ( openParen == 0 )
				{
					std::string error = "Error in definition of ";
					error += statements.at(i).getName();
					error += ":\n\t";
					error += "Found unopened close paren: \")\" at token ";
					error += castIntToString( j );
					error += ".";
					throw error;
				}
				closeParen++;
			}
		}
		if ( openParen != closeParen )
		{
			std::string error = "Error in definition of ";
			error += statements.at(i).getName();
			error += ":\n\t";
			error += "Found parentheses mis-match. (";
			error += castIntToString( openParen );
			error += " open-parens, ";
			error += castIntToString( closeParen );
			error += " close-parens.";
			throw error;
		}
	}
	return;
}

void ParseLang::ensureValidStart()
{
	// Loop over all statements
	for ( int i = 0; i < statements.size(); i++ )
	{
		/*
		std::cout << "Token Zero: [" << statements.at(i).getRule().at(0) << 
			"] at length: " << statements.at(i).getRule().at(0).size() << 
			std::endl;
		*/
		if ( statements.at(i).getRule().at(0).compare( ">" ) == 0 || 
			statements.at(i).getRule().at(0).compare( "|" ) == 0 ||
			statements.at(i).getRule().at(0).compare( "@" ) == 0 )
		{
			std::string error = "Error in definition of ";
			error += statements.at(i).getName();
			error += ":\n\t";
			error += "Invalid start to rule. First token is ";
			error += statements.at(i).getRule().at(0);
			error += " which cannot appear as the beginning of a rule.";
			throw error;
		}
	}
	return;
}

void ParseLang::parseDescription( std::string parseFile )
{
	Lexer lex;
	std::vector<std::string> tokens = lex.tokenizeFile( parseFile );
	int i = 0;
	while ( i < tokens.size() )
	{
		Statement statement;
		statement.setName( tokens.at(i++) );
		while ( tokens.at(i) == "@" )
		{
			i++;
			if ( tokens.at(i).compare( "new" ) == 0 )
			{
				i++;
				if ( tokens.at(i).compare( ":" ) == 0 )
				{
					i++;
					int numSymbolTables = castStringToInt( tokens.at(i++) );
					if ( numSymbolTables > 0 && numSymbolTables <= 255 )
					{
						statement.setSymbolTableNum( numSymbolTables );
					}
					else if ( numSymbolTables <= 0 )
					{
						std::string error = "Error in definition of ";
						error += statement.getName();
						error += ":\n\t";
						error += "@permeate cannot define less than 1 ";
						error += "level of permeation.";
						throw error;
					}
					else
					{
						std::string error = "Error in definition of ";
						error += statement.getName();
						error += ":\n\t";
						error += "@new cannot define greater than 255 ";
						error += "symbol tables.";
						throw error;
					}
				}
				else
				{
					statement.setSymbolTableNum( 1 );
				}
			}
			else if ( tokens.at(i).compare( "permeate" ) == 0 )
			{
				i++;
				if ( tokens.at(i).compare( ":" ) == 0 )
				{
					i++;
					int numPermeations = castStringToInt( tokens.at(i++) );
					if ( numPermeations > 0 && numPermeations <= 255 )
					{
						statement.setPermeateNum( numPermeations );
					}
					else if ( numPermeations <= 0 )
					{
						std::string error = "Error in definition of ";
						error += statement.getName();
						error += ":\n\t";
						error += "@permeate cannot define less than 1 ";
						error += "level of permeation.";
						throw error;
					}
					else
					{
						std::string error = "Error in definition of ";
						error += statement.getName();
						error += ":\n\t";
						error += "@permeate cannot define greater than 255 ";
						error += "levels of permeation.";
						throw error;
					}
				}
				else
				{
					statement.setPermeateNum( 1 );
				}
			}
			// Error
			else
			{
				std::string error = "Error in definition of ";
				error += statement.getName();
				error += ":\n\t";
				error += "Found @, got \"";
				error += tokens.at(i);
				error += "\", expected: \"new\" or \"permeate\".";
				throw error;
			}
		}
		if ( tokens.at(i).compare( "=" ) != 0 )
		{
			std::string error = "Error in definition of ";
				error += statement.getName();
				error += ":\n\t";
				error += "Found \"";
				error += tokens.at(i);
				error += "\", expected: \"=\", after name and ";
				error += "properties declaration.";
				throw error;
		}
		i++;
		// The following is an algorithm to ensure that quotes only enclose
		// exactly one token
		bool inQuote = false;
		int quoteTokens = 0;
		while ( tokens.at(i).compare(";") != 0 || 
			( tokens.at(i).compare(";") == 0 && inQuote ) )
		{
			statement.addTokenToRule( tokens.at(i) );
			if ( inQuote )
			{
				quoteTokens++;
			}
			if ( tokens.at(i).compare( "\"" ) == 0 )
			{
				inQuote = !inQuote;
			}
			if ( quoteTokens == 2 && !inQuote )
			{
				quoteTokens = 0;
			}
			else if ( ( quoteTokens == 1 || quoteTokens > 2 ) && !inQuote )
			{
				std::string error = "Error in definition of ";
				error += statement.getName();
				error += ":\n\t";
				error += "Quotes must enclose exactly one literal token.";
				throw error;
			}
			i++;
		}
		statement.addTokenToRule( ";" );
		i++;
		statements.push_back( statement );
	}
}

void ParseLang::printPassOne()
{
	for ( int i = 0; i < statements.size(); i++ )
	{
		std::cout << "Statement " << i << ", Name: " << 
			statements.at(i).getName() << std::endl;
		std::cout << "  Rule: ";
		std::vector<std::string> rule = statements.at(i).getRule();
		for ( int j = 0; j < rule.size(); j++ )
		{
			std::cout << rule.at(j) << " ";
		}
		std::cout << "\n  Flags: " <<  
			"New sym table? " << ( ( statements.at(i).isNew() ) ? 
			"Yes; ":"No; " ) <<
			"Permeates? " << ( ( statements.at(i).isPermeate() ) ? 
			"Yes; " : "No; " ) <<
			"ST: " << (int)( SYMBOL_DEPTH( statements.at(i).getFlags() ) ) <<
			" P: " << (int)( PERMEATE_DEPTH( statements.at(i).getFlags() ) ) << 
			"\n\n";
	}
	return;
}

void Statement::setName( std::string name )
{
	Statement::name = name;
	return;
}

std::string Statement::getName()
{
	return name;
}

std::vector<std::string> Statement::getRule()
{
	return rule;
}

void Statement::addTokenToRule( std::string token )
{
	rule.push_back( token );
	return;
}

void Statement::setSymbolTableNum( int num )
{
	flags |= NEW_TABLE;
	flags |= num << 8;
	return;
}

void Statement::setPermeateNum( int num )
{
	flags |= PERMEATE;
	flags |= num;
	return;
}

bool Statement::isNew()
{
	return ( flags >> 63 );
}

bool Statement::isPermeate()
{
	return ( flags >> 62 ) & 0b1;
}

unsigned long long int Statement::getFlags()
{
	return flags;
}

Statement::Statement() : flags( 0 ) , anchor( -1 )
{
}

