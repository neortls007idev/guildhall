#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Game/Map.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game
{
public:
	Game();

	void LoadAssets();
	void LoadAllSounds();
	void LoadAllTextures();

	void Update( float deltaSeconds );
	void UpdateCamera();
	
	void Render() const;

	void AddScreenShakeIntensity( float deltaShakeIntensity );

	void Die();

	//----------------------------------------------------------------------------------------------------------
	int GetPaddleHealth() const															{ return 3; }
	Camera* GetWorldCamera()															{ return &m_worldCamera; }
private:
	void UpdateFromKeyBoard();

//--------------------------------------------------------------------------------------------------------------------------------------------

private:
	int							m_controllerID									= -1;
	float						m_screenShakeIntensity							= 0.f;

	Rgba8						m_fadeColor;
	bool						m_hasLoadingStateStarted						= true;

public:
	bool						m_gameJustStarted								= false;
	bool						m_gameOverAudio									= false;
								
	Camera						m_worldCamera;
	Camera						m_uiCamera;
	std::vector<Map*>			m_levels;
	Map*						m_currentLevel									= nullptr;
};
//--------------------------------------------------------------------------------------------------------------------------------------------

