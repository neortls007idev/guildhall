#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/RendererCommon.hpp"
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class RenderContext;
struct Particle2D;
class Texture;
class SpriteSheet;
class Shader;

//--------------------------------------------------------------------------------------------------------------------------------------------

class ParticleEmitter2D
{
public:
	ParticleEmitter2D( RenderContext* renderContext , Texture* tex , Shader* shader = nullptr , eBlendMode blendMode = ADDITIVE );
	~ParticleEmitter2D();

	void SpawnNewParticle( Vec2 position , float orientation , Vec2 velocity , float age , float maxAge , Rgba8 color , IntVec2 spriteCoords );

	void Update( float deltaSeconds );
	void Render();
	
public:
	Texture*						m_texture				= nullptr;
	SpriteSheet*					m_spriteSheet			= nullptr;
	Vec2							m_minsUVs				= Vec2::ZERO;
	Vec2							m_maxsUVs				= Vec2::ONE;
	Shader*							m_shader				= nullptr;
	eBlendMode						m_blendMode				= ADDITIVE;

	//Vec2 m_position;
	//Vec2 m_velocity;
	
	std::vector<Particle2D*>		m_particles;
};

//--------------------------------------------------------------------------------------------------------------------------------------------