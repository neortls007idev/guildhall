#pragma once

struct PhysicsMaterial
{
	PhysicsMaterial(){};
	~PhysicsMaterial() {};

				 void	SetBounciness( float const newBounciness );

	inline const float	GetBounciness() const														{ return m_bounciness; }
public:
	float m_bounciness = 1.f;
};
