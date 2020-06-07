#pragma once
#include <string>

#include "Engine/Math/Vec3.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Map
{
public:
	Map( char const* mapName );
	virtual ~Map();

	virtual void UpdateMeshes() = 0;
	virtual void Render() const = 0;

public:
	std::string		m_name;
	Vec3			m_playerStartPos								= Vec3( 1.5f , 1.5f , 0.65f );
	float			m_playerstartYawDegrees							= 0.f;
	
};

//--------------------------------------------------------------------------------------------------------------------------------------------
