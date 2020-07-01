#include "Game/Map.hpp"


#include "Actor.hpp"
#include "Portal.hpp"
#include "Projectile.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StdExtensions.hpp"
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

Entity* Map::SpawnNewEntityOfType( EntityDef const& entityDef )
{
	Entity* newEntity = nullptr;

	if ( entityDef.m_className == "Actor" )
	{
		newEntity = new Actor( entityDef , this );
	}
	else if( entityDef.m_className == "Projectile" )
	{
		newEntity = new Projectile( entityDef , this );
	}
	else if( entityDef.m_className == "Portal" )
	{
		newEntity = new Portal( entityDef , this );
	}
	else if( entityDef.m_className == "Entity" )
	{
		newEntity = new Entity( entityDef , this );
	}

	AddEntityToMap( newEntity );
	return newEntity;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity* Map::SpawnNewEntityOfType( std::string const& entityDefName )
{
	EntityDef const* entityDef = EntityDef::GetDefinition( entityDefName );
	if ( entityDef )
	{
		return SpawnNewEntityOfType( *entityDef );
	}
	else
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Failed to create Entity of Unknown type" );
		// TODO :- SDST Add more Resonable Error Messages
		return nullptr;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::AddEntityToMap( Entity* entity )
{
	if ( nullptr == entity )
	{
		return;
	}

	AddEntityToList( entity , m_allEntities );

	if ( entity->IsPlayer() )
	{
		AddEntityToList( entity , m_players );
	}
	else if( entity->IsProjectile() )
	{
		AddEntityToList( entity , m_projectiles );
	}
	else if( entity->IsNPC() )
	{
		AddEntityToList( entity , m_NPCs );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::AddEntityToList( Entity* entity , Entitylist& eList )
{
	EmplaceBackAtEmptySpace( eList , entity );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::~Map()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------