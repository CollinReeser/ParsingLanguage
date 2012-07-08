
#include <vector>
#include <string>

NamedVector::NamedVector( std::string name )
{
	NamedVector::name = name;
}

bool NamedVector::addEntry( std::string entry )
{
	for ( int i = 0; i < table.size(); i++ )
	{
		if ( table.at(i).compare( entry ) == 0 )
		{
			return false;
		}
	}
	table.push_back( entry );
	return true;
}

bool NamedVector::isEntry( std::string entry )
{
	for ( int i = 0; i < table.size(); i++ )
	{
		if ( table.at(i).compare( entry ) == 0 )
		{
			return true;
		}
	}
	return false;
}

bool NamedVector::matchNamedTable( std::string posName )
{
	if ( name.compare( posName ) == 0 )
	{
		return true;
	}
	return false;
}


SymbolTable::SymbolTable()
{
	layers = 0;
}

void SymbolTable::newLayer( int tablesInLayer )
{
	std::vector<std::vector<std::string> > newLayer;
	for ( int i = 0; i < tablesInLayer; i++ )
	{
		std::vector<std::string> temp;
		newLayer.push_back( temp );
	}
	hierarchialTables.push_back( newLayer );
	layers++;
	return;
}

void SymbolTable::destroyLowestLayer()
{
	hierarchialTables.erase( hierarchialTables.end() - 1 );
	layers--;
	return;
}

bool SymbolTable::addEntryToLayer( std::string entry , int layer , int table )
{
	if ( hierarchialTables.size() <= layer && 
		hierarchialTables.at( layer ).size() <= table )
	{
		return false;
	}
	for ( int i = 0; i < hierarchialTables.at( layer ).at( table ).size(); 
		i++ )
	{
		if ( hierarchialTables.at( layer ).at( table ).at(i).
			compare( entry ) == 0 )
		{
			return false;
		}
	}
	hierarchialTables.at( layer ).at( table ).push_back( entry );
	return true;
}

bool SymbolTable::isEntryInLayer( std::string entry , int layer , int table )
{
	if ( hierarchialTables.size() > layer && 
		hierarchialTables.at( layer ).size() > table )
	{
		for ( int i = 0; i < hierarchialTables.at( layer ).at( table ).size(); 
			i++ )
		{
			if ( hierarchialTables.at( layer ).at( table ).at(i).
				compare( entry ) == 0 )
			{
				return true;
			}
		}
	}
	return false;
}
