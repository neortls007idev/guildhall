#include "Game/MapMaterial.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

STATIC std::string					MapMaterial::m_spriteSheetName;
STATIC IntVec2						MapMaterial::m_layout;
STATIC Texture*						MapMaterial::m_spriteSheet;
STATIC MaterialType*				MapMaterial::m_defaultMaterialType;
STATIC std::vector<MaterialType>	MapMaterial::s_definitions;

//--------------------------------------------------------------------------------------------------------------------------------------------

void MapMaterial::CreateMapMaterialDefinitions( const char* xmlFilePath )
{
	std::string filePath = xmlFilePath;
	tinyxml2::XMLDocument xmlDocument;
	xmlDocument.LoadFile( xmlFilePath );

	if( xmlDocument.ErrorID() != tinyxml2::XML_SUCCESS )
	{
		ERROR_AND_DIE( "XML FILE = " + filePath + " DID NOT LOAD" );
		return;
	}

	tinyxml2::XMLElement* mapMaterialTypes		= xmlDocument.RootElement();
	std::string defaultMapMaterial				= ParseXmlAttribute( *mapMaterialTypes , "default" , "" );
	
	
	tinyxml2::XMLElement* materialSheet			= mapMaterialTypes->FirstChildElement( "MaterialSheet" );
	std::string spriteSheetName					= ParseXmlAttribute( *materialSheet , "name" , "" );
	IntVec2		layout							= ParseXmlAttribute( *materialSheet , "layout" , IntVec2::ZERO );
				m_spriteSheetName				= spriteSheetName;
				m_layout						= layout;
	
	tinyxml2::XMLElement*	diffuseSheet		= mapMaterialTypes->FirstChildElement( "Diffuse" );
	std::string				diffuseSheetPath	= ParseXmlAttribute( *diffuseSheet , "image" , "" );
	Texture*				diffuseTexture		= g_theRenderer->GetOrCreateTextureFromFile( diffuseSheetPath.c_str() );
							m_spriteSheet		= diffuseTexture;
	
	if( ( diffuseSheetPath == "" ) || ( layout.x == 0 && layout.y == 0 ) )
	{
		ERROR_AND_DIE( "You forgot To mention the filePath for the TileSpreadSheet or it's Grid Layout" );
	}

	tinyxml2::XMLElement* materialType = mapMaterialTypes->FirstChildElement( "MaterialType" );

	while( materialType )
	{
		MaterialType* currentMaterialType = new MaterialType( *materialType );
		//std::string key = ParseXmlAttribute( *materialType , "name" , "Invalid Name" );
		s_definitions.emplace_back( currentMaterialType );
		materialType = materialType->NextSiblingElement();
	}

	for ( size_t index = 0 ; index < s_definitions.size() ; index++ )
	{
		if ( s_definitions[index].m_name == defaultMapMaterial )
		{
			m_defaultMaterialType = &s_definitions[ index ];
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

MapMaterial::~MapMaterial()
{
	m_spriteSheet = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

MaterialType::MaterialType( const tinyxml2::XMLElement& definitionXMLElement )
{
	m_name			= ParseXmlAttribute( definitionXMLElement , "name" , "Invalid Name" );
	m_sheetName		= ParseXmlAttribute( definitionXMLElement , "sheet" , "Invalid Name" );
	m_spriteCoords	= ParseXmlAttribute( definitionXMLElement , "spriteCoords" , IntVec2::ZERO );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
