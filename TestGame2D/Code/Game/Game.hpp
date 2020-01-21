#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/GameCommon.hpp"
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game
{

public:

	Game();

	void LoadAllSounds();

	void LoadAllEntityTextures();

	void Update( float deltaSeconds );
	void Render() const;
	void UpdateCamera();

	void RenderUI() const;
	void RenderUIVerts() const;

	void AddScreenShakeIntensity( float deltaShakeIntensity );

	void GarbageCollection();
	void GarbageDeletion();
	void Die();

	void DrawMouseTest( const Camera& camera ) const;

//--------------------------------------------------------------------------------------------------------------------------------------------

private:

	void UpdateFromKeyBoard();

//--------------------------------------------------------------------------------------------------------------------------------------------

private:

	int m_controllerID = -1;
	float m_screenShakeIntensity = 0.f;
	float m_tilesInViewHorizontally = 16.0f;
	float m_tilesInViewVertically = 9.0f;
	Vec2 m_CameraCenter= Vec2( m_tilesInViewHorizontally / 2 , m_tilesInViewVertically / 2 );

//--------------------------------------------------------------------------------------------------------------------------------------------

public:

	bool m_isClipping	   = false;
	bool m_debugDraw	   = false;
	bool m_gameJustStarted = false;
	bool m_gameOverAudio   = false;
	Camera m_worldCamera;
	Camera m_uiCamera;
	//SpriteAnimDefinition testAnim;

};

//--------------------------------------------------------------------------------------------------------------------------------------------