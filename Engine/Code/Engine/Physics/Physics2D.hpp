#pragma once
#include "Engine/Math/Vec2.hpp"
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
	void ApplyEffectors( float deltaSeconds );
	void MoveRigidbodies( float deltaSeconds );
	void CleanupDestroyedObjects();

	void EndFrame();    // cleanup destroyed objects

	// factory style create/destroy
	Rigidbody2D* CreateRigidbody( Vec2 position , Vec2 coliderPositionRelativeToRigidBody , float ColliderRadius );
	void DestroyRigidbody( Rigidbody2D* rigidbody );

	DiscCollider2D* CreateDiscCollider( Vec2 localPosition , float radius );
	void DestroyCollider( Collider2D* collider );

	void ChangeSceneGravity( Vec2 newGravity );
	Vec2 GetSceneGravity() const																							{ return m_sceneGravity; }
	void GravityBounce( Camera* sceneCamera, Rigidbody2D* rigidBody );
	void ScreenWrapAround( Camera* sceneCamera , Rigidbody2D* rigidBody );

public:
	Vec2 m_sceneGravity = Vec2( 0.f , -200.f );
	// storage for all rigid bodies
	std::vector<Rigidbody2D*>	m_rigidBodies2D;
	// storage for all colliders
	std::vector<Collider2D*>	m_colliders2D;
	Camera*						m_sceneCamera = nullptr;
	
};

//--------------------------------------------------------------------------------------------------------------------------------------------