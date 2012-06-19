// These functions implement the shunting yard algorithm, used to convert an
// infix notation arithmetic expression to a postfix notation expression.
// This function supports integer and floating point constants, as well as
// variable names

// TODO: Uncomment mod (%) as an operator, and add support for mod operations
// in the arithmetic expression assembler and several other functions

#include <string>
#include <stack>
#include <queue>
#include "SimpleTextUtil.h"
using namespace std;

std::string SimpleTextUtil::convertInfixToPostfix( std::string infix )
	throw( std::string )
{
	bool unaryNegPoss = true;
	// Loop over the infix string
	for ( int i = 0; i < infix.size(); i++ )
	{
		// Skip spaces
		if ( infix.at( i ) == ' ' )
		{
			continue;
		}
		// If the next token is an arithmetic operator
		else if ( SimpleTextUtil::isOperator( infix.at( i ) ) )
		{
			if ( unaryNegPoss && infix.at( i ) == '-' )
			{
				SimpleTextUtil::postfix.push( infix.at( i ) );
				continue;
			}
			SimpleTextUtil::postfix.push( ' ' );
			// While that token has the same or less precedence as the current
			// top stack token, pop the stack tokens into the postfix queue
			while ( SimpleTextUtil::opStack.size() > 0 &&
				SimpleTextUtil::opStack.top() != '(' && 
				SimpleTextUtil::operatorForcePop( infix.at(i) ) )
			{
				SimpleTextUtil::postfix.push( SimpleTextUtil::opStack.top() );
				SimpleTextUtil::postfix.push( ' ' );
				SimpleTextUtil::opStack.pop();
			}
			// Then push the new operator
			SimpleTextUtil::opStack.push( infix.at(i) );
			// Set possibility of next token being a unary negative
			unaryNegPoss = true;
		}
		// Unconditionally add a left paren to the stack
		else if ( infix.at( i ) == '(' )
		{
			// Push the opening paren
			SimpleTextUtil::opStack.push( infix.at( i ) );
			// Set possibility of next token being a unary negative
			unaryNegPoss = true;
		}
		// If the next operator is the right paren, pop
		else if ( infix.at( i ) == ')' )
		{
			// Pop all operators into the queue until the first left paren
			while ( SimpleTextUtil::opStack.size() > 0 && 
				SimpleTextUtil::opStack.top() != '(' )
			{
				SimpleTextUtil::postfix.push( ' ' );
				SimpleTextUtil::postfix.push( SimpleTextUtil::opStack.top() );
				SimpleTextUtil::opStack.pop();
			}
			// If the stack is not empty, we can safely pop the '('
			if ( SimpleTextUtil::opStack.size() > 0 )
			{
				// Pop '('
				SimpleTextUtil::opStack.pop();
			}
			else
			{
				string err;
				err += "Mismatched parens in shunt-yard conversion: No left to";
				err += " a lonely right.\nExpression that failed: ";
				err += infix;
				throw err;
			}
		}
		// If it isn't an operator, it must be a variable identifier or a
		// constant
		else
		{
			SimpleTextUtil::postfix.push( infix.at( i ) );
			// Next operator cannot be a unary negative
			unaryNegPoss = false;
		}
	}
	// If there are still tokens in the stack, pop them into the queue
	while ( SimpleTextUtil::opStack.size() > 0 )
	{
		if ( SimpleTextUtil::opStack.top() == '(' )
		{
			string err;
			err += "Mismatched parens in shunt-yard conversion: No right to a ";
			err += "lonely left.\nExpression that failed: ";
			err += infix;
			throw err;
			// UNMATCHED PARENTHESIS ERROR
		}
		SimpleTextUtil::postfix.push( ' ' );
		SimpleTextUtil::postfix.push( SimpleTextUtil::opStack.top() );
		SimpleTextUtil::opStack.pop();
	}
	std::string postfixString;
	while ( SimpleTextUtil::postfix.size() > 0 )
	{
		postfixString += SimpleTextUtil::postfix.front();
		SimpleTextUtil::postfix.pop();
	}
	//
	// CLEAN UP STRING HERE, SPACES WISE
	//
	return postfixString;
}

bool SimpleTextUtil::operatorForcePop( char op )
{
	// If stack is empty, operators can't be forced off by precedence
	if ( SimpleTextUtil::opStack.size() == 0 )
	{
		return false;
	}
	// Operator on top of stack
	char top = SimpleTextUtil::opStack.top();
	// If our next operator is + or -, then 
	if ( ( op == '+' || op == '-' ) )
	{
		// All arithmetic operators on top of stack would need to be pushed
		if ( top == '+' || top == '-' || top == '*' || top == '/' ) 
		{
			return true;
		}
	}
	// Higher order operators only force high order operators off the stack
	if ( ( op == '*' || op == '/' ) && ( top == '*' || top == '/' ) )
	{
		return true;
	}
	// High order operators are pushed on top of lower order operators
	if ( ( op == '*' || op == '/' ) && ( top == '+' || top == '-' ) )
	{
		return false;
	}
}

bool SimpleTextUtil::isOperator( char operatorCandidate )
{
	switch ( operatorCandidate )
	{
		case '+':
		case '-':
		case '*':
		case '/':
		// See isOperator(string) and above
		//case '%':
			return true;
		default:
			return false;
	}
}

bool SimpleTextUtil::isOperator( std::string operatorCandidate )
{
	if ( operatorCandidate.compare( "*" ) == 0 )
	{
		return true;
	}
	if ( operatorCandidate.compare( "/" ) == 0 )
	{
		return true;
	}
	if ( operatorCandidate.compare( "+" ) == 0 )
	{
		return true;
	}
	if ( operatorCandidate.compare( "-" ) == 0 )
	{
		return true;
	}
	// In order to add support for mod, need to update the arithmetic expression
	// assembler, as well as several other things
	/*
	if ( operatorCandidate.compare( "%" ) == 0 )
	{
		return true;
	}
	*/
	return false;
}


