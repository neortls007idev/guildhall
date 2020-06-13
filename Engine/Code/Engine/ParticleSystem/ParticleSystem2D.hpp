#pragma once
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class ParticleEmitter2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

class ParticleSystem2D
{
public:
	ParticleSystem2D();
	~ParticleSystem2D();

	void Startup();
	void Shutdown();
	void RunFrame();

	void BeginFrame();
	void Update( float deltaSeconds );
	void UpdateFromKeyboard();
	void Render() const;

	void EndFrame();

	//----------------------------------------------------------------------------------------------------------

	void CreateNewParticleEmitter();
	
public:
	std::vector<ParticleEmitter2D*> m_emitters;
	
};

//--------------------------------------------------------------------------------------------------------------------------------------------