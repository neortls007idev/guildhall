#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/ParticleSystem/ParticleSystem2D.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Time/Time.hpp"
#include "Game/TheApp.hpp"
#include "Game/UISystem.hpp"


#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*		g_theRenderer;
extern BitmapFont*			g_bitmapFont;
extern AudioSystem*			g_theAudioSystem;
extern TheApp*				g_theApp;
extern InputSystem*			g_theInput;
extern ParticleSystem2D*	g_theParticleSystem2D;
extern DevConsole*			g_theDevConsole;

//--------------------------------------------------------------------------------------------------------------------------------------------

UISystem::UISystem()
{
	m_systemState				= LOADING_STATE;
	m_state[ LOADING_STATE ]	= true;
	//g_theInput->PushCursorSettings( CursorSettings( ABSOLUTE_MODE , MOUSE_IS_WINDOWLOCKED , true ) );
	//float cameraHalfHeight		= g_gameConfigBlackboard.GetValue( "cameraHalfHeight" , 540.f );
	//float cameraAspectRatio		= g_gameConfigBlackboard.GetValue( "windowAspect" , 1.77f );
	m_UICamera					= new Camera();
	m_UICamera->SetOrthoView( 540.f , 1.77f );
	m_UICamera->SetClearMode( CLEAR_COLOR_BIT , BLACK );

	Vec2	camMaxs = m_UICamera->GetOrthoMax().GetXYComponents();
	Vec2	camMins = m_UICamera->GetOrthoMin().GetXYComponents();

	m_screenSpace	=	AABB2( camMins , camMaxs );
	LoadUITextures();
	LoadUIFonts();
	InitalizeMainMenuLabels();
	InitalizeMainMenuButtons();
	InitializeBackButton();
	InitializeSliders();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::LoadUITextures()
{
	m_UITextures[ UI_BACKGROUND ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/UIBackground.png" );
	m_UITextures[ UI_WOODBARK_T1 ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/WoodBarkT2.jpg" );
	m_UITextures[ UI_TITLE ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/titleTex3.png" );
	m_UITextures[ UI_PLAY ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/Play1.png" );
	m_UITextures[ UI_HIGHSCORES ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/HighScore1.png" );
	m_UITextures[ UI_SETTINGS ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/Settings1.png" );
	m_UITextures[ UI_EXIT ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/Exit1.png" );
	m_UITextures[ UI_MM_BRANCH_TEX ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/Branches/branchT4.png" );

	m_UITextures[ UI_HS_TITLE ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/HighScores/HighScoresTitle3.png" );
	m_UITextures[ UI_HS_HBORDER ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/HighScores/HighScoreBorder4.png" );
	m_UITextures[ UI_HS_NAME_HEADER ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/HighScores/NameHeader2.png" );
	m_UITextures[ UI_HS_SCORE_HEADER ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/HighScores/scoreHeader3.png" );
	
	m_UITextures[ GEN_BACK_BTN ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/Back1.png" );

	m_UITextures[ GEN_SLIDER_BASE ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/Sliders/SliderBase.png" );
	m_UITextures[ GEN_SLIDER_FILLBAR ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/Sliders/fillBar.png" );
	m_UITextures[ GEN_SLIDER_BORDER ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/Sliders/sliderBorder.png" );
	m_UITextures[ GEN_SLIDER_BUTTON ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/Sliders/SliderButton.png" );
	
	m_UITextures[ HUD_HEALTH ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/HUD/health.png" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::LoadMainMenuAnimTex()
{
	m_UITextures[ UI_BACKGROUNDANIM ] = g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/BackGround/waterwall1.png" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::LoadUIFonts()
{
	m_UIFonts[ UI_FONT_WOOD1 ] = g_theRenderer->GetOrCreateBitmapFontFromFile( "Data/Fonts/FomaFlibustier" );
	//m_UIFonts[ UI_FONT_TITLE ] = g_theRenderer->GetOrCreateBitmapFontFromFile( "Data/Fonts/FantasyBallFont_Title" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::InitalizeMainMenuLabels()
{	
	Vec2 TitleBoxPosition( 0.f , 250.f );
	m_labels[ UI_TITLEBOX ].SetDimensions( Vec2( m_UITextures[ UI_TITLE ]->GetDimensions() ) );
	m_labels[ UI_TITLEBOX ].SetCenter( TitleBoxPosition );
	
	Vec2 treeBranch1TexDimensions( m_UITextures[ UI_MM_BRANCH_TEX ]->GetDimensions() );
	m_labels[ UI_MM_BRANCH1 ].SetDimensions( treeBranch1TexDimensions );
	m_labels[ UI_MM_BRANCH2 ].SetDimensions( treeBranch1TexDimensions );
	m_labels[ UI_MM_BRANCH3 ].SetDimensions( treeBranch1TexDimensions );
	m_labels[ UI_MM_BRANCH4 ].SetDimensions( treeBranch1TexDimensions );
	
	//treeBranch1TexDimensions *= 0.5f;
	m_labels[ UI_MM_BRANCH1 ].SetCenter( m_labels[ UI_TITLEBOX ].m_maxs - Vec2( -treeBranch1TexDimensions.x , treeBranch1TexDimensions.y ) );

	float yOffset = 75.f;
	Vec2 tileBoxBottomRight( m_labels[ UI_TITLEBOX ].m_maxs.x , m_labels[ UI_TITLEBOX ].m_mins.y );
	m_labels[ UI_MM_BRANCH2 ].SetCenter( tileBoxBottomRight - Vec2( -treeBranch1TexDimensions.x , treeBranch1TexDimensions.y - yOffset ) );

	m_labels[ UI_MM_BRANCH3 ].SetCenter( m_labels[ UI_TITLEBOX ].m_mins + Vec2( -treeBranch1TexDimensions.x , treeBranch1TexDimensions.y ) );

	Vec2 tileBoxTopLeft( m_labels[ UI_TITLEBOX ].m_mins.x , m_labels[ UI_TITLEBOX ].m_maxs.y );
	m_labels[ UI_MM_BRANCH4 ].SetCenter( tileBoxTopLeft + Vec2( -treeBranch1TexDimensions.x , treeBranch1TexDimensions.y - yOffset ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::InitalizeMainMenuButtons()
{
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			MAIN MENU BUTTON COSMETIC BOUNDS
	//--------------------------------------------------------------------------------------------------------------------------------------------
	Vec2 playButtonCosmeticPos( 0.f , 50.f );

	m_labels[ MM_PLAY_BUTTON ].SetDimensions( Vec2( m_UITextures[ UI_PLAY ]->GetDimensions() ) );
	m_labels[ MM_PLAY_BUTTON ].SetCenter( playButtonCosmeticPos );

	Vec2 highscoreButtonCosmeticPos( 0.f , -50.f );
	m_labels[ MM_HIGHSCORE_BUTTON ].SetDimensions( Vec2( m_UITextures[ UI_HIGHSCORES ]->GetDimensions() ) );
	m_labels[ MM_HIGHSCORE_BUTTON ].SetCenter( highscoreButtonCosmeticPos );

	Vec2 settingsButtonCosmeticPos( 0.f , -150.f );
	m_labels[ MM_SETTINGS_BUTTON ].SetDimensions( Vec2( m_UITextures[ UI_SETTINGS ]->GetDimensions() ) );
	m_labels[ MM_SETTINGS_BUTTON ].SetCenter( settingsButtonCosmeticPos );

	Vec2 exitButtonCosmeticPos( 0.f , -250.f );
	m_labels[ MM_EXIT_BUTTON ].SetDimensions( Vec2( m_UITextures[ UI_EXIT ]->GetDimensions() ) );
	m_labels[ MM_EXIT_BUTTON ].SetCenter( exitButtonCosmeticPos );
	
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			MAIN MENU BUTTON PHYSICAL BOUNDS
	//--------------------------------------------------------------------------------------------------------------------------------------------

	constexpr float dimensionRatio = 0.91f;
	
	m_buttons[ PLAY_BUTTON ].SetDimensions( Vec2( m_UITextures[ UI_PLAY ]->GetDimensions() ) * dimensionRatio );
	m_buttons[ PLAY_BUTTON ].SetCenter( playButtonCosmeticPos );
	
	m_buttons[ HIGHSCORE_BUTTON ].SetDimensions( Vec2( m_UITextures[ UI_HIGHSCORES ]->GetDimensions() ) * dimensionRatio );
	m_buttons[ HIGHSCORE_BUTTON ].SetCenter( highscoreButtonCosmeticPos );
	
	m_buttons[ SETTINGS_BUTTON ].SetDimensions( Vec2( m_UITextures[ UI_SETTINGS ]->GetDimensions() ) * dimensionRatio );
	m_buttons[ SETTINGS_BUTTON ].SetCenter( settingsButtonCosmeticPos );

	m_buttons[ EXIT_BUTTON ].SetDimensions( Vec2( m_UITextures[ UI_EXIT ]->GetDimensions() ) * dimensionRatio );
	m_buttons[ EXIT_BUTTON ].SetCenter( exitButtonCosmeticPos );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::InitalizeHUDLabels()
{
	Vec2 HUDHalfDimensions	= g_theGame->m_worldCamera.GetOrthoDimensions().GetXYComponents() * 0.5f;
	Vec2 healthDimensions	= Vec2( m_UITextures[ HUD_HEALTH ]->GetDimensions() );
	Vec2 healthStartPos		= HUDHalfDimensions - healthDimensions;

	m_labels[ HUD_HEALTHBOX ].SetDimensions( Vec2( healthDimensions ) );
	m_labels[ HUD_HEALTHBOX ].SetCenter( Vec2( -healthStartPos.x , healthStartPos.y ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::InitalizeHighScoreData()
{
	m_highScores.reserve( MAX_HIGHSCORE_ENTRIES );
	tinyxml2::XMLDocument xmlDocument;
	xmlDocument.LoadFile( "Data/Gameplay/HighScores.xml" );

	if( xmlDocument.ErrorID() != tinyxml2::XML_SUCCESS )
	{
		//ERROR_RECOVERABLE( "XML FILE FOR HIGHSCORES AT PATH Data/Gameplay/HighScores.xml DID NOT LOAD" );

		XMLElement* element = xmlDocument.NewElement( "HighScores" );
		xmlDocument.LinkEndChild( element );

		float scoreValue	= 75000.f;
		
		for( long index = MAX_HIGHSCORE_ENTRIES; index > 0; index-- )
		{
			XMLElement* highscoreElement = xmlDocument.NewElement( "HighScore" );
			tinyxml2::XMLText* highscoreElementText = xmlDocument.NewText( "" );

			highscoreElement->SetAttribute( "Name" , "Caught Cheating" );
			highscoreElement->SetAttribute( "Score" , scoreValue * index );

			element->LinkEndChild( highscoreElement );
			highscoreElement->LinkEndChild( highscoreElementText );
		}

		xmlDocument.SaveFile( "Data/Gameplay/HighScores.xml" );
	}

	tinyxml2::XMLElement* highScoreRoot = xmlDocument.RootElement();
	int					  numHighScores = 0;

	tinyxml2::XMLElement* highScore = highScoreRoot->FirstChildElement( "HighScore" );
	while ( highScore != nullptr )
	{
		numHighScores++;
		highScore = highScore->NextSiblingElement( "HighScore" );
	}

	if ( numHighScores < MAX_HIGHSCORE_ENTRIES )
	{
		xmlDocument.Clear();

		XMLElement* element = xmlDocument.NewElement( "HighScores" );
		xmlDocument.LinkEndChild( element );

		float scoreValue = 75000.f;

		for( long index = MAX_HIGHSCORE_ENTRIES; index > 0; index-- )
		{
			XMLElement* highscoreElement = xmlDocument.NewElement( "HighScore" );
			tinyxml2::XMLText* highscoreElementText = xmlDocument.NewText( "" );

			highscoreElement->SetAttribute( "Name" , "Caught Cheating" );
			highscoreElement->SetAttribute( "Score" , scoreValue * ( index /*+ 1*/ ) );

			element->LinkEndChild( highscoreElement );
			highscoreElement->LinkEndChild( highscoreElementText );
		}

		xmlDocument.SaveFile( "Data/Gameplay/HighScores.xml" );
	}
	
	highScore = highScoreRoot->FirstChildElement( "HighScore" );
	numHighScores = MAX_HIGHSCORE_ENTRIES;
	while( numHighScores > 0 )
	{
		numHighScores--;
		std::string name = ParseXmlAttribute( *highScore , "Name" , "Caught Cheating" );
		float score		 = ParseXmlAttribute( *highScore , "Score" , numHighScores * 75000.f );
		m_highScores.push_back( HighscoreData( name , ( size_t ) score ) );
		highScore = highScore->NextSiblingElement( "HighScore" );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::InitalizeHighScoreLabels()
{
	Vec2 HSTitleBoxPosition( 0.f , 350.f );
	m_labels[ HS_TITLE ].SetDimensions( Vec2( m_UITextures[ UI_HS_TITLE ]->GetDimensions() ) );
	m_labels[ HS_TITLE ].SetCenter( HSTitleBoxPosition );

	Vec2 HSBorderDim		= Vec2( m_UITextures[ UI_HS_HBORDER ]->GetDimensions() );
	Vec2 HSTopBorder( 0.f , 175.f );
	m_labels[ HS_TOPBORDER ].SetDimensions( HSBorderDim );
	m_labels[ HS_TOPBORDER ].SetCenter( HSTopBorder );

	m_labels[ HS_BOTTOMBORDER ].SetDimensions( HSBorderDim );
	m_labels[ HS_BOTTOMBORDER ].SetCenter( 0.f , 075.f );

	Vec2 HSNameHeaderPos	= Vec2( m_UITextures[ UI_HS_NAME_HEADER ]->GetDimensions() );
	m_labels[ HS_THEADER_NAME ].SetDimensions( HSNameHeaderPos );
	HSNameHeaderPos.x		= -HSNameHeaderPos.x * 0.65f;
	m_labels[ HS_THEADER_NAME ].SetCenter( HSNameHeaderPos.x , 125.f  );

	Vec2 HSScoreHeaderPos	= Vec2( m_UITextures[ UI_HS_SCORE_HEADER ]->GetDimensions() );
	HSScoreHeaderPos.x		= HSScoreHeaderPos.x * 0.65f;
	
	m_labels[ HS_THEADER_SCORE ].SetDimensions( Vec2( m_UITextures[ UI_HS_SCORE_HEADER ]->GetDimensions() ) );
	m_labels[ HS_THEADER_SCORE ].SetCenter( HSScoreHeaderPos.x , 125.f );

	Vec2 entryDimensions( 720.f , 48.f );
	m_labels[ HS_ENTRY_NAME ].SetDimensions( entryDimensions );
	m_labels[ HS_ENTRY_NAME ].SetCenter( -360.f , 16.f );
	m_labels[ HS_ENTRY_SCORE ].SetDimensions( entryDimensions );
	m_labels[ HS_ENTRY_SCORE ].SetCenter( 360.f , 16.f );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::InitializeBackButton()
{
	Vec2 backButtonCosmeticPos( 0.f , -425.f );
	m_labels[ UI_BACK_BUTTON ].SetDimensions( Vec2( m_UITextures[ GEN_BACK_BTN ]->GetDimensions() ) );
	m_labels[ UI_BACK_BUTTON ].SetCenter( backButtonCosmeticPos );

	constexpr float dimensionRatio = 0.91f;
	m_buttons[ BACK_BUTTON ].SetDimensions( Vec2( m_UITextures[ GEN_BACK_BTN ]->GetDimensions() ) * dimensionRatio );
	m_buttons[ BACK_BUTTON ].SetCenter( backButtonCosmeticPos );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::InitializeSliders()
{
	m_labels[ UI_SLIDER ].SetDimensions( Vec2( m_UITextures[ GEN_SLIDER_BASE ]->GetDimensions() ) );
	m_labels[ UI_SLIDER_FILLBAR ].SetDimensions( Vec2( m_UITextures[ GEN_SLIDER_FILLBAR ]->GetDimensions() ) );
	m_labels[ UI_SLIDER_BUTTON ].SetDimensions( Vec2( m_UITextures[ GEN_SLIDER_BUTTON ]->GetDimensions() ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

UISystem::~UISystem()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::Update( float deltaSeconds )
{
	m_UICamera->SetClearMode( CLEAR_COLOR_BIT , BLACK );
	
	switch ( m_systemState )
	{
		case LOADING_STATE :
			LoadingState();
			break;
		case MAIN_MENU_STATE :
			MainMenuState( deltaSeconds );
			break;
		case HUD_STATE :
			m_UICamera->SetClearMode( CLEAR_NONE , BLACK );
			g_theGame->Update( deltaSeconds );
			break;
		case PAUSE_STATE :
			m_UICamera->SetClearMode( CLEAR_NONE , BLACK );
			break;
		case GAME_OVER_STATE :
			m_UICamera->SetClearMode( CLEAR_NONE , BLACK );
			GameOverState();
			break;
		case HIGHSCORE_MENU :
			HighScoreMenuState();
			break;
		case SETTINGS_MENU :
			SettingsMenuState();
			break;
		case TUTORIAL_MENU :
			break;
		default :
			ERROR_AND_DIE( "YOU BROKE THE UI SYSTEM" );
			break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool UISystem::LoadingState()
{
	if( m_systemState == LOADING_STATE )
	{
		if( m_state[ LOADING_STATE ] )
		{
			m_state[ LOADING_STATE ] = false;
			return true;
		}
		else
		{
			m_systemState = MAIN_MENU_STATE;
			LoadMainMenuAnimTex();
			if( g_theGame == nullptr )
			{
				g_theGame = new Game();
				g_theGame->PostGameConstructDataOnce();
				g_theGame->PostGameConstruct();
				g_theDevConsole->PrintString( MAGENTA , "GAME HAS STARTED" );
				InitalizeHUDLabels();
				InitalizeHighScoreData();
			}
			return false;
		}
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::MainMenuState( float deltaSeconds )
{
		m_currentBGAnimFrame += deltaSeconds;
	if( m_currentBGAnimFrame >= m_BGAnimationDuration )
	{
		m_currentBGAnimFrame = 0.f;
	}
	UpdateBackGroundBranches();

	Vec2	normalizedPos	= g_theInput->GetMouseNormalizedClientPosition();
	float	mouseClientPosX = RangeMapFloatNormalizedInput( m_screenSpace.m_mins.x , m_screenSpace.m_maxs.x , normalizedPos.x );
	float	mouseClientPosY = RangeMapFloatNormalizedInput( m_screenSpace.m_mins.y , m_screenSpace.m_maxs.y , normalizedPos.y );
	Vec2	mouseClientPos( mouseClientPosX , mouseClientPosY );

	if( IsPointInsideAABB2D( mouseClientPos , m_buttons[ PLAY_BUTTON ] ) )
	{
		if( g_theInput->WasLeftMouseButtonJustPressed() )
		{
			m_systemState = HUD_STATE;
			g_theInput->PushCursorSettings( CursorSettings( RELATIVE_MODE , MOUSE_IS_WINDOWLOCKED , false ) );
		}
	}

	if( IsPointInsideAABB2D( mouseClientPos , m_buttons[ SETTINGS_BUTTON ] ) )
	{
		if( g_theInput->WasLeftMouseButtonJustPressed() )
		{
			m_systemState = SETTINGS_MENU;
		}
	}

	if( IsPointInsideAABB2D( mouseClientPos , m_buttons[ HIGHSCORE_BUTTON ] ) )
	{
		if( g_theInput->WasLeftMouseButtonJustPressed() )
		{
			m_systemState = HIGHSCORE_MENU;
			InitalizeHighScoreLabels();
		}
	}
	
	if( IsPointInsideAABB2D( mouseClientPos , m_buttons[ EXIT_BUTTON ] ) )
	{
		if ( g_theInput->WasLeftMouseButtonJustPressed() )
		{
			g_theApp->HandleQuitRequested();
			g_theWindow->HandleQuitRequested();
		}
	}

	if( g_theInput->WasKeyJustPressed( KEY_ESC ) )
	{
		g_theApp->HandleQuitRequested();
		g_theWindow->HandleQuitRequested();
	}

	if ( g_theGame->m_isGameDirty )
	{
		delete g_theGame;
		g_theGame = nullptr;
		g_theGame = new Game();
		g_theGame->PostGameConstructDataOnce();
		g_theGame->PostGameConstruct();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::GameOverState()
{
	g_theInput->PushCursorSettings( CursorSettings( ABSOLUTE_MODE , MOUSE_IS_UNLOCKED , true ) );
	m_systemState = MAIN_MENU_STATE;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::HighScoreMenuState()
{
	Vec2	normalizedPos	= g_theInput->GetMouseNormalizedClientPosition();
	float	mouseClientPosX = RangeMapFloatNormalizedInput( m_screenSpace.m_mins.x , m_screenSpace.m_maxs.x , normalizedPos.x );
	float	mouseClientPosY = RangeMapFloatNormalizedInput( m_screenSpace.m_mins.y , m_screenSpace.m_maxs.y , normalizedPos.y );
	Vec2	mouseClientPos( mouseClientPosX , mouseClientPosY );

	if( IsPointInsideAABB2D( mouseClientPos , m_buttons[ BACK_BUTTON ] ) )
	{
		if( g_theInput->WasLeftMouseButtonJustPressed() )
		{
			m_systemState = MAIN_MENU_STATE;
		}
	}

	if( g_theInput->WasKeyJustPressed( KEY_ESC ) )
	{
		m_systemState = MAIN_MENU_STATE;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::SettingsMenuState()
{
	Vec2	normalizedPos = g_theInput->GetMouseNormalizedClientPosition();
	float	mouseClientPosX = RangeMapFloatNormalizedInput( m_screenSpace.m_mins.x , m_screenSpace.m_maxs.x , normalizedPos.x );
	float	mouseClientPosY = RangeMapFloatNormalizedInput( m_screenSpace.m_mins.y , m_screenSpace.m_maxs.y , normalizedPos.y );
	Vec2	mouseClientPos( mouseClientPosX , mouseClientPosY );

	if( IsPointInsideAABB2D( mouseClientPos , m_buttons[ BACK_BUTTON ] ) )
	{
		if( g_theInput->WasLeftMouseButtonJustPressed() )
		{
			m_systemState = MAIN_MENU_STATE;
		}
	}
	
	if( g_theInput->WasKeyJustPressed( KEY_ESC ) )
	{
		m_systemState = MAIN_MENU_STATE;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::UpdateBackGroundBranches()
{
	
	Vec2 treeBranch1TexDimensions( m_UITextures[ UI_MM_BRANCH_TEX ]->GetDimensions() );
	treeBranch1TexDimensions *= 0.5f;
	float positionX = RangeMapFloat( -1.f , 1.f , 384.f + treeBranch1TexDimensions.x , 960.f + treeBranch1TexDimensions.x ,
									 SinDegrees( 100.f * ( float ) GetCurrentTimeSeconds() ) );

	float positionYTop = RangeMapFloat( -1.f , 1.f , 316.f + treeBranch1TexDimensions.y , 266.f + treeBranch1TexDimensions.y ,
									 CosDegrees( 100.f * ( float ) GetCurrentTimeSeconds() ) );

	float positionYBottom = RangeMapFloat( -1.f , 1.f , 49.f + treeBranch1TexDimensions.y , 133.f + treeBranch1TexDimensions.y ,
									 CosDegrees( 100.f * ( float ) GetCurrentTimeSeconds() ) );
	
	Vec2 uiMMBRANCH1 = m_labels[ UI_MM_BRANCH1 ].GetCenter();
	Vec2 uiMMBRANCH2 = m_labels[ UI_MM_BRANCH2 ].GetCenter();
	Vec2 uiMMBRANCH3 = m_labels[ UI_MM_BRANCH3 ].GetCenter();
	Vec2 uiMMBRANCH4 = m_labels[ UI_MM_BRANCH4 ].GetCenter();
	
	m_labels[ UI_MM_BRANCH1 ].SetCenter( Vec2( - positionX , positionYTop ) );
	m_labels[ UI_MM_BRANCH2 ].SetCenter( Vec2( - positionX , positionYBottom ) );
	m_labels[ UI_MM_BRANCH3 ].SetCenter( Vec2( + positionX , positionYTop ) );
	m_labels[ UI_MM_BRANCH4 ].SetCenter( Vec2( + positionX , positionYBottom ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::Render() const
{
	g_theRenderer->BeginCamera( *m_UICamera );
	g_theRenderer->SetBlendMode( ALPHA );
	g_theRenderer->BindShader( nullptr );
	g_theRenderer->BindTexture( nullptr );
	
	switch( m_systemState )
	{
		case LOADING_STATE:
			RenderLoadingScreen();
			break;
		case MAIN_MENU_STATE:
			RenderMainMenuScreen();
			break;
		case HUD_STATE:
			g_theRenderer->EndCamera( *m_UICamera );
			g_theGame->Render();
			g_theRenderer->BeginCamera( *m_UICamera );
			g_theRenderer->SetBlendMode( ALPHA );
			g_theRenderer->BindShader( nullptr );
			g_theRenderer->BindTexture( nullptr );
			RenderHUD();
			break;
		case PAUSE_STATE:
			break;
		case GAME_OVER_STATE:
			g_theRenderer->EndCamera( *m_UICamera );
			g_theGame->Render();
			g_theRenderer->BeginCamera( *m_UICamera );
			g_theRenderer->SetBlendMode( ALPHA );
			g_theRenderer->BindShader( nullptr );
			g_theRenderer->BindTexture( nullptr );
			RenderHUD();
			break;
		case HIGHSCORE_MENU:
			RenderHighScoreMenuScreen();
			break;
		case SETTINGS_MENU:
			RenderSettingsMenuScreen();
			break;
		case TUTORIAL_MENU:
			break;
		default:
			ERROR_AND_DIE( "YOU BROKE THE UI SYSTEM" );
			break;
	}
	
	g_theRenderer->EndCamera( *m_UICamera );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::RenderLoadingScreen() const
{
	g_theRenderer->BindTexture( m_UITextures[ UI_BACKGROUND ] );
	g_theRenderer->DrawAABB2( m_screenSpace , WHITE );
	g_theRenderer->BindTexture( m_UITextures[ UI_WOODBARK_T1 ] );
	std::vector<Vertex_PCU> textVerts;
	Vec2	camDimensions	= m_UICamera->GetOrthoDimensions().GetXYComponents();
	float	cameraAspect	= ( camDimensions.x / camDimensions.y );
	float	cellHeight		= cameraAspect * 100.f;
	float	offsetX			= camDimensions.x * 0.5f;
	m_UIFonts[ UI_FONT_WOOD1 ]->AddVertsForText2D( textVerts , Vec2(-offsetX * 0.5f, 0.f ) , cellHeight , "LOADING..." , WHITE , 0.6f );
	//DrawTextTriangles2D( *g_theRenderer , "LOADING..." , Vec2( -offsetX * 0.5f , 0.f ) , 45.f , WHITE , cameraAspect );
	g_theRenderer->BindTexture( m_UIFonts[ UI_FONT_WOOD1 ]->GetTexture() );
	g_theRenderer->DrawVertexArray( textVerts );
	g_theRenderer->BindTexture( nullptr );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::RenderMainMenuScreen() const
{
	g_theRenderer->BindTexture( m_UITextures[ UI_BACKGROUND ] );
	g_theRenderer->DrawAABB2( m_screenSpace , WHITE );
	g_theRenderer->BindTexture( nullptr );

	SpriteSheet spriteSheet( *m_UITextures[ UI_BACKGROUNDANIM ] , IntVec2( 9 , 15 ) );
	SpriteAnimDefinition anim = SpriteAnimDefinition( spriteSheet , 0 , 126 , m_BGAnimationDuration , SpriteAnimPlaybackType::LOOP );
	const SpriteDefinition& devConsoleAnim = anim.GetSpriteDefAtTime( m_currentBGAnimFrame );
	Vec2 uvMins;
	Vec2 uvMaxs;
	devConsoleAnim.GetUVs( uvMins , uvMaxs );
	std::vector<Vertex_PCU> tempDevConsoleAnim;
	AppendVertsForAABB2( tempDevConsoleAnim , m_screenSpace , WHITE , uvMins , uvMaxs );

	g_theRenderer->BindTexture( m_UITextures[ UI_BACKGROUNDANIM ] );
	//g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->DrawVertexArray( tempDevConsoleAnim );
	//g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->BindTexture( nullptr );
	
	g_theRenderer->BindTexture( m_UITextures[ UI_MM_BRANCH_TEX ] );
	//g_theRenderer->DrawAABB2( m_labels[ UI_MM_BRANCH1 ] , ORANGE );
	//g_theRenderer->DrawAABB2( m_labels[ UI_MM_BRANCH2 ] , ORANGE );
	//g_theRenderer->DrawAABB2( m_labels[ UI_MM_BRANCH3 ] , ORANGE );
	//g_theRenderer->DrawAABB2( m_labels[ UI_MM_BRANCH4 ] , ORANGE );
	g_theRenderer->BindTexture( nullptr );
	
	g_theGame->m_currentLevel->RenderLevelSideBounds();
	
	g_theRenderer->BindTexture( m_UITextures[ UI_TITLE ] );
	std::vector<Vertex_PCU> textVerts;
	Vec2	camDimensions = m_UICamera->GetOrthoDimensions().GetXYComponents();
//	float	cameraAspect = ( camDimensions.x / camDimensions.y );
//	float	cellHeight = cameraAspect * 100.f;
//	float	offsetX = camDimensions.x * 0.5f;
	
	g_theRenderer->BindTexture( m_UITextures[ UI_TITLE ] );
	g_theRenderer->DrawAABB2( m_labels[ UI_TITLEBOX ] , WHITE );

	
	
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			MAIN MENU BUTTON COSMETIC RENDER
	//--------------------------------------------------------------------------------------------------------------------------------------------
	
	g_theRenderer->BindTexture( m_UITextures[ UI_PLAY ] );
	g_theRenderer->DrawAABB2( m_labels[ MM_PLAY_BUTTON ] , WHITE );
	
	g_theRenderer->BindTexture( m_UITextures[ UI_HIGHSCORES ] );
	g_theRenderer->DrawAABB2( m_labels[ MM_HIGHSCORE_BUTTON ] , WHITE );
	
	g_theRenderer->BindTexture( m_UITextures[ UI_SETTINGS ] );
	g_theRenderer->DrawAABB2( m_labels[ MM_SETTINGS_BUTTON ] , WHITE );

	g_theRenderer->BindTexture( m_UITextures[ UI_EXIT ] );
	g_theRenderer->DrawAABB2( m_labels[ MM_EXIT_BUTTON ] , WHITE );
	g_theRenderer->BindTexture( nullptr );

	if ( m_UIDebugDraw )
	{
		g_theRenderer->DrawUnfilledAABB2( m_labels[ UI_TITLEBOX ]			, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_labels[ UI_MM_BRANCH1 ]			, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_labels[ UI_MM_BRANCH2 ]			, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_labels[ UI_MM_BRANCH3 ]			, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_labels[ UI_MM_BRANCH4 ]			, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_labels[ MM_PLAY_BUTTON ]		, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_labels[ MM_HIGHSCORE_BUTTON ]	, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_labels[ MM_SETTINGS_BUTTON ]	, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_labels[ MM_EXIT_BUTTON ]		, MAGENTA , 2.f );

		g_theRenderer->DrawUnfilledAABB2( m_buttons[ PLAY_BUTTON ]			, CYAN	  , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_buttons[ HIGHSCORE_BUTTON ]		, CYAN	  , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_buttons[ SETTINGS_BUTTON ]		, CYAN	  , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_buttons[ EXIT_BUTTON ]			, CYAN	  , 2.f );

		RenderDebugMouse();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::RenderSettingsMenuScreen() const
{
	g_theRenderer->BindTexture( m_UITextures[ UI_BACKGROUND ] );
	g_theRenderer->DrawAABB2( m_screenSpace , WHITE );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->BindTexture( m_UITextures[ GEN_BACK_BTN ] );
	g_theRenderer->DrawAABB2( m_labels[ UI_BACK_BUTTON ] , WHITE );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->BindTexture( m_UITextures[ GEN_SLIDER_BASE ] );
	g_theRenderer->DrawAABB2( m_labels[ UI_SLIDER ] , WHITE );
	g_theRenderer->BindTexture( m_UITextures[ GEN_SLIDER_FILLBAR ] );
	g_theRenderer->DrawAABB2( m_labels[ UI_SLIDER_FILLBAR ] , WHITE );
	g_theRenderer->BindTexture( m_UITextures[ GEN_SLIDER_BORDER ] );
	g_theRenderer->DrawAABB2( m_labels[ UI_SLIDER ] , WHITE );
	g_theRenderer->BindTexture( m_UITextures[ GEN_SLIDER_BUTTON ] );
	g_theRenderer->DrawAABB2( m_labels[ UI_SLIDER_BUTTON ] , WHITE );
	g_theRenderer->BindTexture( nullptr );
	
	if( m_UIDebugDraw )
	{
		g_theRenderer->DrawUnfilledAABB2( m_labels[ UI_BACK_BUTTON ]	, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_labels[ UI_SLIDER ]			, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_labels[ UI_SLIDER_FILLBAR ]	, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_labels[ GEN_SLIDER_BORDER ]	, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_labels[ UI_SLIDER_BUTTON ]	, MAGENTA , 2.f );
		
		g_theRenderer->DrawUnfilledAABB2( m_buttons[ BACK_BUTTON ]		, CYAN , 2.f );
	}

	g_theGame->m_currentLevel->RenderLevelSideBounds();
	g_theRenderer->BindTexture( nullptr );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::RenderHighScoreMenuScreen() const
{
	g_theRenderer->BindTexture( m_UITextures[ UI_BACKGROUND ] );
	g_theRenderer->DrawAABB2( m_screenSpace , WHITE );
	g_theRenderer->BindTexture( nullptr );
	
	g_theRenderer->BindTexture( m_UITextures[ UI_HS_HBORDER ] );
	g_theRenderer->DrawAABB2( m_labels[ HS_TOPBORDER ] , WHITE );
	g_theRenderer->DrawAABB2( m_labels[ HS_BOTTOMBORDER ] , WHITE );
	
	g_theRenderer->BindTexture( m_UITextures[ UI_HS_TITLE ] );
	g_theRenderer->DrawAABB2( m_labels[ HS_TITLE ] , WHITE );

	g_theRenderer->BindTexture( m_UITextures[ UI_HS_NAME_HEADER ] );
	g_theRenderer->DrawAABB2( m_labels[ HS_THEADER_NAME ] , WHITE );

	g_theRenderer->BindTexture( m_UITextures[ UI_HS_SCORE_HEADER ] );
	g_theRenderer->DrawAABB2( m_labels[ HS_THEADER_SCORE ] , WHITE );

	g_theRenderer->BindTexture( m_UITextures[ GEN_BACK_BTN ] );
	g_theRenderer->DrawAABB2( m_labels[ UI_BACK_BUTTON ] , WHITE );
	
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->BindTexture( m_UIFonts[ UI_FONT_WOOD1 ]->GetTexture() );
	
	Vec2	entryDimensions = m_labels[ HS_ENTRY_NAME ].GetDimensions();
	AABB2	nameEntry = m_labels[ HS_ENTRY_NAME ];
	AABB2	scoreEntry = m_labels[ HS_ENTRY_SCORE ];
	
	for( int index = 0 ; index < MAX_HIGHSCORE_ENTRIES ; index++ )
	{
		std::vector<Vertex_PCU> nameVerts;
		std::vector<Vertex_PCU> scoreVerts;
		
		m_UIFonts[ UI_FONT_WOOD1 ]->AddVertsForTextInBox2D( nameVerts , nameEntry , entryDimensions.y ,
		                                                    m_highScores[ index ].name , WHITE , 0.75 ,
		                                                    ALIGN_CENTERED_LEFT );
		m_UIFonts[ UI_FONT_WOOD1 ]->AddVertsForTextInBox2D( scoreVerts , scoreEntry , entryDimensions.y ,
															std::to_string( m_highScores[ index ].score ) , WHITE , 0.775f ,
															ALIGN_CENTERED_RIGHT );
		g_theRenderer->DrawVertexArray( nameVerts );
		g_theRenderer->DrawVertexArray( scoreVerts );
		
		nameEntry.Translate( Vec2( 0.f , -entryDimensions.y ) );
		scoreEntry.Translate( Vec2( 0.f , -entryDimensions.y ) );
	}

	g_theRenderer->BindTexture( nullptr );
	
	if( m_UIDebugDraw )
	{
		g_theRenderer->DrawUnfilledAABB2( m_labels[ HS_TITLE ]			, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_labels[ HS_TOPBORDER ]		, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_labels[ HS_BOTTOMBORDER ]	, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_labels[ HS_THEADER_NAME ]	, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_labels[ HS_THEADER_SCORE ]	, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_labels[ UI_BACK_BUTTON ]	, MAGENTA , 2.f );

		g_theRenderer->DrawUnfilledAABB2( m_buttons[ BACK_BUTTON ]		, CYAN , 2.f );

		entryDimensions		= m_labels[ HS_ENTRY_NAME ].GetDimensions();
		nameEntry			= m_labels[ HS_ENTRY_NAME ];
		scoreEntry			= m_labels[ HS_ENTRY_SCORE ];
		for ( int index = 0 ; index < MAX_HIGHSCORE_ENTRIES ; index++ )
		{
			g_theRenderer->DrawUnfilledAABB2( nameEntry		, MAGENTA , 2.f );
			g_theRenderer->DrawUnfilledAABB2( scoreEntry	, MAGENTA , 2.f );
			nameEntry.Translate( Vec2( 0.f , -entryDimensions.y ) );
			scoreEntry.Translate( Vec2( 0.f , -entryDimensions.y ) );
		}
	}

	g_theGame->m_currentLevel->RenderLevelSideBounds();
	g_theRenderer->BindTexture( nullptr );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::RenderHUD() const
{
	AABB2 healthbox = m_labels[ HUD_HEALTHBOX ];
	Vec2 healthDimensions = Vec2( m_UITextures[ HUD_HEALTH ]->GetDimensions() ) * 1.f;
	healthDimensions.y = 0.f;
	
	int playerHealth = g_theGame->GetPaddleHealth();

	g_theRenderer->BindTexture( m_UITextures[ HUD_HEALTH ] );
	
	for( int health = 0 ; health < playerHealth ; health++ )
	{
		g_theRenderer->DrawAABB2( healthbox , WHITE );
		healthbox.Translate( healthDimensions );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::RenderDebugMouse() const
{
	Vec2 normalizedPos = g_theInput->GetMouseNormalizedClientPosition();
	float mouseClientPosX = RangeMapFloatNormalizedInput( m_screenSpace.m_mins.x , m_screenSpace.m_maxs.x , normalizedPos.x );
	float mouseClientPosY = RangeMapFloatNormalizedInput( m_screenSpace.m_mins.y , m_screenSpace.m_maxs.y , normalizedPos.y );
	Vec2 mouseClientPos( mouseClientPosX , mouseClientPosY );

	g_theRenderer->DrawDisc( mouseClientPos , 5.f , RED );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::EndFrame()
{
	g_theGame->EndFrame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
