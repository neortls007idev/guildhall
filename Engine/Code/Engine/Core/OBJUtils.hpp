#pragma once
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Core/StringUtils.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class GPUMesh;

//--------------------------------------------------------------------------------------------------------------------------------------------

struct MeshBuilderOptions
{
	Mat44		transform					= Mat44::IDENTITY;									// what space transform to apply to the object
	bool		invertV						= false;											// Required - inverts V coordinate (v = 1.0f - v)
	bool		invertWindingOrder			= false;											// Required - Change the winding order of all faces
	bool		generateNormals				= false;											// Required - Generate normals for the surface if they weren't in the file
	bool		generateTangents			= false;											// Required - Generate tangents for the surface if they werent' in the file
	bool		clean						= false;											// Extra    - Convert a vertex array to an index vertex array by removing duplicates
};

//--------------------------------------------------------------------------------------------------------------------------------------------

Strings				GetTrimmedStrings( std::string& stringData );								// TODO :- MOVE TO STRING UTILS
void				ParseObjFile( std::string filePath , std::vector<std::string>& out );
GPUMesh*			LoadObjFileIntoGpuMesh( MeshBuilderOptions options , std::string objFilePath );
//--------------------------------------------------------------------------------------------------------------------------------------------