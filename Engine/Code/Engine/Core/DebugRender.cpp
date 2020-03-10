#include "Engine/Core/DebugRender.hpp"

#include "Engine/Renderer/RenderContext.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC RenderContext* g_debugRenderContext	= nullptr;
STATIC Camera*		  g_debugCamera			= nullptr;

void DebugRenderScreenTo( Texture* output )
{
	RenderContext*	ctx = output->GetRenderContext();

	Camera camera;
	camera.SetColorTarget( output );
	/*camera.SetProjectionOrthographic()*/
}
