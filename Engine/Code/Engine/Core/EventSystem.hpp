#pragma once
#include <string>
#include <vector>
#include "Engine/Core/NamedStrings.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

//typedef void( *EventCallbackFunctionPtrType )( );
typedef NamedStrings EventArgs;
typedef bool( *EventCallbackFunction )( EventArgs& args );

//--------------------------------------------------------------------------------------------------------------------------------------------

struct EventSubscription
{
	std::string m_eventName;
	EventCallbackFunction m_callbackFunctionPtr = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class EventSystem
{
public:
	EventSystem() {};
	~EventSystem() {};

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void SubscribeToEvent( const std::string& eventName , EventCallbackFunction eventToListen );
	void UnSubscribeEvent( const std::string& eventName , EventCallbackFunction eventToUnscribe );
	bool FireEvent(const std::string& eventName);

	//--------------------------------------------------------------------------------------------------------------------------------------------

private:
	std::vector<EventSubscription*> m_eventSubscriptions;

};

//--------------------------------------------------------------------------------------------------------------------------------------------