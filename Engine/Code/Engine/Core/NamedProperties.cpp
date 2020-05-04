#include "Engine/Core/NamedProperties.hpp"

/*
NamedProperties::~NamedProperties()
{
	for ( auto iter : m_keyValuePairs )
	{
		delete iter.second;
		iter.second = nullptr;
	}

	m_keyValuePairs.clear();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

TypedPropertyBase* NamedProperties::FindInMap( std::string const& key ) const
{
	auto iter = m_keyValuePairs.find( key );
	if ( iter != m_keyValuePairs.end() )
	{
		return iter->second;
	}
	else
	{
		return nullptr;
	}
}
*/