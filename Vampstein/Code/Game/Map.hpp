#pragma once
#include "Engine/Math/Vec3.hpp"
#include <string>

class Map
{
public:

	Map( char const* mapName );
	virtual ~Map();

	virtual void UpdateMeshes( ) = 0;
	virtual void Render( ) = 0;

public:
	std::string m_name;
	Vec3 m_playerStartPos;
	float m_playerStartYaw = 0.f;

};