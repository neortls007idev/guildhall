#include "Game/MapMaterial.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

typedef tinyxml2::XMLDocument XMLDocument;
typedef tinyxml2::XMLElement XMLElement;

std::map<std::string , MaterialSheet*> MaterialSheet::s_MaterialSheets;
std::map<std::string , MapMaterial*> MapMaterial::s_mapMaterails;

void MapMaterial::GetUVCoords( Vec2& outMins , Vec2& outMax )
{
	SpriteSheet* sheet = GetDiffuseSpriteSheet();

	IntVec2 spriteSheetLayout = sheet->GetDimensions();
	int spritrIndex = m_spriteCoords.y * spriteSheetLayout.x + m_spriteCoords.x;

	sheet->GetSpriteUVs( outMins , outMax , spritrIndex );
}

MaterialSheet::MaterialSheet( tinyxml2::XMLElement* element )
{
	m_name = ParseXmlAttribute( *element , "name" , "NULL" );
	if ( m_name == "NULL" )
	{
		//Error
	}

	m_layout = ParseXmlAttribute( *element , "layout" , IntVec2( -1 , -1 ) );
	if ( m_layout == IntVec2( -1 , -1 ) )
	{
		//Error
	}

	XMLElement* diffuseElemet = element->FirstChildElement( "Diffuse" );

	if ( diffuseElemet != nullptr )
	{
		std::string diffuseFilePath = ParseXmlAttribute( *diffuseElemet ,"image", "NULL" );
		if ( diffuseFilePath == "NULL" )
		{
			//error
		}
		else
		{
			Texture* spriteSheetTexture=g_theRenderer->GetOrCreateTextureFromFile( diffuseFilePath.c_str() );
			m_diffuseSpriteSheet = new SpriteSheet( *spriteSheetTexture , m_layout );
		}
	}

}

MaterialSheet* MaterialSheet::GetMaterialSheet( std::string sheetName )
{
	auto found = MaterialSheet::s_MaterialSheets.find( sheetName );
	if ( found != MaterialSheet::s_MaterialSheets.end() )
	{
		return found->second;
	}

	return nullptr;
}



MapMaterial::MapMaterial( tinyxml2::XMLElement* element )
{
	m_name = ParseXmlAttribute( *element , "name" , "NULL" );
	if ( m_name == "NULL" )
	{
		//error
	}

	m_sheetName = ParseXmlAttribute( *element , "sheet" , "NULL" );
	if ( m_sheetName == "NULL" )
	{
		//error
	}

	m_spriteCoords = ParseXmlAttribute( *element , "spriteCoords" , IntVec2( -1 , -1 ) );
	if ( m_spriteCoords == IntVec2( -1 , -1 ) )
	{
		//error
	}
	
}

void MapMaterial::LoadDefinitions( const char* filePath )
{
	XMLDocument doc;
	doc.LoadFile( filePath );

	XMLElement* root = doc.RootElement();

	for ( XMLElement* ele = root->FirstChildElement("MaterialsSheet"); ele != nullptr; ele=ele->NextSiblingElement("MaterialSheet") )
	{
		MaterialSheet* newSheet = new MaterialSheet( ele );
		auto found = MaterialSheet::s_MaterialSheets.find( newSheet->m_name );

		if ( found != MaterialSheet::s_MaterialSheets.end() )
		{
			//error multiple definitions
			continue;
		}

		MaterialSheet::s_MaterialSheets[ newSheet->m_name ] = newSheet;

	}

	for ( XMLElement* ele = root->FirstChildElement( "MaterialType" ); ele != nullptr; ele=ele->NextSiblingElement( "MaterialType" ) )
	{
		MapMaterial* newMaterial = new MapMaterial( ele );
		auto found = MapMaterial::s_mapMaterails.find( newMaterial->m_name );

		if ( found != MapMaterial::s_mapMaterails.end() )
		{
			//error multiple definitions
			continue;
		}

		MapMaterial::s_mapMaterails[ newMaterial->m_name ] = newMaterial;

	}
}

MapMaterial* MapMaterial::GetDefinition( std::string materialName )
{
	auto found = MapMaterial::s_mapMaterails.find( materialName );
	if ( found != MapMaterial::s_mapMaterails.end() )
	{
		return found->second;
	}

	return nullptr;
}

SpriteSheet* MapMaterial::GetDiffuseSpriteSheet()
{
	SpriteSheet* sheet = nullptr;

	sheet = MaterialSheet::GetMaterialSheet( m_sheetName )->GetSpriteSheet();
	return sheet;
}


