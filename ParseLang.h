
#ifndef PARSER_LANG_H
#define PARSER_LANG_H

#include <string>
#include <vector>
#include "Statement.h"
#include "lexer/lexer.h"


class SymbolTable
{
private:
	std::string name;
	std::vector<std::string> symbols;
};

class Scope
{
private:
	Scope* upper;
	Scope* lower;
	std::vector<SymbolTable> table;
};

class ParseLang
{
public:
	ParseLang( std::string parseFile , std::string sourcefile );
	ParseLang( std::string parseFile );
	ParseLang();
	void printPassOne();
	// This is a very poor implementation of some terribly designed algorithm
	// that is supposed to take a list of strings and a token, and return a
	// list of strings as a subset of the passed list of strings that is similar
	// to the passed token
	static std::vector<std::string> getSimilarTokens(
		std::vector<std::string> strings , std::string token );
private:
	void toplevelVerification( bool quiet , std::string parseFile );
	void parseSourceFile( std::string sourcefile ,
		std::vector<std::string> (*lexer_function)(std::string) =
		tokenizeFile2 );
	// This builds a table of raw statements, parsing the name and properties
	// of the statement, and then simply throwing all of the tokens of the rule
	// into a vector for later
	void parseDescription( std::string parseFile );
	// This ensures that all parentheses are matched, and that they are used
	// correctly (no empty parens, etc)
	void ensureParentheses();
	// This ensures that no rule starts with invalid tokens
	void ensureValidStart();
	// This ensures there are no two statements with the same name
	void ensureNoRuleDuplicates();
	// This ensures that operators are used correctly
	void ensureOperatorUsage();
	// This ensures that there are not two symbol tokens in a row
	void ensureSymbolsOperatorSeparated();
	// Ensures that all non-literal tokens in all rules are either keywords or
	// rule names
	void ensureMeaningfulTokens();
	// This brings in rules from other files. It first searches for the
	// include_rules statement name, and then performs special operations on it.
	// This stage is performed after parseDescription and before any of the
	// other sanity checks. If the parameter is true, print statements are
	// suppressed
	void pullRuleSets( bool nested );
	// This finds create_list statements and creates those lists
	void createLists();
	// This adds the statements from tempParse to this, ensuring no duplicates
	void mergeStatementList( std::string filename );
	bool isOperator( std::string op );
	bool isKeyword( std::string keyword );
	// This returns true if the string passed in is considered a string literal
	// containing no whitespace
	bool isStringLiteral( std::string token );
	// This returns true if the string passed in is considered a string literal
	// containing anything
	bool isPermissiveStringLiteral( std::string token );
	// Checks if the string is the name of an established rule in the list of
	// statements
	bool isRuleName( std::string token );
	// Checks if the string is the name of an established msg rule in the list
	// of statements
	bool isMsgName( std::string token );
	// This takes in a regular expression and a string to run the regular
	// expression on, and returns true if the string was matched by the regular
	// expression
	bool matchRegex( std::string regex , std::string token );
	// Match characters
	bool matchChar( std::string characters , std::string token );
	// Determine whether an # string is a regular expression or a character
	// list matching request, and then evaluates whichever was requested
	bool matchOctothorpeString( std::string octostring , std::string token );
	std::vector<Statement> statements;
	std::vector<std::vector<std::string> > lists; 
	Scope topScope;
	std::string parseFile;
};

#endif
