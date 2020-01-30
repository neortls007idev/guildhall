#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include <d3dcompiler.h>

#include <stdio.h>

//--------------------------------------------------------------------------------------------------------------------------------------------

void* FileReadToNewBuffer( std::string const& filename, size_t *out_size )
{
	FILE* filePath = nullptr;
	fopen_s( &filePath, filename.c_str() , "r" );
	if ( nullptr == filePath )
	{
		return nullptr;
	}

	// TODO :- ADD ASSERT OR DIE IF FILE DOES NOT EXIST

	fseek( filePath , 0 , SEEK_END );
	long fileSize = ftell( filePath );

	uchar* buffer = new uchar[ fileSize ];
	if ( nullptr != buffer )
	{
		fseek( filePath , 0 , SEEK_SET );
		size_t bytesRead = fread( buffer , 1 , fileSize , filePath );
		//buffer bytesRead = NULL;
		UNUSED( bytesRead );
	}
	
	if ( out_size != nullptr )
	{
		*out_size = ( size_t ) fileSize;
	}

	fclose( filePath );
	return buffer;
}

ShaderStage::ShaderStage()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

ShaderStage::~ShaderStage()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Shader::CreateFromFile( RenderContext* ctx , std::string const& filename )
{
	UNUSED( ctx );
	size_t fileSize = 0;
	void* source = FileReadToNewBuffer( filename, &fileSize );
	if ( nullptr == source )
	{
		return false;
	}

	delete[] source;

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------