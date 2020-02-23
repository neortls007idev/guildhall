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
