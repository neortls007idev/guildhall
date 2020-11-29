#pragma once
#include <atlalloc.h>
#include <list>
#include <string>
#include "GameUDPData.hpp"
#include "GameNetworkCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class GameUDPListner;
class GameTCPServer;
class GameTCPClient;

//--------------------------------------------------------------------------------------------------------------------------------------------

class GameNetworkSystem
{
public:
	GameNetworkSystem();
	~GameNetworkSystem();

	void Startup();
	void Shutdown();

	void BeginFrame();
	void EndFrame();

	//----------------------------------------------------------------------------------------------------------
	bool GetIsListening()																		{ return m_isListening; }
	void SetIsListening( bool listen )															{ m_isListening = listen; }

	std::string GetAddress( SOCKET socket );

	void SendUDPMessage( std::string message );
	void StartUDPListner( int bindPort , int sendPort , std::string host /*= "127.0.0.1"*/ );
	
	//----------------------------------------------------------------------------------------------------------

	void		AddNetowrkingCommandsToConsole();
	static bool StartTCPServer( EventArgs& args );
	static bool CloseTCPServer( EventArgs& args );
	static bool SendMessageToClient( EventArgs& args );
	static bool SendMessageToServer( EventArgs& args );
	static bool ConnectToServer( EventArgs& args );
	static bool DisconnectFromServer( EventArgs& args );
	static bool OpenUDPPort( EventArgs& args );
	static bool SendUDPMessage( EventArgs& args );
	static bool SendUDPMessage( GameUDPData updPacket );
	static bool CloseUDPPort( EventArgs& args );
	
public:
	std::vector< std::string >		m_recievedTCPClientMesageBuffer;
	std::vector< std::string >		m_recievedTCPServerMesageBuffer;
	std::vector< std::string >		m_recievedUDPMesageBuffer;

private:
	bool							m_isListening						= false;
	int								m_listenPort						= -1;
	GameTCPServer*					m_TCPServer;
	GameTCPClient*					m_TCPclient[ MAX_CLIENTS ];
	SOCKET							m_linkSocket[ MAX_CLIENTS ];
	static bool						m_wasMessageJustSentByServer;
	static bool						m_wasMessageJustSentByClient;
	GameUDPListner*					m_UDPListner[ 4 ];
};

//--------------------------------------------------------------------------------------------------------------------------------------------