#include "Game/Actor.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StdExtensions.hpp"
#include "Game/Map.hpp"
#include "Game/TileMap.hpp"
#include "Game/Portal.hpp"
#include "Game/Projectile.hpp"

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

		if( newMap->m_parsedSuccessfully == false )
		{
			g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_ERROR ,
										  "ERROR: %s of TYPE: %s FAILED to parse successfully" , mapName , mapType.c_str() );
			newMap = nullptr;
		}

		return newMap;
	}
	
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ParseAllMapPlayerStartEntities( tinyxml2::XMLElement* parentElement , std::string mapName )
{
	bool parseResult = false;
	
	tinyxml2::XMLElement* entityType = parentElement->FirstChildElement( "PlayerStart" );

	if( entityType != nullptr )
	{
		Entity* newPlayerEntity = SpawnNewPlayerEntity();
		
		Vec2 pos = ParseXmlAttribute( *entityType , "pos" , Vec2::ZERO );
		newPlayerEntity->m_pos = Vec3( pos , 0.65f );
		m_playerStartPos = newPlayerEntity->m_pos;
		
		if( m_playerStartPos.GetLengthSquared() < 0.1f )
		{
			g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_WARNING ,
										  "WARNING: MAP: %s has Player start position - 0 , 0 , 0 )" , mapName.c_str() );
		}

		float startYaw = ParseXmlAttribute( *entityType , "yaw" , -1.f );
		newPlayerEntity->m_startYawDegrees = startYaw;
		m_playerStartYawDegrees = newPlayerEntity->m_startYawDegrees;
		
		if( startYaw <= -0.98f && startYaw >= -1.01 )
		{
			g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_WARNING ,
										  "WARNING: MAP: %s has Player start yaw = %.2f )" , mapName.c_str() , startYaw );
		}
		else
		{
			g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_SYTEMLOG ,
										  "MAP: %s has Player start yaw = %.2f )" , mapName.c_str() , startYaw );
		}
	}
	parseResult = true;
	m_parsedSuccessfully &= parseResult;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ParseAllMapEntitiesOfName( tinyxml2::XMLElement* parentElement , std::string elementName , std::string mapName )
{
	bool parseResult = false;

	for( tinyxml2::XMLElement* element = parentElement->FirstChildElement( elementName.c_str() ); element != nullptr; element = element->NextSiblingElement( elementName.c_str() ) )
	{
		if( element != nullptr )
		{
			std::string entityType = ParseXmlAttribute( *element , "type" , "invalid" );
			
			Entity* newEntity = SpawnNewEntityOfType( entityType );

			if ( nullptr == newEntity )
			{
				g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_WARNING ,
											  "WARNING: MAP: %s has INVALID Entity : %s )" , mapName.c_str() , elementName.c_str() );
				continue;
			}

			Vec2 pos = ParseXmlAttribute( *element , "pos" , Vec2::ZERO );
			newEntity->m_pos = Vec3( pos , 0.0f );

			if( newEntity->m_pos.GetLengthSquared() < 0.1f )
			{
				g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_WARNING ,
											  "WARNING: MAP: %s has %s Entity start position - 0 , 0 , 0 )" , mapName.c_str() , elementName.c_str() );
			}

			float startYaw = ParseXmlAttribute( *element , "yaw" , -1.f );
			newEntity->m_startYawDegrees = startYaw;

			if( startYaw <= -0.98f && startYaw >= -1.01 )
			{
				g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_WARNING ,
											  "WARNING: MAP: %s has %s Entity start yaw = %.2f )" , mapName.c_str() , elementName.c_str() , startYaw );
			}

			newEntity->ConstructEntityBillboard();
		}
	}
	parseResult = true;
	m_parsedSuccessfully &= parseResult;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ParseAllMapEntities( tinyxml2::XMLElement* rootElement , std::string mapName )
{
	XMLElement* entityElement = rootElement->FirstChildElement( "Entities" );
	std::vector<std::string> entityTypes;
	entityTypes.push_back( "PlayerStart" );

	ParseAllMapPlayerStartEntities( entityElement , mapName );
	ParseAllMapEntitiesOfName( entityElement , "Actor" , mapName );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::Update( float deltaSeconds )
{
	for( size_t index = 0 ; index < m_allEntities.size() ; index++ )
	{
		if( m_allEntities[ index ] != nullptr )
		{
			m_allEntities[ index ]->Update( deltaSeconds );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity* Map::SpawnNewEntityOfType( EntityDef const& entityDef )
{
	Entity* newEntity = nullptr;

	if ( entityDef.m_className == "Actor" )
	{
		newEntity = new Actor( entityDef , this );
		newEntity->m_entityType = NPC;
	}
	else if( entityDef.m_className == "Projectile" )
	{
		newEntity = new Projectile( entityDef , this );
		newEntity->m_entityType = PROJECTILE;
	}
	else if( entityDef.m_className == "Portal" )
	{
		newEntity = new Portal( entityDef , this );
		newEntity->m_entityType = PORTAL;
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
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Failed to create Entity of type : %s" , entityDefName.c_str() );
		return nullptr;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity* Map::SpawnNewPlayerEntity()
{
	Entity* newEntity = new Entity();
	newEntity->m_entityType = PLAYER;

	AddEntityToMap( newEntity );
	return newEntity;
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