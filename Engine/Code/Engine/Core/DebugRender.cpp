#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Primitives/GPUMesh.hpp"
#include "Engine/Core/DebugRenderObject.hpp"
#include "Engine/Time/Timer.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*				g_theRenderer;

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

	g_debugCamera->SetProjectionMatrix( cam->GetProjectionMatrix() );
	g_debugCamera->SetCameraTransform( cam->GetCameraTransform() );
	g_debugCamera->SetDepthStencilTargetFromAnotherCamera( cam );
	g_debugCamera->SetClearMode( originalClearMode , originalClearColor );
	//std::vector< Vertex_PCU > vertices;

	g_debugRenderContext = cam->GetColorTarget()->GetRenderContext();

	if ( g_debugRenderContext == nullptr )
	{
		g_debugRenderContext = g_theRenderer;
	}

	//g_debugRenderContext->BeginCamera( *cam );
	
	g_debugRenderContext->SetBlendMode( eBlendMode::SOLID );
	
	g_currentManager->RenderObjectsUseDepth( g_currentManager->m_debugRenderWorldObjectsUseDepth ,g_debugCamera , eBlendMode::SOLID );
	g_currentManager->RenderObjectsXRAY( g_currentManager->m_debugRenderWorldObjectsXRay , g_debugCamera , eBlendMode::SOLID );
	g_currentManager->RenderObjectsAlways( g_currentManager->m_debugRenderWorldObjectsAlways , g_debugCamera , eBlendMode::SOLID );
	g_currentManager->RenderObjectsAlways( g_currentManager->m_debugRenderScreenObjects		, g_debugCamera , eBlendMode::ALPHA );

	//g_debugRenderContext->DrawVertexArray( vertices );
	//g_debugRenderContext->EndCamera( *cam );
	g_debugCamera->ResetDepthStencilTarget();
	cam->SetClearMode( originalClearMode , originalClearColor );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderScreenTo( Texture* output )
{
	Camera camera;

	if ( output )
	{
		RenderContext* ctx = output->GetRenderContext();
		g_debugRenderContext = ctx;
		camera.SetColorTarget( output );
		camera.SetOrthoView( Vec2::ZERO , Vec2( output->GetDimensions() ) );
	}
	else
	{
		g_debugRenderContext = g_theRenderer;
		camera.SetOrthoView( Vec2::ZERO , Vec2( ( 16.f / 9.f ) * 1080.f , 1080.f ) );
	}

/*camera.SetProjectionOrthographic()*/

// 	Vec2 min = Vec2::ZERO;
// 	Vec2 max = Vec2( output->GetDimensions() );
// 
// 	camera.SetProjectionOrthographic( max.y - min.y , -1.0f , 1.0f );
		// should I clear?
	camera.SetClearMode( CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , BLACK );

	g_currentManager->RenderObjectsUseDepth( g_currentManager->m_debugRenderWorldObjectsUseDepth , &camera , eBlendMode::ALPHA );
	g_currentManager->RenderObjectsXRAY( g_currentManager->m_debugRenderWorldObjectsXRay			, &camera , eBlendMode::ALPHA );
	g_currentManager->RenderObjectsAlways( g_currentManager->m_debugRenderWorldObjectsAlways		, &camera , eBlendMode::ALPHA );
	g_currentManager->RenderObjectsAlways( g_currentManager->m_debugRenderScreenObjects			, &camera , eBlendMode::ALPHA );

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
	obj->m_transform.SetPosition( pos );

	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldLine ( Vec3 startPos , Rgba8 startPosStartColor , Rgba8 startPosEndColor , Vec3 endPos ,
                         Rgba8 endPosStartColor , Rgba8 endPosEndColor , float duration , eDebugRenderMode mode ,
                         float radius /*= 1.f */ )
{
	DRO_line3D* obj = new DRO_line3D( startPos , startPosStartColor , startPosEndColor ,
										endPos , endPosStartColor , endPosEndColor ,
	                                  duration , mode , radius );

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

void DebugAddWorldArrow ( Vec3 startPos , Vec3 endPos , Rgba8 shaftStartPosStartColor , Rgba8 shaftStartPosEndColor ,
                          Rgba8 shaftEndPosStartColor , Rgba8 shaftEndPosEndColor , Rgba8 tipStartPosStartColor ,
                          Rgba8 tipStartPosEndColor , Rgba8 tipEndPosStartColor , Rgba8 tipEndPosEndColor ,
                          float duration /*= 0.f */ , eDebugRenderMode mode /*= DEBUG_RENDER_ALWAYS */ ,
                          float shaftRadius /*= 0.9f */ , float tipRadius /*= 1.f */ )
{
	DRO_arrow3D* obj = new DRO_arrow3D( startPos , endPos , shaftStartPosStartColor , shaftStartPosEndColor ,
	                                    shaftEndPosStartColor , shaftEndPosEndColor , tipStartPosStartColor ,
	                                    tipStartPosEndColor , tipEndPosStartColor , tipEndPosEndColor , duration ,
	                                    mode , shaftRadius , tipRadius );
	
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldArrow( Vec3 start , Vec3 end , Rgba8 color , float duration , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DRO_arrow3D* obj = new DRO_arrow3D( start , end , color , duration , mode );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldArrow ( Vec3 startPos , Rgba8 startPosStartColor , Rgba8 startPosEndColor , Vec3 endPos ,
                          Rgba8 endPosStartColor , Rgba8 endPosEndColor , float duration /*= 0.f */ ,
                          eDebugRenderMode mode /*= DEBUG_RENDER_ALWAYS */ , float radius /*= 1.f */ )
{
	DRO_arrow3D* obj = new DRO_arrow3D( startPos , startPosStartColor , startPosEndColor , 
										  endPos   , endPosStartColor	 ,     endPosEndColor ,
												duration , mode , radius );
	
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldWireBounds( AABB3 bounds , Rgba8 color , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DRO_aabb3* obj = new DRO_aabb3( bounds , color , duration , mode , eRasterState::WIREFRAME );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldWireSphere( Vec3 pos , float radius , Rgba8 startColor , Rgba8 endColor , float duration , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DRO_uvSphere* obj = new DRO_uvSphere( pos , radius , startColor , endColor , duration , mode , eRasterState::WIREFRAME );
	//obj->m_transform.SetPosition( pos );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldWireSphere( Vec3 pos , float radius , Rgba8 color , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DRO_uvSphere* obj = new DRO_uvSphere( pos , radius , color , duration , mode , eRasterState::WIREFRAME );
	//obj->m_transform.SetPosition( pos );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldBasis( Mat44 basis , Rgba8 startTint , Rgba8 endTint , float duration , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldBasis( Mat44 basis , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenPoint( Vec2 pos , float size , Rgba8 startColor , Rgba8 endColor , float duration )
{
	DRO_point2D* obj = new DRO_point2D( pos , size , startColor , endColor , duration );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenPoint( Vec2 pos , float size , Rgba8 color , float duration /*= 0.0f */ )
{
	DRO_point2D* obj = new DRO_point2D( pos , size , color , color , duration );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenPoint( Vec2 pos , Rgba8 color )
{
	DRO_point2D* obj = new DRO_point2D( pos , 10.f , color , color , 5.f );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenLine ( Vec2 startPos , Rgba8 startPosStartColor , Rgba8 startPosEndColor , Vec2 endPos ,
                          Rgba8 endPosStartColor , Rgba8 endPosEndColor , float duration )
{
	DRO_line2D* obj = new DRO_line2D( startPos , startPosStartColor , startPosEndColor , endPos , endPosStartColor , endPosEndColor , duration , 10.f );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenLine( Vec2 startPos , Vec2 endPos , Rgba8 color , float duration /*= 0.0f */ )
{
	DRO_line2D* obj = new DRO_line2D( startPos , endPos , color , duration , 10.f );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenArrow ( Vec2 startPos , Vec2 endPos , Rgba8 shaftStartPosStartColor , Rgba8 shaftStartPosEndColor ,
                           Rgba8 shaftEndPosStartColor , Rgba8 shaftEndPosEndColor , Rgba8 tipStartPosStartColor ,
                           Rgba8 tipStartPosEndColor , Rgba8 tipEndPosStartColor , Rgba8 tipEndPosEndColor ,
                           float duration /*= 0.f */ , float thickness /*= 10.f */ )
{
	DRO_arrow2D* obj = new DRO_arrow2D( startPos , endPos , shaftStartPosStartColor , shaftStartPosEndColor ,
	                                    shaftEndPosStartColor , shaftEndPosEndColor , tipStartPosStartColor ,
	                                    tipStartPosEndColor , tipEndPosStartColor , tipEndPosEndColor , duration ,
	                                    thickness );

	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenArrow ( Vec2 startPos , Rgba8 startPosStartColor , Rgba8 startPosEndColor , Vec2 endPos ,
                           Rgba8 endPosStartColor , Rgba8 endPosEndColor , float duration /*= 0.f */ ,
                           float thickness /*= 10.f */ )
{
	DRO_arrow2D* obj = new DRO_arrow2D( startPos , startPosStartColor , startPosEndColor , endPos , endPosStartColor ,
	                                    endPosEndColor , duration , thickness );

	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenArrow( Vec2 startPos , Vec2 endPos , Rgba8 color , float duration /*= 0.0f */ , float thickness /*= 10.f */ )
{
	DRO_arrow2D* obj = new DRO_arrow2D( startPos , endPos , color , duration , thickness );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
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

	if ( g_debugCamera == nullptr )
	{
		g_debugCamera = new Camera();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::Shutdown()
{
 	delete g_debugCamera;
 	g_debugCamera = nullptr;
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

	UpdateDebugObjects( m_debugRenderScreenObjects			, deltaSeconds );
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
	CleanupDebugObjects( m_debugRenderScreenObjects );
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
																	g_currentManager->m_debugRenderWorldObjectsXRay.push_back( object );
																	break;
									default:
									break;
								}
							}break;
		case SCREENSPACE:
							{
								g_currentManager->m_debugRenderScreenObjects.push_back( object );
				
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

void DebugRenderObjectsManager::RenderObjectsAlways( std::vector<DebugRenderObject*>& droArray , Camera* cam , eBlendMode blendMode )
{
	uint originalClearMode = cam->GetClearMode();
	Rgba8 originalClearColor = cam->GetClearColor();
	
	cam->SetClearMode( CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , BLACK );
	g_debugRenderContext->BeginCamera( *cam );
	g_debugRenderContext->SetBlendMode( blendMode );
	//cam->CreateMatchingDepthStencilTarget();
	
	RenderObjectArray( droArray , cam );

	cam->SetClearMode( originalClearMode , originalClearColor );
	g_debugRenderContext->EndCamera( *cam );
}
//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::RenderObjectsUseDepth( std::vector<DebugRenderObject*>& droArray , Camera* cam , eBlendMode blendMode )
{
	uint originalClearMode = cam->GetClearMode();
	Rgba8 originalClearColor = cam->GetClearColor();

	cam->SetClearMode( CLEAR_NONE , BLACK );
	g_debugRenderContext->BeginCamera( *cam );
	g_debugRenderContext->SetBlendMode( blendMode );
	//cam->CreateMatchingDepthStencilTarget();

	RenderObjectArray( droArray , cam );

	cam->SetClearMode( originalClearMode , originalClearColor );
	g_debugRenderContext->EndCamera( *cam );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::RenderObjectsXRAY( std::vector<DebugRenderObject*>& droArray , Camera* cam , eBlendMode blendMode )
{
	uint originalClearMode = cam->GetClearMode();
	Rgba8 originalClearColor = cam->GetClearColor();

	cam->SetClearMode( CLEAR_NONE , BLACK );
	g_debugRenderContext->BeginCamera( *cam );
	g_debugRenderContext->SetBlendMode( blendMode );
	g_debugRenderContext->SetDepthTest( COMPARE_LESS , false );
	
	RenderObjectArray( droArray , cam );

	g_debugRenderContext->EndCamera( *cam );

	//cam->SetClearMode( CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , BLACK );
	g_debugRenderContext->BeginCamera( *cam );
	g_debugRenderContext->SetBlendMode( blendMode );
	g_debugRenderContext->SetDepthTest( COMPARE_GREATER , false );

	RenderObjectArrayXRAYPass2( droArray , cam );


	//cam->SetClearMode( originalClearMode , originalClearColor );
	//g_debugRenderContext->BeginCamera( *cam );
	//g_debugRenderContext->EndCamera( *cam );


	
	cam->SetClearMode( originalClearMode , originalClearColor );
	g_debugRenderContext->EndCamera( *cam );
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::RenderObjectArray( std::vector<DebugRenderObject*>& droArray , Camera* cam )
{
	
	for ( size_t index = 0; index < droArray.size(); index++ )
	{
		if ( droArray[ index ] == nullptr )
		{
			continue;
		}
		g_debugRenderContext->BindShader( nullptr );
		g_debugRenderContext->SetRasterState( FILL_SOLID );
		g_debugRenderContext->BindTexture( nullptr );
		
		eDebugRenderObjectType objType = droArray[ index ]->m_objectType;

		switch ( objType )
		{
		case DRO_INVALID:
		{
			ASSERT_OR_DIE( true , "INVALID DEBUG OBJECT TYPE" );
		}	break;

		case DRO_POINT2D:
		{
			DRO_point2D* point2D = ( DRO_point2D* ) droArray[ index ];

			g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
			g_debugRenderContext->SetModelMatrix( point2D->m_transform.GetAsMatrix() );
			g_debugRenderContext->DrawDisc( point2D->m_position , point2D->m_size , point2D->m_currrentColor );

		}break;
			
		case DRO_POINT3D:
		{
			DRO_point3D* point3D = ( DRO_point3D* ) droArray[ index ];

			g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
			std::vector<Vertex_PCU> pointMeshVerts;
			std::vector<uint>		pointIndices;
			CreateUVSphere( 16 , 8 , pointMeshVerts , pointIndices , point3D->m_size , Vec3::ZERO , point3D->m_currrentColor );
			GPUMesh pointMesh( g_debugRenderContext );
			pointMesh.UpdateVertices( pointMeshVerts );
			pointMesh.UpdateIndices( pointIndices );

			g_debugRenderContext->SetModelMatrix( point3D->m_transform.GetAsMatrix() );
			g_debugRenderContext->DrawMesh( &pointMesh );

		}break;

		case DRO_LINE3D:
		{
			DRO_line3D* line3D = ( DRO_line3D* ) droArray[ index ];

			g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
			std::vector<Vertex_PCU> line3DMeshVerts;
			std::vector<uint>		line3DIndices;
			CreateCylinder( line3DMeshVerts , line3DIndices , line3D->m_radius , line3D->m_startPos ,
				line3D->m_endPos , line3D->m_startPosCurrentColor , line3D->m_endPosCurrentColor );

			GPUMesh line3DMesh( g_debugRenderContext );
			line3DMesh.UpdateVertices( line3DMeshVerts );
			line3DMesh.UpdateIndices( line3DIndices );

			g_debugRenderContext->SetModelMatrix( line3D->m_transform.GetAsMatrix() );
			g_debugRenderContext->DrawMesh( &line3DMesh );

		}break;

		case DRO_ARROW3D:
		{
			DRO_arrow3D* arrow3D = ( DRO_arrow3D* ) droArray[ index ];

			g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
			std::vector<Vertex_PCU> arrow3DMeshVerts;
			std::vector<uint>		arrow3DIndices;
			CreateArrow3D( arrow3DMeshVerts , arrow3DIndices , arrow3D->m_shaftRadius , arrow3D->m_tipRadius ,
				arrow3D->m_startPos , arrow3D->m_endPos ,
				arrow3D->m_shaftStartPosCurrentColor , arrow3D->m_shaftEndPosCurrentColor ,
				arrow3D->m_tipStartPosCurrentColor , arrow3D->m_tipEndPosCurrentColor );

			GPUMesh arrow3DMesh( g_debugRenderContext );
			arrow3DMesh.UpdateVertices( arrow3DMeshVerts );
			arrow3DMesh.UpdateIndices( arrow3DIndices );

			g_debugRenderContext->SetModelMatrix( arrow3D->m_transform.GetAsMatrix() );
			g_debugRenderContext->DrawMesh( &arrow3DMesh );
				
		}break;

		case DRO_AABB3:
		{
			DRO_aabb3* box = ( DRO_aabb3* ) droArray[ index ];

			g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
			std::vector<Vertex_PCU> boxMeshVerts;
			std::vector<uint>		boxIndices;
			CreateCuboid( boxMeshVerts , boxIndices , box->m_AABB3 , box->m_currrentColor );

			GPUMesh boxMesh( g_debugRenderContext );
			boxMesh.UpdateVertices( boxMeshVerts );
			boxMesh.UpdateIndices( boxIndices );

			g_debugRenderContext->SetRasterState( box->m_rasterState );
			g_debugRenderContext->SetModelMatrix( box->m_transform.GetAsMatrix() );
			g_debugRenderContext->DrawMesh( &boxMesh );
				
		}break;

		case DRO_UVSPHERE:
		{
			DRO_uvSphere* uvSphere = ( DRO_uvSphere* ) droArray[ index ];

			g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
			std::vector<Vertex_PCU> uvSphereMeshVerts;
			std::vector<uint>		uvSphereIndices;
			CreateUVSphere( 8,4 , uvSphereMeshVerts , uvSphereIndices , uvSphere->m_radius , uvSphere->m_position , uvSphere->m_currrentColor );
			
			GPUMesh uvSphereMesh( g_debugRenderContext );
			uvSphereMesh.UpdateVertices( uvSphereMeshVerts );
			uvSphereMesh.UpdateIndices( uvSphereIndices );

			g_debugRenderContext->SetRasterState( uvSphere->m_rasterState );
			g_debugRenderContext->SetModelMatrix( uvSphere->m_transform.GetAsMatrix() );
			g_debugRenderContext->DrawMesh( &uvSphereMesh );

		}break;
			
		default:
		{
			ASSERT_RECOVERABLE( true , "ARE YOU SURE YOU WANT TO RENDER A 2D OBJECT IN A PROJECTIVE CAMERA" );
		}
		break;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::RenderObjectArrayXRAYPass2( std::vector<DebugRenderObject*>& droArray , Camera* cam )
{
	for ( size_t index = 0; index < droArray.size(); index++ )
	{
		if ( droArray[ index ] == nullptr )
		{
			continue;
		}

		eDebugRenderObjectType objType = droArray[ index ]->m_objectType;

		switch ( objType )
		{
		case DRO_INVALID:
		{
			ASSERT_OR_DIE( true , "INVALID DEBUG OBJECT TYPE" );
		}	break;

		case DRO_POINT3D:
		{
			DRO_point3D* point3D = ( DRO_point3D* ) droArray[ index ];
			Rgba8 pointColorCopy = point3D->m_currrentColor;

			pointColorCopy.r *= 0.5f;
			pointColorCopy.g *= 0.5f;
			pointColorCopy.b *= 0.5f;
			pointColorCopy.a *= 0.5f;
				
			g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
			std::vector<Vertex_PCU> pointMeshVerts;
			std::vector<uint>		pointIndices;
			CreateUVSphere( 16 , 8 , pointMeshVerts , pointIndices , point3D->m_size , Vec3::ZERO , pointColorCopy );
			GPUMesh pointMesh( g_debugRenderContext );
			pointMesh.UpdateVertices( pointMeshVerts );
			pointMesh.UpdateIndices( pointIndices );

			g_debugRenderContext->BindShader( nullptr );
			g_debugRenderContext->SetBlendMode( eBlendMode::ALPHA );
			g_debugRenderContext->BindTexture( nullptr );

			g_debugRenderContext->SetModelMatrix( point3D->m_transform.GetAsMatrix() );
			g_debugRenderContext->DrawMesh( &pointMesh );

		}break;

		case DRO_LINE3D:
		{
			DRO_line3D* line3D = ( DRO_line3D* ) droArray[ index ];
			Rgba8 lineStartColorCopy = line3D->m_startPosCurrentColor;
			lineStartColorCopy.a *= 0.5f;

			Rgba8 lineEndColorCopy = line3D->m_endPosCurrentColor;
			lineEndColorCopy.a *= 0.5f;
				
			g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
			std::vector<Vertex_PCU> line3DMeshVerts;
			std::vector<uint>		line3DIndices;
			CreateCylinder( line3DMeshVerts , line3DIndices , line3D->m_radius , line3D->m_startPos ,
				line3D->m_endPos , lineStartColorCopy , lineEndColorCopy );

			GPUMesh line3DMesh( g_debugRenderContext );
			line3DMesh.UpdateVertices( line3DMeshVerts );
			line3DMesh.UpdateIndices( line3DIndices );

			g_debugRenderContext->BindShader( nullptr );
			g_debugRenderContext->SetBlendMode( eBlendMode::ALPHA );
			//g_debugRenderContext->SetRasterState( FILL_SOLID );
			g_debugRenderContext->BindTexture( nullptr );

			g_debugRenderContext->SetModelMatrix( line3D->m_transform.GetAsMatrix() );
			g_debugRenderContext->DrawMesh( &line3DMesh );

		}break;

		case DRO_ARROW3D:
		{
			DRO_arrow3D* arrow3D = ( DRO_arrow3D* ) droArray[ index ];
			Rgba8 shaftStartColorCopy = arrow3D->m_shaftStartPosCurrentColor;
			shaftStartColorCopy.a *= 0.5f;

			Rgba8 shaftEndColorCopy = arrow3D->m_shaftEndPosCurrentColor;
			shaftEndColorCopy.a *= 0.5f;

			Rgba8 tipStartColorCopy = arrow3D->m_tipStartPosCurrentColor;
			tipStartColorCopy.a *= 0.5f;

			Rgba8 tipEndColorCopy = arrow3D->m_tipEndPosCurrentColor;
			tipEndColorCopy.a *= 0.5f;
				
			g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
			std::vector<Vertex_PCU> arrow3DMeshVerts;
			std::vector<uint>		arrow3DIndices;
			CreateArrow3D( arrow3DMeshVerts , arrow3DIndices , arrow3D->m_shaftRadius , arrow3D->m_tipRadius ,
				arrow3D->m_startPos , arrow3D->m_endPos ,
				shaftStartColorCopy , shaftEndColorCopy ,
				tipStartColorCopy , tipEndColorCopy );

			GPUMesh arrow3DMesh( g_debugRenderContext );
			arrow3DMesh.UpdateVertices( arrow3DMeshVerts );
			arrow3DMesh.UpdateIndices( arrow3DIndices );

			g_debugRenderContext->BindShader( nullptr );
			g_debugRenderContext->SetBlendMode( eBlendMode::ALPHA );
			//g_debugRenderContext->SetRasterState( FILL_SOLID );
			g_debugRenderContext->BindTexture( nullptr );

			g_debugRenderContext->SetModelMatrix( arrow3D->m_transform.GetAsMatrix() );
			g_debugRenderContext->DrawMesh( &arrow3DMesh );
		}
		break;

		default:
		{
			ASSERT_RECOVERABLE( true , "ARE YOU SURE YOU WANT TO RENDER A 2D OBJECT IN A PROJECTIVE CAMERA" );
		}
		break;
		}
	}

	g_debugRenderContext->BindShader( nullptr );
	g_debugRenderContext->SetBlendMode( eBlendMode::SOLID );
	g_debugRenderContext->SetRasterState( FILL_SOLID );
	g_debugRenderContext->BindTexture( nullptr );
}

//--------------------------------------------------------------------------------------------------------------------------------------------