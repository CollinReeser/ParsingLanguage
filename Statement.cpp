
#include <stack>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include "ParseLang.h"
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

// Used for syntaxTransformOne
struct TokenMovement
{
	// Incremented for every (, decremented for every )
	int position;
	std::string token;
};

// TEMPORARILY (maybe): This function only moves the * and ! operators to
// AFTER the thing it is attached to, instead of before
void Statement::syntaxTransformOne( const ParseLang& parse )
{
	// Note that anywhere a "_" appears is indicative of the semi-arbitrary
	// decision to use underscore as an indicator token in the stack for use
	// with segmenting the tokens on the stack
	std::vector<std::string> newSyntaxRule;
	std::vector<TokenMovement> tokenTracker;
	int i = 0;
	while ( i < rule.size() && rule.at(i).compare(";") != 0 )
	{
		//std::cout << rule.at(i) << std::endl;
		if ( rule.at(i).compare( "*" ) != 0 &&
			rule.at(i).compare( "!" ) != 0 && 
			rule.at(i).compare( "(" ) != 0 && 
			rule.at(i).compare( ")" ) != 0 )
		{
			for ( int j = 0; j < tokenTracker.size(); j++ )
			{
				if ( tokenTracker.at(j).position == 0 )
				{
					newSyntaxRule.push_back( tokenTracker.at(j).token );
					tokenTracker.erase( tokenTracker.begin() + j );
					j--;
				}
			}
			newSyntaxRule.push_back( rule.at(i) );
			i++;
		}
		else if ( rule.at(i).compare( "!" ) == 0 ||
			rule.at(i).compare( "*" ) == 0 )
		{
			if ( rule.at(i).compare( "!" ) == 0 &&
				rule.at(i+1).compare( "#" ) == 0 )
			{
				newSyntaxRule.push_back( rule.at(i) );
				newSyntaxRule.push_back( rule.at(i+1) );
				i++;
			}
			else if ( rule.at(i+1).compare( "(" ) != 0 &&
				rule.at(i+1).compare( "!" ) != 0 && 
				rule.at(i+1).compare( "*" ) != 0 )
			{
				newSyntaxRule.push_back( rule.at(i+1) );
				newSyntaxRule.push_back( rule.at(i) );
				i++;
			}
			else
			{
				TokenMovement temp;
				temp.position = 0;
				temp.token = rule.at(i);
				tokenTracker.push_back( temp );
			}
			i++;
		}
		else if ( rule.at(i).compare( "(" ) == 0 )
		{
			newSyntaxRule.push_back( rule.at(i) );
			for ( int j = 0; j < tokenTracker.size(); j++ )
			{
				tokenTracker.at(j).position++;
			}
			i++;
		}
		else if ( rule.at(i).compare( ")" ) == 0 )
		{
			newSyntaxRule.push_back( rule.at(i) );
			for ( int j = 0; j < tokenTracker.size(); j++ )
			{
				tokenTracker.at(j).position--;
			}
			i++;
		}
	}
	while ( tokenTracker.size() > 0 )
	{
		newSyntaxRule.push_back( 
			tokenTracker.at( tokenTracker.size() - 1 ).token );
		tokenTracker.erase( tokenTracker.end() );
	}
	Statement::ruleTransOne = newSyntaxRule;
	Statement::ruleTransOne.push_back( ";" );
	return;
	//std::stack<std::string> opStack;
	// int i = 0;
	// while ( i < rule.size() && rule.at(i).compare(";") != 0 )
	// {
	// 	if ( parse.isKeyword( rule.at(i) ) ||
	// 		rule.at(i).compare( "#" ) == 0 ||
	// 		rule.at(i).compare( ":" ) == 0 ||
	// 		rule.at(i).compare( "@" ) == 0 ||
	// 		rule.at(i).compare( "^" ) == 0 ||
	// 		rule.at(i).at(0) == '\"' ||
	// 		parse.isRuleName( rule.at(i) ) )
	// 	{
	// 		newSyntaxRule.push_back( rule.at(i) );
	// 		i++;
	// 	}
	// 	else if ( rule.at(i).compare( "!" ) == 0 ||
	// 		rule.at(i).compare( "*" ) == 0 )
	// 	{
	// 		opStack.push( rule.at(i) );
	// 		i++;
	// 	}
	// 	else if ( rule.at(i).compare( ">" ) == 0 ||
	// 		rule.at(i).compare( "|" ) == 0 )
	// 	{
	// 		while ( !opStack.empty() && opStack.top().compare( "_" ) != 0 &&
	// 			opStack.top().compare( "|" ) != 0 )
	// 		{
	// 			newSyntaxRule.push_back( opStack.top() );
	// 			opStack.pop();
	// 		}
	// 		newSyntaxRule.push_back( rule.at(i) );
	// 		i++;
	// 	}
	// 	else if ( rule.at(i).compare( ")" ) == 0 )
	// 	{
	// 		while ( !opStack.empty() && opStack.top().compare( "_" ) != 0 )
	// 		{
	// 			newSyntaxRule.push_back( opStack.top() );
	// 			opStack.pop();
	// 		}
	// 		newSyntaxRule.push_back( rule.at(i) );
	// 		i++;
	// 	}
	// 	else if ( rule.at(i).compare( "(" ) == 0 )
	// 	{
	// 		newSyntaxRule.push_back( rule.at(i) );
	// 		opStack.push( "_" );
	// 		i++;
	// 	}
	// 	// Here, we shouldn't ever enter because it means we have met with
	// 	// unexpected input, which the parser should have notified us of ahead
	// 	// of time, but nonetheless
	// 	else
	// 	{
	// 		std::string error = "  Error on syntactical transform one:\n\t";
	// 		error += "Unexpected input on token ";
	// 		error += castIntToString( i );
	// 		error += " of rule ";
	// 		error += Statement::name;
	// 		error += ", ";
	// 		error += rule.at(i);
	// 		error += ".";
	// 		throw error;
	// 	}
	// }
	// while ( !opStack.empty() )
	// {
	// 	newSyntaxRule.push_back( opStack.top() );
	// 	opStack.pop();
	// }
	// Statement::ruleTransOne = newSyntaxRule;
	// return;
	// while ( i < rule.size() && rule.at(i).compare(";") != 0 )
	// {
	// 	if ( rule.at(i).compare( "symbol" ) == 0 || 
	// 		rule.at(i).compare( "newsym" ) == 0 || 
	// 		rule.at(i).compare( "arbsym" ) == 0 ||
	// 		rule.at(i).compare( "#" ) == 0 ||
	// 		rule.at(i).compare( ":" ) == 0 ||
	// 		rule.at(i).compare( "@" ) == 0 ||
	// 		rule.at(i).compare( "^" ) == 0 ||
	// 		rule.at(i).at(0) == '\"' ||
	// 		parse.isRuleName( rule.at(i) ) )
	// 	{
	// 		newSyntaxRule.push_back( rule.at(i) );
	// 		i++;
	// 	}
	// 	else if ( rule.at(i).compare( "!" ) == 0 ||
	// 		rule.at(i).compare( "*" ) == 0 )
	// 	{
	// 		opStack.push( rule.at(i) );
	// 		i++;
	// 	}
	// 	else if ( rule.at(i).compare( ">" ) == 0 ||
	// 		rule.at(i).compare( "|" ) == 0 )
	// 	{
	// 		while ( !opStack.empty() && opStack.top().compare( "_" ) != 0 &&
	// 			opStack.top().compare( "|" ) != 0 )
	// 		{
	// 			newSyntaxRule.push_back( opStack.top() );
	// 			opStack.pop();
	// 		}
	// 		newSyntaxRule.push_back( rule.at(i) );
	// 		i++;
	// 	}
	// 	else if ( rule.at(i).compare( ")" ) == 0 )
	// 	{
	// 		while ( !opStack.empty() && opStack.top().compare( "_" ) != 0 )
	// 		{
	// 			newSyntaxRule.push_back( opStack.top() );
	// 			opStack.pop();
	// 		}
	// 		newSyntaxRule.push_back( rule.at(i) );
	// 		i++;
	// 	}
	// 	else if ( rule.at(i).compare( "(" ) == 0 )
	// 	{
	// 		newSyntaxRule.push_back( rule.at(i) );
	// 		opStack.push( "_" );
	// 		i++;
	// 	}
	// 	else if ( rule.at(i).compare( "NULL" ) == 0 )
	// 	{
	// 		opStack.push( rule.at(i) );
	// 		i++;
	// 		if ( rule.at(i).compare( "|" ) == 0 )
	// 		{
	// 			opStack.push( rule.at(i) );
	// 			i++;
	// 		}
	// 	}
	// 	// Here, we shouldn't ever enter because it means we have met with
	// 	// unexpected input, which the parser should have notified us of ahead
	// 	// of time, but nonetheless
	// 	else
	// 	{
	// 		std::string error = "  Error on syntactical transform one:\n\t";
	// 		error += "Unexpected input on token ";
	// 		error += castIntToString( i );
	// 		error += " of rule ";
	// 		error += Statement::name;
	// 		error += ", ";
	// 		error += rule.at(i);
	// 		error += ".";
	// 		throw error;
	// 	}
	// }
	// while ( !opStack.empty() )
	// {
	// 	newSyntaxRule.push_back( opStack.top() );
	// 	opStack.pop();
	// }
	// Statement::ruleTransOne = newSyntaxRule;
	// return;
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

std::vector<std::string> Statement::getRuleTransOne() const
{
	return ruleTransOne;
}

void Statement::addTokenToRule( std::string token )
{
	rule.push_back( token );
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

bool Statement::isErrorMsg() const
{
	return ( flags >> 60 ) & 0b1;
}

bool Statement::isNoteMsg() const
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
