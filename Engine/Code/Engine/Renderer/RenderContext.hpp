#pragma once
#include "Engine/Core/CoreUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Engine/Primitives/Disc2D.hpp"
#include "Engine/Primitives/OBB2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/D3D11Utils.hpp"
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

constexpr uint TOTAL_LIGHTS = 1;
//float GAMMA = 2.2f;
//float INVERSE_GAMMA = 1 / GAMMA;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eBlendMode
{
	ALPHA ,
	ADDITIVE ,
	SOLID,
	TOTAL_BLEND_MODES
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eBufferSlot
{
	UBO_FRAME_SLOT			= 0,
	UBO_CAMERA_SLOT			= 1,
	UBO_MODEL_SLOT			= 2,
	UBO_LIGHT_SLOT			= 3,
	UBO_MATERIAL_SLOT		= 4,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eTextureType
{
	TEX_DIFFUSE		= 0 ,
	TEX_NORMAL		= 1 ,
	TEX_ALBEDO		= 2 ,
	TEX_SPECULAR	= 3 ,
	TEX_USER_TYPE	= 8 ,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct FrameDataT
{
	float m_systemTime;
	float m_systemDeltaTime;

	//float m_padding[ 2 ];

	float m_GAMMA				= 2.2f/*GAMMA*/;
	float m_INVERSE_GAMMA		= 1/m_GAMMA;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct ModelDataT
{
	Mat44 model;
	Vec4  normalizedModelColor;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct lightDataT
{
	Vec3        worldPosition				= Vec3::ZERO;
	float       pad00						= -5000.f;                                     // this is not required, but know the GPU will add this padding to make the next variable 16-byte aligned

	Vec3        color						= Vec3::ONE;
	float       intensity					= 1.0f;                                        // rgb and an intensity

	Vec3        direction					= Vec3::UNIT_VECTOR_ALONG_K_BASIS;             // direction light is point, default (0,0,1)
	float       directionFactor				= 0.f;                                         // do we consider this a point light (0.0f) or a directional light (1.0f).  Determine how we calcualte the incident light vector, default (0.0f)

	Vec3        attenuation					= Vec3::UNIT_VECTOR_ALONG_J_BASIS;             // intensity falloff , attenuation for diffuse light, default (0,0,1)
	float       dotInnerAngle				= -1.0f;                                       // cone light inner angle (default -1.0f) - angle at which cone lights begin to stop affecting an object

	Vec3        specularAttenuation			= Vec3::UNIT_VECTOR_ALONG_K_BASIS;             // attenuation for specular lighting (constant,linear,quadratic), default (0,0,1)
	float       dotOuterAngle				= -1.0f;                                       // cone light outer angle (default -1.0f) - angle at which cone lights stop affecting the object completely
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct shaderLightDataT
{
	Vec4		ambientLight;										// rgb and an intensity
	lightDataT	lights[ TOTAL_LIGHTS ];

	// all 0 to 1 and help influence the lighting equation
	float		DIFFUSE_FACTOR		= 1;							// default: 1  - scales diffuse lighting in equation (lower values make an object absorb light
	float		SPECULAR_FACTOR		= 1;							// default: 1  - scales specular lighting in the equation (higher values create a "shinier" surface)
	float		SPECULAR_POWER		= 32;							// default: 32 - controls the specular falloff (higher values create a "smoother" looking surface)
	float		padding00			= 0.f;					
};

//--------------------------------------------------------------------------------------------------------------------------------------------

typedef unsigned uint;

//--------------------------------------------------------------------------------------------------------------------------------------------

class RenderContext
{

public:
	RenderContext() {};
	~RenderContext();

	void					Startup( Window* window );
	void					BeginFrame();
	void					UpdateFrameTime( float deltaSeconds );
	void					EndFrame();
	void					Shutdown();

//--------------------------------------------------------------------------------------------------------------------------------------------
//			CAMERA METHODS
//--------------------------------------------------------------------------------------------------------------------------------------------
	
	void					ClearScreen( const Rgba8& clearColor );											// Clear Color
	void					ClearColor( Texture* colorTarget , Rgba8 color );								// TODO :- IMPLEMENT ME
	void					ClearDepth( Texture* depthStencilTextureTarget , float depth );					
	void					SetDepthTest( eCompareOp compare  = COMPARE_LESS , bool writeOnPass = true );						
	Texture*				GetFrameColorTarget();															// TODO :- IMPLEMENT ME
	
	void					BeginCamera( const Camera& camera );
	void					EndCamera( const Camera& camera);

	void					SetModelMatrix( Mat44 modelmat , Rgba8 color = WHITE );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			D3D11 DEBUG MODULE 
//--------------------------------------------------------------------------------------------------------------------------------------------

	void					CreateDebugModule();
	void					DestroyDebugModule();
	void					ReportLiveObjects();

//--------------------------------------------------------------------------------------------------------------------------------------------
//			RENDERING PIPELINE BINDING METHODS
//--------------------------------------------------------------------------------------------------------------------------------------------
	
	void					SetBlendMode( eBlendMode blendMode );
	void					SetRasterState( eRasterStateFillMode rasterState );
	void					SetTransientRasterStateAsRasterState();
	
	void					BindTexture( const Texture* constTexture , UINT textureType = eTextureType::TEX_DIFFUSE , UINT userTextureIndexOffset = 0 );
	bool					BindShader( Shader* shader );
	void					BindShader( std::string shaderFileName );
	void					BindVertexBuffer( VertexBuffer* vbo );
	void					BindIndexBuffer( IndexBuffer* ibo );
	void					BindUniformBuffer( unsigned int slot , RenderBuffer* ubo ); // UBO - uniform buffer object
	void					BindMaterialData( void* pointerToData , int sizeOfData ); 
	void					BindSampler( const Sampler* sampler );
	void					BindDepthStencil( Texture* depthStencilView );
	
//--------------------------------------------------------------------------------------------------------------------------------------------
//			RENDERING PIPELINE CREATION METHODS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Shader*					GetOrCreateShader( char const* shaderFilename );
	Texture*				GetOrCreateTextureFromFile( const char* imageFilePath );
	BitmapFont*				GetOrCreateBitmapFontFromFile( std::string bitmapFontFilePath );
	void					CreateBlendStates();
	void					CreateRasterStates();
	void					CreateTransientRasterState( eRasterStateFillMode rasterFillMode , eCullMode cullMode , eWindingOrder windingOrder );
	void					SetCullMode( eCullMode cullMode );							
	void					SetFillMode( eRasterStateFillMode rasterFillMode );					
	void					SetWindingOrder( eWindingOrder windingOrder );
	eCullMode				GetCullMode() const;
	eRasterStateFillMode	GetFillMode() const;
	eWindingOrder			GetWindingOrder() const;

//--------------------------------------------------------------------------------------------------------------------------------------------
//			LIGHTING METHODS
//--------------------------------------------------------------------------------------------------------------------------------------------

	void SetAmbientColor( Rgba8 color );

	// Takes in color as RGB8 converted to normalized float4
	void SetAmbientColor( Vec4 color );
	
	void SetAmbientIntensity( float intensity );

	// disabling Ambient is the same as doing...  SetAmbient( WHITE, 1.0f ); 
	void SetAmbientLight( Rgba8 color = WHITE, float intensity = 1.f );

	// Takes in color as RGB8 converted to normalized float4
	void SetAmbientLight( Vec4 color , float intensity );

	// for now, assume idx is 0, we only have one light
	// void EnablePointLight( uint idx, vec3 position, rgba color, float intensity, vec3 attenuation ); 
	void EnableLight( uint idx , lightDataT const& lightInfo );
	void EnableAllLights();

	// disabling a light is the same as just saying the light has 0 intensity
	void DisableLight( uint idx );
	void DisableAllLights();

	void UpdateLightsData( const shaderLightDataT& lightsData );
	
	//how much does specular light show up
	void SetSpecularFactor( float normalizedFactor );

	// Intensity of specular light
	void SetSpecularPower( float specularPower );
	
//--------------------------------------------------------------------------------------------------------------------------------------------
//			DRAW METHODS
//--------------------------------------------------------------------------------------------------------------------------------------------
	void Draw( int numVertexes , int vertexOffset , buffer_attribute_t const* attribs );
	void DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void DrawVertexArray( const std::vector<Vertex_PCU>& vertexArray );
	void DrawVertexArray( int numVertexes , VertexBuffer* vertices );
	void DrawIndexed( uint indexCount , uint startIndex, uint indexStride , buffer_attribute_t const* attribs );
	void DrawMesh( const GPUMesh* mesh );

	// Draw Line in WorldSpace and Transform Relative to World Space Origin
	void DrawLine ( const Vec2& start , const Vec2& end , const Rgba8& color , float thickness , float scale = 1.f ,
	                float orientationDegrees = 0.f , Vec2 translate = Vec2::ZERO );
	
	// Draw Gradient Colored Line in WorldSpace and Transform Relative to World Space Origin
	void DrawLine ( const Vec2& start , const Vec2& end , const Rgba8& startTint , const Rgba8& endTint , float thickness , float scale = 1.f ,
				   float orientationDegrees = 0.f , Vec2 translate = Vec2::ZERO );

	// Draw Arrow in WorldSpace and Transform Relative to World Space Origin
	void DrawArrow2D ( const Vec2& start , const Vec2& end , const Rgba8& color , float thickness , float scale = 1.f ,
					   float orientationDegrees = 0.f , Vec2 translate = Vec2::ZERO );

	void DrawArrow2D ( const Vec2&  start , const Vec2& end , const Rgba8& shaftStartColor ,
	                   const Rgba8& shaftEndColor , const Rgba8& tipStartColor , const Rgba8& tipEndColor , float thickness = 10.f );
	
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

	// Draw AABB2 in WorldSpace with UVs
	void DrawAABB2( const AABB2& box , const Rgba8& tint , const Vec2& uvAtMins , const Vec2& uvAtMaxs );
	
	// Draw Vertically Gradiented AABB2 in WorldSpace
	void DrawAABB2( const AABB2& box, const Rgba8& startTint , const Rgba8& endTint );

	// Draw Vertically Gradiented AABB2 in WorldSpace with UVs
	void DrawAABB2( const AABB2& box , const Rgba8& startTint , const Rgba8& endTint , const Vec2& uvAtMins , const Vec2& uvAtMaxs );
	
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

	// Draw AABB2 with Z in WorldSpace
	void DrawAABB2( const AABB2& box , float z , const Rgba8& tint );


	// Draw Textured Quadrilateral in 3D WorldSpace 
	void DrawQuad3D( Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , AABB2 UVs , Rgba8 startColor , Rgba8 endColor );
	
	// Draw AABB3 in WorldSpace
	//void DrawAABB3( const AABB3& box, const Rgba8& tint);
	
//--------------------------------------------------------------------------------------------------------------------------------------------
	bool		IsDrawing() const;
	void		ReCompileAllShaders();
	bool		HasAnyShaderChangedAtPath( const wchar_t* relativePath , float waitInMilliseconds );

//--------------------------------------------------------------------------------------------------------------------------------------------
	
private:

	void		SetInputLayoutForIA( buffer_attribute_t const* attribs );
	
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
	ID3D11Device*						m_device												= nullptr ;
	ID3D11DeviceContext*				m_context												= nullptr ;					// Immediate context
	SwapChain*							m_swapChain												= nullptr;
	ID3D11DepthStencilState*			m_currentDepthStencilState								= nullptr;

	Camera*								m_currentCamera											= nullptr;

	void*								m_debugModule											= nullptr;
	IDXGIDebug*							m_debug													= nullptr;

	Shader*								m_defaultShader											= nullptr;
	Shader*								m_currentShader											= nullptr;
	VertexBuffer*						m_immediateVBO											= nullptr;
	//IndexBuffer*						m_immediateIBO											= nullptr;
	GPUMesh*							m_immediateMesh											= nullptr;
	ID3D11Buffer*						m_lastBoundVBO											= nullptr;
	ID3D11Buffer*						m_lastBoundIBO											= nullptr;
	Texture*							m_textureTarget											= nullptr;

	ID3D11BlendState*					m_blendStates[eBlendMode::TOTAL_BLEND_MODES];
	ID3D11RasterizerState*				m_rasterStates[eBlendMode::TOTAL_BLEND_MODES];
	ID3D11RasterizerState*				m_currentRasterState;
	ID3D11RasterizerState*				m_transientRaterState;

	RenderBuffer*						m_frameUBO												= nullptr;
	RenderBuffer*						m_modelMatrixUBO										= nullptr;
	RenderBuffer*						m_lightDataUBO											= nullptr;
	RenderBuffer*						m_materialDataUBO										= nullptr;
	Sampler*							m_defaultSampler										= nullptr;
	Texture*							m_textureDefault										= nullptr;
	shaderLightDataT					m_lights;

private:

	std::map<std::string , Texture*>	m_LoadedTextures;									// LOOKUP TABLE OF FILEPATH & TEXTURE
	std::map<std::string , BitmapFont*> m_LoadedBitMapFonts;								// LOOKUP TABLE OF FILEPATH & BITMAPFONT
	std::map<std::string , Shader*>		m_LoadedShaders;									// LOOKUP TABLE OF FILEPATH & SHADERS
};

//--------------------------------------------------------------------------------------------------------------------------------------------