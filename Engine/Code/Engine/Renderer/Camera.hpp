#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Camera
{
public:

	Camera() {};
	~Camera() {};
	// TODO :-  write new constructors;
// 	void SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight );
// 	Vec2 GetOrthoBottomLeft() const;
// 	Vec2 GetOrthoTopRight() const;
// 	void Translate2D( const Vec2 cameraTranslate );
	
	void  SetOutputSize( Vec2 size );
	void  SetPosition( Vec3 position );
		  
	Vec2  GetOutputSize() const;
	Vec2  GetPosition() const;
		  
	void  SetProjectionOrthographic( float height , float nearZ = -1.0f , float farZ = 1.0f );
	Vec2  GetClientToWorldPosition( Vec2 clientPos ) const;
	Vec2  GetWorldNormalizedToClientPosition( Vec2 worldPos ) const;

	float GetAspectRatio() const;
	void  CorrectAspectRaio( float clientAspectRatio );

	Vec2 GetOrthoMin() const;
	Vec2 GetOrthoMax() const;
private:

	Vec2 m_outputSize;
	Vec2 m_position				= Vec2::ZERO;

};

//--------------------------------------------------------------------------------------------------------------------------------------------
