#include "Engine/Time/Clock.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Clock* g_theMasterClock = nullptr;

//--------------------------------------------------------------------------------------------------------------------------------------------

Clock::Clock()
{
	if ( g_theMasterClock!= nullptr )
	{
		m_allClocks.push_back( this );
	}
	else
	{
		// TODO :- find location of masterCLock and replace
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Clock::Clock( Clock* parent )
{
	m_parent = parent;
	m_allClocks.push_back( this );
	parent->m_children.push_back( this );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Clock::~Clock()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Clock::Update( double deltaSeconds )
{
	double deltaTime = deltaSeconds;
	if ( m_isClockPaused )
	{
		deltaTime = 0.0;
	}
	else
	{
		deltaTime *= m_scale;
	}
	m_lastFrameDeltaSeconds	 = deltaTime;
	m_totalTimeElapsed		+= m_lastFrameDeltaSeconds;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Clock::Reset()
{
	m_totalTimeElapsed = 0.0;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Clock::Pause()
{
	m_isClockPaused = true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Clock::Resume()
{
	m_isClockPaused = false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Clock::SetScale( double scale )
{
	m_scale = scale;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

double Clock::GetTotalElapsedSeconds() const
{
	return m_totalTimeElapsed;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

double Clock::GetLastDeltaSeconds() const
{
	return m_lastFrameDeltaSeconds;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

double Clock::GetScale() const
{
	return m_scale;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Clock::IsPaused() const
{
	return m_isClockPaused;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Clock::SetFrameLimits( double minFrameTime , double maxFrameTime )
{
	m_minFrameLimit = minFrameTime;
	m_maxFrameLimit = maxFrameTime;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Clock::SetParent( Clock* clock )
{
	m_parent = clock;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Clock::AddChild( Clock* clock )
{
	Clock* newChild = new Clock( this );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC void Clock::Startup()
{
	g_theMasterClock = new Clock();
	g_theMasterClock->Reset();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC void Clock::Shutdown()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC void Clock::BeginFrame()
{
	Clock* clockGod = GetMaster();
	clockGod->Update( 1.0 / 60.0 );	
}

STATIC void Clock::EndFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC Clock* Clock::GetMaster()
{
	for ( size_t index = 0; index < m_allClocks.size(); index++ )
	{
		if ( !m_allClocks[ index ] )
		{
			continue;
		}

		if ( m_allClocks[ index ]->GetParent() )
		{
			return m_allClocks[ index ];
		}
		ASSERT_OR_DIE( index == ( m_allClocks.size() - 1 ) , "No Master Clock Present" );
	}
}
