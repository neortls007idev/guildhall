#pragma once

//--------------------------------------------------------------------------------------------------------------------------------------------

class Server;

//--------------------------------------------------------------------------------------------------------------------------------------------
	
class Client
{
public:
	Client( Server* ownerServer );
	~Client();

	virtual void	Startup();
	virtual void	Shutdown();
	virtual void	Update( float deltaSeconds );
	virtual void	Render();
	virtual void	BeginFrame();
	virtual void	EndFrame();
	
public:
	Server* m_ownerServer = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------------------------