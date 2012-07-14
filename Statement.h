
#ifndef STATEMENT_H
#define STATEMENT_H

#include <vector>
#include <string>

#define NEW_TABLE ( 1l << ( 8 * sizeof(unsigned long long int) - 1 ) )
#define PERMEATE  ( 1l << ( 8 * sizeof(unsigned long long int) - 2 ) )
#define CENTER    ( 1l << ( 8 * sizeof(unsigned long long int) - 3 ) )
#define ERROR     ( 1l << ( 8 * sizeof(unsigned long long int) - 4 ) )
#define NOTE      ( 1l << ( 8 * sizeof(unsigned long long int) - 5 ) )
#define SYMBOL_DEPTH(x)   ( ( x & 0xFF00   ) >> 8 )
#define PERMEATE_DEPTH(x) (   x & 0xFF     )
#define CENTERING_SPOT(x) ( ( x & 0xFF0000 ) >> 16 )

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
	void setCenterLocation( int num );
	void setIsErrorMsg();
	void setIsNoteMsg();
	bool isNew();
	bool isPermeate();
	bool isCenter();
	bool isErrorMsg();
	bool isNoteMsg();
	unsigned long long int getFlags();
	//Operator overloading. We need copying
	Statement& operator=( const Statement &other );
	// Performs a transform on the rule to produce ruleTransOne that carries the
	// same meaning as rule while in a friendlier-to-interpret syntax
	void syntaxTransformOne();
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
