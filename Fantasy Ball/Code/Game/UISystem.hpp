#pragma once
#include "Engine/Primitives/AABB2.hpp"

class Camera;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum  eUISTATE
{
	INVALID_STATE = -1 ,
	LOADING_STATE ,
	MAIN_MENU_STATE ,
	HUD_STATE ,
	PAUSE_STATE ,
	GAME_OVER_STATE ,
	HIGHSCORE_MENU ,
	SETTINGS_MENU ,
	TUTORIAL_MENU ,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum  eUIButtons
{
	
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class UISystem
{
public:
	UISystem();
	~UISystem();

	void CreateOnFocusEvent();
	void CreateOnClickEvent();
	
private:
	Camera*							m_UICamera					= nullptr;
	std::vector<AABB2>				m_buttons;
	std::vector<AABB2>				m_labels;
	std::vector<AABB2>				m_containers;
};

//--------------------------------------------------------------------------------------------------------------------------------------------
