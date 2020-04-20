#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "GameCommon.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Platform/Window.hpp"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "Engine/Experimental/Raytracer/Ray.hpp"
#include "Engine/Experimental/Raytracer/RayHittableList.hpp"
#include "Engine/Experimental/Raytracer/RayHittableSphere.hpp"
#include "Engine/Experimental/Raytracer/RaytracerUtils.hpp"

#include <memory>			// for std::make_shared



#include "Engine/Experimental/Raytracer/LambertianDiffuseMaterial.hpp"
#include "Engine/Experimental/Raytracer/LDM_Metal.hpp"
#include "Engine/Experimental/Raytracer/RaytracerCamera.hpp"

extern TheApp* g_theApp;
extern InputSystem* g_theInput;

Window* g_theWindow = nullptr;


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move each of these items to its proper place, once that place is established
// 
bool g_isQuitting = false;							// ...becomes App::m_isQuitting
const char* APP_NAME = "Protogame2D";				// ...becomes ??? (Change this per project!)

//-----------------------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
//-----------------------------------------------------------------------------------------------

//int WINAPI WinMain( _In_ HINSTANCE applicationInstanceHandle, _In_opt_ HINSTANCE, _In_ LPSTR commandLineString, _In_ int )
//{
//	UNUSED( applicationInstanceHandle );
//	UNUSED( commandLineString );
//	
//	g_theApp = new TheApp();
//
//	g_theWindow = new Window();
//	g_theWindow -> Open( APP_NAME , CLIENT_ASPECT , 0.9f );
//
//	g_theApp -> Startup();
//		
//	while( !g_theWindow->IsQuitting() )			
//	{
//		g_theApp -> RunFrame();					
//	}
//
//	g_theApp -> Shutdown();
//	delete g_theApp;							
//	g_theApp = nullptr;
//
//	g_theWindow -> Close();
//	delete g_theWindow;
//	g_theWindow = nullptr;
//
//	return 0;
//}

#include <iostream>

int main()
{
	//const int imageWidth		= 2160;
	//const int imageHeight		= 1080;
	const int imageWidth		= 200;
	const int imageHeight		= 100;
	const int samplesPerPixel	= 100;
	const int maxDepth			= 50;
	
	std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

	RayHittableList world;
	//world.Add( std::make_shared<RayHittableSphere>( Vec3( 0 , 0 , -1 ) , 0.5 ) );
	//world.Add( std::make_shared<RayHittableSphere>( Vec3( 0 , -100.5 , -1 ) , 100 ) );

	world.Add( std::make_shared<RayHittableSphere>(	Vec3(  0.f , 0.f , -1.f ) , 0.5f		 , std::make_shared<LambertianDiffuse>( Vec3( 0.7 , 0.3 , 0.3 ) ) ) );
	world.Add( std::make_shared<RayHittableSphere>(	Vec3(  0.f , -100.5f , -1.f ) , 100.f , std::make_shared<LambertianDiffuse>( Vec3( 0.8 , 0.8 , 0.0 ) ) ) );
	world.Add( std::make_shared<RayHittableSphere>( Vec3( 1.f , 0.f , -1.f ) , 0.5f , std::make_shared<LDM_Metal>( Vec3( 0.8 , 0.6 , 0.2 ) ) ) );
	world.Add( std::make_shared<RayHittableSphere>( Vec3( -1.f , 0.f , -1.f ) , 0.5f , std::make_shared<LDM_Metal>( Vec3( 0.8 , 0.8 , 0.8 ) ) ) );
	RaytracerCamera camera;
	RandomNumberGenerator rng;
	
	for ( int j = imageHeight - 1; j >= 0; --j )
	{
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for ( int i = 0; i < imageWidth; ++i )
		{
			Vec3 color = Vec3::ZERO;

			for ( int s = 0; s < samplesPerPixel; ++s )
			{
				float u = ( ( float ) i + rng.RollRandomFloatBetweenZerotoOne() ) / imageWidth;
				rng.manuallyIncrementPosition();
				float v = ( ( float ) j + rng.RollRandomFloatBetweenZerotoOne() ) / imageHeight;
				rng.manuallyIncrementPosition();
				
				Ray ray = camera.GetRay( u , v );
				color += RayColor( ray , world , maxDepth );
			}

			color.WriteColor( std::cout , samplesPerPixel );
		}
	}
	
	std::cerr << "\nDone.\n";
}