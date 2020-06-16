#pragma once
#include "Engine/ThirdParty/tinyxml2.h"
#include <string>
#include <map>


class MapMaterial;

class MapRegionType
{
public:
	static void LoadDefinitions( char const* dataFilePath );
	static MapRegionType const* GetDefinition( std::string const& name );

	bool IsSolid() const { return m_isSolid; };
	MapMaterial* GetSideMaterial() const { return m_sideMaterial; };
	MapMaterial* GetFloorMaterial() const { return m_floorMaterial; };
	MapMaterial* GetCelingMaterial() const { return m_ceilingMaterial; };

	

	static std::map<std::string , MapRegionType*> s_regionTypes;

private:
	MapRegionType( tinyxml2::XMLElement* materialDef );

private:
	
	std::string m_name;
	bool m_isSolid = true;
	MapMaterial* m_sideMaterial = nullptr;
	MapMaterial* m_floorMaterial = nullptr;
	MapMaterial* m_ceilingMaterial = nullptr;

};