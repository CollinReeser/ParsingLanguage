
#include "SimpleTextUtil.h"

bool SimpleTextUtil::isOperatorT( char operatorCandidate )
{
	switch ( operatorCandidate )
	{
		case '+':
		case '-':
		case '*':
		case '/':
		case '=':
		case '(':
		case ')':
		case '<':
		case '>':
		case ';':
		case '"':
		case '%':
		case '&':
		case '\'':
		case '.':
		case '[':
		case ']':
		case '{':
		case '}':
		case '|':
		case '^':
		case '~':
		case ':':
		case ',':
		case '@':
		case '#':
			return true;
		default:
			return false;
	}
}

std::string SimpleTextUtil::stripWhitespace(std::string str)
{
	for ( int i = 0; i < str.size(); i++ )
	{
		if ( str.at(i) == ' ' || str.at(i) == '\t' || str.at(i) == '\n' || 
			str.at(i) == '\r' )
		{
			str.erase( str.begin() + i );
			i--;
		}
		else
		{
			break;
		}
	}
	for ( int i = str.size() - 1; i > -1; i-- )
	{
		if ( str.at(i) == ' ' || str.at(i) == '\t' || str.at(i) == '\n' || 
			str.at(i) == '\r')
		{
			str.erase( str.end() - i );
			i--;
		}
		else
		{
			break;
		}
	}
	return str;
}
