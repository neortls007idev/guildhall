#include "Engine/Platform/Window.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Engine/Input/InputSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------


static TCHAR const* WND_CLASS_NAME = TEXT( "Simple Window Class" );

//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called by Windows whenever we ask it for notifications

//--------------------------------------------------------------------------------------------------------------------------------------------

static LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle , UINT wmMessageCode , WPARAM wParam , LPARAM lParam )
{
	Window* window = ( Window* ) ::GetWindowLongPtr( windowHandle , GWLP_USERDATA );
	InputSystem* input = nullptr;

	if ( window )
	{
		input = window->GetInputSytem();
	}

	switch ( wmMessageCode )
	{
	case WM_CREATE: {
		window = ( Window* ) lParam;
		::SetWindowLongPtr( windowHandle , GWLP_USERDATA , ( LONG_PTR ) window );
	} break;
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
	case WM_CLOSE:
	{
		window->HandleQuitRequested();
		return 0; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
	case WM_KEYDOWN:
	{
		unsigned char asKey = ( unsigned char ) wParam;
		//UNUSED( asKey );
		//input = window->GetInputSytem();
		input->HandleKeyDown( asKey );
		break;
	}

	// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
	case WM_KEYUP:
	{
		unsigned char asKey = ( unsigned char ) wParam;
		//input = window->GetInputSytem();
		input->HandleKeyUp( asKey );
		//UNUSED( asKey );
		break;
	}
	}
	
	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return DefWindowProc( windowHandle , wmMessageCode , wParam , lParam );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

static void RegisterWindowClass() 
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription , 0 , sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast< WNDPROC >( WindowsMessageHandlingProcedure ); // Register our Windows message-handling function
	windowClassDescription.hInstance = ::GetModuleHandle( NULL );
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT( "Simple Window Class" );
	::RegisterClassEx( &windowClassDescription );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

static void UnRegisterWindowClass()
{
	::UnregisterClass( WND_CLASS_NAME , GetModuleHandle( NULL ) );
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Window::Window()
{
	RegisterWindowClass();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Window::~Window()
{
	UnRegisterWindowClass();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Window::Open( std::string const& title , float clientAspect , float maxClientFractionOfDesktop )
{
	// #SD1ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	const DWORD windowStyleFlags = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED /*| WS_BORDER | WS_THICKFRAME */;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle , &desktopRect );
	float desktopWidth = ( float ) ( desktopRect.right - desktopRect.left );
	float desktopHeight = ( float ) ( desktopRect.bottom - desktopRect.top );
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if ( clientAspect > desktopAspect )
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * clientAspect;
	}

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * ( desktopWidth - clientWidth );
	float clientMarginY = 0.5f * ( desktopHeight - clientHeight );
	RECT clientRect;
	clientRect.left = ( int ) clientMarginX;
	clientRect.right = clientRect.left + ( int ) clientWidth;
	clientRect.top = ( int ) clientMarginY;
	clientRect.bottom = clientRect.top + ( int ) clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx( &windowRect , windowStyleFlags , FALSE , windowStyleExFlags );

	WCHAR windowTitle[ 1024 ];
	MultiByteToWideChar( GetACP() , 0 , title.c_str() , -1 , windowTitle , sizeof( windowTitle ) / sizeof( windowTitle[ 0 ] ) );
	HWND hwnd = CreateWindowEx(
		windowStyleExFlags ,
		WND_CLASS_NAME ,
		windowTitle ,
		windowStyleFlags ,
		windowRect.left ,
		windowRect.top ,
		windowRect.right - windowRect.left ,
		windowRect.bottom - windowRect.top ,
		NULL ,
		NULL ,
		( HINSTANCE ) ::GetModuleHandle( NULL ),
		this );

	// TODO :- Taskbar progress 
		//	HRESULT result = HRESULT SetProgressState( HWND hwnd , TBPFLAG TBPF_NORMAL);

// 	HRESULT SetProgressValue(
// 		HWND      hwnd ,
// 		ULONGLONG 50 ,
// 		ULONGLONG 100
// 	);

	m_ClientWidth  = clientRect.right  - clientRect.left;
	m_clientHeight = clientRect.bottom - clientRect.top;

	::SetWindowLongPtr( hwnd , GWLP_USERDATA , ( LONG_PTR ) this );

	if ( hwnd == nullptr )
	{
		// GetLastError();
		return false;
	}

	ShowWindow( hwnd , SW_SHOW );
	SetForegroundWindow( hwnd );
	SetFocus( hwnd );

	HCURSOR cursor = LoadCursor( NULL , IDC_ARROW );
	SetCursor( cursor );

// TODO:- Ask Forseth  this makes the entire window transparent based on alpha 
// 	SetWindowLong( hwnd , GWL_EXSTYLE , WS_EX_LAYERED );
// 	SetLayeredWindowAttributes( hwnd , RGB( 0 , 0 , 0 ) , 128 , LWA_ALPHA );

// 	HICON hIcon1 = LoadIcon( ::GetModuleHandle( NULL ) ,  );
// 		//DrawIcon(  , 10 , 20 , hIcon1 );
// 		SetClassLongA( hwnd ,          // window handle 
// 			-14 ,              // changes icon 
// 			( LONG ) LoadIcon( ::GetModuleHandle( NULL ) , IDI_QUESTION )
// 		);

	m_hwnd = ( void* ) hwnd;
	
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Window::Close()
{
	HWND hwnd = ( HWND ) m_hwnd;
	if ( NULL == hwnd )
	{
		return;
	}

	::DestroyWindow( hwnd );
	m_hwnd = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Window::SetInputSystem( InputSystem* InputSystem )
{
	m_inputSystem = InputSystem;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Window::BeginFrame()
{
	MSG queuedMessage;
	for ( ;; )
	{
		const BOOL wasMessagePresent = PeekMessage( &queuedMessage , NULL , 0 , 0 , PM_REMOVE );
		if ( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage ); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int Window::GetClientWidth()
{
	return m_ClientWidth;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int Window::GetClientHeight()
{
	return m_clientHeight;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Window::HandleQuitRequested()
{
	m_isQuitting = true;
	return m_isQuitting;
}

//--------------------------------------------------------------------------------------------------------------------------------------------