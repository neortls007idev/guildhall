#pragma once
#include <string>

class Window
{
public:
	Window();
	~Window();

	//void SetInputSystem( InputSystem* inputSystem );

	bool Open( std::string const& title , float aspect , float maxClientFractionOfDesktop = 0.9f );
	void Close();

	void BeginFrame(); // process window messages

	int GetClientWidth();
	int GetClientHeight();

	void* m_hwnd = nullptr;

private:
	int m_clientHeight = 0;
	int m_ClientWidth  = 0;
};
