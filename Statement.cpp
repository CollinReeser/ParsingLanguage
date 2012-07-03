
#include "Statement.h"

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
