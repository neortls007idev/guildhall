#pragma once

enum UDPDataPacketType
{
	DATA_ACK ,
	DATA_NACK ,
	TANK_ENTITY_DATA ,
	BULLET_ENTITY_DATA ,
	PLAYER_ENTITY_DATA ,
	EXPLOSION_ENTITY_DATA ,

	NUM_PACKET_DATA ,
};

struct GameUDPData
{
public:
	int					m_frameID;
	UDPDataPacketType	m_packetData;
	
};

//--------------------------------------------------------------------------------------------------------------------------------------------