#include "Engine/Core/OBJUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Primitives/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/VertexMaster.hpp"
#include <fstream>

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

Strings GetTrimmedStrings( std::string& stringData )
{
	Strings temp = SplitStringAtGivenDelimiter( stringData , ' ' );
	Strings toReturn;

	for ( int index = 0; index < temp.size(); index++ )
	{
		if ( temp[ index ] != "" )
		{
			toReturn.push_back( temp[ index ] );
		}
	}
	return toReturn;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ParseObjFile( std::string filePath , std::vector<std::string>& out )
{
	std::ifstream file;
	std::string line;
	file.open( filePath );

	if ( file.is_open() )
	{
		while ( getline( file , line ) )
		{
			out.push_back( line );
		}
		file.close();
	}
	else
	{
		DebuggerPrintf( "Error in reading file" );
		DEBUGBREAK();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GPUMesh* LoadObjFileIntoGpuMesh( MeshBuilderOptions options , std::string objFilePath )
{
	/*std::vector<std::string> linesOfObjFile;
	ParseObjFile( objFilePath , linesOfObjFile );

	std::vector<Vec3>	postions;
	std::vector<Vec3>	normals;
	std::vector<Vec2>	uvs;
	std::vector<int>	indexForVert;
	std::vector<int>	indexForNormal;
	std::vector<int>	indexForTex;

	for ( long index = 0; index < linesOfObjFile.size(); index++ )
	{
		if ( linesOfObjFile[ index ] == "" )
		{
			continue;
		}

		Strings trimedString = GetTrimmedStrings( linesOfObjFile[ index ] );

		if ( trimedString[ 0 ] == "#" )
		{
			continue;
		}

		if ( trimedString[ 0 ] == "v" )
		{
			Vec3 vec;
			vec.x = ( float ) atof( trimedString[ 1 ].c_str() );
			vec.y = ( float ) atof( trimedString[ 2 ].c_str() );
			vec.z = ( float ) atof( trimedString[ 3 ].c_str() );
			postions.push_back( vec );
		}

		if ( trimedString[ 0 ] == "vn" )
		{
			Vec3 vec;
			vec.x = ( float ) atof( trimedString[ 1 ].c_str() );
			vec.y = ( float ) atof( trimedString[ 2 ].c_str() );
			vec.z = ( float ) atof( trimedString[ 3 ].c_str() );
			normals.push_back( vec );
		}

		if ( trimedString[ 0 ] == "vt" )
		{
			Vec2 vec;
			vec.x = ( float ) atof( trimedString[ 1 ].c_str() );
			vec.y = ( float ) atof( trimedString[ 2 ].c_str() );
			uvs.push_back( vec );
		}

		if ( trimedString[ 0 ] == "f" )
		{
			if ( trimedString.size() == 5 )
			{
				Strings t1 = SplitStringAtGivenDelimiter( trimedString[ 1 ] , '/' );
				indexForVert.push_back( atoi( t1[ 0 ].c_str() ) );
				indexForTex.push_back( atoi( t1[ 1 ].c_str() ) );
				indexForNormal.push_back( atoi( t1[ 2 ].c_str() ) );

				Strings t2 = SplitStringAtGivenDelimiter( trimedString[ 2 ] , '/' );
				indexForVert.push_back( atoi( t2[ 0 ].c_str() ) );
				indexForTex.push_back( atoi( t2[ 1 ].c_str() ) );
				indexForNormal.push_back( atoi( t2[ 2 ].c_str() ) );

				Strings t3 = SplitStringAtGivenDelimiter( trimedString[ 3 ] , '/' );
				indexForVert.push_back( atoi( t3[ 0 ].c_str() ) );
				indexForTex.push_back( atoi( t3[ 1 ].c_str() ) );
				indexForNormal.push_back( atoi( t3[ 2 ].c_str() ) );

				Strings t4 = SplitStringAtGivenDelimiter( trimedString[ 4 ] , '/' );
				indexForVert.push_back( atoi( t4[ 0 ].c_str() ) );
				indexForTex.push_back( atoi( t4[ 1 ].c_str() ) );
				indexForNormal.push_back( atoi( t4[ 2 ].c_str() ) );
			}
			else if ( trimedString.size() == 4 )
			{
				Strings t1 = SplitStringAtGivenDelimiter( trimedString[ 1 ] , '/' );
				indexForVert.push_back( atoi( t1[ 0 ].c_str() ) );
				indexForTex.push_back( atoi( t1[ 1 ].c_str() ) );
				indexForNormal.push_back( atoi( t1[ 2 ].c_str() ) );

				Strings t2 = SplitStringAtGivenDelimiter( trimedString[ 2 ] , '/' );
				indexForVert.push_back( atoi( t2[ 0 ].c_str() ) );
				indexForTex.push_back( atoi( t2[ 1 ].c_str() ) );
				indexForNormal.push_back( atoi( t2[ 2 ].c_str() ) );

				Strings t3 = SplitStringAtGivenDelimiter( trimedString[ 3 ] , '/' );
				indexForVert.push_back( atoi( t3[ 0 ].c_str() ) );
				indexForTex.push_back( atoi( t3[ 1 ].c_str() ) );
				indexForNormal.push_back( atoi( t3[ 2 ].c_str() ) );
			}
			else
			{
				DebuggerPrintf( "Error in obj definition" );
				DEBUGBREAK();
			}
		}
	}
	GPUMesh* mesh = new GPUMesh( g_theRenderer );
	std::vector<VertexMaster> finalVerts;

	for ( int index = 0; index < indexForVert.size(); index++ )
	{
		VertexMaster vert;
		int positionIndex			= indexForVert[ index ] - 1;
		int normalIndex				= indexForNormal[ index ] - 1;
		int uvIndex					= indexForTex[ index ] - 1;

		vert.m_position				= postions[ positionIndex ];
		vert.m_normal				= normals[ normalIndex ];
		vert.m_color				= Rgba8( 255 , 255 , 255 , 255 );
		vert.m_normalizedColor		= vert.m_color.GetAsNormalizedFloat4();
		vert.m_uvTexCoords			= uvs[ uvIndex ];

		finalVerts.push_back( vert );
	}
	mesh->UpdateVertices( ( unsigned int ) finalVerts.size() , &finalVerts[ 0 ] );
	return mesh;*/

	std::vector<std::string> linesOfObjFile;
	ParseObjFile( objFilePath , linesOfObjFile );

	std::vector<Vec3>					postions;
	std::vector<Vec3>					normals;
	std::vector<Vec2>					uvs;

	GPUMesh*							mesh = new GPUMesh( g_theRenderer );
	
	std::vector<VertexMaster>			finalVerts;
	std::vector<std::vector<int>>		iForVerts;
	std::vector<std::vector<int>>		iForNormals;
	std::vector<std::vector<int>>		iForTex;
	
	for ( long i = 0; i < linesOfObjFile.size(); i++ )
	{
		if ( linesOfObjFile[ i ] == "" )
		{
			continue;
		}

		Strings s = GetTrimmedStrings( linesOfObjFile[ i ] );

		if ( s[ 0 ] == "#" )
		{
			continue;
		}
		if ( s[ 0 ] == "v" )
		{
			Vec3 vec;
			vec.x = ( float ) atof( s[ 1 ].c_str() );
			vec.y = ( float ) atof( s[ 2 ].c_str() );
			vec.z = ( float ) atof( s[ 3 ].c_str() );
			postions.push_back( vec );
		}
		if ( s[ 0 ] == "vn" )
		{
			Vec3 vec;
			vec.x = ( float ) atof( s[ 1 ].c_str() );
			vec.y = ( float ) atof( s[ 2 ].c_str() );
			vec.z = ( float ) atof( s[ 3 ].c_str() );
			normals.push_back( vec );
		}
		if ( s[ 0 ] == "vt" )
		{
			Vec2 vec;
			vec.x = ( float ) atof( s[ 1 ].c_str() );
			vec.y = ( float ) atof( s[ 2 ].c_str() );
			uvs.push_back( vec );
		}
		if ( s[ 0 ] == "f" )
		{
			if ( s.size() == 5 )
			{
				Strings t1 = SplitStringAtGivenDelimiter( s[ 1 ] , '/' );
				Strings t2 = SplitStringAtGivenDelimiter( s[ 2 ] , '/' );
				Strings t3 = SplitStringAtGivenDelimiter( s[ 3 ] , '/' );
				Strings t4 = SplitStringAtGivenDelimiter( s[ 4 ] , '/' );
				std::vector<int> x1;
				std::vector<int> x2;
				std::vector<int> x3;
				x1.push_back( atoi( t1[ 0 ].c_str() ) );
				x1.push_back( atoi( t2[ 0 ].c_str() ) );
				x1.push_back( atoi( t3[ 0 ].c_str() ) );
				x1.push_back( atoi( t4[ 0 ].c_str() ) );

				x2.push_back( atoi( t1[ 1 ].c_str() ) );
				x2.push_back( atoi( t2[ 1 ].c_str() ) );
				x2.push_back( atoi( t3[ 1 ].c_str() ) );
				x2.push_back( atoi( t4[ 1 ].c_str() ) );
				x3.push_back( atoi( t1[ 2 ].c_str() ) );
				x3.push_back( atoi( t2[ 2 ].c_str() ) );
				x3.push_back( atoi( t3[ 2 ].c_str() ) );
				x3.push_back( atoi( t4[ 2 ].c_str() ) );
				iForVerts.push_back( x1 );
				iForTex.push_back( x2 );
				iForNormals.push_back( x3 );
			}
			else if ( s.size() == 4 )
			{
				Strings t1 = SplitStringAtGivenDelimiter( s[ 1 ] , '/' );
				Strings t2 = SplitStringAtGivenDelimiter( s[ 2 ] , '/' );
				Strings t3 = SplitStringAtGivenDelimiter( s[ 3 ] , '/' );
				std::vector<int> x1;
				std::vector<int> x2;
				std::vector<int> x3;
				x1.push_back( atoi( t1[ 0 ].c_str() ) );
				x1.push_back( atoi( t2[ 0 ].c_str() ) );
				x1.push_back( atoi( t3[ 0 ].c_str() ) );

				x2.push_back( atoi( t1[ 1 ].c_str() ) );
				x2.push_back( atoi( t2[ 1 ].c_str() ) );
				x2.push_back( atoi( t3[ 1 ].c_str() ) );

				x3.push_back( atoi( t1[ 2 ].c_str() ) );
				x3.push_back( atoi( t2[ 2 ].c_str() ) );
				x3.push_back( atoi( t3[ 2 ].c_str() ) );

				iForVerts.push_back( x1 );
				iForTex.push_back( x2 );
				iForNormals.push_back( x3 );

			}
			else
			{
				DebuggerPrintf( "Error in obj definition" );
				DEBUGBREAK();
			}
		}
	}
	for ( int i = 0; i < iForVerts.size(); i++ )
	{
		if ( iForVerts[ i ].size() == 4 )
		{
			VertexMaster v1;
			VertexMaster v2;
			VertexMaster v3;
			VertexMaster v4;
			v1.m_normalizedColor = Rgba8( 255 , 255 , 255 , 255 ).GetAsNormalizedFloat4();
			v2.m_normalizedColor = Rgba8( 255 , 255 , 255 , 255 ).GetAsNormalizedFloat4();
			v3.m_normalizedColor = Rgba8( 255 , 255 , 255 , 255 ).GetAsNormalizedFloat4();
			v4.m_normalizedColor = Rgba8( 255 , 255 , 255 , 255 ).GetAsNormalizedFloat4();
			v1.m_position = postions[ ( iForVerts[ i ][ 0 ] ) - 1 ];
			v2.m_position = postions[ ( iForVerts[ i ][ 1 ] ) - 1 ];
			v3.m_position = postions[ ( iForVerts[ i ][ 2 ] ) - 1 ];
			v4.m_position = postions[ ( iForVerts[ i ][ 3 ] ) - 1 ];
			v1.m_normal = normals[ ( iForNormals[ i ][ 0 ] ) - 1 ];
			v2.m_normal = normals[ ( iForNormals[ i ][ 1 ] ) - 1 ];
			v3.m_normal = normals[ ( iForNormals[ i ][ 2 ] ) - 1 ];
			v4.m_normal = normals[ ( iForNormals[ i ][ 3 ] ) - 1 ];
			v1.m_uvTexCoords = uvs[ ( iForTex[ i ][ 0 ] ) - 1 ];
			v2.m_uvTexCoords = uvs[ ( iForTex[ i ][ 1 ] ) - 1 ];
			v3.m_uvTexCoords = uvs[ ( iForTex[ i ][ 2 ] ) - 1 ];
			v4.m_uvTexCoords = uvs[ ( iForTex[ i ][ 3 ] ) - 1 ];
			finalVerts.push_back( v1 );
			finalVerts.push_back( v2 );
			finalVerts.push_back( v3 );
			finalVerts.push_back( v1 );
			finalVerts.push_back( v3 );
			finalVerts.push_back( v4 );
		}
		else if ( iForVerts[ i ].size() == 3 )
		{
			VertexMaster v1;
			VertexMaster v2;
			VertexMaster v3;
			v1.m_normalizedColor = Rgba8( 255 , 255 , 255 , 255 ).GetAsNormalizedFloat4();
			v2.m_normalizedColor = Rgba8( 255 , 255 , 255 , 255 ).GetAsNormalizedFloat4();
			v3.m_normalizedColor = Rgba8( 255 , 255 , 255 , 255 ).GetAsNormalizedFloat4();
			v1.m_position = postions[ ( iForVerts[ i ][ 0 ] ) - 1 ];
			v2.m_position = postions[ ( iForVerts[ i ][ 1 ] ) - 1 ];
			v3.m_position = postions[ ( iForVerts[ i ][ 2 ] ) - 1 ];
			v1.m_normal = normals[ ( iForNormals[ i ][ 0 ] ) - 1 ];
			v2.m_normal = normals[ ( iForNormals[ i ][ 1 ] ) - 1 ];
			v3.m_normal = normals[ ( iForNormals[ i ][ 2 ] ) - 1 ];
			v1.m_uvTexCoords = uvs[ ( iForTex[ i ][ 0 ] ) - 1 ];
			v2.m_uvTexCoords = uvs[ ( iForTex[ i ][ 1 ] ) - 1 ];
			v3.m_uvTexCoords = uvs[ ( iForTex[ i ][ 2 ] ) - 1 ];
			finalVerts.push_back( v1 );
			finalVerts.push_back( v2 );
			finalVerts.push_back( v3 );
		}

	}
	mesh->UpdateVertices( ( unsigned int ) finalVerts.size() , &finalVerts[ 0 ] );
	return mesh;
}

//--------------------------------------------------------------------------------------------------------------------------------------------