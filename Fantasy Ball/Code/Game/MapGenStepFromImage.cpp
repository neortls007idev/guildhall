#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Image.hpp"
#include "Game/Game.hpp"
#include "Game/MapGenStepFromImage.hpp"
#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"
#include <string>

//--------------------------------------------------------------------------------------------------------------------------------------------

extern Game* g_theGame;

//--------------------------------------------------------------------------------------------------------------------------------------------

MapGenStepFromImage::MapGenStepFromImage( const XMLElement& element ) : 
																		MapGenStep( element )
{
	std::string imageFilePath = "Data/MapGenImages/";
	std::string imageFileName = ParseXmlAttribute( element , "ImageFileName" , "" );
	if ( imageFileName == "" )
	{
		ERROR_AND_DIE("You forgot to give the image file path correctly");
	}
	imageFilePath.append( imageFileName );
	m_mutationImage = new Image( imageFilePath.c_str() );
	//m_mutationImage->GetOrCreateImageFromFile( imageFilePath.c_str() );
	
	m_rotations = ParseXmlAttribute( element , "Rotations" , m_rotations );
	m_alignment = ParseXmlAttribute( element , "Alignment" , m_alignment );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void MapGenStepFromImage::RunStepOnce( Map& map )
{

	/*Camera* gameCamera		= g_theGame->GetWorldCamera();
	Vec2 cameraMins			= gameCamera->GetOrthoMin().GetXYComponents();
	Vec2 cameraMaxs			= gameCamera->GetOrthoMax().GetXYComponents();

	//float aspectRatio		= gameCamera->GetAspectRatio();
	AABB2 cameraArea		= AABB2( cameraMins , cameraMaxs );
	Vec2 cameraDimensions	= cameraArea.GetDimensions();
	*/
	Vec2 cameraDimensions( 1920.f , 1080.f );
	RandomNumberGenerator rng;

	//float	chancePerTile = m_chancePerTile.GetRandomInRange( rng ); //TODO:- fix asap
	IntVec2 tileCoords;

	Vec2 startpointatbottomleft( -845.f , -139.f );
	//const float xOffset = ( ( cameraDimensions.x * 0.5f ) / 15.f ) + halfTileDimensions.x;
	//const float yOffset = ( cameraDimensions.y * 0.5f ) / 17.f;
	const float xOffset = TILE_LENGTH;
	const float yOffset = TILE_HEIGHT;
	
	for ( tileCoords.y = 0; ( tileCoords.y < map.m_dimensions.y ) && ( tileCoords.y < m_mutationImage->GetDimensions().y ); tileCoords.y++ )
	{
		for ( tileCoords.x = 0;( tileCoords.x <  map.m_dimensions.x ) && ( tileCoords.y < m_mutationImage->GetDimensions().x ); tileCoords.x++ )
		{
			// TODO - FIX ME
			
			//Tile& tile = *map.GetTileAtTileCoords( tileCoords );
			TileDefinition* curTileTypeInImage = DoesRGBA8AtpixelCoordsMatchAnyTileDefinition( tileCoords );
			if ( curTileTypeInImage )
			{
				//if ( rng.RollPercentChance( chancePerTile ) )
				//{
				map.SpawnNewEntity( eEntityType::TILE , startpointatbottomleft + Vec2( tileCoords.x * xOffset , tileCoords.y * yOffset ) , curTileTypeInImage );
					//tile.SetTileType( curTileTypeInImage );
				//}
			}
		}
	}

}

//--------------------------------------------------------------------------------------------------------------------------------------------

TileDefinition* MapGenStepFromImage::DoesRGBA8AtpixelCoordsMatchAnyTileDefinition( IntVec2 pixelCoords )
{
	Rgba8 colorAtPixelCoords = m_mutationImage->GetTexelColor( pixelCoords );

	for ( auto index = TileDefinition::s_definitions.cbegin(); index != TileDefinition::s_definitions.cend(); index++ )
	{
		if ( index->second->m_tileColor == colorAtPixelCoords )
		{
			return index->second;
		}
	}
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
