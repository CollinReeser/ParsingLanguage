
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <regex.h>
#include <stdlib.h>
#include "parserLang.h"
#include "lexer/lexer.h"

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
	toplevelVerification( false , parseFile );
}

// Constructor
ParseLang::ParseLang( std::string parseFile )
{
	toplevelVerification( false , parseFile );
}

ParseLang::ParseLang()
{
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
	else if ( op.compare( ";" ) == 0 || op.compare( "#" ) == 0)
	{
		return true;
	}
	return false;
}

// Returns true if the passed string is considered a keyword
bool ParseLang::isKeyword( std::string keyword )
{
	if ( keyword.compare( "newsym" ) == 0 || keyword.compare( "symbol" ) == 0 )
	{
		return true;
	}
	if ( keyword.compare( "NULL" ) == 0 )
	{
		return true;
	}
	return false;
}

void ParseLang::toplevelVerification( bool quiet , std::string parseFile )
{
	if ( quiet )
	{
		parseDescription( parseFile );
		pullRuleSets( true );
		ensureNoRuleDuplicates();
		ensureParentheses();
		ensureValidStart();
		ensureOperatorUsage();
		ensureSymbolsOperatorSeparated();
		return;
	}
	try
	{
		std::cout << "Beginning top-level absorbtion of rule-set... \t\t\t";
		parseDescription( parseFile );
		std::cout << "Absorbed.\nAbsorbing requested external rulesets... " <<
			"\t\t\t";
		pullRuleSets( false );
		std::cout << "\nEnsuring no statements share a common " <<
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
		std::cout << "Verified.\nEnsuring all symbols are separated by " <<
			"operators... \t\t";
		ensureSymbolsOperatorSeparated();
		std::cout << "Verified." << std::endl;
	}
	catch ( std::string msg )
	{
		std::cout << "\n" << msg << std::endl;
	}
	/*
	for ( int k = 0; k < (int) statements.size(); k++ )
	{
		std::cout << "Rule: " << statements.at(k).getName() << std::endl;
		for ( int m = 0; m < statements.at(k).getRule().size(); m++ )
		{
			std::cout << "\t" << statements.at(k).getRule().at(m) << std::endl;
		}
	}
	*/
	return;
}

void parseSourceFile( std::string sourcefile , 
	std::vector<std::string> (*lexer_function)(std::string) )
{
	// Use either the default or user-defined lexing function to tokenize the
	// source file for the language being parsed by the parsing language
	std::vector<std::string> tokens = lexer_function( sourcefile );

	// Here, do the algorithm for parsing the source file
	
	return;
}

void ParseLang::pullRuleSets( bool nested )
{
	Statement statement;
	for ( int i = 0; i < (int) statements.size(); i++ )
	{
		if ( statements.at(i).getName().compare( "include_rules" ) == 0 )
		{
			statement = statements.at(i);
			// Remove include_rules from list of statements
			statements.erase( statements.begin() + i );
			break;
		}
	}
	// No include_rules statement
	if ( statement.getName().size() == 0 )
	{
		if ( !nested )
		{
			std::cout << "Left out.";
		}
		return;
	}
	// Otherwise, perform include
	if ( !nested && statement.isNew() || statement.isPermeate() )
	{
		std::cout << "\tWarning: \"include_rules\" properties are meaningless."
			<< std::endl;
	}
	std::vector<std::string> rule = statement.getRule();
	for ( int i = 0; i < (int) rule.size() - 1; i++ )
	{
		try
		{
			if ( rule.at( i ).compare( ParseLang::parseFile ) == 0 )
			{
				std::string error = "\tCannot include self-referential files: ";
				error += "File [";
				error += ParseLang::parseFile;
				error += "] attempts to include itself in rule ";
				error += "\"include_rules\".";
				throw error;
			}
			mergeStatementList( rule.at(i) );
		}
		catch ( std::string msg )
		{
			std::string error = "\tWhile pulling external rulesets from file [";
			error += rule.at(i);
			error += "]\n\tin file [";
			error += ParseLang::parseFile;
			error += "]:\n\t";
			error += msg;
			throw error;
		}
	}
	if ( !nested )
	{
		std::cout << "Absorbed.";
	}
	return;
}

