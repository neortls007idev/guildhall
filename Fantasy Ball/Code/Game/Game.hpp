#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Camera.hpp"

class ParticleEmitter2D;
class SpriteSheet;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game
{
public:
	Game();

	void LoadAssets();
	void LoadAllSounds();
	void LoadAllTextures();

	void PostGameConstructDataOnce();
	void PostGameConstruct();
	
	void CreateAllTileDefinitions();
	void CreateAllMapDefinitions();
	
	void Update( float deltaSeconds );
	void UpdateCamera();
	
	void Render() const;

	void AddScreenShakeIntensity( float deltaShakeIntensity );

	void Die();

	SoundPlaybackID GetSFX( eGameAudioFX SFXid ) const;
	
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
	int							m_currentLevelNumber							= 0;
	std::vector<Map*>			m_levels;
	Map*						m_currentLevel									= nullptr;
	Texture*					m_gameTex[ NUM_GAME_TEX ];
	SpriteSheet*				m_gameSS[ NUM_GAME_SS ];
	SoundPlaybackID				m_sounds[ NUM_GAME_SFX ];

	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			DEBUG CODE
	//--------------------------------------------------------------------------------------------------------------------------------------------

	bool m_isDebugDraw = false;
};

//--------------------------------------------------------------------------------------------------------------------------------------------