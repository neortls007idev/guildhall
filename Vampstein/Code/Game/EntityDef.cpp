#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/EntityDef.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*					  g_theRenderer;
STATIC std::map<std::string , EntityDef*> EntityDef::s_entityDefs;

//--------------------------------------------------------------------------------------------------------------------------------------------

EntityDef::EntityDef()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void EntityDef::CreateDefinitions( char const* dataFilePath )
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( dataFilePath );

	tinyxml2::XMLElement* root = doc.RootElement();

	ParseAllActors( root );
	ParseAllEntitiesOfName( root , "Entity" );
	ParseAllEntitiesOfName( root , "Projectile" );
	ParseAllEntitiesOfName( root , "Portal" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void EntityDef::ParseAllEntitiesOfName( tinyxml2::XMLElement* root , std::string elementName )
{
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			PARSING ALL ENTITIES WITH NAME
	//--------------------------------------------------------------------------------------------------------------------------------------------

	for( tinyxml2::XMLElement* element = root->FirstChildElement( elementName.c_str() ); element != nullptr; element = element->NextSiblingElement( elementName.c_str() ) )
	{
		std::string name			= ParseXmlAttribute( *element , "name" , "invalid" );

		if( name == "invalid" )
		{
			//error
			continue;
		}

		EntityDef* newDef = new EntityDef();
		tinyxml2::XMLElement* physicsEle = element->FirstChildElement( "Physics" );

		if( physicsEle != nullptr )
		{
			newDef->m_physicsRadius = ParseXmlAttribute( *physicsEle , "radius" , 0.0f );
			newDef->m_height		= ParseXmlAttribute( *physicsEle , "height" , 0.0f );
			newDef->m_speed			= ParseXmlAttribute( *physicsEle , "speed"  , 0.f );
		}

		tinyxml2::XMLElement* gamePlayEle = element->FirstChildElement( "Gameplay" );

		if( gamePlayEle != nullptr )
		{
			newDef->m_damage		= ParseXmlAttribute( *gamePlayEle , "damage" , IntVec2::ZERO );
		}

		newDef->m_className			= "Projectile";
		newDef->m_typeName			= name;

		s_entityDefs[ name ]		= newDef;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

/*
void EntityDef::ParseAllEntities( tinyxml2::XMLElement* root )
{
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			PARSING ALL ENTITIES
	//--------------------------------------------------------------------------------------------------------------------------------------------

	for( tinyxml2::XMLElement* entityElement = root->FirstChildElement( "Entity" ); entityElement != nullptr; entityElement = entityElement->NextSiblingElement( "Entity" ) )
	{
		std::string name			= ParseXmlAttribute( *entityElement , "name" , "invalid" );

		if( name == "invalid" )
		{
			//error
			continue;
		}

		EntityDef* newDef = new EntityDef();
		tinyxml2::XMLElement* physicsEle = entityElement->FirstChildElement( "Physics" );

		if( physicsEle != nullptr )
		{
			newDef->m_physicsRadius = ParseXmlAttribute( *physicsEle , "radius" , 0.0f );
			newDef->m_height		= ParseXmlAttribute( *physicsEle , "height" , 0.0f );
		}
		newDef->m_className			= "Entity";
		newDef->m_typeName			= name;

		s_entityDefs[ name ] = newDef;
	}
}
*/
//--------------------------------------------------------------------------------------------------------------------------------------------

void EntityDef::ParseAllActors( tinyxml2::XMLElement* root )
{
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			PARSING ALL ACTORS
	//--------------------------------------------------------------------------------------------------------------------------------------------

	for( tinyxml2::XMLElement* element = root->FirstChildElement( "Actor" ); element != nullptr; element = element->NextSiblingElement( "Actor" ) )
	{
		std::string name = ParseXmlAttribute( *element , "name" , "invalid" );

		if( name == "invalid" )
		{
			//error
			continue;
		}

		EntityDef* newDef = new EntityDef();

		tinyxml2::XMLElement* physicsEle = element->FirstChildElement( "Physics" );

		if( physicsEle != nullptr )
		{
			newDef->m_physicsRadius = ParseXmlAttribute( *physicsEle , "radius" , 0.f );
			newDef->m_height = ParseXmlAttribute( *physicsEle , "height" , 0.f );
			newDef->m_eyeHeight = ParseXmlAttribute( *physicsEle , "eyeHeight" , 0.f );
			newDef->m_speed = ParseXmlAttribute( *physicsEle , "walkSpeed" , 0.0f );
		}

		tinyxml2::XMLElement* appereanceElement = element->FirstChildElement( "Appearance" );

		if( appereanceElement != nullptr )
		{
			newDef->m_spriteSize = ParseXmlAttribute( *appereanceElement , "size" , Vec2( 0.f , 0.f ) );
			std::string billboardType = ParseXmlAttribute( *appereanceElement , "billboard" , "invalid" );

			if( billboardType != "invalid" )
			{
				if( billboardType == "CameraFacingXY" )
				{
					newDef->m_billboard = CAMERA_FACING_XY;
				}
				else if( billboardType == "CameraOpposedXY" )
				{
					newDef->m_billboard = CAMERA_OPPOSED_XY;
				}
				else if( billboardType == "CameraFacingXYZ" )
				{
					newDef->m_billboard = CAMERA_FACING_XYZ;
				}
				else if( billboardType == "CameraOpposedXYZ" )
				{
					newDef->m_billboard = CAMERA_OPPOSED_XYZ;
				}
			}

			std::string spritePath = ParseXmlAttribute( *appereanceElement , "spriteSheet" , "invalid" );
			IntVec2		spriteLayout = ParseXmlAttribute( *appereanceElement , "layout" , IntVec2::ZERO );

			if( spritePath != "invalid" && spriteLayout != IntVec2( 0 , 0 ) )
			{
				Texture* spriteTex = g_theRenderer->GetOrCreateTextureFromFile( spritePath.c_str() );
				newDef->m_spriteSheet = new SpriteSheet( *spriteTex , spriteLayout );
			}

			if( newDef->m_spriteSheet != nullptr )
			{
				int index = 0;
				
				for( tinyxml2::XMLElement* animElement = appereanceElement->FirstChildElement() ; animElement != nullptr ; animElement = animElement->NextSiblingElement() )
				{
					if( animElement == nullptr )
					{
						continue;
					}
					std::string animName = animElement->Name();
					index = GetIndexForAnimName( animName );
					ParseEntityAnim( animElement , newDef , index , "front" );
					ParseEntityAnim( animElement , newDef , index , "back" );
					ParseEntityAnim( animElement , newDef , index , "left" );
					ParseEntityAnim( animElement , newDef , index , "right" );
					ParseEntityAnim( animElement , newDef , index , "frontLeft" );
					ParseEntityAnim( animElement , newDef , index , "frontRight" );
					ParseEntityAnim( animElement , newDef , index , "backLeft" );
					ParseEntityAnim( animElement , newDef , index , "backRight" );
				}
			}
			newDef->m_className = "Actor";
			newDef->m_typeName = name;

			s_entityDefs[ name ] = newDef;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void EntityDef::ParseEntityAnim( tinyxml2::XMLElement* animElement , EntityDef* newDef , int index , std::string animName )
{
	std::string animationName = ParseXmlAttribute( *animElement , animName.c_str() , "invalid" );

	if( animationName != "invalid" )
	{
		Strings nums = SplitStringAtGivenDelimiter( animationName , ',' );

		newDef->m_entityAnims[ index ][ animName ] = new SpriteAnimDefinition( *( newDef->m_spriteSheet ) , atoi( nums[ 0 ].c_str() ) , atoi( nums[ nums.size() - 1 ].c_str() ) , 1.f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int EntityDef::GetIndexForAnimName( std::string animName )
{
	if ( animName == "Walk" )																{ return ( int ) ENTITY_WALK_ANIM; }
	if ( animName == "Attack" )																{ return ( int ) ENTITY_ATTACK_ANIM; }
	if ( animName == "Pain" )																{ return ( int ) ENTITY_PAIN_ANIM; }
	if ( animName == "Death" )																{ return ( int ) ENTITY_DEATH_ANIM; }

	return (int) ENTITY_INVALID_ANIM;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC EntityDef* EntityDef::GetDefinition( std::string const& entityDefName )
{
	// TODO :- SDST FIX ME
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
