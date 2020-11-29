#include "Game/Server.hpp"

#include "Engine/Core/StdExtensions.hpp"
#include "Game/GameSinglePlayer.hpp"
#include "Game/GameMultiPlayer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Server* g_theServer = nullptr;

//--------------------------------------------------------------------------------------------------------------------------------------------

Server::Server()				
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Server::~Server()
{
	SAFE_RELEASE_POINTER( m_singlePlayerGame );
	SAFE_RELEASE_POINTER( m_multiPlayerGame );
	
	for ( auto index : m_clients )
	{
		index = nullptr;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::Startup()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::Shutdown()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::BeginFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::Update( float deltaSeconds )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::EndFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::CreateSinglePlayerGame()
{
	m_singlePlayerGame = new GameSinglePlayer();
	m_gameType = SINGLE_PLAYER;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::CreateMultiPlayerGame()
{
	m_multiPlayerGame = new GameMultiplayer();
	m_gameType = MULTIPLAYER;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::AddPlayerClientToServer( Client* client )
{
	EmplaceBackAtEmptySpace( m_clients , client );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Server::RemovePlayerClientFromServer( Client* client )
{
	for( int index = 0 ; index < m_clients.size(); index++ )
	{
		if( m_clients[ index ] == nullptr )
		{
			continue;
		}

		if( m_clients[ index ] == client )
		{
			m_clients[ index ] = nullptr;
			return true;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::AddRemoteNewRemotePlayer()
{
	Player* secondplayer = m_multiPlayerGame->CreateAndAddPlayerAtpositionAndOrientation( Vec2( 46.5 , 1.5f ) , 135.f );
	secondplayer->UpdatePlayerColor( MAGENTA );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Game* Server::GetGame()
{
	if ( m_gameType == SINGLE_PLAYER )
	{
		return m_singlePlayerGame;
	}
	else if ( m_gameType == MULTIPLAYER )
	{
		return m_multiPlayerGame;
	}
	else
	{
		__debugbreak();
	}
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::ParseReceivedMessages( std::vector< std::string > messageBuffer )
{
	if ( m_gameType == MULTIPLAYER && m_isRemoteClientConnectionComplete )
	{
		for ( auto index : messageBuffer )
		{
			Game* currentGame = GetGame();

			/*if ( index != "" )
			{
				Strings data = SplitStringAtGivenDelimiter( index , '|' );
				int identifier = atoi( data[ 0 ].c_str() );
				//LOG_SYSMESSAGE( " UniqueKey = %d" , identifier );
				if ( identifier != m_uniqueKey )
				{
					continue;
				}

				Game* currentGame = GetGame();
				Entitylist& entityList = currentGame->m_world->m_currentMap->m_entityListsByType[ atoi( data[ 1 ].c_str() ) ];
				int entityID = atoi( data[ 2 ].c_str() );

				for ( int entityIndex = 0; entityIndex < entityList.size(); entityIndex++ )
				{
					if ( entityList[ entityIndex ]->m_entityID == entityID )
					{
						entityList[ entityIndex ]->m_position = entityList[ entityIndex ]->m_position.SetFromText( data[ 3 ].c_str() );
						entityList[ entityIndex ]->m_orientationDegrees = StringConvertToValue( data[ 4 ].c_str() , entityList[ entityIndex ]->m_orientationDegrees );
						entityList[ entityIndex ]->m_faction = ( Faction ) StringConvertToValue( data[ 5 ].c_str() , ( int ) entityList[ entityIndex ]->m_faction );
						entityList[ entityIndex ]->m_health = StringConvertToValue( data[ 6 ].c_str() , entityList[ entityIndex ]->m_health );
					}
				}
			}*/

			if ( index != "" )
			{
				Strings data = SplitStringAtGivenDelimiter( index , '|' );
				int identifier = atoi( data[ 0 ].c_str() );
				//LOG_SYSMESSAGE( " UniqueKey = %d" , identifier );
				if ( identifier != m_uniqueKey )
				{
					continue;
				}
				EntityType entityType = ( EntityType ) atoi( data[ 1 ].c_str() );
				int entityID = atoi( data[ 2 ].c_str() );
				Map* curMap = GetGame()->m_world->m_currentMap;
				if ( !curMap->IsEntityOfTypeWithIDPresent( entityType , entityID ) )
				{
					if ( entityType == GOOD_BULLET_ENTITY )
					{
						curMap->SpawnNewEntity( GOOD_BULLET_ENTITY , FACTION_ALLY , Vec2::ZERO , 0.f );
					}
					else if ( entityType == EVIL_BULLET_ENTITY )
					{
						curMap->SpawnNewEntity( EVIL_BULLET_ENTITY , FACTION_ENEMY , Vec2::ZERO , 0.f );
					}
					else if ( entityType == EXPLOSION_ENTITY )
					{
						curMap->SpawnNewEntity( EVIL_BULLET_ENTITY , FACTION_NEUTRAL , Vec2::ZERO , 0.f );
					}
				}
				Entitylist& entityList = currentGame->m_world->m_currentMap->m_entityListsByType[ entityType ];

				for ( int entityIndex = 0; entityIndex < entityList.size(); entityIndex++ )
				{
					if ( entityList[ entityIndex ]->m_entityID == entityID )
					{
						entityList[ entityIndex ]->m_position = entityList[ entityIndex ]->m_position.SetFromText( data[ 3 ].c_str() );
						entityList[ entityIndex ]->m_orientationDegrees = StringConvertToValue( data[ 4 ].c_str() , entityList[ entityIndex ]->m_orientationDegrees );
						entityList[ entityIndex ]->m_faction = ( Faction ) StringConvertToValue( data[ 5 ].c_str() , ( int ) entityList[ entityIndex ]->m_faction );
						entityList[ entityIndex ]->m_health = StringConvertToValue( data[ 6 ].c_str() , entityList[ entityIndex ]->m_health );
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------