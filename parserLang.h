
#ifndef PARSER_LANG_H
#define PARSER_LANG_H

#include <string>
#include <vector>
#include "lexer/lexer.h"

#define NEW_TABLE ( 1l << ( 8 * sizeof(unsigned long long int) - 1 ) )
#define PERMEATE ( 1l << ( 8 * sizeof(unsigned long long int) - 2 ) )
#define SYMBOL_DEPTH(x) ( ( x & 0xFF00 ) >> 8 )
#define PERMEATE_DEPTH(x) ( x & 0xFF )

class Statement
{
public:
	Statement();
	void setName( std::string name );
	std::string getName() const;
	void addTokenToRule( std::string token );
	std::vector<std::string> getRule() const;
	void setAnchor( int anchor );
	void setSymbolTableNum( int num );
	void setPermeateNum( int num );
	bool isNew();
	bool isPermeate();
	unsigned long long int getFlags();
	//Operator overloading. We need copying
	Statement& operator=( const Statement &other );
private:
	std::string name;
	std::vector<std::string> rule;
	int anchor;
	unsigned long long int flags;
};

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
	// This brings in rules from other files. It first searches for the
	// include_rules statement name, and then performs special operations on it.
	// This stage is performed after parseDescription and before any of the
	// other sanity checks. If the parameter is true, print statements are
	// suppressed
	void pullRuleSets( bool nested );
	// This adds the statements from tempParse to this, ensuring no duplicates
	void mergeStatementList( std::string filename );
	bool isOperator( std::string op );
	bool isKeyword( std::string keyword );
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
	Scope topScope;
	std::string parseFile;
};

#endif
