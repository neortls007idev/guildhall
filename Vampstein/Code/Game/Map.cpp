#include "Game/Map.hpp"

#include "Engine/Core/DevConsole.hpp"
#include "Game/TileMap.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::Map( char const* mapName ) :
	m_name( mapName )
{
	m_parsedSuccessfully = false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::Map( char const* mapName , XMLElement* element )
{
	m_parsedSuccessfully = false;
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
		
		if ( newMap->m_parsedSuccessfully == false )
		{
			g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_ERROR ,
										  "ERROR: %s of TYPE: %s FAILED to parse successfully" , mapType.c_str() );
			newMap = nullptr;
		}
		
		return newMap;
	}
	
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::~Map()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------