#include "Engine/ParticleSystem/Particle3D.hpp"


//--------------------------------------------------------------------------------------------------------------------------------------------

#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

Particle3D::Particle3D( Vec3 position , Vec3 velocity , float age , float maxAge , Rgba8 color ) :
																							m_position( position ) ,
																							m_velocity( velocity ) ,
																							m_age( age ) ,
																							m_maxAge( maxAge ) ,
																							m_color( color )
{
	m_scale = 1.f;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Particle3D::Particle3D( AABB2 cosmeticBounds , Vec3 position , Vec3 velocity , float age , float maxAge , Rgba8 color ) :
																							m_position( position ) ,
																							m_velocity( velocity ) ,
																							m_age( age ) ,
																							m_maxAge( maxAge ) ,
																							m_cosmeticBounds( cosmeticBounds ) , 
																							m_color( color )
{
	//m_cosmeticBounds.Translate( m_position );
	m_scale = 1.f;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Particle3D::Particle3D ( AABB2 cosmeticBounds , Vec2 minUVs , Vec2 maxUVs , Vec3 position ,
						 Vec3 velocity , float age , float maxAge , Rgba8 color ) :
																							m_cosmeticBounds( cosmeticBounds ) ,
																							m_minsUVs( minUVs ),
																							m_maxsUVs( maxUVs ),
																							m_position( position ) ,
																							m_velocity( velocity ) ,
																							m_age( age ) ,
																							m_maxAge( maxAge ) ,
																							m_color( color )
{
	//m_cosmeticBounds.Translate( m_position );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Particle3D::Particle3D ( AABB2 cosmeticBounds , Vec2 minUVs , Vec2 maxUVs , Vec3 position , float scale ,
						 Vec3 velocity , float age , float maxAge , Rgba8 color ) :
																							m_cosmeticBounds( cosmeticBounds ) ,
																							m_minsUVs( minUVs ) ,
																							m_maxsUVs( maxUVs ) ,
																							m_position( position ) ,
																							m_velocity( velocity ) ,
																							m_age( age ) ,
																							m_maxAge( maxAge ) ,
																							m_color( color )
{
	m_scale = scale;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Particle3D::Particle3D ( AABB2 cosmeticBounds , Vec3 position , float scale , Vec3 velocity ,
						 float age , float maxAge , Rgba8 color ) :
																							m_cosmeticBounds( cosmeticBounds ) ,
																							m_position( position ) ,
																							m_velocity( velocity ) ,
																							m_age( age ) ,
																							m_maxAge( maxAge ) ,
																							m_color( color )

{
	m_scale = scale;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Particle3D::~Particle3D()
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Particle3D::Update( float deltaSeconds )
{
//	if( m_isBillboarded )
//	{
//		Mat44 lookAt = LookAtMatrix( m_position , m_target );
//		m_model.SetBasisVectors4D( -lookAt.GetIBasis4D() , lookAt.GetJBasis4D() , lookAt.GetKBasis4D() , lookAt.GetTranslation4D() );
//	}
	
	if ( m_age < m_maxAge )
	{
		m_age += deltaSeconds;
		Move( deltaSeconds );
		m_color.LerpColorOverTime( m_color , CLEAR , m_maxAge , m_age );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Particle3D::Move( float deltaSeconds )
{
	Vec3 deltaPosition = m_velocity * deltaSeconds;
	m_position += deltaPosition;

	//m_cosmeticBounds.Translate( deltaPosition );
}

//--------------------------------------------------------------------------------------------------------------------------------------------