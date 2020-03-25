#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "VertexUtils.hpp"
#include "Engine/Primitives/GPUMesh.hpp"

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

	g_debugRenderContext->BeginCamera( *cam );

	for ( size_t index = 0; index < g_currentManager->m_debugRenderWorldObjects.size(); index++ )
	{
		if ( g_currentManager->m_debugRenderWorldObjects[ index ] == nullptr )
		{
			continue;
		}
		eDebugRenderObjectType objType = g_currentManager->m_debugRenderWorldObjects[ index ]->m_objectType;

		switch ( objType )
		{
		case DRO_INVALID:
		{
			ASSERT_OR_DIE( true , "INVALID DEBUG OBJECT TYPE" );
		}	break;

		case DRO_POINT3D:
		{
			DRO_point3D* point3D = ( DRO_point3D* ) g_currentManager->m_debugRenderWorldObjects[ index ];

			switch ( point3D->m_renderMode )
			{
				case DEBUG_RENDER_ALWAYS:
				{
					cam->SetClearMode( CLEAR_NONE , BLACK );

					g_debugRenderContext->BeginCamera( *cam );
					cam->CreateMatchingDepthStencilTarget();
					g_theRenderer->BindDepthStencil( cam->GetDepthStencilTarget() );
					std::vector<Vertex_PCU> pointMeshVerts;
					std::vector<uint>		pointIndices;
					CreateUVSphere( 16 , 8 , pointMeshVerts , pointIndices , point3D->m_size , Vec3::ZERO , point3D->m_startColor );
					GPUMesh pointMesh( g_debugRenderContext );
					pointMesh.UpdateVertices( pointMeshVerts );
					pointMesh.UpdateIndices( pointIndices );
					
					g_debugRenderContext->BindShader( nullptr );
					g_debugRenderContext->SetBlendMode( eBlendMode::SOLID );
					g_theRenderer->SetRasterState( FILL_SOLID );
					g_theRenderer->BindTexture( nullptr );
				
					g_debugRenderContext->SetModelMatrix( point3D->m_transform.GetAsMatrix() );
					g_debugRenderContext->DrawMesh( &pointMesh );
					//g_debugRenderContext->DrawDisc( Vec2( point3D->m_position.x , point3D->m_position.y ) , point3D->m_size , *point3D->m_startColor );
					cam->SetClearMode( originalClearMode , originalClearColor );
					g_debugRenderContext->EndCamera( *cam );
				}	break;
			}
		}break;

		case DRO_LINE3D:
			break;

		case DRO_ARROW3D:
			break;

		default:
		{
			ASSERT_RECOVERABLE( true , "ARE YOU SURE YOU WANT TO RENDER A 2D OBJECT IN A PROJECTIVE CAMERA" );
		}
		break;
		}
	}

	//g_debugRenderContext->DrawVertexArray( vertices );
	g_debugRenderContext->EndCamera( *cam );
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

	for ( size_t index = 0 ; index < g_currentManager->m_debugRenderScreenObjects.size() ; index++ )
	{
		if ( nullptr == g_currentManager->m_debugRenderScreenObjects[index] )
		{
			continue;
		}
		eDebugRenderObjectType objType = g_currentManager->m_debugRenderScreenObjects[ index ]->m_objectType;
		
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
	g_currentManager->m_debugRenderWorldObjects.push_back( obj );
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

	for ( size_t index = 0 ; index < m_debugRenderScreenObjects.size() ; index++ )
	{
		if ( m_debugRenderScreenObjects[ index ] != nullptr )
		{
			eDebugRenderObjectType objType = m_debugRenderScreenObjects[ index ]->m_objectType;

			switch ( objType )
			{
			case DRO_INVALID:
			{
				ASSERT_OR_DIE( true ," INCORRECT DEBUG OBJECT TYPE " );
			}break;

			case DRO_POINT2D:
			{
							
			}
				break;
			case DRO_POINT3D:
				break;
			case DRO_LINE2D:
				break;
			case DRO_LINE3D:
				break;
			case DRO_ARROW2D:
				break;
			case DRO_ARROW3D:
				break;
			case DRO_SPHERE:
				break;
			case DRO_SPHERE_WIREFRAME:
				break;
			default:
				break;
			}
		}
	}

	//g_debugRenderContext->DrawVertexArray( vertices );
	g_debugRenderContext->EndCamera( camera );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::CleanupScreenObjects()
{
	for ( size_t index = 0; index < m_debugRenderScreenObjects.size(); index++ )
	{
		if ( m_debugRenderScreenObjects[ index ]->m_isGarbage )
		{
			delete m_debugRenderScreenObjects[ index ];
			m_debugRenderScreenObjects[ index ] = nullptr;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::CleanupWorldObjects()
{
	for ( size_t index = 0; index < m_debugRenderWorldObjects.size(); index++ )
	{
		if ( m_debugRenderWorldObjects[ index ]->m_isGarbage )
		{
			delete m_debugRenderWorldObjects[ index ];
			m_debugRenderWorldObjects[ index ] = nullptr;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------