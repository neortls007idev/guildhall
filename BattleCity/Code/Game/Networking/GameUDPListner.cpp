#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Networking/GameNetworkSystem.hpp"
#include "Game/Networking/GameUDPData.hpp"
#include "Game/Networking/GameUDPListner.hpp"
#include "Game/Networking/GameUDPSocket.hpp"
#include <thread>

//--------------------------------------------------------------------------------------------------------------------------------------------

extern	DevConsole* g_theDevConsole;
extern GameNetworkSystem* g_theGameNetworkSys;

//--------------------------------------------------------------------------------------------------------------------------------------------
	
GameUDPListner::GameUDPListner()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameUDPListner::~GameUDPListner()
{
	if ( readerThread != nullptr )
	{
		readerThread->join();
	}
	if ( writerThread != nullptr )
	{
		writerThread->join();
	}

	SAFE_RELEASE_POINTER( readerThread );
	SAFE_RELEASE_POINTER( writerThread );
	SAFE_RELEASE_POINTER( m_listenSocket );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
	
void GameUDPListner::StartSocket( int bindPort , int sendPort , std::string host /*= "127.0.0.1" */ )
{
	m_listenSocket = new GameUDPSocket( host , sendPort );
	m_listenSocket->Bind( bindPort );

	 readerThread = new std::thread( &GameUDPListner::Writer , this , std::ref(*m_listenSocket), std::ref( m_writeQueue ) );
	 writerThread = new std::thread( &GameUDPListner::Reader, this , std::ref(*m_listenSocket), std::ref( m_readQueue ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

//void GameUDPListner::AddMessage( std::string& message )
//{
//	m_writeQueue.push( message );
//}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameUDPListner::AddMessage( GameUDPData& udpPacket )
{
	m_writeQueue.push( udpPacket );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

//void GameUDPListner::Reader( GameUDPSocket& socket , SynchronizedLockFreeQueue<std::string>& readQueue )
//{
//	while ( socket.IsValid() )
//	{
//		UDPMessageHeader const* pMsg = nullptr;
//		if ( socket.Receive() > 0 )
//		{
//			auto& buffer = socket.ReceiveBuffer();
//			pMsg = reinterpret_cast< UDPMessageHeader const* > ( &buffer[ 0 ] );
//			std::string receivedMessage = &buffer[ sizeof( UDPMessageHeader ) ];
//			//LOG_SYSMESSAGE( "UDP Received Message: %s" , receivedMessage.c_str() );
//			readQueue.push( receivedMessage );
//
//			if( receivedMessage != "" )
//			{
//				bool wasMessageAddedToBuffer = false;
//				for( auto index : g_theGameNetworkSys->m_recievedUDPMesageBuffer )
//				{
//					if( index == "" )
//					{
//						g_theGameNetworkSys->m_recievedUDPMesageBuffer.emplace_back( receivedMessage );
//						wasMessageAddedToBuffer = true;
//						break;
//					}
//				}
//				if( !wasMessageAddedToBuffer )
//				{
//					g_theGameNetworkSys->m_recievedUDPMesageBuffer.emplace_back( receivedMessage );
//				}
//			}
//		}
//	}
//}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameUDPListner::Reader( GameUDPSocket& socket , SynchronizedLockFreeQueue<GameUDPData>& readQueue )
{
	while ( socket.IsValid() )
	{
		//GameUDPData const* pMsg = nullptr;
		if ( socket.Receive() > 0 )
		{
			auto& buffer = socket.ReceiveBuffer();
			//pMsg = reinterpret_cast< GameUDPData const* > ( &buffer[ 0 ] );
			GameUDPData const* receivedMessage = reinterpret_cast< GameUDPData const* > ( &buffer[ 0 ] );
			//LOG_SYSMESSAGE( "UDP Received Message: %s" , receivedMessage.c_str() );
			readQueue.push( *receivedMessage );

			if ( receivedMessage != nullptr )
			{
				bool wasMessageAddedToBuffer = false;
				for ( auto index : g_theGameNetworkSys->m_recievedUDPMesageBuffer )
				{
					if ( index.m_packetType != INVAlID_PACKET )
					{
						g_theGameNetworkSys->m_recievedUDPMesageBuffer.emplace_back( receivedMessage );
						wasMessageAddedToBuffer = true;
						break;
					}
				}
				if ( !wasMessageAddedToBuffer )
				{
					g_theGameNetworkSys->m_recievedUDPMesageBuffer.emplace_back( receivedMessage );
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------
	
//void GameUDPListner::Writer( GameUDPSocket& socket , SynchronizedLockFreeQueue<std::string>& writeQueue )
//{
//	UDPMessageHeader header;
//	while ( socket.IsValid() )
//	{
//		std::string message = writeQueue.pop();
//		header.id = 1;
//		header.length = ( uint16_t ) strlen( message.c_str() );
//		header.sequenceNo = 1;
//		if ( message != "" )
//		{
//			auto& buffer = socket.SendBuffer();
//
//			*reinterpret_cast< UDPMessageHeader* >( &buffer[ 0 ] ) = header;
//			memcpy( &( socket.SendBuffer()[ sizeof( UDPMessageHeader ) ] ) , message.c_str() , header.length );
//			socket.SendBuffer()[ sizeof( UDPMessageHeader ) + header.length ] = NULL;
//			m_listenSocket->Send( sizeof( UDPMessageHeader ) + ( int ) message.length() + 1 );
//		}
//	}
//}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameUDPListner::Writer( GameUDPSocket& socket , SynchronizedLockFreeQueue<GameUDPData>& writeQueue )
{
	//UDPMessageHeader header;
	while ( socket.IsValid() )
	{
		GameUDPData message = writeQueue.pop();
		if ( message.m_packetType != INVAlID_PACKET )
		{
			auto& buffer = socket.SendBuffer();

			memcpy( &( socket.SendBuffer()[ sizeof( GameUDPData ) ] ) , &message , sizeof( GameUDPData ) );
			socket.SendBuffer()[ sizeof( GameUDPData ) + 1 ] = NULL;
			m_listenSocket->Send( sizeof( GameUDPData ) + 1 );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------