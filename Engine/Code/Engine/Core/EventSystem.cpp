#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

EventSystem* g_theEventSystem = nullptr;

//--------------------------------------------------------------------------------------------------------------------------------------------

void EventSystem::Startup()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void EventSystem::BeginFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void EventSystem::EndFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void EventSystem::Shutdown()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void EventSystem::SubscribeToEvent( const std::string& eventName , EventCallbackFunction eventToListen )
{
	EventSubscription* newSubscription = new EventSubscription();
	newSubscription->m_eventName = eventName;
	newSubscription->m_callbackFunctionPtr = eventToListen;
	m_eventSubscriptions.push_back( newSubscription );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool EventSystem::FireEvent( const std::string& eventName , EventArgs& defaultArguments )
{
	for ( int eventIndex = 0; eventIndex < ( int ) m_eventSubscriptions.size(); eventIndex++ )
	{
		EventSubscription* subscription = m_eventSubscriptions[ eventIndex ];
		if ( subscription->m_eventName == eventName )
		{
			subscription->m_callbackFunctionPtr( defaultArguments );
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void EventSystem::UnSubscribeEvent( const std::string& eventName , EventCallbackFunction eventToUnscribe )
{
	UNUSED( eventToUnscribe );
	for ( int eventIndex = 0; eventIndex < ( int ) m_eventSubscriptions.size(); eventIndex++ )
	{
		if ( m_eventSubscriptions[ eventIndex ] != nullptr )
		{
			if ( m_eventSubscriptions[ eventIndex ]->m_eventName == eventName )
			{
				if ( m_eventSubscriptions[ eventIndex ]->m_eventName == eventName )
				{
					delete m_eventSubscriptions[ eventIndex ];
					m_eventSubscriptions[ eventIndex ] = nullptr;
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------