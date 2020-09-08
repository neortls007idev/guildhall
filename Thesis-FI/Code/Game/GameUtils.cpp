#include "Game/Game.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::SetLightPositionToOrigin( int lightIndex )
{
	m_lights.lights[ lightIndex ].worldPosition = Vec3::ZERO;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::SetLightPositionToCameraPos( int lightIndex )
{
	m_lights.lights[ lightIndex ].worldPosition = m_gameCamera.GetPosition();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::SetLightDirectionToCameraForwardVector( int lightIndex )
{
	Vec3 direction = m_gameCamera.GetCameraTransform().GetAsMatrix().GetKBasis3D();
	m_lights.lights[ lightIndex ].direction = -direction;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateToneMapData()
{
	switch( m_currentToneMap )
	{
		case ToneMap::NO_TONE:
			{
				m_toneMapTransform = Mat44::IDENTITY;
			}break;

		case ToneMap::GRAYSCALE:
			{
				Vec3 grayScale = Vec3( 0.2126f , 0.7152f , 0.0722f );
				m_toneMapTransform.SetBasisVectors3D( grayScale , grayScale , grayScale );
			}break;

		case ToneMap::SEPHIA:
			{
				Vec3 newRed = Vec3( 0.393f , 0.769f , 0.189f );
				Vec3 newGreen = Vec3( 0.394f , 0.686f , 0.168f );
				Vec3 newBlue = Vec3( 0.272f , 0.534f , 0.131f );
				m_toneMapTransform.SetBasisVectors3D( newRed , newGreen , newBlue );
			}break;
	}

	m_toneMapTransform.ScaleUniform3D( m_tonePower );
}

//--------------------------------------------------------------------------------------------------------------------------------------------