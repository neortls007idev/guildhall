#pragma once
#include <string>


#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS 
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#include <winsock2.h>
#include "ws2tcpip.h"
#pragma comment(lib, "Ws2_32.lib")


//--------------------------------------------------------------------------------------------------------------------------------------------

class UDPSocket
{
public:
	UDPSocket( const std::string& host , int port );
	~UDPSocket();

	//----------------------------------------------------------------------------------------------------------
	
	void	Bind( int port );
	int		Send();
	int		Receive();
	void	Close();
	
protected:
private:
	sockaddr_in		m_toAddress;
	SOCKET			m_socket;
	
};

//--------------------------------------------------------------------------------------------------------------------------------------------