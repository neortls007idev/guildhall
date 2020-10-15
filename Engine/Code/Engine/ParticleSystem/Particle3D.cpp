#include "Engine/ParticleSystem/Particle3D.hpp"


//--------------------------------------------------------------------------------------------------------------------------------------------

#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

Particle3D::Particle3D( Vec3 position , Vec3 velocity , float age , float maxAge , Rgba8 color , Rgba8 endColor /*= CLEAR*/ ) :
																							m_age( age ) ,
																							m_maxAge( maxAge ) ,
																							m_position( position ) ,
																							m_velocity( velocity ) ,
																							m_startColor( color ) ,
																							m_endColor( endColor )
{
	m_scale = 1.f;
//	m_color = m_startColor;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Particle3D::Particle3D( AABB2 cosmeticBounds , Vec3 position , Vec3 velocity , float age , float maxAge , Rgba8 startColor , Rgba8 endColor /*= CLEAR*/ ) :
																							m_age( age ) ,
																							m_maxAge( maxAge ) ,
																							m_position( position ) ,
																							m_velocity( velocity ) ,
																							m_startColor( startColor ) ,
																							m_endColor( endColor ) ,
																							m_cosmeticBounds( cosmeticBounds )
{
	//m_cosmeticBounds.Translate( m_position );
	m_scale = 1.f;
//	m_color = m_startColor;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Particle3D::Particle3D ( AABB2 cosmeticBounds , Vec2 minUVs , Vec2 maxUVs , Vec3 position ,
						 Vec3 velocity , float age , float maxAge , Rgba8 startColor , Rgba8 endColor /*= CLEAR*/ ) :
																							m_age( age ) ,
																							m_maxAge( maxAge ) ,
																							m_position( position ) ,
																							m_velocity( velocity ) ,
																							m_startColor( startColor ) ,
																							m_endColor( endColor ) ,
																							m_cosmeticBounds( cosmeticBounds ) ,
																							m_minsUVs( minUVs ),
																							m_maxsUVs( maxUVs )
{
	//m_cosmeticBounds.Translate( m_position );
//	m_color = m_startColor;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Particle3D::Particle3D ( AABB2 cosmeticBounds , Vec2 minUVs , Vec2 maxUVs , Vec3 position , float scale ,
						 Vec3 velocity , float age , float maxAge , Rgba8 startColor , Rgba8 endColor /*= CLEAR*/ ) :
																							m_age( age ) ,
																							m_maxAge( maxAge ) ,
																							m_position( position ) ,
																							m_velocity( velocity ) ,
																							m_startColor( startColor ) ,
																							m_endColor( endColor ) ,
																							m_cosmeticBounds( cosmeticBounds ) ,
																							m_minsUVs( minUVs ) ,
																							m_maxsUVs( maxUVs ) 
{
	m_scale = scale;
//	m_color = m_startColor;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Particle3D::Particle3D ( AABB2 cosmeticBounds , Vec3 position , float scale , Vec3 velocity ,
						 float age , float maxAge , Rgba8 startColor , Rgba8 endColor /*= CLEAR*/ ) :
																							m_age( age ) ,
																							m_maxAge( maxAge ) ,
																							m_position( position ) ,
																							m_velocity( velocity ) ,
																							m_startColor( startColor ) ,
																							m_endColor( endColor ) ,
																							m_cosmeticBounds( cosmeticBounds )

{
	m_scale = scale;
//	m_color = m_startColor;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Particle3D::Particle3D( Particle3D& copy )
{
	m_age				= copy.m_age;				
	m_maxAge			= copy.m_maxAge;
	m_position			= copy.m_position;			
	m_velocity			= copy.m_velocity;			
	m_startColor		= copy.m_startColor;		
	m_endColor			= copy.m_endColor;			
	m_cosmeticBounds	= copy.m_cosmeticBounds;
	m_minsUVs			= copy.m_minsUVs;			
	m_maxsUVs			= copy.m_maxsUVs;			
	m_scale				= copy.m_scale;
//	m_color				= copy.m_color;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Particle3D::~Particle3D()
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Particle3D::Update( float deltaSeconds )
{
	if ( m_age < m_maxAge )
	{
		m_age += deltaSeconds;
		Move( deltaSeconds );
		// change to current color.
		m_startColor.LerpColorOverTime( m_startColor , m_endColor , m_maxAge , m_age );
		//DebuggerPrintf("Particle Current Age = %f , max AGE = %f , Start Color = %u, %u ,%u , %u", m_age , )
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