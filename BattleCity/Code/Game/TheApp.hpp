#pragma once

//--------------------------------------------------------------------------------------------------------------------------------------------

class RemoteServer;
class AuthoritativeServer;

class TheApp
{

public:

	TheApp();
	~TheApp();
	void Startup();
	void Shutdown();
	void RunFrame();

	bool IsQuitting() const																							{ return m_isQuitting; }
	bool HandleQuitRequested();

	void BeginFrame();
	void Update( float deltaSeconds );
	void UpdateFromKeyboard();
	void Render() const;

	void EndFrame();
	

//--------------------------------------------------------------------------------------------------------------------------------------------

private :

	bool					m_isQuitting			= false ;
	
public:

	bool					m_isPaused				= false;

	AuthoritativeServer*	m_theAuthServer			= nullptr;
	RemoteServer*			m_remotteServer			= nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------------------------
