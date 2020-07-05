#pragma once
#include "Game/UISystemStructs.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include <vector>

class Camera;

//--------------------------------------------------------------------------------------------------------------------------------------------

class UISystem
{
public:
	UISystem();

	void		LoadUITextures();
	void		LoadMainMenuAnimTex();
	void		LoadUIFonts();
	void		InitalizeMainMenuLabels();
	void		InitalizeMainMenuButtons();
	void		InitalizeHUDLabels();
	void		InitalizeHighScoreData();
	void		InitalizeHighScoreLabels();
	void		InitializeBackButton();
	void		InitializeSliders();

	//--------------------------------------------------------------------------------------------------------------------------------------------


	~UISystem();

	void		Update( float deltaSeconds );
	bool		LoadingState();
	void		MainMenuState( float deltaSeconds );
	void		GameOverState();
	void		HighScoreMenuState();
	void		SettingsMenuState();
	void		UpdateBackGroundBranches();
	
	void		Render() const;
	void		RenderLoadingScreen() const;
	void		RenderMainMenuScreen() const;
	void		RenderSettingsMenuScreen() const;
	void		RenderHighScoreMenuScreen() const;
	void		RenderHUD() const;
	void		RenderDebugMouse() const;

	void		SetGameState( const eUISTATE state )																			{ m_systemState = state; }
	eUISTATE	GetGameState()const																								{ return m_systemState; }
	void		ResetAnimTime()																									{ m_currentBGAnimFrame = 0.f; }
public:
	bool										m_UIDebugDraw				= false;
	
private:
	eUISTATE									m_systemState				= INVALID_STATE;
	Camera*										m_UICamera					= nullptr;
	AABB2										m_screenSpace				= AABB2::ZERO_TO_ONE;
	bool										m_state[ NUM_UI_STATE ];
	Texture*									m_UITextures[ NUM_UI_TEX ];
	BitmapFont*									m_UIFonts[ NUM_UI_FONTS ];

	AABB2										m_labels[ NUM_UI_LABELS ];
	AABB2										m_buttons[ NUM_UI_BUTTONS ];

	float										m_BGAnimationDuration		= 2.1f;
	float										m_currentBGAnimFrame		= 0.f;

	std::vector< HighscoreData >				m_highScores;
};

//--------------------------------------------------------------------------------------------------------------------------------------------
