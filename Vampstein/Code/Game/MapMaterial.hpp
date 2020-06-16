#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/ThirdParty/tinyxml2.h"
#include <string>
#include <map>
#include "Engine/Math/Vec2.hpp"

class SpriteSheet;



class MaterialSheet
{
	friend class MapMaterial;

public:
	SpriteSheet* GetSpriteSheet() const { return m_diffuseSpriteSheet; };

private:
	MaterialSheet(tinyxml2::XMLElement* element);
	static MaterialSheet* GetMaterialSheet( std::string sheetName );

	std::string m_name;
	IntVec2 m_layout;
	SpriteSheet* m_diffuseSpriteSheet = nullptr;

	static std::map<std::string , MaterialSheet*> s_MaterialSheets;

};

class MapMaterial
{

private:
	IntVec2 m_spriteCoords;
	std::string m_name;
	std::string m_sheetName;

	MapMaterial( tinyxml2::XMLElement* element );
	static std::map<std::string , MapMaterial*> s_mapMaterails;

public:
	static void LoadDefinitions( const char* filePath );
	static MapMaterial* GetDefinition( std::string materialName );
	SpriteSheet* GetDiffuseSpriteSheet();
	void GetUVCoords( Vec2& outMins , Vec2& outMax );
};
