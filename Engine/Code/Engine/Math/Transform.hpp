#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Matrix4x4.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Transform
{
public:
	Transform();
	~Transform();
	void	SetScale();
	void	SetScale( float xScale , float yScale , float zScale );
	void	SetRotation();
	void	SetRotation( float pitch , float yaw , float roll );
	void	SetTranslation();
	void	SetTranslation( float xTranslation , float yTranslation , float zTranslation );
	void	SetTransform( const Mat44 transform );



	Vec3	GetScale();
	Vec3	GetRotation();
	Vec3	GetTranslation();
	Mat44	GetTransform() const																			{ return m_transform; }

private:
	Mat44 m_transform;
};

//--------------------------------------------------------------------------------------------------------------------------------------------