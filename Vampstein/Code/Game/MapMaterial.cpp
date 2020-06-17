#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/GameCommon.hpp"
#include "Game/MapMaterial.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC std::map<std::string , MaterialSheet*>	MaterialSheet::s_materialSheets;
STATIC std::map<std::string , MapMaterial*>		MapMaterial::s_mapMaterials;

//--------------------------------------------------------------------------------------------------------------------------------------------

void MapMaterial::GetUVCoords( Vec2& outMins , Vec2& outMax )
{
	SpriteSheet* sheet = GetDiffuseSpriteSheet();

	IntVec2 spriteSheetLayout = sheet->GetSpriteDimension();
	int spriteIndex = m_spriteCoords.y * spriteSheetLayout.x + m_spriteCoords.x;

	sheet->GetSpriteUVs( outMins , outMax , spriteIndex );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

MaterialSheet::MaterialSheet( XMLElement* element )
{
	m_name								= ParseXmlAttribute( *element , "name" , "NULL" );
	
	if ( m_name == "NULL" )
	{
		//Error
	}

	m_layout							= ParseXmlAttribute( *element , "layout" , -IntVec2::ONE );
	
	if ( m_layout == -IntVec2::ONE )
	{
		//Error
	}

	XMLElement* diffuseElemet = element->FirstChildElement( "Diffuse" );

	if ( diffuseElemet != nullptr )
	{
		std::string diffuseSSFilePath	= ParseXmlAttribute( *diffuseElemet ,"image", "NULL" );
		
		if ( diffuseSSFilePath == "NULL" )
		{
			//error
		}
		else
		{
			Texture* diffuseSSTexture	= g_theRenderer->GetOrCreateTextureFromFile( diffuseSSFilePath.c_str() );
			m_diffuseSpriteSheet		= new SpriteSheet( *diffuseSSTexture , m_layout );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

MaterialSheet* MaterialSheet::GetMaterialSheet( std::string sheetName )
{
	auto materialSheet = MaterialSheet::s_materialSheets.find( sheetName );
	
	if ( materialSheet != MaterialSheet::s_materialSheets.end() )
	{
		return materialSheet->second;
	}

	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

MapMaterial::MapMaterial( XMLElement* element )
{
	m_name	= ParseXmlAttribute( *element , "name" , "NULL" );
	
	if ( m_name == "NULL" )
	{
		//error
	}

	m_sheetName = ParseXmlAttribute( *element , "sheet" , "NULL" );
	
	if ( m_sheetName == "NULL" )
	{
		//error
	}

	m_spriteCoords = ParseXmlAttribute( *element , "spriteCoords" , -IntVec2::ONE );
	
	if ( m_spriteCoords == -IntVec2::ONE )
	{
		//error
	}	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void MapMaterial::LoadDefinitions( const char* filePath )
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( filePath );

	XMLElement* root = doc.RootElement();

	for( XMLElement* element = root->FirstChildElement( "MaterialsSheet" ); element != nullptr; element = element->NextSiblingElement( "MaterialSheet" ) )
	{
		MaterialSheet* newSheet		= new MaterialSheet( element );

		auto materialSheet	= MaterialSheet::s_materialSheets.find( newSheet->m_name );

		if ( materialSheet != MaterialSheet::s_materialSheets.end() )
		{
			//error multiple definitions
			continue;
		}

		MaterialSheet::s_materialSheets[ newSheet->m_name ] = newSheet;
	}

	for( XMLElement* element = root->FirstChildElement( "MaterialType" ); element != nullptr; element = element->NextSiblingElement( "MaterialType" ) )
	{
		MapMaterial* newMaterial			= new MapMaterial( element );
		
		auto mapMaterial	= MapMaterial::s_mapMaterials.find( newMaterial->m_name );

		if ( mapMaterial != MapMaterial::s_mapMaterials.end() )
		{
			//error multiple definitions
			continue;
		}

		MapMaterial::s_mapMaterials[ newMaterial->m_name ] = newMaterial;

	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

MapMaterial* MapMaterial::GetDefinition( std::string materialName )
{
	auto mapMaterial = MapMaterial::s_mapMaterials.find( materialName );
	
	if ( mapMaterial != MapMaterial::s_mapMaterials.end() )
	{
		return mapMaterial->second;
	}

	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

SpriteSheet* MapMaterial::GetDiffuseSpriteSheet()
{
	SpriteSheet* sheet = nullptr;

	sheet = MaterialSheet::GetMaterialSheet( m_sheetName )->GetSpriteSheet();
	return sheet;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
