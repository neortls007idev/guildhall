#pragma once

struct PhysicsMaterial
{
	PhysicsMaterial(){};
	~PhysicsMaterial() {};

				 void	SetBounciness( float const newBounciness );
				 void	SetFriciton(   float const newfriciton );

	inline const float	GetBounciness() const														{ return m_bounciness; }
	inline const float	GetFriction() const															{ return m_friction; }
public:
	float m_bounciness = 0.8f;
	float m_friction   = 1.0f;
};
