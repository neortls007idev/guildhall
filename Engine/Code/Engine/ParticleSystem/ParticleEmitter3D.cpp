#include "Engine/ParticleSystem/ParticleEmitter3D.hpp"
#include "Engine/Core/StdExtensions.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/ParticleSystem/Particle3D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

ParticleEmitter3D::ParticleEmitter3D( RenderContext* renderContext , Texture* tex , size_t intialArraySize ,
									  Vec3 targetPos , Shader* shader /*= nullptr */ ,
									  eBlendMode blendMode /*= ADDITIVE */ , eCullMode cullMode /*= CULL_BACK */ ) :
																														m_renderContext( renderContext ) ,
																														m_texture( tex ) ,
																														m_targetPos( targetPos ),
																														m_shader( shader ) ,
																														m_blendMode( blendMode ) ,
																														m_cullMode( cullMode )
{
	m_spriteSheet = nullptr;
	m_particles.resize( intialArraySize );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

ParticleEmitter3D::ParticleEmitter3D ( RenderContext* renderContext , SpriteSheet* spriteSheet ,
                                       size_t intialArraySize , Vec3 targetPos , Shader* shader /*= nullptr */ ,
									   eBlendMode blendMode /*= ADDITIVE */ , eCullMode cullMode /*= CULL_BACK */ ) :
																														m_renderContext( renderContext ) ,
																														m_spriteSheet( spriteSheet ) ,
																														m_shader( shader ) ,
																														m_blendMode( blendMode ) ,
																														m_targetPos( targetPos ) ,
																														m_cullMode( cullMode )
{
	m_texture = const_cast< Texture* >( &spriteSheet->GetTexture() );
	//m_texture = const_cast< Texture* >( &( spriteSheet->GetTexture() ) );
	m_particles.resize( intialArraySize );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

ParticleEmitter3D::~ParticleEmitter3D()
{
	//m_particles.clear();
	//m_particleVerts.clear();

	m_texture		= nullptr;
	m_spriteSheet	= nullptr;
	m_shader		= nullptr;
	m_renderContext = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::SpawnNewParticle ( AABB2 cosmeticBounds , Vec3 position , Vec3 velocity , float age ,
                                           float maxAge , Rgba8 color , IntVec2 spriteCoords )
{
	if( m_spriteSheet != nullptr )
	{		
		Particle3D* temp = new Particle3D( cosmeticBounds , position , velocity , age , maxAge , color );
		int		spriteSheetWidth = m_spriteSheet->GetSpriteDimension().x;
		int		spriteIndex = spriteCoords.x + ( spriteSheetWidth * spriteCoords.y );

		const SpriteDefinition& currentParticleSprite = m_spriteSheet->GetSpriteDefinition( spriteIndex );
		currentParticleSprite.GetUVs( temp->m_minsUVs , temp->m_maxsUVs );

		EmplaceBackNewParticle( temp );
	}
	else
	{
		SpawnNewParticle( cosmeticBounds , position , velocity , age , maxAge , color );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::SpawnNewParticle( AABB2 cosmeticBounds , Vec3 position , Vec3 velocity , float age , float maxAge , Rgba8 color )
{
	Particle3D* temp = new Particle3D( cosmeticBounds, position , velocity , age , maxAge , color );
	EmplaceBackAtEmptySpace( m_particles , temp );

	//temp->m_cosmeticBounds = cosmeticBounds;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::SpawnNewParticle ( AABB2 cosmeticBounds , Vec3 position , float scale ,
                                           Vec3 velocity , float age , float maxAge ,
                                           Rgba8 color , IntVec2 spriteCoords )
{
	if( m_spriteSheet != nullptr )
	{
		Particle3D* temp = new Particle3D( cosmeticBounds , position , scale ,
		                                   velocity , age , maxAge , color );
		int		spriteSheetWidth = m_spriteSheet->GetSpriteDimension().x;
		int		spriteIndex = spriteCoords.x + ( spriteSheetWidth * spriteCoords.y );

		const SpriteDefinition& currentParticleSprite = m_spriteSheet->GetSpriteDefinition( spriteIndex );
		currentParticleSprite.GetUVs( temp->m_minsUVs , temp->m_maxsUVs );

		EmplaceBackNewParticle( temp );
	}
	else
	{
		SpawnNewParticle( cosmeticBounds , position , velocity , age , maxAge , color );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::EmplaceBackNewParticle( Particle3D* temp )
{
	for( size_t index = m_lastSpawnPointPos ; index < m_particles.size() ; index++ )
	{
		if( nullptr == m_particles[ index ] )
		{
			m_particles[ index ] = temp;
			m_lastSpawnPointPos = index;
			return;
		}
	}
	m_particles.push_back( temp );
	m_lastSpawnPointPos = m_particles.size();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::SpawnNewRandomParticleFromSpriteSheet ( AABB2 cosmeticBounds , Vec3 position ,
																float scale , Vec3 velocity , float age ,
                                                                float maxAge , Rgba8 color )
{
	if ( m_spriteSheet != nullptr )
	{
		IntVec2 randSprite = m_spriteSheet->RollRandomSpriteCoordsInSpriteSheet();
		SpawnNewParticle( cosmeticBounds , position , scale , velocity , age , maxAge , color , randSprite );
	}
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::Update( float deltaSeconds )
{
	m_particleVerts.clear();

	for( size_t index = 0 ; index < m_particles.size() ; index++ )
	{
		if( m_particles[ index ] != nullptr )
		{
			m_particles[ index ]->Update( deltaSeconds );

			if ( m_particles[ index ]->m_age >= m_particles[ index ]->m_maxAge )
			{
				delete m_particles[ index ];
				m_particles[ index ] = nullptr;
			}
		}
	}
	
	for ( size_t index = 0 ; index < m_particles.size() ; index++ )
	{
		Particle3D* particle = m_particles[ index ];
		if( particle != nullptr )
		{
			//TransformAndAppendVertsForAABB2( m_particleVerts , particle->m_cosmeticBounds , particle->m_color ,
			//                                 particle->m_minsUVs , particle->m_maxsUVs , particle->m_scale ,
			//                                 particle->m_orientation , particle->m_position );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::Render()
{
	if ( nullptr == m_renderContext )
	{
		return;
	}
	
	m_renderContext->BindShader( m_shader );
	m_renderContext->BindTexture( m_texture );
	
	m_renderContext->SetBlendMode( m_blendMode );
    m_renderContext->DrawVertexArray( m_particleVerts );
	m_renderContext->SetBlendMode( eBlendMode::ALPHA );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::Destroy()
{
	m_texture				= nullptr;
	m_spriteSheet			= nullptr;
	m_shader				= nullptr;
	m_renderContext			= nullptr;

	m_particles.clear();
	m_particleVerts.clear();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
