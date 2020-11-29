#pragma once

enum UDPDataPacketType
{
	INVAlID_PACKET = -1 ,
	PLAYERTANK_ENTITY_DATA ,
	NPCTANK_ENTITY_DATA ,
	NPCTURRET_ENTITY_DATA ,
	NPCBOULDER_ENTITY_DATA ,
	GOOD_BULLET_ENTITY_DATA ,
	EVIL_BULLET_ENTITY_DATA ,
	EXPLOSION_ENTITY_DATA ,

	DATA_ACK ,
	DATA_NACK ,

	NUM_PACKET_DATA ,
};

struct GameUDPData
{
public:
	GameUDPData() {};
	GameUDPData( const GameUDPData &data )
	{
		m_frameID = data.m_frameID;
		m_packetType = data.m_packetType;
	}
public:
	int					m_frameID	= 0;
	UDPDataPacketType	m_packetType = INVAlID_PACKET;
	char				m_data[ 512 - 8 /*sizeof( GameUDPData )*/ ];
	
};

//--------------------------------------------------------------------------------------------------------------------------------------------