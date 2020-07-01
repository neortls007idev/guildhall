#pragma once
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Game/Entity.hpp"
#include <string>
#include "EntityDef.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

typedef std::vector< Entity* > Entitylist;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Map
{
public:
	Map( char const* mapName );
	Map( char const* mapName , XMLElement* rootElement );
	virtual ~Map();
	static Map*		CreateNewMapOfType( std::string mapType , char const* mapName , XMLElement* rootElement );

	virtual void	UpdateMeshes() = 0;
	virtual void	Render() const = 0;
	
	virtual Entity* SpawnNewEntityOfType( std::string const& entityDefName );
	virtual Entity* SpawnNewEntityOfType( EntityDef const& entityDef );
			void	AddEntityToMap( Entity* entity );
			void	AddEntityToList( Entity* entity , Entitylist& eList );
public:
	std::string		m_name;
	Vec3			m_playerStartPos;
	float			m_playerStartYawDegrees;
	bool			m_parsedSuccessfully;

	Entitylist		m_allEntities;
	Entitylist		m_players;
	Entitylist		m_projectiles;
	Entitylist		m_NPCs;
	
};

//--------------------------------------------------------------------------------------------------------------------------------------------
