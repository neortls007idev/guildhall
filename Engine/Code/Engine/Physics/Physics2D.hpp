#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Physics/Collision2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Primitives/Polygon2D.hpp"
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class DiscCollider2D;
class Rigidbody2D;
class Collider2D;
class Camera;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Physics2D
{
public:
	Physics2D();
	~Physics2D();

	void BeginFrame();
	void Update( float deltaSeconds );

	void AdvanceSimulation( float deltaSeconds );
	void ApplyEffectors( Rigidbody2D* rigidbody , float deltaSeconds );
	void MoveRigidbodies( Rigidbody2D* rigidbody , float deltaSeconds );
	void ResetCollisions();
	void DetectCollisions();
	void ResolveCollision( Collision2D collider );
	void ResolveCollisions();
	void CleanupDestroyedObjects();

	void EndFrame();    // cleanup destroyed objects

	// factory style create/destroy
	Rigidbody2D* CreateRigidbody( Vec2 rigidBodyPosition , Vec2 coliderPositionRelativeToRigidBody , float ColliderRadius );
	Rigidbody2D* CreateRigidbody( Vec2 rigidBodyPosition , Vec2 coliderPositionRelativeToRigidBody , Polygon2D convexgon );
	void DestroyRigidbody( Rigidbody2D* rigidbody );

	DiscCollider2D* CreateDiscCollider( Vec2 localPosition , float radius );
	void DestroyCollider( Collider2D* collider );

	void ChangeSceneGravity( Vec2 newGravity );
	Vec2 GetSceneGravity() const																							{ return m_sceneGravity; }
	void GravityBounce( Camera* sceneCamera, Rigidbody2D* rigidBody );
	void ScreenWrapAround( Camera* sceneCamera , Rigidbody2D* rigidBody );

public:
	Vec2 m_sceneGravity = Vec2( 0.f , -9.8f );
	// storage for all rigid bodies
	std::vector<Rigidbody2D*>	m_rigidBodies2D;
	// storage for all colliders
	std::vector<Collider2D*>	m_colliders2D;
	std::vector<Collision2D>	m_frameCollisions;
	Camera*						m_sceneCamera = nullptr;

};

//--------------------------------------------------------------------------------------------------------------------------------------------