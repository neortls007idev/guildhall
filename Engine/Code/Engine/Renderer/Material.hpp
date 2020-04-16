#pragma once
#include <map>
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Shader;
class RenderBuffer;
class Texture;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Material
{
public:
	Shader* m_shader;

	RenderBuffer* m_data;														// may be null if material doesn't need it
	std::map< uint , Texture* > m_userSlotIndexedTextures;
};
