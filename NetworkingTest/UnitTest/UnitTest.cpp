#include "pch.h"
#include "CppUnitTest.h"

#include <WinSock2.h>
#include "ws2tcpip.h"
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>

//#include "Engine/Networking/UDPSocket.hpp"

#include "Engine/Networking/UDPSocket.hpp"
//#include "Engine/Core/DevConsole.hpp"
//#include "Engine/Renderer/RenderContext.hpp"
//#include "Engine/Input/InputSystem.hpp"
//#include "Engine/Platform/Window.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

//extern RenderContext* g_theRenderer;
//extern InputSystem* g_theInput;
//extern DevConsole* g_theDevConsole;
//extern Window*								g_theWindow;

//--------------------------------------------------------------------------------------------------------------------------------------------
	
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS( UnitTest )
	{
	public:

		TEST_METHOD_INITIALIZE( InitUDPSocketTest )
		{
			WSAData wsaData;
			WORD wVersion MAKEWORD( 2 , 2 );
			int iResult = WSAStartup( wVersion , &wsaData );

			if ( iResult != 0 )
			{
				std::cout << "Call to WSAStartup Failed " << WSAGetLastError() << std::endl;
			}
		}

		TEST_METHOD( UDPSocketTest )
		{
			Logger::WriteMessage( "Starting UDP SOcket Test" );

			UDPSocket test1UDPSocket( "127.0.0.1" , 48000 );
			test1UDPSocket.Bind( 48001 );

			UDPSocket test2UDPSocket( "127.0.0.1" , 48001 );
			test2UDPSocket.Bind( 48000 );
			
			Assert::AreEqual( true , true , L"They match" );
		//	Assert::AreEqual( 5 , 6 , L"They Don't match" );
		}

		TEST_METHOD_CLEANUP( DeInitUDPSocketTest )
		{
			int iResult = WSACleanup();

			if ( iResult == SOCKET_ERROR )
			{
				std::cout << "Call to WSACleanup Failed " << WSAGetLastError() << std::endl;
			}
		}
	};
}
