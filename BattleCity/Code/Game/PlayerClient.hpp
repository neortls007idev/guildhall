#pragma once
#include "Game/Client.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
	
class PlayerClient : protected Client
{
public:
	PlayerClient( Server* ownerServer );
	~PlayerClient();

	virtual void	Startup() override;
	virtual void	Shutdown() override;
	virtual void	Update( float deltaSeconds ) override;
	virtual void	Render() override;
	virtual void	BeginFrame() override;
	virtual void	EndFrame() override;
};

//--------------------------------------------------------------------------------------------------------------------------------------------