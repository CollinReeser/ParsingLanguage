
#include <stack>
#include <vector>
#include <string>
#include "Statement.h"

// Takes an int and returns a string representation
static std::string castIntToString( int val )
{
	std::stringstream ss;
	ss << val;
	return ss.str();
}

std::vector<std::string> Statement::getListRuleNames(
	const std::vector<Statement> statements )
{
	std::vector<std::string> names;
	for ( int i = 0; i < statements.size(); i++ )
	{
		names.push_back( statements.at(i).getName() );
	}
	return names;
}

void Statement::syntaxTransformOne()
{
	// Note that anywhere a "_" appears is indicative of the semi-arbitrary
	// decision to use underscore as an indicator token in the stack for use
	// with segemnting the tokens on the stack
	std::vector<std::string> newSyntaxRule;
	std::stack<std::string,std::vector<std::string> > opStack;
	int i = 0;
	while ( rule.at(i).compare(";") != 0 )
	{
		if ( rule.at(i).compare( "symbol" ) == 0 || 
			rule.at(i).compare( "newsym" ) == 0 || 
			rule.at(i).compare( "arbsym" ) == 0 ||
			rule.at(i).compare( "#" ) == 0 ||
			rule.at(i).compare( ":" ) == 0 ||
			rule.at(i).compare( "@" ) == 0 ||
			rule.at(i).compare( "^" ) == 0 ||
			rule.at(i).at(0) == '\"' )
		{
			newSyntaxRule.push_back( rule.at(i) );
			i++;
		}
		else if ( rule.at(i).compare( "!" ) == 0 ||
			rule.at(i).compare( "*" ) == 0 )
		{
			opStack.push( rule.at(i) );
			i++;
		}
		else if ( rule.at(i).compare( ">" ) == 0 ||
			rule.at(i).compare( "|" ) == 0 )
		{
			while ( !opStack.empty() && opStack.top().compare( "_" ) != 0 &&
				opStack.top().compare( "|" ) != 0 )
			{
				newSyntaxRule.push_back( opStack.top() );
				opStack.pop();
			}
			if ( rule.at(i).compare( "|" ) == 0 )
			{
				newSyntaxRule.push_back( rule.at(i) );
			}
			i++;
		}
		else if ( rule.at(i).compare( ")" ) == 0 )
		{
			while ( !opStack.empty() && opStack.top().compare( "_" ) != 0 )
			{
				newSyntaxRule.push_back( opStack.top() );
				opStack.pop();
			}
			newSyntaxRule.push_back( rule.at(i) );
			i++;
		}
		else if ( rule.at(i).compare( "(" ) == 0 )
		{
			newSyntaxRule.push_back( rule.at(i) );
			opStack.push( "_" );
			i++;
		}
		else if ( rule.at(i).compare( "NULL" ) == 0 )
		{
			opStack.push( rule.at(i) );
			i++;
			if ( rule.at(i).compare( "|" ) == 0 )
			{
				opStack.push( rule.at(i) );
				i++;
			}
		}
		// HERE, WE NEED TO CHECK FOR INTEGER VALUES SUCH AS SYMBOL TABLE
		// SELECTION
		/*
		else if ()
		{

		}
		*/
		// Here, we shouldn't ever enter because it means we have met with
		// unexpected input, which the parser should have notified us of ahead
		// of time, but nonetheless
		else
		{
			std::string error = "  Error on syntactical transform one:\n\t";
			error += "Unexpected input on token ";
			error += castIntToString( i );
			error += " of rule ";
			error += Statement::name;
			error += ", ";
			error += rule.at(i);
			error += ".";
			throw error;
		}
	}
	while ( !opStack.empty() )
	{
		newSyntaxRule.push_back( opStack.top() );
		opStack.pop();
	}
	Statement::ruleTransOne = newSyntaxRule;
	return;
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

void Statement::setCenterLocation( int num )
{
	flags |= CENTER;
	flags |= num << 16;
	return;
}

void Statement::setIsErrorMsg()
{
	flags |= ERROR;
	return;
}

void Statement::setIsNoteMsg()
{
	flags |= NOTE;
	return;
}

bool Statement::isNew()
{
	return ( flags >> 63 ) & 0b1;
}

bool Statement::isPermeate()
{
	return ( flags >> 62 ) & 0b1;
}

bool Statement::isCenter()
{
	return ( flags >> 61 ) & 0b1;
}

bool Statement::isErrorMsg()
{
	return ( flags >> 60 ) & 0b1;
}

bool Statement::isNoteMsg()
{
	return ( flags >> 59 ) & 0b1;
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
