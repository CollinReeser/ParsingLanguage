
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include <regex.h>
#include <stdlib.h>
#include "ParseLang.h"
#include "Statement.h"
#include "lexer/lexer.h"

// Takes an int and returns a string representation
static std::string castIntToString( int val )
{
	std::stringstream ss;
	ss << val;
	return ss.str();
}

// Takes a string and attempts to convert it to an int. On failure, returns 0
static int castStringToInt( std::string val )
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
	else if ( op.compare( ">" ) == 0 )
	{
		return true;
	}
	else if ( op.compare( "(" ) == 0 || op.compare( ")" ) == 0 )
	{
		return true;
	}
	else if ( op.compare( ";" ) == 0 || op.compare( "#" ) == 0 )
	{
		return true;
	}
	else if ( op.compare( "!" ) == 0 || op.compare( "^" ) == 0 )
	{
		return true;
	}
	else if ( op.compare( ":" ) == 0 )
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
	else if ( keyword.compare( "NULL" ) == 0 ||
			keyword.compare( "arbsym" ) == 0 )
	{
		return true;
	}
	else if ( keyword.compare( "newscope" ) == 0 ||
			keyword.compare( "delscope" ) == 0 )
	{
		return true;
	}
	else if ( keyword.compare( "addToList" ) == 0 )
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
		createLists();
		ensureNoRuleDuplicates();
		ensureParentheses();
		ensureValidStart();
		ensureOperatorUsage();
		ensureSymbolsOperatorSeparated();
		ensureMeaningfulTokens();
		return;
	}
	try
	{
		std::cout << "Beginning top-level absorbtion of rule-set... \t\t\t";
		parseDescription( parseFile );
		std::cout << "Absorbed.\nAbsorbing requested external rulesets... " <<
			"\t\t\t";
		pullRuleSets( false );
		std::cout << "\nCreating requested lists... \t\t";
		createLists();
		std::cout << "\nCompleted. Ensuring no statements share a common " <<
			"identifier... \t";
		ensureNoRuleDuplicates();
		std::cout << "Verified.\nEnsuring no statements contain parentheses " <<
			"problems... \t\t";
		ensureParentheses();
		std::cout << "Verified.\nEnsuring statements start with valid " <<
			"tokens... \t\t\t";
		ensureValidStart();
		std::cout << "Verified.\nEnsuring all symbols are valid tokens..." <<
			"\t\t\t";
		ensureMeaningfulTokens();
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

void ParseLang::ensureMeaningfulTokens()
{
	for ( int i = 0; i < (int) statements.size(); i++ )
	{
		// Get copy of the rule
		std::vector<std::string> rule = statements.at(i).getRule();
		for ( int j = 0; j < (int) rule.size(); j++ )
		{
			if ( !isKeyword( rule.at(j) ) && !isOperator( rule.at(j) )
				&& !isPermissiveStringLiteral( rule.at(j) ) &&
				!isRuleName( rule.at(j) ) && !isMsgName( rule.at(j) ) &&
				castStringToInt( rule.at(j) ) == 0 )
			{
				// Up until the continue, this just acts as another check. If
				// the token is the name of a list, it is meaningful and should
				// not raise an error
				bool isListName = false;
				for ( int k = 0; k < lists.size(); k++ )
				{
					if ( lists.at(k).at(0).compare( rule.at(j) ) == 0 )
					{
						isListName = true;
						break;
					}
				}
				if ( isListName )
				{
					continue;
				}
				std::string error = "  Error in definition of ";
				error += statements.at(i).getName();
				error += ":\n\t";
				error += "Non-operator, non-keyword, non-rule-name, ";
				error += "non-literal found at token\n\t";
				error += castIntToString(j);
				error += ". Did you mean to treat the token as a string ";
				error += "literal? Did you spell a\n\trule-name wrong?";
				std::vector<std::string> simNames = ParseLang::getSimilarTokens(
					Statement::getListRuleNames( statements ) , rule.at(j) );
				if ( simNames.size() > 0 )
				{
					error += "\n\tRule-names similar to token:";
					for ( int m = 0; m < simNames.size(); m++ )
					{
						error += "\n\t\t";
						error += simNames.at(m);
					}
				}
				error += "\n\tFound as: [";
				error += rule.at( j );
				error += "].";
				throw error;
			}
			else if ( castStringToInt( rule.at(j) ) != 0 && j > 0 &&
				rule.at(j-1).compare( "@" ) != 0 )
			{
				std::string error = "  Error in definition of ";
				error += statements.at(i).getName();
				error += ":\n\t";
				error += "Non-operator, non-keyword, non-rule-name, ";
				error += "non-literal found at token\n\t";
				error += castIntToString(j);
				error += ". Did you mean to treat the token as a string ";
				error += "literal? Did you spell a\n\trule-name wrong?";
				error += "\n\tFound as: [";
				error += rule.at( j );
				error += "].";
				throw error;
			}
		}
	}
}

bool ParseLang::isRuleName( std::string token )
{
	for ( int i = 0; i < (int) statements.size(); i++ )
	{
		if ( statements.at(i).getName().compare( token ) == 0 &&
			!statements.at(i).isErrorMsg() && !statements.at(i).isNoteMsg() )
		{
			return true;
		}
	}
	return false;
}

bool ParseLang::isMsgName( std::string token )
{
	for ( int i = 0; i < (int) statements.size(); i++ )
	{
		if ( statements.at(i).getName().compare( token ) == 0 &&
			( statements.at(i).isErrorMsg() || statements.at(i).isNoteMsg() ) )
		{
			return true;
		}
	}
	return false;
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

void ParseLang::createLists()
{
	Statement statement;
	for ( int i = 0; i < (int) statements.size(); i++ )
	{
		if ( statements.at(i).getName().compare( "create_list" ) == 0 )
		{
			statement = statements.at(i);
			// Remove this create_list line from list of statements
			statements.erase( statements.begin() + i );
			i--;
			std::vector<std::string> rule = statement.getRule();
			// First argument must not be a string literal
			if ( isStringLiteral( rule.at( 0 ) ) )
			{
				std::string error = "  Error in create_list directive:\n\t"
					"List name argument was a string literal.\n\tFound as: [";
				error += rule.at( 0 );
				error += "].";
				throw error;
			}
			std::vector<std::string> newList;
			// Check to ensure the list name is not already taken
			for ( int i = 0; i < lists.size(); i++ )
			{
				if ( rule.at(0).compare( lists.at(i).at(0) ) == 0 )
				{
					std::string error = "  Error in create_list directive:\n\t"
						"List name argument already in use.\n\tFound as: [";
					error += rule.at( 0 );
					error += "].";
					throw error;
				}
			}
			// The first entry in the list vector is the name of the list.
			// Every entry after that will be an entry to the list
			newList.push_back( rule.at( 0 ) );
			lists.push_back( newList );
			// Everything after must be a string literal, and will be added to
			// the specified list
			for ( int i = 1; i < rule.size() - 1; i++ )
			{
				if ( !isStringLiteral( rule.at( i ) ) )
				{
					std::string error = "  Error in create_list directive:\n\t"
						"List entries must be string literals.\n\tFound as:"
						" [";
					error += rule.at( 0 );
					error += "].";
					throw error;
				}
				else
				{
					lists.at( lists.size() - 1 ).push_back( rule.at( i ) );
				}
			}
		}
	}
	for ( int i = 0; i < lists.size(); i++ )
	{
		std::cout << "\n  List name: " << lists.at(i).at(0) << std::endl;
		for ( int j = 1; j < lists.at(i).size(); j++ )
		{
			std::cout << "\t\t" << lists.at(i).at(j);
			if ( j < lists.at(i).size() - 1 )
			{
				std::cout << std::endl;
			}
		}
	}
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
				std::string error = "  Error in absorbtion of file [";
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
			if ( !isOperator( rule.at(j) ) && !isOperator( rule.at(j+1) ) ) //&&
				//!isStringLiteral( rule.at(j) ) )
			{
				std::string error = "  Error in definition of ";
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
				&& isStringLiteral( rule.at(j) ) )
			{
				std::string error = "  Error in definition of ";
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
				isStringLiteral( rule.at(j+1) ) )
			{
				std::string error = "  Error in definition of ";
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
				isStringLiteral( rule.at(j+1) ) ||
				isKeyword( rule.at(j+1) ) ||
				rule.at(j+1).compare("(") == 0 ) )
			{
				std::string error = "  Error in definition of ";
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
				isStringLiteral( rule.at(j+1) ) ||
				/*rule.at(j+1).compare("#") == 0 ||*/
				isKeyword( rule.at(j+1) ) ||
				rule.at(j+1).compare("(") == 0 ) )
			{
				std::string error = "  Error in definition of ";
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
					rule.at(j+1).compare( "#" ) != 0 &&
					rule.at(j+1).compare( "!" ) != 0 )
				{
					std::string error = "  Error in definition of ";
					error += statements.at(i).getName();
					error += ":\n\t";
					error += "* operator misused at token ";
					error += castIntToString(j);
					error += ", * must be left-attached to a symbol\n\t";
					error += "or open-paren. Found left-attached to: ";
					error += rule.at( j + 1 );
					error += ".";
					throw error;
				}
			}
			else if ( rule.at(j).compare( "!" ) == 0 )
			{
				// If "*" is attached to something other than a symbol or an
				// open-paren, fail
				if ( ( isKeyword( rule.at( j + 1 ) ) ||
					isOperator( rule.at( j + 1 ) ) ) &&
					rule.at(j+1).compare( "(" ) != 0 &&
					rule.at(j+1).compare( "#" ) != 0 &&
					rule.at(j+1).compare( "*" ) != 0 )
				{
					std::string error = "  Error in definition of ";
					error += statements.at(i).getName();
					error += ":\n\t";
					error += "! operator misused at token ";
					error += castIntToString(j);
					error += "; ! must be left-attached to a rule name,\n\t";
					error += "open-paren, *, or #. Found left-attached to: ";
					error += rule.at( j + 1 );
					error += ".";
					throw error;
				}
			}
			else if ( rule.at(j).compare( "|" ) == 0 )
			{
				/*
				std::cout << rule.at( j - 1 ) << " " << rule.at( j ) <<
					" " << rule.at( j + 1 ) << std::endl;
				*/
				if ( j - 1 < 0 || ( ( isOperator( rule.at( j + 1 ) ) &&
					rule.at( j + 1 ).compare( "(" ) != 0 &&
					rule.at( j + 1 ).compare( "#" ) != 0 &&
					rule.at( j + 1 ).compare( "*" ) != 0 ) ||
					( isOperator( rule.at( j - 1 ) ) &&
					rule.at( j - 1 ).compare( ")" ) != 0 ) ) )
				{
					std::string error = "  Error in definition of ";
					error += statements.at(i).getName();
					error += ":\n\t";
					error += "\"|\" operator misused at token ";
					error += castIntToString(j);
					error += ", \"|\" must be between symbols\n\t";
					error += "or parentheses [)|(].";
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
			else if ( rule.at(j).compare( "^" ) == 0 )
			{
				/*
				std::cout << rule.at( j - 1 ) << " " << rule.at( j ) <<
					" " << rule.at( j + 1 ) << std::endl;
				*/
				if ( j - 1 < 0 || !isMsgName( rule.at( j + 1) ) ||
					( rule.at( j - 1 ).compare( ")" ) != 0 &&
					!isStringLiteral( rule.at( j - 1 ) ) &&
					!isRuleName( rule.at( j - 1 ) ) ) &&
					( !isKeyword( rule.at( j - 1 ) ) ||
					rule.at(j - 1).compare( "arbsym" ) == 0 ||
					rule.at(j - 1).compare( "NULL" ) == 0 ) )
				{
					// Up until the continue, this just acts as another check. 
					// If the token is the name of a list, it is meaningful and
					// should not raise an error
					bool isListName = false;
					for ( int k = 0; k < lists.size(); k++ )
					{
						if ( lists.at(k).at(0).compare( rule.at(j-1) ) == 0 )
						{
							isListName = true;
							break;
						}
					}
					if ( isListName )
					{
						continue;
					}
					std::string error = "  Error in definition of ";
					error += statements.at(i).getName();
					error += ":\n\t";
					error += "\"^\" operator misused at token ";
					error += castIntToString(j);
					error += ", \"^\" must be before a message rule\n\t";
					error += "and after \"symbol\", end-paren, string ";
					error += "literal, list name, or rulename.";
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
			else if ( rule.at(j).compare( ":" ) == 0 )
			{
				/*
				std::cout << rule.at( j - 1 ) << " " << rule.at( j ) <<
					" " << rule.at( j + 1 ) << std::endl;
				*/
				if ( j - 1 < 0 || ( rule.at(j - 1).compare( "symbol" ) != 0 &&
					rule.at(j - 1).compare( "newsym" ) != 0 &&
					rule.at(j - 1).compare( "addToList" ) != 0 &&
					rule.at(j + 1).compare( "newscope" ) != 0 &&
					rule.at(j + 1).compare( "delscope" ) != 0 &&
					!isStringLiteral( rule.at(j+1) ) ) )
				{
					/*
					// Up until the continue, this just acts as another check. 
					// If the token is the name of a list, it is meaningful and
					// should not raise an error
					bool isListName = false;
					for ( int k = 0; k < lists.size(); k++ )
					{
						if ( lists.at(k).at(0).compare( rule.at(j+1) ) == 0 )
						{
							isListName = true;
							break;
						}
					}
					if ( isListName )
					{
						continue;
					}
					*/
					std::string error = "  Error in definition of ";
					error += statements.at(i).getName();
					error += ":\n\t";
					error += "\":\" operator misused at token ";
					error += castIntToString(j);
					error += ", \":\" must be before newscope/delscope\n\t";
					error += "and/or after \"symbol\", or \"newsym\".";
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
				if ( j - 1 < 0 || ( ( isOperator( rule.at( j + 1 ) ) &&
					rule.at( j + 1 ).compare( "(" ) != 0 &&
					rule.at( j + 1 ).compare( "*" ) != 0 &&
					rule.at( j + 1 ).compare( "#" ) != 0 &&
					rule.at( j + 1 ).compare( "!" ) != 0 ) ||
					( isOperator( rule.at( j - 1 ) ) &&
					rule.at( j - 1 ).compare( ")" ) != 0 ) ||
					( !isOperator( rule.at( j - 1 ) ) &&
					!isStringLiteral( rule.at( j - 1 ) ) &&
					!isKeyword( rule.at( j - 1 ) ) &&
					!isRuleName( rule.at( j - 1 ) ) &&
					castStringToInt( rule.at( j - 1 ) ) == 0 ) ) )
				{
					std::string error = "  Error in definition of ";
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
			else if ( rule.at(j).compare( "#" ) == 0 )
			{
				//std::cout << "\tEnter octothorpe" << std::endl;
				// If "*" is attached to something other than a symbol or an
				// open-paren, fail

				if ( rule.at(j+1).at(0) != '"' ||
					( j - 1 > -1 && rule.at(j-1).compare( "NULL" ) == 0 ) )
				/*
				if ( j - 1 < 0 || ( rule.at(j-1).compare("symbol") != 0 &&
					rule.at(j-1).compare("newsym") != 0 ) ||
					( castStringToInt( rule.at(j-1) ) <= 0 ||
					castStringToInt( rule.at(j-1) ) > 255 ) )
				*/
				{
					std::string error = "  Error in definition of ";
					error += statements.at(i).getName();
					error += ":\n\t";
					error += "\"#\" operator misused at token ";
					error += castIntToString(j);
					error += ", \"#\" must precede a literal string,\n\t";
					error += "and can optionally follow exclusively newsym, ";
					error += "arbsym or symbol, with or\n\twithout ";
					error += "an @ qualifier.";
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
				std::string error = "  Error in definition of ";
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
						std::string error = "  Error in definition of ";
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
					std::string error = "  Error in definition of ";
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
			std::string error = "  Error in definition of ";
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
			statements.at(i).getRule().at(0).compare( "@" ) == 0 ||
			statements.at(i).getRule().at(0).compare( ":" ) == 0 ||
			statements.at(i).getRule().at(0).compare( "^" ) == 0 )
		{
			std::string error = "  Error in definition of ";
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
			if ( tokens.at(i).compare( "error" ) == 0 )
			{
				if ( statement.isNoteMsg() )
				{
					std::string error = "  Error in definition of ";
					error += statement.getName();
					error += ":\n\t";
					error += "@error cannot be applied when the rule is ";
					error += "a notification.";
					throw error;
				}
				else if ( statement.isNew() || statement.isPermeate() ||
					statement.isCenter() )
				{
					std::string error = "  Error in definition of ";
					error += statement.getName();
					error += ":\n\t";
					error += "@error cannot be applied when the rule declares ";
					error += "use of symbol tables\n\t(rule is a true grammar ";
					error += "rule).";
					throw error;
				}
				statement.setIsErrorMsg();
				i++;
			}
			else if ( tokens.at(i).compare( "note" ) == 0 )
			{
				if ( statement.isErrorMsg() )
				{
					std::string error = "  Error in definition of ";
					error += statement.getName();
					error += ":\n\t";
					error += "@note cannot be applied when the rule is ";
					error += "an error message.";
					throw error;
				}
				else if ( statement.isNew() || statement.isPermeate() ||
					statement.isCenter() )
				{
					std::string error = "  Error in definition of ";
					error += statement.getName();
					error += ":\n\t";
					error += "@note cannot be applied when the rule declares ";
					error += "use of symbol tables\n\t(rule is a true grammar ";
					error += "rule).";
					throw error;
				}
				statement.setIsNoteMsg();
				i++;
			}
			// Error
			else
			{
				std::string error = "  Error in definition of ";
				error += statement.getName();
				error += ":\n\t";
				error += "Found @, got \"";
				error += tokens.at(i);
				error += "\", expected: ";
				error += "\"error\" or \"note\".";
				throw error;
			}
		}
		if ( tokens.at(i).compare( "=" ) != 0 )
		{
			std::string error = "  Error in definition of ";
				error += statement.getName();
				error += ":\n\t";
				error += "Found \"";
				error += tokens.at(i);
				error += "\", expected: \"=\", after name and ";
				error += "properties declaration.";
				throw error;
		}
		i++;
		if ( statement.isErrorMsg() || statement.isNoteMsg() )
		{
			if ( !isPermissiveStringLiteral( tokens.at(i) ) ||
				i + 1 == tokens.size() || tokens.at(i+1).compare( ";" ) != 0 )
			{
				std::string error = "  Error in definition of ";
				error += statement.getName();
				error += ":\n\t";
				error += "Message rules must consist of exactly one string ";
				error += "literal token and a\n\tterminating semicolon. First ";
				error += "and second tokens:\n\t[";
				error += tokens.at(i);
				error += "] [";
				if ( i + 1 == tokens.size() )
				{
					error += "END OF TOKEN STREAM";
				}
				else if ( tokens.at(i).compare(";") == 0 )
				{
					error += "PREVIOUS TOKEN IS TERMINATING SEMICOLON";
				}
				else
				{
					error += tokens.at(i+1);
				}
				error += "].";
				throw error;
			}
			std::cout << "\nBegin after newline:\n  " << tokens.at(i) <<
				std::endl;
			statement.addTokenToRule( tokens.at(i) );
			i++;
		}
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
					std::string error = "  Error in definition of ";
					error += statement.getName();
					error += ":\n\t";
					error += "Quotes must enclose exactly one literal token; ";
					error += "Malformed string literal\n\tfound at token ";
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
		if ( statement.getRule().size() == 1 )
		{
			std::string error = "  Error in definition of ";
			error += statement.getName();
			error += ":\n\t";
			error += "Rule must contain content other than the terminating ";
			error += "semicolon.";
			throw error;
		}
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
			"Centered? " << ( ( statements.at(i).isCenter() ) ?
			"Yes; " : "No; " ) <<
			"ST: " << (int)( SYMBOL_DEPTH( statements.at(i).getFlags() ) ) <<
			" P: " << (int)( PERMEATE_DEPTH( statements.at(i).getFlags() ) ) <<
			" C: " << (int)( CENTERING_SPOT( statements.at(i).getFlags() ) ) <<
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

	ret = regcomp( &regex , regexString.c_str() , REG_EXTENDED );
	if( ret )
	{
		regerror( ret , &regex , msgbuf , len );
		std::string errormsg = msgbuf;
		free( msgbuf );
		regfree( &regex );
		throw errormsg;
	}

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
	regfree( &regex );
	free( msgbuf );
	return success;
}

bool ParseLang::isStringLiteral( std::string token )
{
	return matchRegex( "^\"[^ \t\n]+\"$" , token );
}

bool ParseLang::isPermissiveStringLiteral( std::string token )
{
	return matchRegex( "^\".*\"$" , token );
}

std::vector<std::string> ParseLang::getSimilarTokens(
	std::vector<std::string> strings , std::string token )
{
	std::vector<std::string> simStrings;
	// Not a long enough token to be worth the work
	if ( token.size() <= 3 )
	{
		return simStrings;
	}
	// Terrible algorithm
	for ( int i = 0; i < strings.size(); i++ )
	{
		// Not a long enough string to be worth the work
		if ( strings.at(i).size() <= 3 )
		{
			continue;
		}
		// If they are the same length but one or two characters are different
		int mistakes = 0;
		// Loop over characters in strings
		if ( token.size() == strings.at(i).size() )
		{
			for ( int j = 0; j < token.size(); j++ )
			{
				if ( token.at(j) != strings.at(i).at(j) )
				{
					mistakes++;
				}
			}
			if ( mistakes <= 2 )
			{
				simStrings.push_back( strings.at(i) );
				continue;
			}
		}
		bool isSubstr = false;
		// If they share a substring of length four. Terrible
		for ( int j = 0; j < strings.at(i).size() - 3; j++ )
		{
			std::string temp = strings.at(i).substr( j , 4 );
			for ( int k = 0; k < token.size() - 3; k++ )
			{
				if ( temp.compare( token.substr( k , 4 ) ) == 0 )
				{
					// They share a substr of length four
					simStrings.push_back( strings.at(i) );
					isSubstr = true;
					break;
				}
			}
			if ( isSubstr )
			{
				break;
			}
		}
		if ( isSubstr )
		{
			continue;
		}
		// See if they share mostly the same characters
		int misses = 0;
		bool hit = false;
		std::string temp = strings.at(i);
		std::string temp2 = token;
		for ( int j = 0; j < temp.size(); j++ )
		{
			for ( int k = 0; k < temp2.size(); k++ )
			{
				if ( temp.at(j) == temp2.at(k) )
				{
					temp.erase( j , 1 );
					j--;
					temp2.erase( k , 1 );
					k--;
					hit = true;
					break;
				}
			}
			if ( hit )
			{
				hit = false;
				continue;
			}
			misses++;
		}
		if ( misses <= 2 )
		{
			simStrings.push_back( strings.at(i) );
			continue;
		}
	}
	return simStrings;
}

