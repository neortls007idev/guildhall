#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"

#include "Engine/Physics/Collider2D.hpp"
#include "Game/TheApp.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*	g_theRenderer;
extern TheApp*			g_theApp;

//--------------------------------------------------------------------------------------------------------------------------------------------

Player::Player( Game* CurrentGameInstance , Vec2 pos , Vec2 velocity , float orientation ) :
	Entity( CurrentGameInstance ,
		pos ,
		velocity ,
		orientation )
{
	InitializePlayerShipMemberVariables();

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::Update( float deltaSeconds )
{
	UpdateFromJoystick( deltaSeconds );
	if ( m_isDead )
	{
		shipColor = Rgba8( 132 , 156 , 169 , 0 );
		return;
	}
	checkKeyboardKeyPressForShipMovement( deltaSeconds );
	Entity::Movement( deltaSeconds );
	//Entity::WrapAroundScreen();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::Render() const
{
	g_theRenderer->DrawPolygon( &m_playerShape.m_points[ 0 ] , ( uint ) m_playerShape.m_points.size() , shipColor , 1.f , 0.f , m_position );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::IsDead()
{
	if ( m_health == 0 )
	{
		m_isDead = true;
		m_game->AddScreenShakeIntensity( 1.f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::UpdateFromJoystick( float deltaSeconds )
{
	m_controllerID = g_theInput->GetXboxController( m_controllerID ).GetControllerID();
	// Do nothing if this player ship has no controller assigned to it
	if ( m_controllerID < 0 )
		return;

	// Do nothing if this player's controller is not plugged in
	const XboxController& controller = g_theInput->GetXboxController( m_controllerID );
	if ( !controller.IsConnected() )
		return;
	if ( g_theApp->m_isPaused == false )
	{
		XboxButtonPresses( controller , deltaSeconds );
	}

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::checkKeyboardKeyPressForShipMovement( float deltaSeconds )
{
	if ( g_theInput->IsKeyHeldDown( KEY_UPARROW ) ) // Up key
	{
		Accelerate( deltaSeconds );
	}
	if ( g_theInput->IsKeyHeldDown( KEY_LEFTARROW ) ) // Left key
	{
		RotateLeft( deltaSeconds );
	}
	if ( g_theInput->IsKeyHeldDown( KEY_RIGHTARROW ) ) // Right key
	{
		RotateRight( deltaSeconds );
	}
}

void Player::XboxButtonPresses( const XboxController& controller , float deltaSeconds )
{
	// Check if player is dead; if so, respawn on Start button (only)
	if ( m_isDead )
	{
		const KeyButtonState& startButton = controller.GetButtonState( XBOX_BUTTON_ID_START );
		if ( startButton.WasJustPressed() )
		{
			if ( m_isDead )
			{
				m_game->m_player = nullptr;
				delete m_game->m_player;
				m_game->m_player = new Player( m_game , Vec2( WORLD_CENTER_X , WORLD_CENTER_Y ) , Vec2( 0.f , 0.f ) , 0.f );
			}
		}
	}

	// Apply turning and thrust from left stick IF it is actively being used
	const AnalogJoystick& leftStick = controller.GetLeftJoystick();
	float leftStickMagnitude = leftStick.GetMagnitude();
	if ( leftStickMagnitude > 0.f && m_health > 0 ) // This is already deadzone-corrected; so anything non-zero means “significant movement” (outside the inner dead zone)
	{
		m_orientationDegrees = leftStick.GetAngleDegrees();
		m_thrustFraction = leftStickMagnitude;

		m_velocity.x = m_velocity.x + ( ( PLAYER_SHIP_ACCELERATION * deltaSeconds * CosDegrees( m_orientationDegrees ) ) * m_thrustFraction );
		m_velocity.y = m_velocity.y + ( ( PLAYER_SHIP_ACCELERATION * deltaSeconds * SinDegrees( m_orientationDegrees ) ) * m_thrustFraction );

		if ( leftStickMagnitude > 0.9 )
		{
			g_theInput->GetXboxController( m_controllerID ).SetVibration( 0.5f , 0.5f );
		}
		else
		{
			g_theInput->GetXboxController( m_controllerID ).SetVibration( 0.f , 0.f );
		}

	}

	// Fire a bullet if "A" was just pressed this frame
	const KeyButtonState& aButton = controller.GetButtonState( XBOX_BUTTON_ID_A );
	if ( aButton.WasJustPressed() && m_health > 0 )
	{
		m_game->AddScreenShakeIntensity( 0.2f );
		//m_game->SpawnBullet( ( m_position + ( 2.5 * Vec2( CosDegrees( m_orientationDegrees ) , SinDegrees( m_orientationDegrees ) ) ) ) , m_orientationDegrees );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::Accelerate( float deltaTime )
{
	m_velocity.x = m_velocity.x + ( PLAYER_SHIP_ACCELERATION * deltaTime * CosDegrees( m_orientationDegrees ) );
	m_velocity.y = m_velocity.y + ( PLAYER_SHIP_ACCELERATION * deltaTime * SinDegrees( m_orientationDegrees ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::RotateLeft( float deltaSeconds )
{
	m_orientationDegrees += ( 300.f * deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::RotateRight( float deltaSeconds )
{
	m_orientationDegrees -= ( 300.f * deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::InitializePlayerShipMemberVariables()
{
	m_health				= 1;
	m_isDead				= false;
	m_isGarbage				= false;

	Vec2 m_playerShip[ 3 ];
	m_playerShip[ 0 ]		= Vec2( 100.f , 0.f );
	m_playerShip[ 1 ]		= Vec2( -50.f , 86.6f );
	m_playerShip[ 2 ]		= Vec2( -50.f , -86.6f );

	m_playerShape			= Polygon2D::MakeConvexFromPointCloud( &m_playerShip[ 0 ] , 3 );
}

//--------------------------------------------------------------------------------------------------------------------------------------------