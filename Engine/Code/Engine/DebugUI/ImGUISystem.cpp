#include "Engine/DebugUI/ImGUISystem.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SwapChain.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

ImGUISystem::ImGUISystem( Window* window, RenderContext* renderContext )
{
	m_renderContext = renderContext;
	m_theWindow		= window;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

ImGUISystem::~ImGUISystem()
{
	
}

void ImGUISystem::Startup()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init( m_theWindow->m_hwnd );
	ImGui_ImplDX11_Init( m_renderContext->m_device , m_renderContext->m_context );
	ImGui::StyleColorsDark();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ImGUISystem::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ImGUISystem::EndFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ImGUISystem::BeginFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ImGUISystem::Update( float deltaSeconds )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ImGUISystem::Render() const
{
	ImGui::Render();
	ImDrawData* data = ImGui::GetDrawData();

	// Reset Shader Data
	m_renderContext->BindShader( nullptr );
	m_renderContext->BindTexture( nullptr );
	
	//Set Render Target View for ImGUI
	//Texture* backbuffer = m_renderContext->m_swapChain->GetBackBuffer();
	//m_renderContext->m_context->OMSetRenderTargets(1,)
	
	ImGui_ImplDX11_RenderDrawData( data );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
