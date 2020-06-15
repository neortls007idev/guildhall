#include "Engine/ParticleSystem/ParticleEmitter2D.hpp"


#include "Engine/Core/StdExtensions.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/ParticleSystem/Particle2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

ParticleEmitter2D::ParticleEmitter2D ( RenderContext* renderContext , Texture* tex , Shader* shader /*= nullptr */ ,
                                       eBlendMode blendMode /*= ADDITIVE */ )											:
																															m_renderContext( renderContext ) ,
																															m_texture( tex ) ,
																															m_shader( shader ) ,
																															m_blendMode( blendMode )
{
	m_spriteSheet = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

ParticleEmitter2D::ParticleEmitter2D ( RenderContext* renderContext , SpriteSheet* spriteSheet ,
									   Shader* shader /*= nullptr */ , eBlendMode blendMode /*= ADDITIVE */ ) :
																												m_renderContext( renderContext ) ,
																												m_spriteSheet( spriteSheet ) ,
																												m_shader( shader ) ,
																												m_blendMode( blendMode )
{
	
	m_texture = const_cast< Texture* >( &spriteSheet->GetTexture() );
	//m_texture = const_cast< Texture* >( &( spriteSheet->GetTexture() ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

ParticleEmitter2D::~ParticleEmitter2D()
{
	m_particles.clear();
	m_particleVerts.clear();

	m_texture		= nullptr;
	m_spriteSheet	= nullptr;
	m_shader		= nullptr;
	m_renderContext = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter2D::SpawnNewParticle ( AABB2 cosmeticBounds , Vec2 position , float orientation , Vec2 velocity , float age ,
                                           float maxAge , Rgba8 color , IntVec2 spriteCoords )
{
	if( m_spriteSheet != nullptr )
	{
		//int spriteSheetWidth = g_theGame->m_gameSS[ SS_BRICKS ]->GetSpriteDimension().x;
		//int tileSpriteIndex = m_spriteCoords.x + ( spriteSheetWidth * m_spriteCoords.y );
		//const SpriteDefinition& currentTileSprite = g_theGame->m_gameSS[ SS_BRICKS ]->GetSpriteDefinition( tileSpriteIndex );
		//currentTileSprite.GetUVs( m_spriteUVs.m_mins , m_spriteUVs.m_maxs );
		
		Particle2D* temp = new Particle2D( cosmeticBounds , position , orientation , velocity , age , maxAge , color );
		int		spriteSheetWidth = m_spriteSheet->GetSpriteDimension().x;
		int		spriteIndex = spriteCoords.x + ( spriteSheetWidth * spriteCoords.y );

		const SpriteDefinition& currentParticleSprite = m_spriteSheet->GetSpriteDefinition( spriteIndex );
		currentParticleSprite.GetUVs( temp->m_minsUVs , temp->m_maxsUVs );

		EmplaceBackAtEmptySpace( m_particles , temp );

		//temp->m_cosmeticBounds = cosmeticBounds;
	}
	else
	{
		SpawnNewParticle( cosmeticBounds , position , orientation , velocity , age , maxAge , color );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter2D::SpawnNewParticle( AABB2 cosmeticBounds , Vec2 position , float orientation , Vec2 velocity , float age , float maxAge , Rgba8 color )
{
	Particle2D* temp = new Particle2D( cosmeticBounds, position , orientation , velocity , age , maxAge , color );
	EmplaceBackAtEmptySpace( m_particles , temp );

	//temp->m_cosmeticBounds = cosmeticBounds;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter2D::Update( float deltaSeconds )
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
		Particle2D* particle = m_particles[ index ];
		if( particle != nullptr )
		{
			AppendVertsForAABB2( m_particleVerts , particle->m_cosmeticBounds , particle->m_color , particle->m_minsUVs , particle->m_maxsUVs );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter2D::Render()
{
	m_renderContext->BindShader( m_shader );
	m_renderContext->BindTexture( m_texture );
	
	m_renderContext->SetBlendMode( m_blendMode );
    m_renderContext->DrawVertexArray( m_particleVerts );
	m_renderContext->SetBlendMode( eBlendMode::ALPHA );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
