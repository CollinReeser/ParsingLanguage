
#ifndef STATEMENT_H
#define STATEMENT_H

#include <vector>
#include <string>

// Forward declaration of ParseLang needed because ParseLang.h and Statement.h
// are now interdependent
class ParseLang;

#define ERROR     ( 1l << ( 8 * sizeof(unsigned long long int) - 4 ) )
#define NOTE      ( 1l << ( 8 * sizeof(unsigned long long int) - 5 ) )

class Statement
{
public:
	Statement();
	void setName( std::string name );
	std::string getName() const;
	void addTokenToRule( std::string token );
	std::vector<std::string> getRule() const;
	std::vector<std::string> getRuleTransOne() const;
	void setAnchor( int anchor );
	void setIsErrorMsg();
	void setIsNoteMsg();
	bool isErrorMsg() const;
	bool isNoteMsg() const;
	unsigned long long int getFlags();
	//Operator overloading. We need copying
	Statement& operator=( const Statement &other );
	// Performs a transform on the rule to produce ruleTransOne that carries the
	// same meaning as rule while in a friendlier-to-interpret syntax
	void syntaxTransformOne( const ParseLang& parse );
	static std::vector<std::string> getListRuleNames(
		const std::vector<Statement> statements );
private:
	std::string name;
	std::vector<std::string> rule;
	std::vector<std::string> ruleTransOne;
	int anchor;
	unsigned long long int flags;
};

#endif
