#include "Game/GameCommon.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

#include "Game/TheApp.hpp"
#include "Game/Game.hpp"
#include "Game/Gameobject.hpp"
//#include "Engine/Physics/Polygon2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*	g_theRenderer;
extern AudioSystem*		g_theAudioSystem;
extern Physics2D*		g_thePhysicsSystem;
extern TheApp*			g_theApp;
extern BitmapFont*		g_bitmapFont;

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC RandomNumberGenerator Game::m_rng;

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	m_worldCamera.SetOutputSize( m_currentCameraOutputSize );
	m_worldCamera.SetPosition( m_cameraDefaultPosition );

	//m_worldCamera.SetProjectionOrthographic( 800.f );
	m_worldCamera.SetOrthoView( Vec2::ZERO , Vec2( 1600.f , 800.f ) );

	m_UICamera.SetOutputSize( m_currentCameraOutputSize );
	m_UICamera.SetPosition( m_cameraDefaultPosition );
	m_UICamera.SetClearMode( CLEAR_NONE , WHITE , 0.f , 0 );
	g_thePhysicsSystem->m_sceneCamera = &m_worldCamera;

	m_mousePosition = g_theInput->GetMouseNormalizedClientPosition();
	m_mousePosition = m_worldCamera.GetClientToWorldPosition( m_mousePosition );

	RandomizePointCloud( m_rng );

	InitialGameObjectsSpawner();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::InitialGameObjectsSpawner()
{
	int numInitialGameObjects = m_rng.RollRandomIntInRange( MIN_INITIAL_GAME_OBJECTS , MAX_INITIAL_GAME_OBJECTS );

	for ( int index = 0; index < numInitialGameObjects; index++ )
	{
		m_currentColliderRadius = m_rng.RollRandomFloatInRange( m_minColliderRadius , m_maxColliderRadius );
		Vec2 spawnPos = Vec2( m_rng.RollRandomFloatBetweenZerotoAlmostOne() , m_rng.RollRandomFloatBetweenZerotoAlmostOne() );
		spawnPos = m_worldCamera.GetWorldNormalizedToClientPosition( spawnPos );
		GameObject* temp = new GameObject( g_thePhysicsSystem , spawnPos , Vec2::ZERO , m_currentColliderRadius );
		m_gameObjects.push_back( temp );
		m_isMouseOnGameObject.push_back( false );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	m_mousePosition = m_worldCamera.GetClientToWorldPosition( m_mousePosition );
	m_mousePosition = m_worldCamera.GetWorldNormalizedToClientPosition( m_mousePosition );

	m_frameDelay++;

	if ( m_frameDelay % 5 == 0 )
	{
		m_MouseDragFrames[ m_frameCount ] = m_worldCamera.GetWorldNormalizedToClientPosition( g_theInput->GetMouseNormalizedClientPosition() );
		m_frameCount++;
		m_frameCount %= 10;
		m_frameDelay = 0;
		m_dragTime += deltaSeconds;
	}
	UpdateCamera();
	UpdateGameObject( deltaSeconds );
	UpdateGameObjects();
	UpdateFromUserInput( deltaSeconds );
	DrawConvexgonMode();
	g_thePhysicsSystem->Update( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	g_theRenderer->BeginCamera( m_worldCamera );
	g_theRenderer->SetModelMatrix( Mat44::IDENTITY );
	g_theRenderer->BindShader( nullptr );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->SetBlendMode( ALPHA );

	for ( unsigned int index = 0; index < ( unsigned int ) m_gameObjects.size(); index++ )
	{
		if ( m_gameObjects[index] == nullptr )
		{
			continue;
		}

		Collider2D* Collider = m_gameObjects[ index ]->m_rigidbody->GetCollider();

		if ( Collider )
		{
			Collider->DebugRender( g_theRenderer , m_gameObjects[index]->m_borderColor , m_gameObjects[index]->m_fillColor );
		}
	}

	DrawMouseCurrentPosition( m_worldCamera );

	RenderDrawFromPointCloudMode();
	g_theRenderer->SetBlendMode( SOLID );
	g_theRenderer->EndCamera( m_worldCamera );
	RenderUI();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderUI() const
{
	g_theRenderer->BeginCamera( m_UICamera );
	g_theRenderer->SetBlendMode( ALPHA );
	RenderGravityUI();
	g_theRenderer->SetBlendMode( SOLID );
	g_theRenderer->EndCamera( m_UICamera );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderGravityUI() const
{
	AABB2 uiArea = AABB2( m_UICamera.GetOrthoMin().GetXYComponents() , m_UICamera.GetOrthoMax().GetXYComponents() );

	uiArea = uiArea.GetBoxAtTop( 0.9f , 0.f ).GetBoxAtRight( 0.5f , 0.f );
	/*uiArea.CarveBoxOffRight( 0.5f , 0.f );*/

	std::vector<Vertex_PCU> uiVerts;

	std::string currGravityX = "Current Gravity Along X= ";
	currGravityX += std::to_string( g_thePhysicsSystem->GetSceneGravity().x );

	std::string currGravityY = "Current Gravity Along Y= ";
	currGravityY += std::to_string( g_thePhysicsSystem->GetSceneGravity().y );

	g_bitmapFont->AddVertsForTextInBox2D( uiVerts , uiArea , uiArea.GetDimensions().y * 0.2f , currGravityX , RED , 1.f , ALIGN_TOP_RIGHT );
	g_bitmapFont->AddVertsForTextInBox2D( uiVerts , uiArea , uiArea.GetDimensions().y * 0.2f , currGravityY , RED, 1.f , ALIGN_CENTERED_RIGHT  );
	g_bitmapFont->AddVertsForTextInBox2D( uiVerts , uiArea , uiArea.GetDimensions().y * 0.2f , "Press + or - key to change Gravity" , RED , 1.f , ALIGN_BOTTOM_RIGHT );

	g_theRenderer->BindTexture( g_bitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( uiVerts );
	g_theRenderer->BindTexture( nullptr );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderDrawMode() const
{
	if ( !m_isDrawModeActive )
	{
		return;
	}

	for ( size_t index = 0 ; index < m_drawModePoints.size() ; index++ )
	{
		g_theRenderer->DrawDisc( Disc2D( m_drawModePoints[ index ] , 2.f ) , CYAN );
	}

	if ( m_drawModePoints.size() > 1 )
	{
		for ( size_t index = 0; index < m_drawModePoints.size() - 1; index++ )
		{
			g_theRenderer->DrawLine( m_drawModePoints[ index ] , m_drawModePoints[ index + 1 ] , BLUE , 3.f );
		}
	}

	if ( m_isLastPointInvalid )
	{
		size_t totalPoints = m_drawModePoints.size();
		g_theRenderer->DrawLine( m_drawModePoints[ totalPoints - 1 ] , m_invalidPoint , RED , 3.f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderDrawFromPointCloudMode() const
{
	if ( !m_isDrawModeActive )
	{
		return;
	}

	for ( size_t index = 0; index < m_drawModePoints.size(); index++ )
	{
		g_theRenderer->DrawDisc( Disc2D( m_drawModePoints[ index ] , 2.f ) , CYAN );
	}

	Polygon2D temp;
	if ( m_drawModePoints.size() > 0 )
	{
		temp = Polygon2D::MakeConvexFromPointCloud( &m_drawModePoints[ 0 ] , ( uint ) m_drawModePoints.size() );
		g_theRenderer->DrawPolygon( &temp.m_points[ 0 ] , ( unsigned int ) temp.m_points.size() , Rgba8( 255 , 255 , 255 , 127 ) );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DebugRender() const
{
	g_theRenderer->DrawPolygon( &m_testPolygon.m_points[ 0 ] , ( unsigned int ) m_testPolygon.m_points.size() , WHITE );

	for ( size_t index = 0; index < m_pointCloud.size(); index++ )
	{
		g_theRenderer->DrawDisc( Disc2D( m_pointCloud[ index ] , 3.f ) , RED );
	}

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DrawConvexgonMode()
{
	if ( g_theInput->WasKeyJustPressed( '2' ) && m_selectedGameObject == nullptr )
	{
		m_isDrawModeActive = true;

		Vec2 point = m_worldCamera.GetWorldNormalizedToClientPosition( g_theInput->GetMouseNormalizedClientPosition() );
		m_drawModePoints.clear();
		m_isLastPointInvalid = false;
		m_drawModePoints.push_back( point );
	}

	//PolygonDrawMode();
	PolygonDrawPointCloudMode();

	if ( g_theInput->WasKeyJustPressed( KEY_ESC ) )
	{
		m_isDrawModeActive = false;
		m_drawModePoints.clear();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::PolygonDrawMode()
{
	if ( !m_isDrawModeActive )
	{
		return;
	}

	m_isDrawModeActive = true;

	if ( g_theInput->WasLeftMouseButtonJustPressed() )
	{
		if ( m_drawModePoints.size() == 1 )
		{
			m_drawModePoints.push_back( m_worldCamera.GetWorldNormalizedToClientPosition( g_theInput->GetMouseNormalizedClientPosition() ) );
			return;
		}

		Vec2 point = m_worldCamera.GetWorldNormalizedToClientPosition( g_theInput->GetMouseNormalizedClientPosition() );
		
		if ( IsPolygonPotentiallyConvex( point ) )
		{
			m_drawModePoints.push_back( point );
			m_isLastPointInvalid = false;
		}
		else
		{
			m_invalidPoint = point;
			m_isLastPointInvalid = true;
		}
	}

	if ( g_theInput->WasKeyJustPressed( KEY_BACKSPACE ) )
	{
		if ( m_isLastPointInvalid )
		{
			m_isLastPointInvalid = false;
		}
		else
		{
			m_drawModePoints.pop_back();
		}
	}

	if ( g_theInput->WasRightMouseButtonJustPressed() )
	{
		if ( m_drawModePoints.size() < 3 || m_isLastPointInvalid )
		{
			m_drawModePoints.clear();
			m_isLastPointInvalid = false;
			m_isDrawModeActive = false;
			return;
		}

		Vec2* points = new Vec2[ m_drawModePoints.size() ];

		for ( size_t index = 0; index < m_drawModePoints.size(); index++ )
		{
			points[ index ] = m_drawModePoints[ index ];
		}

		Polygon2D temp;
		temp = Polygon2D::MakeFromLineLoop( points , ( uint ) m_drawModePoints.size() );
		GameObject* polyGameobject = new GameObject( g_thePhysicsSystem , temp.GetCenter() , Vec2::ZERO , temp );
		m_gameObjects.push_back( polyGameobject );
		m_isMouseOnGameObject.push_back( false );
		m_isDrawModeActive = false;
		m_drawModePoints.clear();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::PolygonDrawPointCloudMode()
{
	if ( !m_isDrawModeActive )
	{
		return;
	}

	m_isDrawModeActive = true;

	if ( g_theInput->WasLeftMouseButtonJustPressed() )
	{
		Vec2 point = m_worldCamera.GetWorldNormalizedToClientPosition( g_theInput->GetMouseNormalizedClientPosition() );
		m_drawModePoints.push_back( point );
	}

	if ( g_theInput->WasKeyJustPressed( KEY_BACKSPACE ) )
	{
		if ( m_drawModePoints.size() > 0 )
		{
			m_drawModePoints.pop_back();
		}
	}

	if ( g_theInput->WasRightMouseButtonJustPressed() )
	{
		if ( m_drawModePoints.size() < 3 || m_isLastPointInvalid )
		{
			m_drawModePoints.clear();
			m_isDrawModeActive = false;
			return;
		}

		Vec2* points = new Vec2[ m_drawModePoints.size() ];

		for ( size_t index = 0; index < m_drawModePoints.size(); index++ )
		{
			points[ index ] = m_drawModePoints[ index ];
		}

		Polygon2D temp;
		temp = Polygon2D::MakeConvexFromPointCloud( &points[ 0 ] , ( uint ) m_drawModePoints.size() );
		temp.SetCenter();
		GameObject* polyGameobject = new GameObject( g_thePhysicsSystem , temp.GetCenter() , Vec2::ZERO , temp );
		m_gameObjects.push_back( polyGameobject );
		m_isMouseOnGameObject.push_back( false );
		m_isDrawModeActive = false;
		m_drawModePoints.clear();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Game::IsPolygonPotentiallyConvex( Vec2 latestPoint )
{
	Polygon2D convexgon;
	convexgon.m_points = m_drawModePoints;

	convexgon.m_points.push_back( latestPoint );

	return convexgon.IsConvex();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateCamera()
{
	m_worldCamera.SetPosition( m_cameraCurrentPosition );
	m_worldCamera.SetOutputSize( m_currentCameraOutputSize );
	m_worldCamera.CorrectAspectRaio( CLIENT_ASPECT );
	m_worldCamera.SetProjectionOrthographic( m_currentCameraOutputSize.y );

	m_UICamera.SetPosition( m_cameraCurrentPosition );
	m_UICamera.SetOutputSize( m_currentCameraOutputSize );
	m_UICamera.CorrectAspectRaio( CLIENT_ASPECT );
	m_UICamera.SetProjectionOrthographic( m_currentCameraOutputSize.y );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateGameObject( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	static Vec2 originalPosition;

	if ( !m_selectedGameObject )
	{
		return;
	}
	else
	{
		UpdateSimulationType( &m_simMode );
		Vec2 colliderPos		= m_selectedGameObject->m_rigidbody->GetPosition();
		Vec2 newWorldPosition	= m_worldCamera.GetWorldNormalizedToClientPosition( g_theInput->GetMouseNormalizedClientPosition() );

		if ( !m_isDragOffsetSet )
		{
			m_rigidBodyMouseOffset	=  newWorldPosition - colliderPos;
			m_isDragOffsetSet = true;
			originalPosition = newWorldPosition;
		}
		m_selectedGameObject->m_rigidbody->SetPosition( newWorldPosition - m_rigidBodyMouseOffset );
	}
}

void Game::UpdateSimulationType( eSimulationMode* simMode )
{
	if ( g_theInput->WasKeyJustPressed( '1' ) )
	{
		m_selectedGameObject->m_rigidbody->SetSimulationMode( SIMULATIONMODE_STATIC );
		*simMode = SIMULATIONMODE_STATIC;
	}
	else if ( g_theInput->WasKeyJustPressed( '2' ) )
	{
		m_selectedGameObject->m_rigidbody->SetSimulationMode( SIMULATIONMODE_KINEMATIC );
		*simMode = SIMULATIONMODE_KINEMATIC;
	}
	else if ( g_theInput->WasKeyJustPressed( '3' ) )
	{
		m_selectedGameObject->m_rigidbody->SetSimulationMode( SIMULATIONMODE_DYNAMIC );
		*simMode = SIMULATIONMODE_DYNAMIC;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateGravity()
{
	if ( g_theInput->WasKeyJustPressed( KEY_PLUS ) )
	{
		Vec2 currentGravity = g_thePhysicsSystem->GetSceneGravity();
		currentGravity -= Vec2( 0.f , DELTA_GRAVITY_CHANGE_ALONGY );
		g_thePhysicsSystem->ChangeSceneGravity( currentGravity );
	}

	if ( g_theInput->WasKeyJustPressed( KEY_MINUS ) )
	{
		Vec2 currentGravity = g_thePhysicsSystem->GetSceneGravity();
		currentGravity += Vec2( 0.f , DELTA_GRAVITY_CHANGE_ALONGY );
		g_thePhysicsSystem->ChangeSceneGravity( currentGravity );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateFriction()
{
	if ( m_selectedGameObject && g_theInput->WasKeyJustPressed( '<' ) )
	{
		Collider2D* currentObjectCollider = m_selectedGameObject->m_rigidbody->GetCollider();
		float friction = currentObjectCollider->GetPhysicsMaterial()->GetFriction();
		friction -= DELTA_FRICTION_CHANGE;
		friction = ClampZeroToOne( friction );
		currentObjectCollider->GetPhysicsMaterial()->SetFriciton( friction );
	}

	if ( m_selectedGameObject && g_theInput->WasKeyJustPressed( '>' ) )
	{
		Collider2D* currentObjectCollider = m_selectedGameObject->m_rigidbody->GetCollider();
		float friction = currentObjectCollider->GetPhysicsMaterial()->GetFriction();
		friction += DELTA_FRICTION_CHANGE;
		friction = ClampZeroToOne( friction );
		currentObjectCollider->GetPhysicsMaterial()->SetFriciton( friction );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateMass()
{
	if ( m_selectedGameObject && g_theInput->WasKeyJustPressed( '[' ) )
	{
		Rigidbody2D* currentObjectRigidBody = m_selectedGameObject->m_rigidbody;
		float mass = m_selectedGameObject->m_rigidbody->GetMass();
		mass -= DELTA_MASS_CHANGE;
		mass = Clamp( mass,0.001f, INFINITY );
		currentObjectRigidBody->SetMass( mass );
	}

	if ( m_selectedGameObject && g_theInput->WasKeyJustPressed( ']' ) )
	{
		Rigidbody2D* currentObjectRigidBody = m_selectedGameObject->m_rigidbody;
		float mass = m_selectedGameObject->m_rigidbody->GetMass();
		mass += DELTA_MASS_CHANGE;
		mass = Clamp( mass , 0.001f , INFINITY );
		currentObjectRigidBody->SetMass( mass );
	}
}

void Game::UpdateDrag()
{
	if ( m_selectedGameObject && g_theInput->WasKeyJustPressed( ':' ) )
	{
		Rigidbody2D* currentObjectRigidBody = m_selectedGameObject->m_rigidbody;
		float drag = m_selectedGameObject->m_rigidbody->GetDrag();
		drag -= DELTA_DRAG_CHANGE;
		drag = ClampZeroToOne( drag );
		currentObjectRigidBody->SetDrag( drag );
	}

	if ( m_selectedGameObject && g_theInput->WasKeyJustPressed( '/' ) )
	{
		Rigidbody2D* currentObjectRigidBody = m_selectedGameObject->m_rigidbody;
		float drag = m_selectedGameObject->m_rigidbody->GetMass();
		drag += DELTA_MASS_CHANGE;
		drag = ClampZeroToOne( drag );
		currentObjectRigidBody->SetDrag( drag );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 Game::GetMouseDragVelocity() const
{
	return m_MouseDragFrames[0] - m_MouseDragFrames[ m_frameCount ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateGameObjects()
{
	IsMouseInsideGameObject();
	ResetCollisions();
	AreObjectsColliding();
	//ChangeColorOnCollision();
	ChangeAlphaByBounciness();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::IsMouseInsideGameObject()
{
	for ( size_t colliderIndex = 0; colliderIndex < m_gameObjects.size(); colliderIndex++ )
	{
		if ( m_gameObjects[ colliderIndex ] == nullptr )
		{
			continue;
		}

		Collider2D* collider = m_gameObjects[ colliderIndex ]->m_rigidbody->GetCollider();

		bool isMouseInside = collider->Contains( m_worldCamera.GetWorldNormalizedToClientPosition( g_theInput->GetMouseNormalizedClientPosition() ) );

		bool isSelectedGameObject = m_gameObjects[ colliderIndex ]->m_isSelected;

		if ( isSelectedGameObject )
		{
			m_gameObjects[ colliderIndex ]->m_borderColor = m_selectedObjectColor;
		}

		if ( isMouseInside && !isSelectedGameObject )
		{
			m_gameObjects[ colliderIndex ]->m_borderColor = YELLOW;
		}
		else if ( !isMouseInside && !isSelectedGameObject )
		{
			m_gameObjects[ colliderIndex ]->m_borderColor = BLUE;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::ResetCollisions()
{
	for ( size_t index = 0; index < m_gameObjects.size(); index++ )
	{
		if ( m_gameObjects[ index ] )
		{
			m_gameObjects[ index ]->m_isColliding = false;
			m_gameObjects[ index ]->m_fillColor = m_fillColor;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::AreObjectsColliding()
{
	for ( size_t firstColliderIndex = 0; firstColliderIndex < m_gameObjects.size(); firstColliderIndex++ )
	{
		if ( m_gameObjects[ firstColliderIndex ] == nullptr )
		{
			continue;
		}
		for ( size_t secondColliderIndex = firstColliderIndex + 1; secondColliderIndex < m_gameObjects.size(); secondColliderIndex++ )
		{
			if ( m_gameObjects[ secondColliderIndex ] == nullptr )
			{
				continue;
			}
			if ( m_gameObjects[ firstColliderIndex ] != m_gameObjects[ secondColliderIndex ] )
			{
				if ( m_gameObjects[ firstColliderIndex ]->m_rigidbody->m_collider->Intersects( m_gameObjects[ secondColliderIndex ]->m_rigidbody->m_collider ) )
				{
					m_gameObjects[ firstColliderIndex ]->m_isColliding = true;
					m_gameObjects[ secondColliderIndex ]->m_isColliding = true;
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::ChangeColorOnCollision()
{
	for ( size_t index = 0; index < m_gameObjects.size(); index++ )
	{
		if ( m_gameObjects[ index ] && m_gameObjects[ index ]->m_isColliding )
		{
			m_gameObjects[ index ]->m_fillColor = m_overlapColor;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::ChangeAlphaByBounciness()
{
	for ( size_t index = 0; index < m_gameObjects.size(); index++ )
	{
		if ( m_gameObjects[ index ] )
		{
			float bounciness = m_gameObjects[ index ]->m_rigidbody->GetCollider()->GetPhysicsMaterial()->GetBounciness();
			m_gameObjects[ index ]->m_fillColor.a = ( uchar ) RangeMapFloat( 0.f , 1.f , 0.f , 127.f , bounciness );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RandomizePointCloud( RandomNumberGenerator rng )
{
	uint count = rng.RollRandomIntInRange( 2 , 50 );

	for ( size_t index = 0; index < count; index++ )
	{
		Vec2 temp = Vec2( rng.RollRandomFloatLessThan( 800.f ) , rng.RollRandomFloatLessThan( 800.f ) );
		m_pointCloud.push_back( temp );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DrawMouseCurrentPosition( const Camera& camera ) const
{
	Vec2 mouseNormalizedPos = g_theInput->GetMouseNormalizedClientPosition();
	Vec2 mouseCurrentClientPosition = camera.GetClientToWorldPosition( mouseNormalizedPos );
	g_theRenderer->DrawDisc( Disc2D( mouseCurrentClientPosition , 2.5f ) , RED );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameObject* Game::PickGameobject( Vec2 mousePos )
{
	bool isMouseOverAnygameobject = false;
	for ( size_t gameObjectIndex = 0; gameObjectIndex < m_gameObjects.size(); gameObjectIndex++ )
	{
		if ( !m_gameObjects[gameObjectIndex] )
		{
			m_isMouseOnGameObject[ gameObjectIndex ] = false;
			continue;
		}

		Collider2D* collider = m_gameObjects[ gameObjectIndex ]->m_rigidbody->m_collider;

		if ( collider && collider->Contains( mousePos ) )
		{
			m_isMouseOnGameObject[ gameObjectIndex ] = true;
			isMouseOverAnygameobject = true;
		}
	}

	if ( !isMouseOverAnygameobject  || m_gameObjects.size() == 0 )
	{
		return nullptr;
	}
	else
	{
		size_t gameObjectIndex = m_isMouseOnGameObject.size();

		for ( --gameObjectIndex; gameObjectIndex >= 0; --gameObjectIndex )
		{
			if ( m_isMouseOnGameObject[ gameObjectIndex ] )
			{
				return m_gameObjects[ gameObjectIndex ];
			}
		}
		return nullptr;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateFromUserInput( float deltaSeconds )
{

	if ( !m_isDrawModeActive )
	{
		UpdateGravity();
	}

	if ( !m_selectedGameObject )
	{
		UpdateCameraFromUserInput( deltaSeconds );
	}

	SelectGameObjectFormUserInput();
	UpdateSelectedGameObjectBouncinessFromUserInput( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::SelectGameObjectFormUserInput()
{
	if ( !m_isDrawModeActive )
	{
		if ( g_theInput->WasRightMouseButtonJustPressed() && !m_selectedGameObject )
		{
			m_currentColliderRadius = m_rng.RollRandomFloatInRange( m_minColliderRadius , m_maxColliderRadius );
			Vec2 spawnPos = m_worldCamera.GetWorldNormalizedToClientPosition( g_theInput->GetMouseNormalizedClientPosition() );
			GameObject* temp = new GameObject( g_thePhysicsSystem , spawnPos , Vec2::ZERO , m_currentColliderRadius );
			m_gameObjects.push_back( temp );
			m_isMouseOnGameObject.push_back( false );
		}

		if ( g_theInput->WasLeftMouseButtonJustPressed() )
		{
			if ( !m_isGameObjectSelected )
			{
				Vec2 PickObjectPosition = m_worldCamera.GetWorldNormalizedToClientPosition( g_theInput->GetMouseNormalizedClientPosition() );
				m_selectedGameObject = PickGameobject( PickObjectPosition );
				if ( m_selectedGameObject )
				{
					m_dragTime = 0.f;
					m_frameCount = 0;
					m_MouseDragFrames[ m_frameCount ] = m_worldCamera.GetWorldNormalizedToClientPosition( g_theInput->GetMouseNormalizedClientPosition() );
					m_selectedGameObject->m_rigidbody->ChangeIsSimulationActive( false );
					m_selectedGameObject->m_borderColor = Rgba8( 0 , 127 , 0 , 255 );
					m_selectedGameObject->m_isSelected = true;
					m_isGameObjectSelected = true;
				}
			}
			else
			{
				if ( m_selectedGameObject != nullptr )
				{
					Vec2 newVelocity = GetMouseDragVelocity() / ( m_dragTime );
					m_selectedGameObject->m_rigidbody->SetVelocity( newVelocity );
					m_selectedGameObject->m_rigidbody->ChangeIsSimulationActive( true );
					m_selectedGameObject->m_isSelected = false;
					m_rigidBodyMouseOffset = Vec2::ZERO;
					m_isDragOffsetSet = false;
					m_isGameObjectSelected = false;
				}
				m_selectedGameObject = nullptr;
				for ( size_t gameObjectIndex = 0; gameObjectIndex < m_gameObjects.size(); gameObjectIndex++ )
				{
					m_isMouseOnGameObject[ gameObjectIndex ] = false;
				}
			}
		}

		if ( g_theInput->WasKeyJustPressed( KEY_BACKSPACE ) || g_theInput->WasKeyJustPressed( KEY_DELETE ) )
		{
			if ( m_selectedGameObject )
			{
				for ( size_t index = 0; index < m_gameObjects.size(); index++ )
				{
					if ( !m_gameObjects[ index ] )
					{
						continue;
					}

					if ( m_gameObjects[ index ] == m_selectedGameObject )
					{
						delete m_gameObjects[ index ];
						m_gameObjects[ index ] = nullptr;
						m_selectedGameObject = nullptr;
						m_isGameObjectSelected = false;
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateCameraFromUserInput( float deltaSeconds )
{
	if ( g_theInput->WasKeyJustPressed( 'O' ) )
	{
		m_cameraCurrentPosition = m_cameraDefaultPosition;
	}

	if ( g_theInput->IsKeyHeldDown( 'D' ) )
	{
		m_cameraCurrentPosition += ( Vec3( m_cameraMoveVelocity.x , 0.f , 0.f ) * deltaSeconds );
	}

	if ( g_theInput->IsKeyHeldDown( 'A' ) )
	{
		m_cameraCurrentPosition -= ( Vec3( m_cameraMoveVelocity.x , 0.f , 0.f ) * deltaSeconds );
	}

	if ( g_theInput->IsKeyHeldDown( 'S' ) )
	{
		m_cameraCurrentPosition -= ( Vec3( 0.f , m_cameraMoveVelocity.y , 0.f ) * deltaSeconds );
	}

	if ( g_theInput->IsKeyHeldDown( 'W' ) )
	{
		m_cameraCurrentPosition += ( Vec3( 0.f , m_cameraMoveVelocity.y , 0.f ) * deltaSeconds );
	}

	if ( g_theInput->GetMouseWheelValue() < 0 )
	{
		m_currentCameraOutputSize = m_worldCamera.GetOutputSize() + ( Vec2( MAX_CAMERA_ZOOM_VELOCITY_X , MAX_CAMERA_ZOOM_VELOCITY_Y ) * deltaSeconds );
		m_currentCameraOutputSize.x = Clamp( m_currentCameraOutputSize.x , 200.f , 20000.f );
		m_currentCameraOutputSize.y = Clamp( m_currentCameraOutputSize.y , 200.f , 20000.f );
	}

	if ( g_theInput->GetMouseWheelValue() > 0 )
	{
		m_currentCameraOutputSize = m_worldCamera.GetOutputSize() - ( Vec2( MAX_CAMERA_ZOOM_VELOCITY_X , MAX_CAMERA_ZOOM_VELOCITY_Y ) * deltaSeconds );
		m_currentCameraOutputSize.x = Clamp( m_currentCameraOutputSize.x , 200.f , 20000.f );
		m_currentCameraOutputSize.y = Clamp( m_currentCameraOutputSize.y , 200.f , 20000.f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateSelectedGameObjectBouncinessFromUserInput( float deltaSeconds )
{
	if ( m_selectedGameObject && g_theInput->IsKeyHeldDown(KEY_SHIFT) && g_theInput->GetMouseWheelValue() < 0 )
	{
		Collider2D* currentObjectCollider = m_selectedGameObject->m_rigidbody->GetCollider();
		float bounciness = m_selectedGameObject->m_rigidbody->GetCollider()->GetPhysicsMaterial()->GetBounciness();
		bounciness  += DELTA_BOUNCINESS_CHANGE * deltaSeconds;
		bounciness = ClampZeroToOne( bounciness );
		currentObjectCollider->GetPhysicsMaterial()->SetBounciness( bounciness );
	}

	if ( m_selectedGameObject && g_theInput->IsKeyHeldDown( KEY_SHIFT ) && g_theInput->GetMouseWheelValue() > 0 )
	{
		Collider2D* currentObjectCollider = m_selectedGameObject->m_rigidbody->GetCollider();
		float bounciness = currentObjectCollider->GetPhysicsMaterial()->GetBounciness();
		bounciness -= DELTA_BOUNCINESS_CHANGE * deltaSeconds;
		bounciness = ClampZeroToOne( bounciness );
		currentObjectCollider->GetPhysicsMaterial()->SetBounciness( bounciness );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------