void ParseLang::mergeStatementList( std::string filename )
{
	ParseLang tempParse;
	try
	{
		tempParse.parseDescription( filename );
		tempParse.pullRuleSets( true );
	}
	catch ( std::string msg )
	{
		std::string error = "Malformed file [";
		error += filename;
		error += "] could not be ";
		error += "parsed. This may be because\n\tthe file does not exist. ";
		error += "Run the parser in \"verify\" mode on the file\n\tto ";
		error += "determine errors."; 
		throw error;
	}
	for ( int i = 0; i < (int) tempParse.statements.size(); i++ )
	{
		for ( int j = 0; j < (int) statements.size(); j++ )
		{
			if ( tempParse.statements.at(i).getName().compare( 
				statements.at(j).getName() ) == 0 )
			{
				std::string error = "Error in absorbtion of file [";
				error += filename;
				error += "]";
				error += ":\n\t";
				error += "Rule ";
				error += castIntToString(i);
				error += " conflicts with rule ";
				error += castIntToString(j);
				error += " (zero-indexed) in file\n\t[";
				error += ParseLang::parseFile;
				error += "] ";
				error += "with name \"";
				error += tempParse.statements.at(i).getName();
				error += "\".\n\t";
				error += "Note that this error message may be misleading if ";
				error += "file\n\t[";
				error += filename;
				error += "] itself uses \"include_rules\" to\n\tabsorb a ruleset;";
				error += " this tertiary ruleset (or a ruleset any number of ";
				error += "\n\tlayers down) may be the true culprit. Think \"C ";
				error += "Include Guards\" and what\n\tthey do, except you ";
				error += "don\'t have the luxury of using them here.";
				throw error;
			}
		}
		statements.push_back( tempParse.statements.at(i) );
	}
	return;
}

