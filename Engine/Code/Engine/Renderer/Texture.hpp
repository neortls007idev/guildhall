#pragma once
#include "Engine/Math/IntVec2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Texture
{

public:
	 
	 Texture(); // DEFAULT CONSTRUCTOR ENSURING THE FILE PATH IS NULL AND NO TEXTURE IS MAPPED TO THE OBJECT
	 Texture( const char* imageFilePath, unsigned int textureID, IntVec2 dimensions );
	~Texture() {};

	unsigned int	 GetTextureID() const;
		     float	 GetAspect() const;
	const    IntVec2 GetDimensions() const;

private:

	const char*  m_imageFilePath = nullptr;
	unsigned int m_textureID = 0;
	IntVec2		 m_dimensions;

//--------------------------------------------------------------------------------------------------------------------------------------------

};
