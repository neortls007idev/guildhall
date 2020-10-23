#pragma once
#include "Game/Client.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class RemoteClient : protected Client
{
public:
	RemoteClient();
	~RemoteClient();

	virtual void	Startup() override;
	virtual void	Shutdown() override;
	virtual void	Update( float deltaSeconds ) override;
	virtual void	BeginFrame() override;
	virtual void	EndFrame() override;
};

//--------------------------------------------------------------------------------------------------------------------------------------------