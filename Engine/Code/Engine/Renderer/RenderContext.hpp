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
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class BitmapFont;
class Window;
class SwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGIDebug;
struct ID3D11Buffer;
struct ID3D11BlendState;

class Shader;
class VertexBuffer;
class RenderBuffer;
class Sampler;
class Image;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum class BlendMode
{
	ALPHA ,
	ADDITIVE ,
	NEGATIVE,
	TOTAL
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eBufferSlot
{
	UBO_FRAME_SLOT	= 0,
	UBO_CAMERA_SLOT = 1,
};

struct FrameDataT
{
	float m_systemTime;
	float m_systemDeltaTime;

	float m_padding[ 2 ];
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class RenderContext
{

public:	
	RenderContext() {};
	~RenderContext();
	
	void Startup( Window* window );
	void BeginFrame();
	void UpdateFrameTime( float deltaSeconds );
	void EndFrame();
	void Shutdown();
		
	void ClearScreen( const Rgba8& clearColor );
	void BeginCamera( const Camera& camera );
	void EndCamera( const Camera& camera);

	void SetBlendMode( BlendMode blendMode );

	void CreateDebugModule();
	void DestroyDebugModule();
	void ReportLiveObjects();

//--------------------------------------------------------------------------------------------------------------------------------------------
// TEXTURE FUNCTIONS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Shader*		GetOrCreateShader( char const* shaderFilename );
	Texture*	GetOrCreateTextureFromFile( const char* imageFilePath );
	BitmapFont* GetOrCreateBitmapFontFromFile( std::string bitmapFontFilePath );
	void		BindTexture( const Texture* constTexture );
	void		CreateBlendStates();
//--------------------------------------------------------------------------------------------------------------------------------------------
// DRAW FUNCTIONS
//--------------------------------------------------------------------------------------------------------------------------------------------
	void Draw( int numVertexes , int vertexOffset = 0 );
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
	bool		IsDrawing() const;
	bool		BindShader( Shader* shader );
	void		BindShader( std::string shaderFileName );
	void		BindVertexInput( VertexBuffer* vbo );
	void		BindUniformBuffer( unsigned int slot , RenderBuffer* ubo ); // ubo - uniform buffer object
	void		BindSampler( const Sampler* sampler );
	Texture*	CreateTextureFromColor( Rgba8 color );
	Texture*	CreateFromImage( Image* image );

private:

//--------------------------------------------------------------------------------------------------------------------------------------------
// TEXTURE FUNCTIONS ( TO BE ACCESSED BY THE RENDERER ONLY )
//--------------------------------------------------------------------------------------------------------------------------------------------
	Shader*		CreateShaderFromFile( char const* shaderFillePath );
	Texture*	CreateTextureFromFile( const char* imageFilePath );
	BitmapFont* CreateBitMapFontFromFile( std::string bitmapFontFilePath );

//--------------------------------------------------------------------------------------------------------------------------------------------
public:
	ID3D11Device*		 m_device			= nullptr ;
	ID3D11DeviceContext* m_context			= nullptr ; // Immediate context
	SwapChain*			 m_swapChain		= nullptr;
	Camera*				 m_currentCamera	= nullptr;

	void*				 m_debugModule		= nullptr;
	IDXGIDebug*			 m_debug			= nullptr;


	Shader*				 m_defaultShader	= nullptr;
	Shader*				 m_currentShader	= nullptr;
	VertexBuffer*		 m_immediateVBO		= nullptr;
	ID3D11Buffer*		 m_lastBoundVBO		= nullptr;
	Texture*			 m_textureTarget	= nullptr;
	ID3D11BlendState*	 m_alphaBlendState = nullptr;
	ID3D11BlendState*	 m_additiveBlendState = nullptr;


	RenderBuffer*		 m_frameUBO			= nullptr;
	Sampler*			 m_defaultSampler	= nullptr;
	Texture*			 m_textureDefault	= nullptr;

private:

	std::map<std::string , Texture*>	m_LoadedTextures;	 // LOOKUP TABLE OF FILEPATH & TEXTUREID
	std::map<std::string , BitmapFont*> m_LoadedBitMapFonts; // LOOKUP TABLE OF FILEPATH & BITMAPFONTID
	std::map<std::string , Shader*>		m_LoadedShaders;	 // LOOKUP TABLE OF FILEPATH & Shaders
};

