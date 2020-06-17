#pragma once
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Math/Vec3.hpp"
#include <string>

//--------------------------------------------------------------------------------------------------------------------------------------------



class Map
{
public:
	Map( char const* mapName );
	Map( char const* mapName , XMLElement* rootElement );
	static Map* CreateNewMapOfType( std::string mapType , char const* mapName , XMLElement* rootElement );
	virtual ~Map();

	virtual void UpdateMeshes() = 0;
	virtual void Render() const = 0;

public:
	std::string		m_name;
	Vec3			m_playerStartPos;
	float			m_playerStartYawDegrees;
	bool			m_parsedSuccessfully;
};

//--------------------------------------------------------------------------------------------------------------------------------------------
