#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Transform.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

void Transform::SetScale( float xScale , float yScale , float zScale )
{
	m_scale = Vec3( xScale , yScale , zScale );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Transform::SetRotation( float pitch , float yaw , float roll )
{
	m_rotationDegrees = Vec3( pitch , yaw , roll );
// 	float finalPitch = Clamp( pitch , -180.f , 180.f );
// 	float finalYaw	 = Clamp( yaw , -180.f , 180.f );
// 	float finalRoll	 = Clamp( roll , -90.f , 90.f );
// 
// 	m_rotationDegrees = Vec3( finalPitch , finalYaw , finalRoll );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Transform::SetPosition( float xPosition , float yPosition , float zPosition )
{
	m_position = Vec3( xPosition , yPosition , zPosition );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Transform::SetPosition( Vec3 pos )
{
	m_position = pos;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Mat44 Transform::GetAsMatrix() const
{
	Mat44 transformBy = Mat44::CreateFromScaleRotationTransformation( m_scale , m_rotationDegrees , m_position );
	return transformBy;
}
