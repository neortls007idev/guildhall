#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Engine/Primitives/Disc2D.hpp"
#include "Engine/Primitives/OBB2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"

#include <map>
#include <string>
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------
//				FORWARD DECLARATIONS 
//--------------------------------------------------------------------------------------------------------------------------------------------
class	Window;
class	Shader;
class	SwapChain;
class	VertexBuffer;
class	IndexBuffer;
class	RenderBuffer;
class	GPUMesh;
class	Sampler;
class	Image;
class	BitmapFont;

struct	ID3D11Device;
struct	ID3D11DeviceContext;
struct	IDXGIDebug;
struct  ID3D11RasterizerState;
struct	ID3D11Buffer;
struct	ID3D11BlendState;
struct	ID3D11DepthStencilState;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eBlendMode
{
	ALPHA ,
	ADDITIVE ,
	SOLID,
	TOTAL_BLEND_MODES
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum  eRasterState
{
	FILL_SOLID ,
	WIREFRAME ,
	TOTAL_RASTER_STATES
};


//--------------------------------------------------------------------------------------------------------------------------------------------

enum eBufferSlot
{
	UBO_FRAME_SLOT	= 0,
	UBO_CAMERA_SLOT = 1,
	UBO_MODEL_SLOT	= 2,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct FrameDataT
{
	float m_systemTime;
	float m_systemDeltaTime;

	float m_padding[ 2 ];
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct ModelDataT
{
	Mat44 model;
};


//--------------------------------------------------------------------------------------------------------------------------------------------

typedef unsigned uint;

class RenderContext
{

public:
	RenderContext() {};
	~RenderContext();

	void		Startup( Window* window );
	void		BeginFrame();
	void		UpdateFrameTime( float deltaSeconds );
	void		EndFrame();
	void		Shutdown();

//--------------------------------------------------------------------------------------------------------------------------------------------
//			CAMERA METHODS
//--------------------------------------------------------------------------------------------------------------------------------------------
	
	void		ClearScreen( const Rgba8& clearColor );										// Clear Color
	void		ClearColor( Texture* colorTarget , Rgba8 color );							// TODO :- IMPLEMENT ME
	void		ClearDepth( Texture* depthStencilTextureTarget , float depth );				// TODO :- TEST ME
	void		SetDepthTest( eCompareOp compare , bool writeOnPass );						// TODO :- IMPLEMENT ME
	Texture*	GetFrameColorTarget();														// TODO :- IMPLEMENT ME
	
	void		BeginCamera( const Camera& camera );
	void		EndCamera( const Camera& camera);

	void		SetModelMatrix( Mat44 modelmat );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			D3D11 DEBUG MODULE 
//--------------------------------------------------------------------------------------------------------------------------------------------

	void		CreateDebugModule();
	void		DestroyDebugModule();
	void		ReportLiveObjects();


//--------------------------------------------------------------------------------------------------------------------------------------------
//			RENDERING PIPELINE BINDING METHODS
//--------------------------------------------------------------------------------------------------------------------------------------------

	void		SetBlendMode( eBlendMode blendMode );
	void		SetRasterState( eRasterState rasterState );
	
	void		BindTexture( const Texture* constTexture );
	bool		BindShader( Shader* shader );
	void		BindShader( std::string shaderFileName );
	void		BindVertexBuffer( VertexBuffer* vbo );
	void		BindIndexBuffer( IndexBuffer* ibo );
	void		BindUniformBuffer( unsigned int slot , RenderBuffer* ubo ); // UBO - uniform buffer object
	void		BindSampler( const Sampler* sampler );
	void		BindDepthStencil( Texture* depthStencilView );
	
//--------------------------------------------------------------------------------------------------------------------------------------------
//			RENDERING PIPELINE CREATION METHODS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Shader*		GetOrCreateShader( char const* shaderFilename );
	Texture*	GetOrCreateTextureFromFile( const char* imageFilePath );
	BitmapFont* GetOrCreateBitmapFontFromFile( std::string bitmapFontFilePath );
	void		CreateBlendStates();
	void		CreateRasterStates();
	
//--------------------------------------------------------------------------------------------------------------------------------------------
//			DRAW METHODS
//--------------------------------------------------------------------------------------------------------------------------------------------
	void Draw( int numVertexes , int vertexOffset = 0 );
	void DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void DrawVertexArray( const std::vector<Vertex_PCU>& vertexArray );
	void DrawVertexArray( int numVertexes , VertexBuffer* vertices );
	void DrawIndexed( uint indexCount , uint startIndex, uint indexStride );
	void DrawMesh( const GPUMesh* mesh );


	// Draw Line in WorldSpace and Transform Relative to World Space Origin
	void DrawLine ( const Vec2& start , const Vec2& end , const Rgba8& color , float thickness , float scale = 1.f ,
	                float orientationDegrees = 0.f , Vec2 translate = Vec2::ZERO );

	// Draw Arrow in WorldSpace and Transform Relative to World Space Origin
	void DrawArrow ( const Vec2& start , const Vec2& end , const Rgba8& color , float thickness , float scale = 1.f ,
	                 float orientationDegrees = 0.f , Vec2 translate = Vec2::ZERO );

	// Draw Ring in WorldSpace around Center and Transform Relative to World Space Origin
	void DrawRing ( const Vec2& center , float radius , const Rgba8& color , float thickness , float scale = 1.f ,
	                float orientationDegrees = 0.f , Vec2 translate = Vec2::ZERO );

	// Draw Disc in WorldSpace around Center and Transform Relative to World Space Origin
	void DrawDisc ( const Disc2D& disc , const Rgba8& tint , float scale = 1.f , float orientationDegrees = 0.f ,
	                Vec2 translate = Vec2::ZERO );

	// Draw Disc in WorldSpace around Center and Transform Relative to World Space Origin
	void DrawDisc ( const Vec2& center , const float& radius , const Rgba8& tint , float scale = 1.f ,
	                float orientationDegrees = 0.f , Vec2 translate = Vec2::ZERO );

	// Draw DiscFraction ( fraction ranges from 0 to 1 ) in WorldSpace around Center and Transform Relative to World Space Origin
 	void DrawDiscFraction ( const Disc2D& disc , const float drawFraction , const Rgba8& tint ,
                            const float orientationDegrees );

	// Draw AABB2 in WorldSpace
	void DrawAABB2( const AABB2& box, const Rgba8& tint);

	// Draw OBB2 in WorldSpace
	void DrawOBB2( const OBB2& box , const Rgba8& tint );

	// Draw Capsule in WorldSpace
	void DrawCapsule2D ( const Vec2& capsuleMidStart , const Vec2& capsuleMidEnd , const float capsuleRadius ,
	                     const Rgba8& tint );
	
	// Draw Polygon in WorldSpace and Transform Relative to World Space Origin
	void DrawPolygon ( const Vec2* points , unsigned int count , const Rgba8& tint , float scale = 1.f ,
	                   float orientationDegrees = 0.f , Vec2 translate = Vec2::ZERO );

	// Draw UnfilledPolygon in WorldSpace and Transform Relative to World Space Origin
	void DrawUnfilledPolygon ( const Vec2* points , unsigned int count , const Rgba8& tint , float scale = 1.f ,
	                           float orientationDegrees = 0.f , Vec2 translate = Vec2::ZERO );						// TODO :- IMPLEMENT ME

	// Draw Polygon in WorldSpace and Transform Relative to World Space Origin
	void DrawRegularPolygon();																						// TODO :- IMPLEMENT ME

//--------------------------------------------------------------------------------------------------------------------------------------------
	bool		IsDrawing() const;
	void		ReCompileAllShaders();
	bool		HasAnyShaderChangedAtPath( const wchar_t* relativePath , float waitInMilliseconds );

//--------------------------------------------------------------------------------------------------------------------------------------------
	

private:

//--------------------------------------------------------------------------------------------------------------------------------------------
//			RENDERING PIPELINE CREATION METHODS ( TO BE ACCESSED BY THE RENDERER ONLY )
//--------------------------------------------------------------------------------------------------------------------------------------------

	Shader*		CreateShaderFromFile( char const* shaderFilePath );
	Texture*	CreateTextureFromFile( const char* imageFilePath );
	Texture*	CreateTextureFromColor( Rgba8 color );
	Texture*	CreateFromImage( Image* image );											// TODO :- IMPLEMENT ME
	BitmapFont* CreateBitMapFontFromFile( std::string bitmapFontFilePath );

//--------------------------------------------------------------------------------------------------------------------------------------------
public:
	ID3D11Device*						m_device								= nullptr ;
	ID3D11DeviceContext*				m_context								= nullptr ; // Immediate context
	SwapChain*							m_swapChain								= nullptr;
	ID3D11DepthStencilState*			m_currentDepthStencilState				= nullptr;

	Camera*								m_currentCamera							= nullptr;

	void*								m_debugModule							= nullptr;
	IDXGIDebug*							m_debug									= nullptr;

	Shader*								m_defaultShader							= nullptr;
	Shader*								m_currentShader							= nullptr;
	VertexBuffer*						m_immediateVBO							= nullptr;
	//IndexBuffer*						m_immediateIBO							= nullptr;
	GPUMesh*							m_immediateMesh							= nullptr;
	ID3D11Buffer*						m_lastBoundVBO							= nullptr;
	ID3D11Buffer*						m_lastBoundIBO							= nullptr;
	Texture*							m_textureTarget							= nullptr;

	ID3D11BlendState*					m_blendStates[eBlendMode::TOTAL_BLEND_MODES];
	ID3D11RasterizerState*				m_rasterStates[eBlendMode::TOTAL_BLEND_MODES];

	RenderBuffer*						m_frameUBO								= nullptr;
	RenderBuffer*						m_modelMatrixUBO						= nullptr;
	Sampler*							m_defaultSampler						= nullptr;
	Texture*							m_textureDefault						= nullptr;

private:

	std::map<std::string , Texture*>	m_LoadedTextures;									// LOOKUP TABLE OF FILEPATH & TEXTURE
	std::map<std::string , BitmapFont*> m_LoadedBitMapFonts;								// LOOKUP TABLE OF FILEPATH & BITMAPFONT
	std::map<std::string , Shader*>		m_LoadedShaders;									// LOOKUP TABLE OF FILEPATH & SHADERS
};

//--------------------------------------------------------------------------------------------------------------------------------------------