#pragma once
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Engine/Primitives/LineSegment2D.hpp"
#include "Engine/Primitives/Capsule2D.hpp"
#include "Engine/Primitives/OBB2.hpp"
#include "Engine/Primitives/Disc2D.hpp"
#include "Game/GameCommon.hpp"
#include <vector>
#include "Engine/Primitives/Polygon2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
class GameObject;
class Polygon2D;
//--------------------------------------------------------------------------------------------------------------------------------------------

class Game
{

public:

	Game();
	void InitialGameObjectsSpawner();

	void Update( float deltaSeconds );
	
	void Render() const;
	void RenderUI() const;
	void RenderGravityUI() const;
	void RenderDrawMode() const;
	void RenderDrawFromPointCloudMode() const;
	void DebugRender() const;
	
	void DrawConvexgonMode();
	void PolygonDrawMode();
	void PolygonDrawPointCloudMode();
	bool IsPolygonPotentiallyConvex( Vec2 latestPoint );

	void UpdateCamera();
	void UpdateGameObject( float deltaSeconds );
	void UpdateGameObjects();
	void UpdateSimulationType( eSimulationMode* simMode );
	void UpdateGravity();
	Vec2 GetMouseDragVelocity() const;

	void RandomizePointCloud( RandomNumberGenerator rng );
		
	void DrawMouseCurrentPosition( const Camera& camera ) const;
	GameObject* PickGameobject( Vec2 mousePos );
//--------------------------------------------------------------------------------------------------------------------------------------------

private:

	void UpdateFromUserInput( float deltaSeconds );

//--------------------------------------------------------------------------------------------------------------------------------------------

private:

	int m_controllerID = -1;
	float m_screenShakeIntensity = 0.f;
	float m_tilesInViewHorizontally = 16.0f;
	float m_tilesInViewVertically = 9.0f;
	Vec2 m_CameraCenter = Vec2( m_tilesInViewHorizontally / 2 , m_tilesInViewVertically / 2 );
	bool m_hasCursorChangedToOBB = false;
	eSimulationMode m_simMode = SIMULATIONMODE_STATIC;
//--------------------------------------------------------------------------------------------------------------------------------------------

public:

	static RandomNumberGenerator	m_rng;
		   Camera					m_worldCamera;
		   Camera					m_UICamera;
		   AABB2					m_aabb2D					= AABB2( -800.f , -400.f , 0.f , 0.f );
		   Rgba8					m_fillColor					= Rgba8( 255 , 255 , 255 , 127 );
		   Rgba8					m_overlapColor				= Rgba8( 255 , 0 , 0 , 127 );
		   Rgba8					m_selectedObjectColor		= Rgba8( 0 , 110 , 0 , 255 );
		   Disc2D					m_disc2D					= Disc2D( Vec2( 800.f , -400.f ) , 50.f );
		   
		   Vec2						m_mousePosition				= Vec2::ZERO;
		   Vec2						m_rigidBodyMouseOffset		= Vec2::ZERO;
		   GameObject*				m_selectedGameObject		= nullptr;
		   bool						m_isDragOffsetSet			= false;
		   bool						m_isGameObjectSelected		= false;

		   Vec3						m_cameraDefaultPosition		= Vec3::ZERO;
		   Vec3						m_cameraCurrentPosition		= Vec3::ZERO;
		   Vec3						m_cameraMoveVelocity		= Vec3( MAX_CAMERA_MOVE_VELOCITY_X , MAX_CAMERA_MOVE_VELOCITY_Y , 0.f );
		   Vec2						m_currentCameraOutputSize	= Vec2( 1600.f , 800.f );

		   float					m_minColliderRadius			= MIN_COLLIDER_RADIUS;
		   float					m_maxColliderRadius			= MAX_COLLIDER_RADIUS;
		   float					m_currentColliderRadius;
		   Polygon2D				testPolygon;

		   std::vector<GameObject*>	m_gameObjects;
		   std::vector<bool>		m_isMouseOnGameObject;
		   std::vector<Vec2>		m_pointCloud;

		   std::vector<Vec2>		m_drawModePoints;
		   bool						m_isDrawModeActive = false;
		   Vec2						m_invalidPoint;
		   bool						m_isLastPointInvalid = false;
};

//--------------------------------------------------------------------------------------------------------------------------------------------