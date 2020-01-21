#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/Game.hpp"
#include "Engine/Primitives/OBB2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class TheApp
{

public:

	TheApp();
	~TheApp();
	void Startup();
	void Shutdown();
	void RunFrame();

	bool IsQuitting() const { return m_isQuitting; }
	bool HandleQuitRequested();

	void BeginFrame();
	void Update( float deltaSeconds );
	void UpdateFromKeyboard();
	void Render() const;

	void EndFrame();

// 	static bool CommandQuit( EventArgs& args );
// 	FireEvent
// 	SubscribeEventCallBackFunction(const std::string );
// 	UnsubscribeEventCallBackFunction( const std::string );

//--------------------------------------------------------------------------------------------------------------------------------------------

private :

	bool m_isQuitting  = false ;
	bool m_isSloMo = false;
	bool m_isSpeedMo = false;
	AABB2 m_testBox = AABB2( 300.f , 300.f , 600.f , 400.f );
	OBB2 m_testObb = OBB2( m_testBox , -60.f );

public:

	bool m_isPaused			   = false;
	bool m_debugCamera		   = false;
	bool m_isDevConsoleVisbile = false;
};

//--------------------------------------------------------------------------------------------------------------------------------------------
