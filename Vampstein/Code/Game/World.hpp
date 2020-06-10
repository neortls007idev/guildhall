#pragma once
#include <string>
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;
class Map;

//--------------------------------------------------------------------------------------------------------------------------------------------

class World
{
public:
	World( Game* owner , char const* name );
	~World();

	void CreateMaps();
	void Update();
	void Render() const;

public:
	Game*								m_owner							= nullptr;
	Map* 								m_currentMap					= nullptr;
	std::string							m_name;
	std::vector< Map* >					m_maps;
	
};

//--------------------------------------------------------------------------------------------------------------------------------------------