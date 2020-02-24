#include "Engine/Renderer/IndexBuffer.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

void IndexBuffer::Update( uint icount , uint const* indices )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void IndexBuffer::Update( std::vector<uint> const& indices )
{
	Update( ( uint ) indices.size() , &indices[ 0 ] );
}
