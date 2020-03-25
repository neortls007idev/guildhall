#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Primitives/GPUMesh.hpp"
#include "Engine/Core/DebugRenderObject.hpp"
#include "Engine/Time/Timer.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

	   RenderContext*				g_debugRenderContext	= nullptr;
	   Camera*						g_debugCamera			= nullptr;
//static DebugRenderObjectsManager*	g_currentManager		= nullptr;
	   DebugRenderObjectsManager*	g_currentManager        = nullptr;


//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderBeginFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderWorldToCamera( Camera* cam )
{
	uint originalClearMode = cam->GetClearMode();
	Rgba8 originalClearColor = cam->GetClearColor();

	cam->SetClearMode( CLEAR_NONE , BLACK );
	//std::vector< Vertex_PCU > vertices;

	g_debugRenderContext = cam->GetColorTarget()->GetRenderContext();

	if ( g_debugRenderContext == nullptr )
	{
		g_debugRenderContext = g_theRenderer;
	}

	//g_debugRenderContext->BeginCamera( *cam );

	g_currentManager->RenderObjectsAlways( g_currentManager->m_debugRenderWorldObjectsAlways , cam );
	g_currentManager->RenderObjectsAlways( g_currentManager->m_debugRenderScreenObjectsAlways , cam );


	//g_debugRenderContext->DrawVertexArray( vertices );
	//g_debugRenderContext->EndCamera( *cam );
	cam->SetClearMode( originalClearMode , originalClearColor );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderScreenTo( Texture* output )
{
	RenderContext*	ctx = output->GetRenderContext();

	Camera camera;
	camera.SetColorTarget( output );
	/*camera.SetProjectionOrthographic()*/
	g_debugRenderContext = ctx;

	Vec2 min = Vec2::ZERO;
	Vec2 max = Vec2( output->GetDimensions() );

	camera.SetProjectionOrthographic( max.y - min.y , -1.0f , 1.0f );
		// should I clear?
	camera.SetClearMode( CLEAR_NONE , BLACK );

	//std::vector< Vertex_PCU > vertices;

	g_debugRenderContext->BeginCamera( camera );

	for ( size_t index = 0 ; index < g_currentManager->m_debugRenderScreenObjectsAlways.size() ; index++ )
	{
		if ( nullptr == g_currentManager->m_debugRenderScreenObjectsAlways[index] )
		{
			continue;
		}
		eDebugRenderObjectType objType = g_currentManager->m_debugRenderScreenObjectsAlways[ index ]->m_objectType;
		
		switch ( objType )
		{
			case DRO_INVALID:
			{
				ASSERT_OR_DIE( true , "INVALID DEBUG OBJECT TYPE" );
			}	break;

			case DRO_POINT2D:

				break;
					
			case DRO_LINE2D:
				break;
					
			case DRO_ARROW2D:
				break;

			default:
			{
				ASSERT_RECOVERABLE( true , "ARE YOU SURE YOU WANT TO RENDER A 3D OBJECT IN A ORTHOGRAPHIC CAMERA" );
			}
				break;
		}
	}

	//g_debugRenderContext->DrawVertexArray( vertices );
	g_debugRenderContext->EndCamera( camera );
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldPoint( Vec3 pos , float size , Rgba8 color , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DRO_point3D* obj = new DRO_point3D( pos , size , color , duration , mode );
	obj->m_transform.SetPosition( pos );

	switch ( mode )
	{
	case DEBUG_RENDER_ALWAYS:
									g_currentManager->m_debugRenderWorldObjectsAlways.push_back( obj );
									break;
	case DEBUG_RENDER_USE_DEPTH:
									g_currentManager->m_debugRenderWorldObjectsUseDepth.push_back( obj );
									break;
	case DEBUG_RENDER_XRAY:
									g_currentManager->m_debugRenderWorldObjectsUseDepth.push_back( obj );
									break;
	default:
		break;
	}
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldPoint( Vec3 pos , float size , Rgba8 startColor , Rgba8 endColor , float duration , eDebugRenderMode mode )
{
	DRO_point3D* obj = new DRO_point3D( pos , size , startColor , endColor , duration , mode );
	//obj->m_transform.SetPosition( pos );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
	//g_currentManager->m_debugRenderWorldObjects.push_back( obj );
}

void DebugAddWorldLine( Vec3 p0 , Rgba8 p0_start_color , Rgba8 p0_end_color , Vec3 p1 , Rgba8 p1_start_color , Rgba8 p1_end_color , float duration , eDebugRenderMode mode , float radius /*= 1.f */ )
{
	DRO_line3D* obj = new DRO_line3D( p0 , p0_start_color , p0_end_color ,
										p1 , p1_start_color , p1_end_color ,
	                                  duration , mode );
	//obj->m_transform.SetPosition( p0 );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldLine( Vec3 start , Vec3 end , Rgba8 color , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ , float radius /*= 1.f */ )
{
	DRO_line3D* obj = new DRO_line3D( start , end , color , duration , mode , radius );
	//obj->m_transform.SetPosition( p0 );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::Startup()
{
	if ( g_currentManager == nullptr )
	{
		// instantiating a default DRO_Manager
		g_currentManager = new DebugRenderObjectsManager();
		return;
	}
	g_currentManager = this;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::Shutdown()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::BeginFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::Update( float deltaSeconds )
{
// 	UNUSED( deltaSeconds );
// 	for ( size_t index = 0; index < m_debugRenderScreenObjects.size(); index++ )
// 	{
// 		if ( m_debugRenderScreenObjects[ index ] )
// 		{
// 			m_debugRenderScreenObjects[ index ]->Update();
// 		}
// 	}
// 
// 	for ( size_t index = 0; index < m_debugRenderWorldObjects.size(); index++ )
// 	{
// 		if ( m_debugRenderWorldObjects[ index ] )
// 		{
// 			m_debugRenderWorldObjects[ index ]->Update();
// 		}
// 	}

	UpdateDebugObjects( m_debugRenderWorldObjectsAlways		, deltaSeconds );
	UpdateDebugObjects( m_debugRenderWorldObjectsUseDepth		, deltaSeconds );
	UpdateDebugObjects( m_debugRenderWorldObjectsXRay			, deltaSeconds );

	UpdateDebugObjects( m_debugRenderScreenObjectsAlways		, deltaSeconds );
	UpdateDebugObjects( m_debugRenderScreenObjectsUseDepth	, deltaSeconds );
	UpdateDebugObjects( m_debugRenderScreenObjectsXRay		, deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::EndFrame()
{
	CleanupScreenObjects();
	CleanupWorldObjects();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::EnableDebugRendering()
{
	m_isDebugRenderEnabled = true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::DisableDebugRendering()
{
	m_isDebugRenderEnabled = false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::DebugRenderWorldToCamera( Camera* cam )
{
	UNUSED( cam );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::DebugRenderScreenTo( Texture* output )
{
	RenderContext* ctx = output->GetRenderContext();

	Camera camera;
	camera.SetColorTarget( output );
	g_debugRenderContext = ctx;

	Vec2 min = Vec2::ZERO;
	Vec2 max = Vec2( output->GetDimensions() );

	camera.SetProjectionOrthographic( max.y - min.y , -1.0f , 1.0f );
	// should I clear?
	camera.SetClearMode( CLEAR_NONE , BLACK );

	//std::vector< Vertex_PCU > vertices;

	g_debugRenderContext->BeginCamera( camera );

	// 	foreach( object )
	// 	{
	// 		AppendObjectToVertexArray( vertices , indices , obj );
	// 		if ( obj->IsOld() )
	// 		{
	// 			obj->MarkForGarbage();
	// 		}
	// 	}

// 	for ( size_t index = 0 ; index < m_debugRenderScreenObjects.size() ; index++ )
// 	{
// 		if ( m_debugRenderScreenObjects[ index ] != nullptr )
// 		{
// 			eDebugRenderObjectType objType = m_debugRenderScreenObjects[ index ]->m_objectType;
// 
// 			switch ( objType )
// 			{
// 			case DRO_INVALID:
// 			{
// 				ASSERT_OR_DIE( true ," INCORRECT DEBUG OBJECT TYPE " );
// 			}break;
// 
// 			case DRO_POINT2D:
// 			{
// 							
// 			}
// 				break;
// 			case DRO_POINT3D:
// 				break;
// 			case DRO_LINE2D:
// 				break;
// 			case DRO_LINE3D:
// 				break;
// 			case DRO_ARROW2D:
// 				break;
// 			case DRO_ARROW3D:
// 				break;
// 			case DRO_SPHERE:
// 				break;
// 			case DRO_SPHERE_WIREFRAME:
// 				break;
// 			default:
// 				break;
// 			}
// 		}
// 	}

	//g_debugRenderContext->DrawVertexArray( vertices );
	g_debugRenderContext->EndCamera( camera );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::CleanupScreenObjects()
{
	CleanupDebugObjects( m_debugRenderScreenObjectsAlways );
	CleanupDebugObjects( m_debugRenderScreenObjectsUseDepth );
	CleanupDebugObjects( m_debugRenderScreenObjectsXRay );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::CleanupWorldObjects()
{
	CleanupDebugObjects( m_debugRenderWorldObjectsAlways );
	CleanupDebugObjects( m_debugRenderWorldObjectsUseDepth );
	CleanupDebugObjects( m_debugRenderWorldObjectsXRay );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::AddDebugObjectTo( eRenderSpace space , DebugRenderObject* object )
{
	eDebugRenderMode objectRenderMode = object->m_renderMode;
	
	switch ( space )
	{
		case WORLDSPACE:
							{
								switch ( objectRenderMode )
								{
									case DEBUG_RENDER_ALWAYS:
																	g_currentManager->m_debugRenderWorldObjectsAlways.push_back( object );
																	break;
									case DEBUG_RENDER_USE_DEPTH:
																	g_currentManager->m_debugRenderWorldObjectsUseDepth.push_back( object );
																	break;
									case DEBUG_RENDER_XRAY:
																	g_currentManager->m_debugRenderWorldObjectsUseDepth.push_back( object );
																	break;
									default:
									break;
								}
							}break;
		case SCREENSPACE:
							{
								switch ( objectRenderMode )
								{
								case DEBUG_RENDER_ALWAYS:
															g_currentManager->m_debugRenderScreenObjectsAlways.push_back( object );
															break;
								case DEBUG_RENDER_USE_DEPTH:
															g_currentManager->m_debugRenderScreenObjectsUseDepth.push_back( object );
															break;
								case DEBUG_RENDER_XRAY:
															g_currentManager->m_debugRenderScreenObjectsXRay.push_back( object );
															break;
								default:
									break;
								}
							}break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::CleanupDebugObjects( std::vector<DebugRenderObject*>& droArray )
{
	for ( size_t index = 0; index < droArray.size(); index++ )
	{
		if ( droArray[ index ] )
		{
			if ( droArray[ index ]->m_isGarbage )
			{
				delete droArray[ index ];
				droArray[ index ] = nullptr;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::UpdateDebugObjects( std::vector<DebugRenderObject*>& droArray , float deltaSeconds )
{
	UNUSED( deltaSeconds );
	
	for ( size_t index = 0; index < droArray.size(); index++ )
	{
		if ( droArray[ index ] )
		{
			droArray[ index ]->Update();
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::RenderObjectsAlways( std::vector<DebugRenderObject*>& droArray , Camera* cam )
{
	uint originalClearMode = cam->GetClearMode();
	Rgba8 originalClearColor = cam->GetClearColor();
	
	cam->SetClearMode( CLEAR_NONE , BLACK );
	g_debugRenderContext->BeginCamera( *cam );
	cam->CreateMatchingDepthStencilTarget();
	
	for ( size_t index = 0; index < g_currentManager->m_debugRenderWorldObjectsAlways.size(); index++ )
	{
		if ( g_currentManager->m_debugRenderWorldObjectsAlways[ index ] == nullptr )
		{
			continue;
		}

		eDebugRenderObjectType objType = g_currentManager->m_debugRenderWorldObjectsAlways[ index ]->m_objectType;

		switch ( objType )
		{
			case DRO_INVALID:
			{
				ASSERT_OR_DIE( true , "INVALID DEBUG OBJECT TYPE" );
			}	break;

			case DRO_POINT3D:
			{
				DRO_point3D* point3D = ( DRO_point3D* ) g_currentManager->m_debugRenderWorldObjectsAlways[ index ];

				g_theRenderer->BindDepthStencil( cam->GetDepthStencilTarget() );
				std::vector<Vertex_PCU> pointMeshVerts;
				std::vector<uint>		pointIndices;
				CreateUVSphere( 16 , 8 , pointMeshVerts , pointIndices , point3D->m_size , Vec3::ZERO , point3D->m_currrentColor );
				GPUMesh pointMesh( g_debugRenderContext );
				pointMesh.UpdateVertices( pointMeshVerts );
				pointMesh.UpdateIndices( pointIndices );

				g_debugRenderContext->BindShader( nullptr );
				g_debugRenderContext->SetBlendMode( eBlendMode::SOLID );
				g_theRenderer->SetRasterState( FILL_SOLID );
				g_theRenderer->BindTexture( nullptr );

				g_debugRenderContext->SetModelMatrix( point3D->m_transform.GetAsMatrix() );
				g_debugRenderContext->DrawMesh( &pointMesh );
				
			}break;

			case DRO_LINE3D:
			{
				DRO_line3D* line3D = ( DRO_line3D* ) g_currentManager->m_debugRenderWorldObjectsAlways[ index ];

				g_theRenderer->BindDepthStencil( cam->GetDepthStencilTarget() );
				std::vector<Vertex_PCU> line3DMeshVerts;
				std::vector<uint>		line3DIndices;
				CreateCylinder( line3DMeshVerts , line3DIndices , line3D->m_radius , line3D->m_startPos ,
				                line3D->m_endPos , line3D->m_startPosCurrentColor , line3D->m_endPosCurrentColor );
					
				GPUMesh line3DMesh( g_debugRenderContext );
				line3DMesh.UpdateVertices( line3DMeshVerts );
				line3DMesh.UpdateIndices( line3DIndices );

				g_debugRenderContext->BindShader( nullptr );
				g_debugRenderContext->SetBlendMode( eBlendMode::SOLID );
				g_theRenderer->SetRasterState( FILL_SOLID );
				g_theRenderer->BindTexture( nullptr );

				g_debugRenderContext->SetModelMatrix( line3D->m_transform.GetAsMatrix() );
				g_debugRenderContext->DrawMesh( &line3DMesh );

			}break;

			case DRO_ARROW3D:
				break;

			default:
			{
				ASSERT_RECOVERABLE( true , "ARE YOU SURE YOU WANT TO RENDER A 2D OBJECT IN A PROJECTIVE CAMERA" );
			}
			break;
		}
	}
	cam->SetClearMode( originalClearMode , originalClearColor );
	g_debugRenderContext->EndCamera( *cam );
}

//--------------------------------------------------------------------------------------------------------------------------------------------