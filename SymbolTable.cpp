
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
