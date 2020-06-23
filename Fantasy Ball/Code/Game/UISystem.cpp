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
#include "Engine/Platform/Window.hpp"

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
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::LoadUITextures()
{
	m_UITextures[ UI_BACKGROUND ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/UIBackground.png" );
	m_UITextures[ UI_WOODBARK_T1 ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/WoodBarkT2.jpg" );
	m_UITextures[ UI_TITLE ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/titleTex2.png" );
	m_UITextures[ UI_PLAY ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/Play1.png" );
	m_UITextures[ UI_HIGHSCORES ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/HighScore1.png" );
	m_UITextures[ UI_SETTINGS ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/Settings1.png" );
	m_UITextures[ UI_EXIT ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/Exit1.png" );
	m_UITextures[ UI_MM_BRANCH_TEX ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/UI/Images/Branches/branchT4.png" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::LoadUIFonts()
{
	m_UIFonts[ UI_FONT_WOOD1 ] = g_theRenderer->GetOrCreateBitmapFontFromFile( "Data/Fonts/RahulFixedFont2" );
	m_UIFonts[ UI_FONT_TITLE ] = g_theRenderer->GetOrCreateBitmapFontFromFile( "Data/Fonts/FantasyBallFont_Title" );
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

UISystem::~UISystem()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::Update( float deltaSeconds )
{
	switch ( m_systemState )
	{
		case LOADING_STATE :
			LoadingState();
			break;
		case MAIN_MENU_STATE :
			MainMenuState();
			break;
		case HUD_STATE :
			g_theGame->Update( deltaSeconds );
			break;
		case PAUSE_STATE :
			break;
		case GAME_OVER_STATE :
			break;
		case HIGHSCORE_MENU :
			break;
		case SETTINGS_MENU :
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
			if( g_theGame == nullptr )
			{
				g_theGame = new Game();
				g_theGame->PostGameConstructDataOnce();
				g_theGame->PostGameConstruct();
				g_theDevConsole->PrintString( MAGENTA , "GAME HAS STARTED" );
			}
			return false;
		}
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISystem::MainMenuState()
{
	UpdateBackGroundBranches();

	Vec2 normalizedPos	= g_theInput->GetMouseNormalizedClientPosition();
	float mouseClientPosX = RangeMapFloatNormalizedInput( m_screenSpace.m_mins.x , m_screenSpace.m_maxs.x , normalizedPos.x );
	float mouseClientPosY = RangeMapFloatNormalizedInput( m_screenSpace.m_mins.y , m_screenSpace.m_maxs.y , normalizedPos.y );
	Vec2 mouseClientPos( mouseClientPosX , mouseClientPosY );

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
			g_theGame->Render();
			break;
		case PAUSE_STATE:
			break;
		case GAME_OVER_STATE:
			break;
		case HIGHSCORE_MENU:
			break;
		case SETTINGS_MENU:
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

	g_theRenderer->BindTexture( m_UITextures[ UI_MM_BRANCH_TEX ] );
	g_theRenderer->DrawAABB2( m_labels[ UI_MM_BRANCH1 ] , ORANGE );
	g_theRenderer->DrawAABB2( m_labels[ UI_MM_BRANCH2 ] , ORANGE );
	g_theRenderer->DrawAABB2( m_labels[ UI_MM_BRANCH3 ] , ORANGE );
	g_theRenderer->DrawAABB2( m_labels[ UI_MM_BRANCH4 ] , ORANGE );
	g_theRenderer->BindTexture( nullptr );
	
	g_theGame->m_currentLevel->RenderLevelSideBounds();
	
	g_theRenderer->BindTexture( m_UITextures[ UI_TITLE ] );
	std::vector<Vertex_PCU> textVerts;
	Vec2	camDimensions = m_UICamera->GetOrthoDimensions().GetXYComponents();
	float	cameraAspect = ( camDimensions.x / camDimensions.y );
	float	cellHeight = cameraAspect * 100.f;
	float	offsetX = camDimensions.x * 0.5f;
	
//	DrawTextTriangles2D( *g_theRenderer , "FANTASY BALL" , Vec2( -offsetX * 0.65f , camDimensions.y * 0.25f ) , 45.f , WHITE , cameraAspect );
// 	m_UIFonts[ UI_FONT_WOOD1 ]->AddVertsForText2D( textVerts , Vec2(-offsetX * 0.5f, 0.f ) , cellHeight ,
// 	                                 "FANTASY BALL" , WHITE , 0.6f );
// 	g_theRenderer->BindTexture( m_UIFonts[ UI_FONT_WOOD1 ]->GetTexture() );
// 	
// 	g_theRenderer->DrawVertexArray( textVerts );
// 	g_theRenderer->BindTexture( nullptr );

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

void UISystem::RenderDebugMouse() const
{
	Vec2 normalizedPos = g_theInput->GetMouseNormalizedClientPosition();
	float mouseClientPosX = RangeMapFloatNormalizedInput( m_screenSpace.m_mins.x , m_screenSpace.m_maxs.x , normalizedPos.x );
	float mouseClientPosY = RangeMapFloatNormalizedInput( m_screenSpace.m_mins.y , m_screenSpace.m_maxs.y , normalizedPos.y );
	Vec2 mouseClientPos( mouseClientPosX , mouseClientPosY );

	g_theRenderer->DrawDisc( mouseClientPos , 5.f , RED );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
