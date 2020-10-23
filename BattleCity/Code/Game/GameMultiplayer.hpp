#pragma once
#include "Game/Game.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class GameMultiplayer : public Game
{

public:

	GameMultiplayer();
	~GameMultiplayer();

	virtual void Startup() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;
		void CreateAndAddPlayer();

	std::vector< Player* >	m_player;
	Camera*					m_playerCamera	= nullptr;
	Camera*					m_UICamera		= nullptr;

};

//--------------------------------------------------------------------------------------------------------------------------------------------
