#include "Engine/Renderer/Texture.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Texture::Texture( const char* imageFilePath , unsigned int textureID , IntVec2 dimensions ) : m_imageFilePath( imageFilePath ) , m_textureID( textureID ) , m_dimensions( dimensions )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Texture::Texture():m_imageFilePath(nullptr),m_textureID(0)
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

unsigned int Texture::GetTextureID() const
{
	return m_textureID;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float Texture::GetAspect() const
{
	return static_cast< float >( m_dimensions.x / m_dimensions.y );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const IntVec2 Texture::GetDimensions() const
{
	return m_dimensions;
}

//--------------------------------------------------------------------------------------------------------------------------------------------