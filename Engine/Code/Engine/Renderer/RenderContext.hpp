#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Engine/Primitives/OBB2.hpp"
#include "Engine/Primitives/Disc2D.hpp"
#include <vector>
#include <map>
#include <string>

//--------------------------------------------------------------------------------------------------------------------------------------------

class BitmapFont;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum class BlendMode
{
	ALPHA ,
	ADDITIVE ,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class RenderContext
{

public:	
	
	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	
	void ClearScreen( const Rgba8& ClearColor );
	void BeginCamera( const Camera& camera );
	void EndCamera( const Camera& camera);

	void SetBlendMode( BlendMode blendMode );

//--------------------------------------------------------------------------------------------------------------------------------------------
// TEXTURE FUNCTIONS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Texture*	GetOrCreateTextureFromFile( const char* imageFilePath );
	BitmapFont* GetOrCreateBitmapFontFromFile( std::string bitmapFontFilePath );
	void BindTexture( const Texture* texture );

//--------------------------------------------------------------------------------------------------------------------------------------------
// DRAW FUNCTIONS
//--------------------------------------------------------------------------------------------------------------------------------------------

	void DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void DrawVertexArray( const std::vector<Vertex_PCU>& vertexArray );

	void DrawLine(const Vec2& start, const Vec2& end, const Rgba8& color, float thickness);
	void DrawRing(const Vec2& center, float radius, const Rgba8& color, float thickness);
	void DrawAABB2( const AABB2& box, const Rgba8& tint);
	void DrawDisc( const Disc2D& disc, const Rgba8& tint); 
	void DrawOBB2( const OBB2& box , const Rgba8& tint );
	void DrawCapsule2D( const Vec2& capsuleMidStart , const Vec2& capsuleMidEnd , const float capsuleRadius, const Rgba8& tint );
 	void DrawDiscFraction(const Disc2D& disc, const float drawFraction, const Rgba8& tint, const float orientationDegrees );
 	void DrawRegularPolygon(); // TO DO

//--------------------------------------------------------------------------------------------------------------------------------------------


private:

//--------------------------------------------------------------------------------------------------------------------------------------------
// TEXTURE FUNCTIONS ( TO BE ACCESSED BY THE RENDERER ONLY )
//--------------------------------------------------------------------------------------------------------------------------------------------
	
	Texture*	CreateTextureFromFile( const char* imageFilePath );
	BitmapFont* CreateBitMapFontFromFile( std::string bitmapFontFilePath );

//--------------------------------------------------------------------------------------------------------------------------------------------

private:

	std::map<std::string , Texture*>	m_LoadedTextures;	 // LOOKUP TABLE OF FILEPATH & TEXTUREID
	std::map<std::string , BitmapFont*> m_LoadedBitMapFonts; // LOOKUP TABLE OF FILEPATH & BITMAPFONTID
	
};


