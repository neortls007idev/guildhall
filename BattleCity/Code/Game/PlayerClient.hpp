#pragma once
#include "Game/Client.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;

//--------------------------------------------------------------------------------------------------------------------------------------------

class PlayerClient : public Client
{
public:
	PlayerClient();
	~PlayerClient();

	virtual void	Startup() override;
	virtual void	Shutdown() override;
	virtual void	Update( float deltaSeconds ) override;
	virtual void	Render() override;
	virtual void	BeginFrame() override;
	virtual void	EndFrame() override;

public:
	Game* m_game = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------------------------