#include "Engine/ParticleSystem/ParticleEmitter3D.hpp"
#include "Engine/Core/StdExtensions.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MatrixUtils.hpp"
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
	m_particles.reserve( intialArraySize );
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
	m_particles.reserve( intialArraySize );
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

void ParticleEmitter3D::SpawnNewParticle ( AABB2 cosmeticBounds , Vec3 position , Vec3 target , Vec3 velocity , float age ,
                                           float maxAge , Rgba8 startColor , Rgba8 endColor , IntVec2 spriteCoords )
{
	if( m_spriteSheet != nullptr )
	{		
		Particle3D* temp = new Particle3D( cosmeticBounds , m_position + position , m_velocity + velocity , age , maxAge , startColor ,endColor );
		int		spriteSheetWidth = m_spriteSheet->GetSpriteDimension().x;
		int		spriteIndex = spriteCoords.x + ( spriteSheetWidth * spriteCoords.y );

		const SpriteDefinition& currentParticleSprite = m_spriteSheet->GetSpriteDefinition( spriteIndex );
		currentParticleSprite.GetUVs( temp->m_minsUVs , temp->m_maxsUVs );

		EmplaceBackNewParticle( temp );
		m_numAliveParticles++;
	}
	else
	{
		//SpawnNewParticle( cosmeticBounds , position , velocity , age , maxAge , color );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::SpawnNewParticle( AABB2 cosmeticBounds , Vec3 position , Vec3 target , Vec3 velocity , float age , float maxAge , Rgba8 startColor , Rgba8 endColor )
{
	Particle3D* temp = new Particle3D( cosmeticBounds, m_position + position , m_velocity + velocity , age , maxAge , startColor , endColor );
	EmplaceBackAtEmptySpace( m_particles , temp );
	m_numAliveParticles++;
	//temp->m_cosmeticBounds = cosmeticBounds;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::SpawnNewParticle ( AABB2 cosmeticBounds , Vec3 position , Vec3 target , float scale ,
                                           Vec3 velocity , float age , float maxAge ,
                                           Rgba8 startColor , Rgba8 endColor , IntVec2 spriteCoords )
{
	if( m_spriteSheet != nullptr )
	{
		Particle3D* temp = new Particle3D( cosmeticBounds , m_position + position , scale ,
		                                   m_velocity + velocity , age , maxAge , startColor , endColor );
		int		spriteSheetWidth = m_spriteSheet->GetSpriteDimension().x;
		int		spriteIndex = spriteCoords.x + ( spriteSheetWidth * spriteCoords.y );

		const SpriteDefinition& currentParticleSprite = m_spriteSheet->GetSpriteDefinition( spriteIndex );
		currentParticleSprite.GetUVs( temp->m_minsUVs , temp->m_maxsUVs );

		EmplaceBackNewParticle( temp );
		m_numAliveParticles++;
	}
	else
	{
		SpawnNewParticle( cosmeticBounds , position , target , velocity , age , maxAge , startColor , endColor );
		m_numAliveParticles++;
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

void ParticleEmitter3D::SpawnNewRandomParticleFromSpriteSheet ( AABB2 cosmeticBounds , Vec3 position , Vec3 target ,
																float scale , Vec3 velocity , float age ,
                                                                float maxAge , Rgba8 startColor , Rgba8 endColor /*= CLEAR*/ )
{
	if ( m_spriteSheet != nullptr )
	{
		IntVec2 randSprite = m_spriteSheet->RollRandomSpriteCoordsInSpriteSheet();
		SpawnNewParticle( cosmeticBounds , m_position + position , target , scale , m_velocity + velocity , age , maxAge , startColor , endColor , randSprite );
		m_numAliveParticles++;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::Update( float deltaSeconds )
{
	m_particleVerts.clear();
	m_numAliveParticles = 0;
	
	for( size_t index = 0 ; index < m_particles.size() ; index++ )
	{
		if( m_particles[ index ] != nullptr )
		{
			m_particles[ index ]->Update( deltaSeconds );
			m_numAliveParticles++;

			if ( m_particles[ index ]->m_age >= m_particles[ index ]->m_maxAge )
			{
				delete m_particles[ index ];
				m_particles[ index ] = nullptr;
				m_numAliveParticles--;
			}
		}
	}

	Mat44 lookAt;
	
	for ( size_t index = 0 ; index < m_particles.size() ; index++ )
	{
		Particle3D* particle = m_particles[ index ];
		if( particle != nullptr )
		{
			if ( m_areParticlesBillboarded )
			{
				lookAt = LookAtMatrix( particle->m_position , m_targetPos );
				Vec4 ibasis = -lookAt.GetIBasis4D();
				lookAt.Ix = ibasis.x;
				lookAt.Iy = ibasis.y;
				lookAt.Iz = ibasis.z;
				lookAt.Iw = ibasis.w;
				
				Transform3DAndAppendVertsForAABB2( m_particleVerts , particle->m_cosmeticBounds , particle->m_startColor ,
				                                 particle->m_minsUVs , particle->m_maxsUVs , particle->m_position , lookAt );
			}
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
	m_renderContext->SetModelMatrix( Mat44::IDENTITY , HALF_ALPHA_WHITE );
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

void ParticleEmitter3D::UpdateTargetPos( Vec3 newTargetPos )
{
	m_targetPos = newTargetPos;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::Move( float deltaSeconds )
{
	m_position += m_velocity * deltaSeconds;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::UpdatePosition( Vec3 newPos )
{
	m_position = newPos;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::UpdateVelocity( Vec3 newVelocity )
{
	m_velocity = newVelocity;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