void ParseLang::ensureSymbolsOperatorSeparated()
{
	for ( int i = 0; i < (int) statements.size(); i++ )
	{
		// Get copy of the rule
		std::vector<std::string> rule = statements.at(i).getRule();
		for ( int j = 0; j < (int) rule.size(); j++ )
		{
			/*
			std::cout << "\n" << rule.at(j) << std::endl;
			std::cout << i << " " << j << std::endl;
			*/
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
				error += ". Two symbols\n\tmust have binary operators ";
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
				error += ".\n\tTwo literal symbols must ";
				error += "binary operators between them.\n\tFound as: [";
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
			else if ( !isOperator( rule.at(j) ) && !isOperator( rule.at(j+1) ) 
				&& isKeyword( rule.at(j) ) &&
				rule.at(j+1).compare( "\"" ) == 0 )
			{
				std::string error = "Error in definition of ";
				error += statements.at(i).getName();
				error += ":\n\t";
				error += "Keyword found followed by a literal symbol not ";
				error += "separated by an operator\n\tfound at token ";
				error += castIntToString(j);
				error += ".\n\tSymbols must have ";
				error += "binary operators between them.\n\tFound as: [";
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
			else if ( rule.at(j).compare(")") == 0 &&
				( rule.at(j+1).compare("*") == 0 ||
				rule.at(j+1).compare("\"") == 0 || 
				isKeyword( rule.at(j+1) ) || 
				rule.at(j+1).compare("(") == 0 ) )
			{
				std::string error = "Error in definition of ";
				error += statements.at(i).getName();
				error += ":\n\t";
				error += "Parentheses-section followed by symbol not ";
				error += "separated by an operator\n\tfound at token ";
				error += castIntToString(j);
				error += ".\n\tSymbols must have ";
				error += "binary operators between them.\n\tFound as: [";
				error += rule.at( j );
				error += " ";
				error += rule.at( j + 1 );
				error += "].";
				throw error;
			}
			else if ( isKeyword( rule.at(j) ) &&
				( rule.at(j+1).compare("*") == 0 ||
				rule.at(j+1).compare("\"") == 0 || 
				rule.at(j+1).compare("#") == 0 || 
				isKeyword( rule.at(j+1) ) || 
				rule.at(j+1).compare("(") == 0 ) )
			{
				std::string error = "Error in definition of ";
				error += statements.at(i).getName();
				error += ":\n\t";
				error += "Symbol followed by parentheses-section or symbol ";
				error += "not separated by an\n\toperator found at token ";
				error += castIntToString(j);
				error += ".\n\tSymbols must have ";
				error += "binary operators between them.\n\tFound as: [";
				error += rule.at( j );
				error += " ";
				error += rule.at( j + 1 );
				error += "].";
				throw error;
			}
		}
	}
	return;
}

void ParseLang::ensureOperatorUsage()
{
	//std::cout << "\nEnter EOU" << std::endl;
	for ( int i = 0; i < (int) statements.size(); i++ )
	{
		// Get copy of the rule
		std::vector<std::string> rule = statements.at(i).getRule();
		for ( int j = 0; j < (int) rule.size(); j++ )
		{
			if ( rule.at(j).compare( "*" ) == 0 )
			{
				// If "*" is attached to something other than a symbol or an
				// open-paren, fail
				if ( isOperator( rule.at( j + 1 ) ) && 
					rule.at(j+1).compare( "(" ) != 0 &&
					rule.at(j+1).compare( "#" ) != 0)
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
				// open-paren or quotes, fail
				if ( j - 1 < 0 || ( ( isOperator( rule.at( j + 1 ) ) && 
					rule.at( j + 1 ).compare( "(" ) != 0 &&
					rule.at( j + 1 ).compare( "*" ) != 0 &&
					rule.at( j + 1 ).compare( "#" ) != 0 ) ||
					( isOperator( rule.at( j - 1 ) ) &&
					rule.at( j - 1 ).compare( ")" ) != 0 ) ||
					( !isOperator( rule.at( j - 1 ) ) &&
					rule.at( j - 1 ).compare( "\"") != 0 &&
					rule.at( j - 1 ).compare( "symbol") != 0 &&
					rule.at( j - 1 ).compare( "newsym") != 0 &&
					castStringToInt( rule.at( j - 1 ) ) == 0 ) ) )
				{
					std::string error = "Error in definition of ";
					error += statements.at(i).getName();
					error += ":\n\t";
					error += "\">\" operator misused at token ";
					error += castIntToString(j);
					error += ", \">\" must be between symbols\n\t";
					error += "or parentheses [)>(].";
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
			else if ( rule.at(j).compare( "#" ) == 0 )
			{
				//std::cout << "\tEnter octothorpe" << std::endl;
				// If "*" is attached to something other than a symbol or an
				// open-paren, fail
				
				if ( rule.at(j+1).at(0) != '"' )
				/*
				if ( j - 1 < 0 || ( rule.at(j-1).compare("symbol") != 0 &&
					rule.at(j-1).compare("newsym") != 0 ) ||
					( castStringToInt( rule.at(j-1) ) <= 0 ||
					castStringToInt( rule.at(j-1) ) > 255 ) ) 
				*/
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
				//std::cout << "\tExit octothorpe" << std::endl;
			}
		}
	}
	//std::cout << "Exit  EOU" << std::endl;
	return;
}

void ParseLang::ensureNoRuleDuplicates()
{
	for ( int i = 0; i < (int) statements.size(); i++ )
	{
		for ( int j = i + 1; j < (int) statements.size(); j++ )
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
	for ( int i = 0; i < (int) statements.size(); i++ )
	{
		// The num of open and close parentheses must match at exit of
		// statement analysis
		int openParen = 0;
		int closeParen = 0;
		// Get copy of the rule
		std::vector<std::string> rule = statements.at(i).getRule();
		for ( int j = 0; j < (int) rule.size(); j++ )
		{
			if ( rule.at(j).compare( "(" ) == 0 )
			{
				openParen++;
				// This for loop is to detect empty parentheses: ()
				for ( int k = j + 1; k < (int) rule.size(); k++ )
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
	for ( int i = 0; i < (int) statements.size(); i++ )
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
			error += " which cannot appear as the\n\tbeginning of a rule.";
			throw error;
		}
	}
	return;
}

void ParseLang::parseDescription( std::string parseFile )
{
	ParseLang::parseFile = parseFile;
	// Here we want consistent lexing of the parsing language file, and thus
	// it is not user definable. However, the lexer for the source file to be
	// parsed by the parsing algorithm will be user definable
	std::vector<std::string> tokens = tokenizeFile2( parseFile );
	/*
	std::cout << "TOKENS:" << std::endl;
	for ( int i = 0; i < tokens.size(); i++ )
	{
		std::cout << tokens.at(i) << std::endl;
	}
	std::cout << "END TOKENS" << std::endl;
	*/
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
		while ( tokens.at(i).compare(";") != 0 )
		{
			if ( i >= tokens.size() - 1 )
			{
				std::string error = "\tError in definition of ";
				error += statement.getName();
				error += "\n\tnot terminated with a semicolon, \";\".";
				throw error;
			}
			statement.addTokenToRule( tokens.at(i) );
			if ( tokens.at(i).at(0) == '"' )
			{
				// This will match the token against a string literal that
				// contains no whitespace
				if ( !matchRegex( "^\"[^ \t\n]+\"$" , tokens.at(i) ) )
				{
					std::string error = "Error in definition of ";
					error += statement.getName();
					error += ":\n\t";
					error += "Quotes must enclose exactly one literal token; ";
					error += "Malformed string literal found at token\n\t";
					error += castIntToString( i );
					error += ", ";
					error += tokens.at(i);
					error += ".";
					throw error;
				}
			}
			i++;
		}
		statement.addTokenToRule( ";" );
		i++;
		statements.push_back( statement );
	}
	return;
}

void ParseLang::printPassOne()
{
	for ( int i = 0; i < (int) statements.size(); i++ )
	{
		std::cout << "Statement " << i << ", Name: " << 
			statements.at(i).getName() << std::endl;
		std::cout << "  Rule: ";
		std::vector<std::string> rule = statements.at(i).getRule();
		for ( int j = 0; j < (int) rule.size(); j++ )
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

bool ParseLang::matchOctothorpeString( std::string octostring , 
	std::string token )
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

bool ParseLang::matchChar( std::string characters , std::string token )
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

// http://www.peope.net/old/regex.html
bool ParseLang::matchRegex( std::string regexString , std::string tokenString )
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

bool isStringLiteral( std::string token )
{
	return false;
}

void Statement::setName( std::string name )
{
	Statement::name = name;
	return;
}

std::string Statement::getName() const
{
	return name;
}

std::vector<std::string> Statement::getRule() const
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

Statement& Statement::operator=( const Statement &other )
{
	if ( this == &other )
	{
		return *this;
	}
	name = other.getName();
	rule = other.getRule();
	anchor = other.anchor;
	flags = other.flags;
	return *this;
}

