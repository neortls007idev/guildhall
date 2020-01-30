#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Collider2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::BeginFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::Update()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::EndFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Rigidbody2D* Physics2D::CreateRigidbody()
{
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::DestroyRigidbody( Rigidbody2D* rigidbody )
{
	Collider2D* collider = rigidbody->GetCollider();
	if (collider)
	{
		rigidbody->SetCollider( nullptr );
		collider->Destroy();
	}
	rigidbody->Destroy();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

DiscCollider2D* Physics2D::CreateDiscCollider( Vec2 localPosition , float radius )
{
	UNUSED( localPosition );
	UNUSED( radius );
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::DestroyCollider( Collider2D* collider )
{
	collider->~Collider2D();
}

//--------------------------------------------------------------------------------------------------------------------------------------------