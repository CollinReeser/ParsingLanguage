
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <vector>
#include <string>

class NamedVector
{
public:
	// Create a new named symbol table
	NamedVector( std::string name );
	// Attempts to add a string to the table, returns true if it is a new
	// entry, and false if the entry already exists
	bool addEntry( std::string entry );
	// // Adds a list of strings to the table, ensuring there are no duplicates
	// // but otherwise not caring whether there are already certain entries
	// void addEntries( std::vector<std::string> entries );
	// Returns true if the passed string is already an entry in the table
	bool isEntry( std::string entry );
	// Returns true if the table is the same name as the 
	// name passed in, case sensitive
	bool matchNamedTable( std::string posName );
	// Returns the name of the table
	std::string getName();
	// Returns the list table
	std::vector<std::string> getTable();
private:
	std::string name;
	std::vector<std::string> table;
};

class SymbolTable
{
	SymbolTable();
	// Creates a new hierarchial layer below the current lowest one, with 
	// some number of tables at the new layer
	void newLayer( int tablesInLayer = 1 );
	// Destroys the lowest layer
	void destroyLowestLayer();
	// Attempts to add a new entry into the table specified at the hierarchial
	// layer specified. If the entry is already present, return false.
	// Otherwise, return true for success in adding the entry
	bool addEntryToLayer( std::string entry , int layer , int table );
	// Attempts to find an entry in the table specified at the hierarchial
	// layer specified. If the entry is present, return true. Otherwise, return
	// false for failure in adding the entry
	bool isEntryInLayer( std::string entry , int layer , int table );
private:
	std::vector<std::vector<std::vector<std::string> > > hierarchialTables;
	std::vector<NamedVector> namedTables;
	int layers;
};

#endif
