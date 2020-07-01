﻿#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game/BillBoard.hpp"
#include <string>

class SpriteSheet;

//--------------------------------------------------------------------------------------------------------------------------------------------

class EntityDef
{
public:
	static EntityDef* GetDefinition( std::string const& entityDefName );

public:
	std::string		m_className				= "";
	std::string		m_entityName			= "";
	float			m_physicsRadius			= 0.f;
	float			m_walkSpeed				= 0.f;
	float			m_height				= 0.f;
	float			m_eyeHeight				= 0.f;

	Vec2			m_cosmeticBounds		= Vec2::ZERO;
	eBillBoardType	m_billBoardType;
	SpriteSheet*	m_spriteSheet			= nullptr;
	IntVec2			m_spriteSheetLayout		= IntVec2::ZERO;
	/*
	<Actor name = "Marine">
		<Physics radius = "0.18" height = "0.62" eyeHeight = "0.58" walkSpeed = "1.0" / >
		<Appearance size = "1.5,1.5" billboard = "CameraFacingXY" spriteSheet = "Data/Images/Actor_Marine_7x12.png" layout = "7,12">
		<Walk front = "0" left = "14" back = "28" right = "42" / >
		<Attack front = "0" / >
		<Pain front = "0" / >
		<Death front = "0" / >
		< / Appearance>
		< / Actor>

		<Actor name = "Pinky">
		<Physics radius = "0.22" height = "0.56" eyeHeight = "0.40" walkSpeed = "1.5" / >
		<Appearance size = "1.5,1.5" billboard = "CameraFacingXY" spriteSheet = "Data/Images/Actor_Pinky_8x9.png" layout = "8,9">
		<Walk front = "0,1,2,3" frontLeft = "8,9,10,11" left = "16,17,18,19" backLeft = "24,25,26,27" back = "32,33,34,35" backRight = "40,41,42,43" right = "48,49,50,51" frontRight = "56,57,58,59" / >
		<Attack front = "4,5,6" frontLeft = "12,13,14" left = "20,21,22" backLeft = "28,29,30" back = "36,37,38" backRight = "44,45,46" right = "52,53,54" frontRight = "60,61,62" / >
		<Pain front = "7" frontLeft = "15" left = "23" backLeft = "31" back = "39" backRight = "47" right = "55" frontRight = "63" / >
		<Death front = "64,65,66,67,68,69" / >
		< / Appearance>
		< / Actor>

		<Actor name = "Imp">
		<Physics radius = "0.2" height = "0.65" eyeHeight = "0.6" walkSpeed = "1.2" / >
		< / Actor>

		<Entity name = "Lamp">
		<Physics radius = "0.1" height = "0.9" / >
		< / Entity>

		<Projectile name = "Plasma Bolt">
		<Physics radius = "0.05" height = "0.1" speed = "5" / >
		<Gameplay damage = "5~15" / >
		< / Projectile>
	*/
};

//--------------------------------------------------------------------------------------------------------------------------------------------