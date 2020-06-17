#include "Game/World.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "TileMap.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

World::World( Game* owner , char const* name , char const* mapsFolderPath ) :
																		m_owner( owner ) ,
																		m_name( name )
{
	CreateMaps( mapsFolderPath );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

World::~World()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void World::CreateMaps( std::string mapsFolderPath )
{
	Strings mapFileNames = GetFileNamesInfolder( mapsFolderPath , "*.xml" );

	for ( size_t index = 0 ; index < mapFileNames.size() ; index++ )
	{
		std::string mapFilePath = mapsFolderPath + "/" + mapFileNames[ index ];
		TileMap* temp = TileMap::CreateTileMapFromXml( mapFileNames[ index ].c_str() , mapFilePath.c_str() );
		m_maps[ mapFileNames[ index ] ] = temp;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void World::Update()
{
	if( nullptr != m_currentMap )
	{
		m_currentMap->UpdateMeshes();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void World::Render() const
{
	if ( nullptr != m_currentMap )
	{
		m_currentMap->Render();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Map* World::GetMapByName( std::string mapName )
{
	std::string mapFileName = mapName + ".xml";
	for ( auto it = m_maps.begin(); it != m_maps.end() ; ++it )
	{
		if ( it->first == mapFileName )
		{
			return it->second;
		}
	}
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Strings World::GetAllMapNames() const
{
	Strings allMapNames;

	for( auto it = m_maps.begin(); it != m_maps.end(); ++it )
	{
		std::string name = it->first;
		name.erase( name.length() - 4 , 4 );
		allMapNames.push_back( name );
	}

	return allMapNames;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
