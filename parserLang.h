
#ifndef PARSER_LANG_H
#define PARSER_LANG_H

#include <string>
#include <vector>
#include "lexer.h"

#define NEW_TABLE ( 1l << ( 8 * sizeof(unsigned long long int) - 1 ) )
#define PERMEATE ( 1l << ( 8 * sizeof(unsigned long long int) - 2 ) )
#define SYMBOL_DEPTH(x) ( ( x & 0xFF00 ) >> 8 )
#define PERMEATE_DEPTH(x) ( x & 0xFF )

class Statement
{
public:
	Statement();
	void setName( std::string name );
	std::string getName();
	void addTokenToRule( std::string token );
	std::vector<std::string> getRule();
	void setAnchor( int anchor );
	void setSymbolTableNum( int num );
	void setPermeateNum( int num );
	bool isNew();
	bool isPermeate();
	unsigned long long int getFlags();
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
	void printPassOne();
private:
	void parseDescription( std::string parseFile );
	void ensureParentheses();
	void ensureValidStart();
	void ensureNoRuleDuplicates();
	void ensureOperatorUsage();
	void ensureSymbolsOperatorSeparated();
	bool isOperator( std::string op );
	std::vector<Statement> statements;
	Scope topScope;
};

#endif
