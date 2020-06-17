#include "Game/Map.hpp"

#include "TileMap.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::Map( char const* mapName ) :
	m_name( mapName )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::Map( char const* mapName , XMLElement* element )
{
	XMLElement* root = element;
	std::string mapType = element->Attribute( "type"  "NULL" );
	CreateNewMapOfType( mapType , mapName , root );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC Map* Map::CreateNewMapOfType( std::string mapType , char const* mapName , XMLElement* rootElement )
{
	if( _stricmp( mapType.c_str() , "TileMap" ) == 0 )
	{
		Map* newMap = new TileMap( mapName , rootElement );
		return newMap;
	}
	
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::~Map()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------