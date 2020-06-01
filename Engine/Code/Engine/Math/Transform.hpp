#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Matrix4x4.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Transform
{
public:
	Transform(){};
	~Transform(){};

	void	SetScale( float xScale , float yScale , float zScale );
	void	SetRotation( float pitch , float yaw , float roll );
	void	SetPosition( float xPosition , float yPosition , float zPosition );
	void	SetPosition( Vec3 pos );
	void	SetTransform( const Mat44 transform );
	Mat44	GetAsMatrix() const;

	inline Vec3	GetScale() const															{ return m_scale; }
	inline Vec3	GetRotation() const															{ return m_rotationDegrees; }
	inline Vec3	GetPostion() const															{ return m_position; }

private:
	Vec3 m_position			= Vec3( 0.0f );
	Vec3 m_rotationDegrees	= Vec3( 0.0f );
	//float pitch				= 0.f;
	//float yaw				= 0.f;
	//float roll				= 0.f;
	Vec3 m_scale			= Vec3( 1.0f );
};

//--------------------------------------------------------------------------------------------------------------------------------------------