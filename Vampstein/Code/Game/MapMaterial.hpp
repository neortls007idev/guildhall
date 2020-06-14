#pragma once
#include <string>
#include <vector>
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "ThirdParty/TinyXML2/tinyxml2.h"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Texture;

//--------------------------------------------------------------------------------------------------------------------------------------------

struct MaterialType
{
public:
	MaterialType( const tinyxml2::XMLElement& definitionXMLElement );

public:
	std::string												m_name;
	std::string												m_sheetName;
	IntVec2													m_spriteCoords;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class MapMaterial
{
	static void  CreateMapMaterialDefinitions( const char* xmlFilePath );
	friend MaterialType ParseXmlAttribute( const tinyxml2::XMLElement& element , const char* attributeName , MaterialType defaultValue );
	~MapMaterial();
	
public:
	static std::string									m_spriteSheetName;
	static IntVec2										m_layout;
	static Texture*										m_spriteSheet;
	static MaterialType*								m_defaultMaterialType;
	static std::vector<MaterialType>					s_definitions;
	//std::vector<SpriteSheet*>								m_spriteSheets;
};

//--------------------------------------------------------------------------------------------------------------------------------------------
