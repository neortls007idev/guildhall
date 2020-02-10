#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
//				THIRD PARTY LIBRARIES
//--------------------------------------------------------------------------------------------------------------------------------------------
#pragma warning( push )
#pragma warning ( disable: 4100 )
#include "ThirdParty/stb/stb_image.h"
#pragma warning( pop )

//--------------------------------------------------------------------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
		// #include this (massive, platform-specific) header in very few places
#include <gl/GL.h>									// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )	


//--------------------------------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------------------------------

BitmapFont* g_bitmapFont = nullptr;

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::Startup()
{
	if ( g_bitmapFont == nullptr )
	{
		g_bitmapFont = GetOrCreateBitmapFontFromFile( "Data/Fonts/SquirrelFixedFont" ); // TO DO PASS IN THE FONT ADDRESS AND THE TEXTURE POINTER TO IT.
	}
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glLoadIdentity();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::BeginFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::EndFrame()
{
	//SwapBuffers( g_displayDeviceContext );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::Shutdown()
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::ClearScreen( const Rgba8& ClearColor )
{
	glClear( GL_COLOR_BUFFER_BIT ); 
	glClearColor( ClearColor.r * ( 1.f / 255.f ) , ClearColor.g * ( 1.f / 255.f ) , ClearColor.b * ( 1.f / 255.f ) , ClearColor.a * ( 1.f / 255.f ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::BeginCamera( const Camera& camera )
{
	glLoadIdentity();
	Vec2 cameraBottomLeft	= camera.GetPosition() - ( camera.GetOutputSize() / 2.f );
	Vec2 cameraTopRight		= camera.GetPosition() + ( camera.GetOutputSize() / 2.f );
	glOrtho( cameraBottomLeft.x , cameraTopRight.x , cameraBottomLeft.y , cameraTopRight.y , 0.f , 1.f );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::EndCamera( const Camera& camera )
{
	UNUSED(camera);
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::SetBlendMode( BlendMode blendMode )
{
	if ( blendMode == BlendMode::ALPHA )
	{
		glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
	}
	else if ( blendMode == BlendMode::ADDITIVE )
	{
		glBlendFunc( GL_SRC_ALPHA , GL_ONE );
	}
	else
	{
		ERROR_AND_DIE( Stringf( "Unknown / unsupported blend mode #%i" , blendMode ) );
	}
}// state of openGL need to change every time like texture.

//--------------------------------------------------------------------------------------------------------------------------------------------

Texture* RenderContext::CreateTextureFromFile( const char* imageFilePath )
{
	//const char* imageFilePath = "Data/Images/Test_StbiFlippedAndOpenGL.png";
	unsigned int textureID = 0;
	int imageTexelSizeX = 0; // This will be filled in for us to indicate image width
	int imageTexelSizeY = 0; // This will be filled in for us to indicate image height
	int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 0; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)

	stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* imageData = stbi_load( imageFilePath , &imageTexelSizeX , &imageTexelSizeY , &numComponents , numComponentsRequested );

	// Check if the load was successful
	GUARANTEE_OR_DIE( imageData , Stringf( "Failed to load image \"%s\"" , imageFilePath ));
	GUARANTEE_OR_DIE( numComponents >= 3 && numComponents <= 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0 , Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)" , imageFilePath , numComponents , imageTexelSizeX , imageTexelSizeY ) );
	glEnable( GL_TEXTURE_2D );
	glPixelStorei( GL_UNPACK_ALIGNMENT , 1 );
	glGenTextures( 1 , ( GLuint* ) &textureID );
	glBindTexture( GL_TEXTURE_2D , textureID );
	glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_REPEAT ); // GL_Clamp or GL_REPEAT
	glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_REPEAT );

	// THESE 2 LINES ARE COMPULSORY DO NOT REMOVE THEM.
	// Set magnification (texel > pixel) and minification (texel < pixel) filters
	glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_NEAREST ); // one of: GL_NEAREST, GL_LINEAR
	glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR ); // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	if ( numComponents == 3 )
	{
		bufferFormat = GL_RGB;
	}
	GLenum internalFormat = bufferFormat;

	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
		GL_TEXTURE_2D ,		// Creating this as a 2d texture
		0 ,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
		internalFormat ,		// Type of texel format we want OpenGL to use for this texture internally on the video card
		imageTexelSizeX ,	// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,11], and B is the border thickness [0,1]
		imageTexelSizeY ,	// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,11], and B is the border thickness [0,1]
		0 ,					// Border size, in texels (must be 0 or 1, recommend 0)
		bufferFormat ,		// Pixel format describing the composition of the pixel data in buffer
		GL_UNSIGNED_BYTE ,	// Pixel color components are unsigned bytes (one byte per color channel/component)
		imageData );		// Address of the actual pixel data bytes/buffer in system memory

	// Free the raw image texel data now that we've sent a copy of it down to the GPU to be stored in video memory
	
	stbi_image_free( imageData );
	Texture* Temp = new Texture( imageFilePath , textureID , IntVec2 (imageTexelSizeX, imageTexelSizeY ) /* DIMENSION OF THE TEXURE */ );
	m_LoadedTextures[ imageFilePath ] = Temp;
	return Temp;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

BitmapFont* RenderContext::CreateBitMapFontFromFile( std::string bitmapFontFilePath )
{
	Texture* bitmapFontTexture = GetOrCreateTextureFromFile( bitmapFontFilePath.c_str() );
	BitmapFont* newBitmapFont = new BitmapFont( "BitMapFont" , bitmapFontTexture );
	m_LoadedBitMapFonts[ bitmapFontFilePath ] = newBitmapFont;
	return newBitmapFont;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Texture* RenderContext::GetOrCreateTextureFromFile( const char* imageFilePath )
{
	Texture* Temp = m_LoadedTextures[ imageFilePath ];
	if (Temp == nullptr)
	{
		Temp = CreateTextureFromFile( imageFilePath );
	}
	return Temp;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::BindTexture( const Texture* texture )
{
	if ( texture )
	{
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D , texture->GetTextureID() );
	}
	else
	{
		glDisable( GL_TEXTURE_2D );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

BitmapFont* RenderContext::GetOrCreateBitmapFontFromFile( std::string bitmapFontFilePath )
{
	bitmapFontFilePath.append( ".png" );
	BitmapFont* Temp = m_LoadedBitMapFonts[ bitmapFontFilePath ];
	if ( Temp == nullptr )
	{
		Temp = CreateBitMapFontFromFile( bitmapFontFilePath );
	}
	return Temp;
		
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes )
{
	glBegin( GL_TRIANGLES );
	{
		for ( int vertIndex = 0; vertIndex < numVertexes; vertIndex++)
		{
			const Vertex_PCU& vert = vertexes[ vertIndex ];
			glTexCoord2f( vert.m_uvTexCoords.x , vert.m_uvTexCoords.y );
			glColor4ub( vert.m_color.r , vert.m_color.g , vert.m_color.b , vert.m_color.a );
			glVertex3f( vert.m_position.x , vert.m_position.y , vert.m_position.z );
		}
	}
	glEnd();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawVertexArray( const std::vector<Vertex_PCU>& vertexArray )
{
	DrawVertexArray( (int) vertexArray.size() , &vertexArray[ 0 ] );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext:: DrawLine(const Vec2& start, const Vec2& end, const Rgba8& color, float thickness , float scale , float orientationDegrees , Vec2 translate )
{
	Vec2 displacement = end - start;
	Vec2 forward = displacement.GetNormalized();
	forward.SetLength( thickness / 2.f );
	Vec2 left = forward.GetRotated90Degrees();

	Vec2 endLeft	= end	+ forward + left;
	Vec2 endRight	= end	+ forward - left;
	Vec2 startLeft  = start - forward + left;
	Vec2 startRight = start - forward - left;

	Vec3 endLeftVec3( endLeft.x , endLeft.y , 0.f );
	Vec3 endRightVec3( endRight.x , endRight.y , 0.f );
	Vec3 startLeftVec3( startLeft.x , startLeft.y , 0.f );
	Vec3 startRightVec3( startRight.x , startRight.y , 0.f );

	Vertex_PCU lineVerts[6] = { Vertex_PCU(startRightVec3, color, Vec2(0.f, 0.f)),
								Vertex_PCU(endRightVec3  , color, Vec2(0.f, 0.f)),
								Vertex_PCU(endLeftVec3   , color, Vec2(0.f, 0.f)),
								Vertex_PCU(endLeftVec3   , color, Vec2(0.f, 0.f)),
								Vertex_PCU(startLeftVec3 , color, Vec2(0.f, 0.f)),
								Vertex_PCU(startRightVec3, color, Vec2(0.f, 0.f)) };

	TransformVertexArray2D( 6 , lineVerts , scale , orientationDegrees , translate );
	DrawVertexArray(6, lineVerts);
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawRing(const Vec2& center, float radius, const Rgba8& color, float thickness)
{
	constexpr float degreePerSide = 360.f / 64.f;

	for (int NumberOfLines = 0 ; NumberOfLines <= 64 ; NumberOfLines++ )
	{
		float theta1Degrees = degreePerSide * ( float ) NumberOfLines;
		float theta2Degrees = degreePerSide * ( float ) ( NumberOfLines + 1 );

		Vec2 start = center + Vec2( radius * CosDegrees( theta1Degrees ) , radius * SinDegrees( theta1Degrees ) );
		Vec2 end   = center + Vec2( radius * CosDegrees( theta2Degrees ) , radius * SinDegrees( theta2Degrees ) );

			DrawLine( start , end , color , thickness );
	}

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawAABB2( const AABB2& box , const Rgba8& tint )
{
	const Vertex_PCU AABB2Verts[ 6 ] = { 
								Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,0.f ) , tint, Vec2( 0.f, 0.f ) ),
								Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,0.f ) , tint, Vec2( 1.f, 0.f ) ),
								Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,0.f ) , tint, Vec2( 0.f, 1.f ) ),

								Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,0.f ) , tint, Vec2( 1.f, 0.f ) ),
								Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,0.f ) , tint, Vec2( 1.f, 1.f ) ),
								Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,0.f ) , tint, Vec2( 0.f, 1.f ) ) };
	
	DrawVertexArray( 6 , AABB2Verts );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawDisc( const Disc2D& disc , const Rgba8& tint )
{
	constexpr int  NUMBER_OF_DISC_VERTS = 120;
	Vertex_PCU discVerts[ NUMBER_OF_DISC_VERTS ];
	const Vec2 UVCoordsAtCenter = Vec2( 0.5f , 0.5f );
	const float UVRadius = 0.5f;

	float angleInDegreesBetweenDiscTriangles = 0.f;

	discVerts[ 0 ] = Vertex_PCU( ( Vec3( 0.f , 0.f , 0.f ) ) , tint , Vec2( 0.5f , 0.5f ) );
	discVerts[ 1 ] = Vertex_PCU( ( Vec3( disc.m_radius , 0.f , 0.f ) ) , tint , Vec2( 1.f , 0.5f ) );
	angleInDegreesBetweenDiscTriangles = ( 360.f * 3.f ) / static_cast< float >( NUMBER_OF_DISC_VERTS );

	//-----------------------------------------------------------------------------------------------------------------
	float costheta = CosDegrees( angleInDegreesBetweenDiscTriangles );
	float intialXVertex = disc.m_radius * costheta;

	float sintheha = SinDegrees( angleInDegreesBetweenDiscTriangles );
	float initialYVertex = disc.m_radius * sintheha;

	discVerts[ 2 ] = Vertex_PCU( ( Vec3( intialXVertex , initialYVertex , 0.f ) ) , tint , Vec2( UVRadius + UVRadius * costheta , UVRadius + UVRadius * sintheha ) );

	//-----------------------------------------------------------------------------------------------------------------
	int discVertIndex = 3;
	for ( discVertIndex = 3; discVertIndex < NUMBER_OF_DISC_VERTS; discVertIndex += 3 )
	{
		angleInDegreesBetweenDiscTriangles = angleInDegreesBetweenDiscTriangles + ( ( 360.f * 3.f ) / ( NUMBER_OF_DISC_VERTS ) );
		discVerts[ discVertIndex ] = discVerts[ discVertIndex - 3 ];
		discVerts[ discVertIndex + 1 ] = discVerts[ discVertIndex - 1 ];

		discVerts[ discVertIndex + 2 ].m_position = Vec3( disc.m_radius * CosDegrees( angleInDegreesBetweenDiscTriangles ) ,
			disc.m_radius * SinDegrees( angleInDegreesBetweenDiscTriangles ) , 0.f );
		discVerts[ discVertIndex + 2 ].m_color = tint;
		discVerts[ discVertIndex + 2 ].m_uvTexCoords = Vec2( UVRadius + UVRadius * CosDegrees( angleInDegreesBetweenDiscTriangles ) ,
															 UVRadius + UVRadius * SinDegrees( angleInDegreesBetweenDiscTriangles ) );
	}
	discVerts[ NUMBER_OF_DISC_VERTS - 1 ] = discVerts[ 1 ];


	// MOVE  THIS LINE OF CODE INTO A SEPARATE FUNCTION LATER
	TransformVertexArray2D( NUMBER_OF_DISC_VERTS , discVerts , 1 , 0.f , disc.m_center );
	DrawVertexArray( NUMBER_OF_DISC_VERTS , discVerts );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawDisc( const Vec2& center , const float& radius , const Rgba8& tint )
{
	constexpr int  NUMBER_OF_DISC_VERTS = 120;
	Vertex_PCU discVerts[ NUMBER_OF_DISC_VERTS ];
	const Vec2 UVCoordsAtCenter = Vec2( 0.5f , 0.5f );
	const float UVRadius = 0.5f;

	float angleInDegreesBetweenDiscTriangles = 0.f;

	discVerts[ 0 ] = Vertex_PCU( ( Vec3( 0.f , 0.f , 0.f ) ) , tint , Vec2( 0.5f , 0.5f ) );
	discVerts[ 1 ] = Vertex_PCU( ( Vec3( radius , 0.f , 0.f ) ) , tint , Vec2( 1.f , 0.5f ) );
	angleInDegreesBetweenDiscTriangles = ( 360.f * 3.f ) / static_cast< float >( NUMBER_OF_DISC_VERTS );

	//-----------------------------------------------------------------------------------------------------------------
	float costheta = CosDegrees( angleInDegreesBetweenDiscTriangles );
	float intialXVertex = radius * costheta;

	float sintheha = SinDegrees( angleInDegreesBetweenDiscTriangles );
	float initialYVertex = radius * sintheha;

	discVerts[ 2 ] = Vertex_PCU( ( Vec3( intialXVertex , initialYVertex , 0.f ) ) , tint , Vec2( UVRadius + UVRadius * costheta , UVRadius + UVRadius * sintheha ) );

	//-----------------------------------------------------------------------------------------------------------------
	int discVertIndex = 3;
	for ( discVertIndex = 3; discVertIndex < NUMBER_OF_DISC_VERTS; discVertIndex += 3 )
	{
		angleInDegreesBetweenDiscTriangles = angleInDegreesBetweenDiscTriangles + ( ( 360.f * 3.f ) / ( NUMBER_OF_DISC_VERTS ) );
		discVerts[ discVertIndex ] = discVerts[ discVertIndex - 3 ];
		discVerts[ discVertIndex + 1 ] = discVerts[ discVertIndex - 1 ];

		discVerts[ discVertIndex + 2 ].m_position = Vec3( radius * CosDegrees( angleInDegreesBetweenDiscTriangles ) ,
			radius * SinDegrees( angleInDegreesBetweenDiscTriangles ) , 0.f );
		discVerts[ discVertIndex + 2 ].m_color = tint;
		discVerts[ discVertIndex + 2 ].m_uvTexCoords = Vec2( UVRadius + UVRadius * CosDegrees( angleInDegreesBetweenDiscTriangles ) ,
			UVRadius + UVRadius * SinDegrees( angleInDegreesBetweenDiscTriangles ) );
	}
	discVerts[ NUMBER_OF_DISC_VERTS - 1 ] = discVerts[ 1 ];


	// MOVE  THIS LINE OF CODE INTO A SEPARATE FUNCTION LATER
	TransformVertexArray2D( NUMBER_OF_DISC_VERTS , discVerts , 1 , 0.f , center );
	DrawVertexArray( NUMBER_OF_DISC_VERTS , discVerts );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawOBB2( const OBB2& box , const Rgba8& tint )
{
	float orientationDegrees = box.m_iBasis.GetAngleDegrees();

	AABB2 newbox = AABB2::ZERO_TO_ONE;
	newbox.SetDimensions( box.GetDimensions() );

	Vertex_PCU OBB2Verts[ 6 ] = {
								Vertex_PCU( Vec3( newbox.m_mins.x,newbox.m_mins.y,0.f ) , tint, Vec2( 0.f, 0.f ) ),
								Vertex_PCU( Vec3( newbox.m_maxs.x,newbox.m_mins.y,0.f ) , tint, Vec2( 1.f, 0.f ) ),
								Vertex_PCU( Vec3( newbox.m_mins.x,newbox.m_maxs.y,0.f ) , tint, Vec2( 0.f, 1.f ) ),
												  				  
								Vertex_PCU( Vec3( newbox.m_maxs.x,newbox.m_mins.y,0.f ) , tint, Vec2( 1.f, 0.f ) ),
								Vertex_PCU( Vec3( newbox.m_maxs.x,newbox.m_maxs.y,0.f ) , tint, Vec2( 1.f, 1.f ) ),
								Vertex_PCU( Vec3( newbox.m_mins.x,newbox.m_maxs.y,0.f ) , tint, Vec2( 0.f, 1.f ) ) };

	TransformVertexArray2D( 6 , OBB2Verts , 1.f , orientationDegrees , box.GetCenter() );
	DrawVertexArray( 6 , OBB2Verts );

// ALTERNATE WAY
// 
// 	Vec2 cornerPoints[ 4 ];
// 	box.GetCornerPositions( cornerPoints );
// 	Vertex_PCU obbVertices[ 6 ];
// 	obbVertices[ 0 ] = Vertex_PCU( Vec3( cornerPoints[ 0 ].x , cornerPoints[ 0 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );
// 	obbVertices[ 1 ] = Vertex_PCU( Vec3( cornerPoints[ 1 ].x , cornerPoints[ 1 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );
// 	obbVertices[ 2 ] = Vertex_PCU( Vec3( cornerPoints[ 2 ].x , cornerPoints[ 2 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );
// 
// 	obbVertices[ 3 ] = Vertex_PCU( Vec3( cornerPoints[ 0 ].x , cornerPoints[ 0 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );
// 	obbVertices[ 4 ] = Vertex_PCU( Vec3( cornerPoints[ 3 ].x , cornerPoints[ 3 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );
// 	obbVertices[ 5 ] = Vertex_PCU( Vec3( cornerPoints[ 2 ].x , cornerPoints[ 2 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );
// 
// 	DrawVertexArray( 6 , obbVertices );


}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawCapsule2D( const Vec2& capsuleMidStart , const Vec2& capsuleMidEnd , const float capsuleRadius , const Rgba8& tint )
{
	//DrawLine( capsuleMidStart , capsuleMidEnd , RED , capsuleRadius * 2.f );

	Vec2 displacement = capsuleMidEnd - capsuleMidStart;
	Vec2 forward = displacement.GetNormalized();
	forward.SetLength( capsuleRadius );
	Vec2 left = forward.GetRotated90Degrees();

	Vec2 endLeft	= capsuleMidEnd + left;
	Vec2 endRight	= capsuleMidEnd - left;
	Vec2 startLeft  = capsuleMidStart + left;
	Vec2 startRight = capsuleMidStart - left;

	Vec3 endLeftVec3( endLeft.x , endLeft.y , 0.f );
	Vec3 endRightVec3( endRight.x , endRight.y , 0.f );
	Vec3 startLeftVec3( startLeft.x , startLeft.y , 0.f );
	Vec3 startRightVec3( startRight.x , startRight.y , 0.f );

	const Vertex_PCU lineVerts[ 6 ] = { Vertex_PCU( startRightVec3, tint, Vec2( 0.f, 0.f ) ),
										Vertex_PCU( endRightVec3  , tint, Vec2( 0.f, 0.f ) ),
										Vertex_PCU( endLeftVec3   , tint, Vec2( 0.f, 0.f ) ),
										Vertex_PCU( endLeftVec3   , tint, Vec2( 0.f, 0.f ) ),
										Vertex_PCU( startLeftVec3 , tint, Vec2( 0.f, 0.f ) ),
										Vertex_PCU( startRightVec3, tint, Vec2( 0.f, 0.f ) ) };

	DrawVertexArray( 6 , lineVerts );

	DrawDiscFraction( Disc2D( capsuleMidStart , capsuleRadius ) , 0.5f , tint, (capsuleMidEnd-capsuleMidStart).GetRotated90Degrees().GetAngleDegrees() );
	DrawDiscFraction( Disc2D( capsuleMidEnd , capsuleRadius ) , 0.5f , tint , 180.f + ( capsuleMidEnd - capsuleMidStart ).GetRotated90Degrees().GetAngleDegrees() );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawDiscFraction(const Disc2D& disc, const float drawFraction, const Rgba8& tint, const float OrientationDegrees)
{
	constexpr int  NUMBER_OF_DISC_VERTS = 120 ;
	Vertex_PCU discVerts[ NUMBER_OF_DISC_VERTS ];
	const int NUMBER_OF_VERTS_IN_DISC_FRACTION = static_cast< int >( 120 * drawFraction );
	const Vec2 UVCoordsAtCenter = Vec2( 0.5f , 0.5f );
	const float UVRadius = 0.5f;

	float angleInDegreesBetweenDiscTriangles = 0.f;

	discVerts[ 0 ] = Vertex_PCU( ( Vec3( 0.f , 0.f , 0.f ) ) , tint , Vec2( 0.5f , 0.5f ) );
	discVerts[ 1 ] = Vertex_PCU( ( Vec3( disc.m_radius , 0.f , 0.f ) ) , tint , Vec2( 1.f , 0.5f ) );
	angleInDegreesBetweenDiscTriangles = ( 360.f * 3.f ) / static_cast< float >( NUMBER_OF_DISC_VERTS );

	//-----------------------------------------------------------------------------------------------------------------
	float costheta = CosDegrees( angleInDegreesBetweenDiscTriangles );
	float intialXVertex = disc.m_radius * costheta;

	float sintheha = SinDegrees( angleInDegreesBetweenDiscTriangles );
	float initialYVertex = disc.m_radius * sintheha;

	discVerts[ 2 ] = Vertex_PCU( ( Vec3( intialXVertex , initialYVertex , 0.f ) ) , tint , Vec2( UVRadius + UVRadius * costheta , UVRadius + UVRadius * sintheha ) );

	//-----------------------------------------------------------------------------------------------------------------
	int discVertIndex = 3;
	for ( discVertIndex = 3; discVertIndex < NUMBER_OF_VERTS_IN_DISC_FRACTION; discVertIndex += 3 )
	{
		angleInDegreesBetweenDiscTriangles = angleInDegreesBetweenDiscTriangles + ( ( 360.f * 3.f ) / ( NUMBER_OF_DISC_VERTS ) );
		discVerts[ discVertIndex ] = discVerts[ discVertIndex - 3 ];
		discVerts[ discVertIndex + 1 ] = discVerts[ discVertIndex - 1 ];

		discVerts[ discVertIndex + 2 ].m_position = Vec3( disc.m_radius * CosDegrees( angleInDegreesBetweenDiscTriangles ) ,
			disc.m_radius * SinDegrees( angleInDegreesBetweenDiscTriangles ) , 0.f );
		discVerts[ discVertIndex + 2 ].m_color = tint;
		discVerts[ discVertIndex + 2 ].m_uvTexCoords = Vec2( UVRadius + UVRadius * CosDegrees( angleInDegreesBetweenDiscTriangles ) ,
			UVRadius + UVRadius * SinDegrees( angleInDegreesBetweenDiscTriangles ) );
	}
	discVerts[ NUMBER_OF_DISC_VERTS - 1 ] = discVerts[ 1 ];


	// MOVE  THIS LINE OF CODE INTO A SEPARATE FUNCTION LATER
	TransformVertexArray2D( NUMBER_OF_VERTS_IN_DISC_FRACTION , discVerts , 1 , OrientationDegrees , disc.m_center );
	DrawVertexArray( NUMBER_OF_VERTS_IN_DISC_FRACTION , discVerts );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawPolygon( const Vec2* points , unsigned int count , const Rgba8& tint )
{
	if ( count < 3 )
	{
		return;
	}

	std::vector<Vertex_PCU> polygonVerts;

	AppendVertsForPolygon( polygonVerts , points , count , tint );

	DrawVertexArray( ( int ) polygonVerts.size() , &polygonVerts[ 0 ] );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawX( Vec2 position , const Rgba8& color , float scale , float thickness )
{
	DrawLine( Vec2(  -100.f , -100.f ) , Vec2(  100.f ,  100.f ) , color , thickness , scale );
	DrawLine( Vec2( -100.f , -100.f ) , Vec2( 100.f , 100.f ) , color , thickness , scale , 90.f );
}

//---------------------------------------------------------------------------------------------------------------------------------------------