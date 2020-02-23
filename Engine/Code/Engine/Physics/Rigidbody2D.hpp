#pragma once
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Math/Vec2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class DiscCollider2D;
class Rigidbody2D;
class Collider2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eSimulationMode
{
	SIMULATIONMODE_STATIC,
	SIMULATIONMODE_KINEMATIC,
	SIMULATIONMODE_DYNAMIC
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class Rigidbody2D
{
	friend class Physics2D;

public:
	Rigidbody2D( Physics2D* system , Vec2 worldPosition , Collider2D* collider );
	Rigidbody2D( Physics2D* system , Vec2 worldPosition );
	void			Destroy();																	// helper for destroying myself (uses owner to mark self for destruction)
	void			Update( float deltaSeconds );

	void			TakeCollider( Collider2D* collider );										// takes ownership of a collider (destroying my current one if present)

	void SetSimulationModeBasedOnCollider( Collider2D* collider );

	Collider2D* GetCollider() const { return m_collider; }
	Vec2			GetPosition() const								{ return m_worldPosition; }
	Vec2			GetVelocity() const								{ return m_velocity; }
	eSimulationMode GetSimulationMode() const						{ return m_simulationMode; }
	float			GetMass() const									{ return m_mass; }
	bool			IsSimulationActive() const						{ return m_isSimulationActive; }

	void			SetCollider( Collider2D* collider );
	void			SetPosition( Vec2 position );
	void			SetVeloity( Vec2 velocity );
	void			ReverseVelocity();
	void			SetSimulationMode( eSimulationMode simulationMode );
	void			SetMass( float newMass );
	void			ChangeIsSimulationActive( bool newSimulationStatus );

public:
	Physics2D*			m_system				= nullptr;											// which scene created/owns this object
	bool				m_isGarbage				= false;
	Collider2D*			m_collider				= nullptr;
	Vec2				m_worldPosition;															// where in the world is this rigidbody
	Vec2				m_velocity				= Vec2( 0.f , 0.f );
	float				m_mass;
	eSimulationMode		m_simulationMode		= SIMULATIONMODE_DYNAMIC;
	bool				m_isSimulationActive	= true;

private:
	~Rigidbody2D();																					// destroys the collider
};