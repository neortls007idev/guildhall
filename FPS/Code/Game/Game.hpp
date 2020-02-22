#include "Engine/Math/Vec3.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/GameCommon.hpp"
#pragma once

//--------------------------------------------------------------------------------------------------------------------------------------------

class Shader;
class Texture;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game
{

public:

	Game();
	~Game();
	void Update( float deltaSeconds );
	void Render() const;
	void UpdateCamera();
	void RenderUI() const;
	void CheckCollision();

	void AddScreenShakeIntensity( float deltaShakeIntensity );

	void GarbageCollection();
	void GarbageDeletion();
	void Die();

private:

	void UpdateFromKeyBoard();


private:

	int			m_controllerID			= -1;
	float		m_screenShakeIntensity	= 0.f;

public:

	AABB2		m_normalImage;
	AABB2		m_invertedColorImage;
	Texture*	m_imageTex				= nullptr;

	Camera		m_gameCamera;
	Camera		m_uiCamera;

	Rgba8		m_color;
	Vec3		m_cameraPosition;
	Shader* 	m_invertColorShader;
};