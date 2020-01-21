#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Primitives/AABB2.hpp"

class Texture;

//--------------------------------------------------------------------------------------------------------------------------------------------

class PlayerShip :public Entity {

public:
	explicit PlayerShip(Game* CurrentGameInstance, Vec2 m_position, Vec2 m_velocity, float m_orientationDegrees);
	void InitializePlayerShipMemberVariables();
	
	void Update(float deltaSeconds) override;
	void Render() const override;
	void IsDead() override;
		
	//void RenderingDeveloperMode();

	void checkKeyboardKeyPressForShipMovement(float deltaSeconds);
	void UpdateFromJoystick( float deltaSeconds );
	void XboxButtonPresses(const XboxController& controller, float deltaSeconds);

	void Accelerate(float deltaTime);
	void RotateLeft(float deltaSeconds);
	void RotateRight(float deltaSeconds);

private:
	
	int			m_controllerID		 = -1;
	float		m_thrustFraction	 = 0.f;
	Rgba8		m_shipColor			 = Rgba8(132, 156, 169, 255);
	bool		m_updatefromjoystick = false;
	//AABB2		m_localShip			 = AABB2( Vec2( -2.f , -2.f ) , Vec2( 2.f , 2.f ) );
	AABB2		m_localShip			 = AABB2( Vec2::ZERO , Vec2( 10.02f , 6.f ) );
	Texture*	m_shipTexture		 = nullptr;
	
	Vertex_PCU m_localShipVerts[NUM_SHIP_VERTS] = { 
					Vertex_PCU( ( Vec3(  2.f,  1.f, 0.f)), m_shipColor, Vec2(0.f, 0.f) ),
					Vertex_PCU( ( Vec3(  0.f,  2.f, 0.f)), m_shipColor, Vec2(0.f, 0.f) ),
					Vertex_PCU( ( Vec3( -2.f,  1.f, 0.f)), m_shipColor, Vec2(0.f, 0.f) ),
					Vertex_PCU( ( Vec3( -2.f,  1.f, 0.f)), m_shipColor, Vec2(0.f, 0.f) ),
					Vertex_PCU( ( Vec3( -2.f, -1.f, 0.f)), m_shipColor, Vec2(0.f, 0.f) ),
					Vertex_PCU( ( Vec3(  0.f,  1.f, 0.f)), m_shipColor, Vec2(0.f, 0.f) ),
					Vertex_PCU( ( Vec3(  0.f,  1.f, 0.f)), m_shipColor, Vec2(0.f, 0.f) ),
					Vertex_PCU( ( Vec3(  1.f,  0.f, 0.f)), m_shipColor, Vec2(0.f, 0.f) ), // nose of the ship
					Vertex_PCU( ( Vec3(  0.f, -1.f, 0.f)), m_shipColor, Vec2(0.f, 0.f) ),
					Vertex_PCU( ( Vec3(  0.f,  1.f, 0.f)), m_shipColor, Vec2(0.f, 0.f) ),
					Vertex_PCU( ( Vec3( -2.f, -1.f, 0.f)), m_shipColor, Vec2(0.f, 0.f) ),
					Vertex_PCU( ( Vec3(  0.f, -1.f, 0.f)), m_shipColor, Vec2(0.f, 0.f) ),
					Vertex_PCU( ( Vec3( -2.f, -1.f, 0.f)), m_shipColor, Vec2(0.f, 0.f) ),
					Vertex_PCU( ( Vec3(  2.f, -1.f, 0.f)), m_shipColor, Vec2(0.f, 0.f) ),
					Vertex_PCU( ( Vec3(  0.f, -2.f, 0.f)), m_shipColor, Vec2(0.f, 0.f) )
	};
};
