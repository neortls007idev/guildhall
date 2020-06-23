#pragma once

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eUIFONTS
{
	UI_FONT_WOOD1 ,
	UI_FONT_TITLE ,

	NUM_UI_FONTS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eUITEX
{
	UI_BACKGROUND ,
	UI_WOODBARK_T1 ,
	UI_TITLE ,
	UI_PLAY ,
	UI_HIGHSCORES ,
	UI_SETTINGS ,
	UI_EXIT ,
	UI_MM_BRANCH_TEX,
	UI_MM_BRANCHT2,
	UI_MM_BRANCHT3,
	UI_MM_BRANCHT4,
	UI_MM_BRANCHT5,
	UI_MM_BRANCHT6,
	
	NUM_UI_TEX
};

enum  eUILABLES
{
	UI_TITLEBOX ,
	UI_LOADINGBOX ,

	UI_MM_BRANCH1 , 
	UI_MM_BRANCH2 , 
	UI_MM_BRANCH3 , 
	UI_MM_BRANCH4 , 
	UI_MM_BRANCH5 , 
	UI_MM_BRANCH6 , 

	MM_PLAY_BUTTON ,
	MM_HIGHSCORE_BUTTON ,
	MM_SETTINGS_BUTTON ,
	MM_EXIT_BUTTON ,
	
	NUM_UI_LABELS
};

//--------------------------------------------------------------------------------------------------------------------------------------------



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

	NUM_UI_STATE
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum  eUIButtons
{
	PLAY_BUTTON ,
	HIGHSCORE_BUTTON ,
	SETTINGS_BUTTON ,
	EXIT_BUTTON ,

	NUM_UI_BUTTONS
};

//--------------------------------------------------------------------------------------------------------------------------------------------