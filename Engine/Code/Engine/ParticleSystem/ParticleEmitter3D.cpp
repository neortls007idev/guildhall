#include "Engine/ParticleSystem/ParticleEmitter3D.hpp"
#include "Engine/Core/StdExtensions.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/ParticleSystem/Particle3D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

ParticleEmitter3D::ParticleEmitter3D( RenderContext* renderContext , Texture* tex , size_t initialArraySize ,
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

	m_totalSpawnableParticles = initialArraySize;
	m_particles = new Particle3D[ initialArraySize ];
	m_isParticleGarbage = new bool[ initialArraySize ];
	memset( &m_isParticleGarbage[ 0 ] , 1 , sizeof( bool ) * initialArraySize );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

ParticleEmitter3D::ParticleEmitter3D ( RenderContext* renderContext , SpriteSheet* spriteSheet ,
                                       size_t initialArraySize , Vec3 targetPos , Shader* shader /*= nullptr */ ,
									   eBlendMode blendMode /*= ADDITIVE */ , eCullMode cullMode /*= CULL_BACK */ ) :
																														m_renderContext( renderContext ) ,
																														m_spriteSheet( spriteSheet ) ,
																														m_shader( shader ) ,
																														m_blendMode( blendMode ) ,
																														m_targetPos( targetPos ) ,
																														m_cullMode( cullMode )
{
	m_texture = const_cast< Texture* >( &spriteSheet->GetTexture() );

	m_totalSpawnableParticles = initialArraySize;
	m_particles = new Particle3D[ initialArraySize ];
	m_isParticleGarbage = new bool[ initialArraySize ];
	memset( &m_isParticleGarbage[ 0 ] , 1 , sizeof( bool ) * initialArraySize );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

ParticleEmitter3D::~ParticleEmitter3D()
{
	delete [] m_particles;
	m_particles = nullptr;

	delete[] m_isParticleGarbage;
	m_isParticleGarbage = nullptr;
	
	m_particleVerts.clear();

	m_texture		= nullptr;
	m_spriteSheet	= nullptr;
	m_shader		= nullptr;
	m_renderContext = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::SpawnNewParticle ( AABB2 cosmeticBounds , Vec3 position , Vec3 velocity , uint16_t maxAge ,
										   Rgba8 startColor , Rgba8 endColor , IntVec2 spriteCoords )
{
	if ( m_numAliveParticles < m_totalSpawnableParticles )
	{
		if ( m_spriteSheet != nullptr )
		{
			Particle3D temp( cosmeticBounds , m_position + position , m_velocity + velocity , 0 , maxAge , startColor , endColor );
			int		spriteSheetWidth = m_spriteSheet->GetSpriteDimension().x;
			int		spriteIndex = spriteCoords.x + ( spriteSheetWidth * spriteCoords.y );


			temp.m_spriteIndex = spriteIndex;
			//const SpriteDefinition& currentParticleSprite = m_spriteSheet->GetSpriteDefinition( spriteIndex );
			//currentParticleSprite.GetUVs( temp.m_minsUVs , temp.m_maxsUVs );

			EmplaceBackNewParticle( temp );
		}
		else
		{
			//SpawnNewParticle( cosmeticBounds , position , velocity , 0 , maxAge , color );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::SpawnNewParticle( AABB2 cosmeticBounds , Vec3 position , Vec3 velocity , uint16_t maxAge , Rgba8 startColor , Rgba8 endColor )
{
	if( m_numAliveParticles < m_totalSpawnableParticles )
	{
		Particle3D temp( cosmeticBounds, m_position + position , m_velocity + velocity , 0 , maxAge , startColor , endColor );
		EmplaceBackNewParticle( temp );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::SpawnNewParticle ( AABB2 cosmeticBounds , Vec3 position , float scale , Vec3 velocity ,
										   uint16_t maxAge , Rgba8 startColor , Rgba8 endColor , IntVec2 spriteCoords )
{
	if ( m_numAliveParticles < m_totalSpawnableParticles )
	{
		if ( m_spriteSheet != nullptr )
		{
			Particle3D temp( cosmeticBounds , m_position + position , scale ,
				m_velocity + velocity , 0 , maxAge , startColor , endColor );
			int		spriteSheetWidth = m_spriteSheet->GetSpriteDimension().x;
			int		spriteIndex = spriteCoords.x + ( spriteSheetWidth * spriteCoords.y );

			temp.m_spriteIndex = spriteIndex;

			EmplaceBackNewParticle( temp );
		}
		else
		{
			SpawnNewParticle( cosmeticBounds , position , velocity , maxAge , startColor , endColor );
			
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::EmplaceBackNewParticle( Particle3D temp )
{
	if( m_numAliveParticles < m_totalSpawnableParticles )
	{		
		for ( size_t index = m_lastSearchPos ;  index < m_totalSpawnableParticles ; index++ )
		{
			if ( m_isParticleGarbage[ index ] )
			{
				m_isParticleGarbage[ index ] = false;
				m_particles[ index ] = temp;
				m_numAliveParticles++;
				m_lastSearchPos = index;

				return;
			}
		}

		for ( size_t index = 0; index < m_lastSearchPos; index++ )
		{
			if ( m_isParticleGarbage[ index ] )
			{
				m_isParticleGarbage[ index ] = false;
				m_particles[ index ] = temp;
				m_numAliveParticles++;
				m_lastSearchPos = index;

				return;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::SpawnNewRandomParticleFromSpriteSheet ( AABB2 cosmeticBounds , Vec3 position , float scale , Vec3 velocity ,
																uint16_t maxAge , Rgba8 startColor , Rgba8 endColor /*= CLEAR*/ )
{
	if ( m_spriteSheet != nullptr )
	{
		IntVec2 randSprite = m_spriteSheet->RollRandomSpriteCoordsInSpriteSheet();
		SpawnNewParticle( cosmeticBounds , m_position + position , scale , m_velocity + velocity , maxAge , startColor , endColor , randSprite );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::Update( float deltaSeconds )
{
	m_particleVerts.clear();

	if( m_numAliveParticles == 0 )
	{
		return;
	}
	
	for( size_t index = 0 ; index < m_totalSpawnableParticles ; index++ )
	{
		if ( m_isParticleGarbage[ index ] )
		{
			continue;
		}
		m_particles[ index ].Update( deltaSeconds );

		if ( m_particles[ index ].m_age >= m_particles[ index ].m_maxAge )
		{
			m_isParticleGarbage[ index ] = true;
			m_numAliveParticles--;
		}
	}

	Mat44 lookAt;
	Vec2 minUVs = Vec2::ZERO;
	Vec2 maxUVs = Vec2::ONE;
	
	for ( size_t index = 0 ; index < m_totalSpawnableParticles ; index++ )
	{
		Particle3D* particle = &m_particles[ index ];

		if ( m_isParticleGarbage[ index ] )
		{
			continue;
		}

		const SpriteDefinition& currentParticleSprite = m_spriteSheet->GetSpriteDefinition( m_particles[ index ].m_spriteIndex );
		currentParticleSprite.GetUVs( minUVs , maxUVs );
		
		lookAt = LookAtMatrix( particle->m_position , m_targetPos );
		Vec4 ibasis = -lookAt.GetIBasis4D();
		lookAt.Ix = ibasis.x;
		lookAt.Iy = ibasis.y;
		lookAt.Iz = ibasis.z;
		lookAt.Iw = ibasis.w;

		Transform3DAndAppendVertsForAABB2( m_particleVerts , particle->m_cosmeticBounds , particle->m_startColor ,
			minUVs , maxUVs , particle->m_position , lookAt );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::UpdateParticlesData( float deltaSeconds )
{
	m_particleVerts.clear();
	
	for ( size_t index = 0; ( index < m_totalSpawnableParticles ) && ( m_numAliveParticles > 0 ); index++ )
	{
		if ( m_isParticleGarbage[ index ] )
		{
			continue;
		}
		m_particles[ index ].Update( deltaSeconds );

		if ( m_particles[ index ].m_age >= m_particles[ index ].m_maxAge )
		{
			m_isParticleGarbage[ index ] = true;
			m_numAliveParticles--;
		}
	}

	m_particleVerts.resize( 6 * m_totalSpawnableParticles );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::UpdateParticlesVBO( size_t startIndex , size_t endIndex )
{
	Mat44 lookAt;
	Vec2 minUVs = Vec2::ZERO;
	Vec2 maxUVs = Vec2::ONE;
	
	for ( size_t index = startIndex ; index <= endIndex ; index++ )
	{
		Particle3D* particle = &m_particles[ index ];

		if ( m_isParticleGarbage[ index ] )
		{
			continue;
		}

		const SpriteDefinition& currentParticleSprite = m_spriteSheet->GetSpriteDefinition( m_particles[ index ].m_spriteIndex );
		currentParticleSprite.GetUVs( minUVs , maxUVs );
		
		lookAt = LookAtMatrix( particle->m_position , m_targetPos );
		Vec4 ibasis = -lookAt.GetIBasis4D();
		lookAt.Ix = ibasis.x;
		lookAt.Iy = ibasis.y;
		lookAt.Iz = ibasis.z;
		lookAt.Iw = ibasis.w;

		Transform3DAndAppendVertsForAABB2AtIndex( m_particleVerts , index * 6 , particle->m_cosmeticBounds , particle->m_startColor ,
			minUVs , maxUVs , particle->m_position , lookAt );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParticleEmitter3D::Render()
{
	if ( nullptr == m_renderContext )
	{
		return;
	}

	if( m_numAliveParticles == 0 )
	{
		return;
	}
	
	m_renderContext->BindShader( m_shader );
	m_renderContext->BindTexture( m_texture );
	m_renderContext->SetBlendMode( m_blendMode );	
	m_renderContext->SetModelMatrix( Mat44::IDENTITY /*, HALF_ALPHA_WHITE*/ );
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

	delete[] m_particles;
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
