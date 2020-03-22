#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include <vector>
#include "Engine/Primitives/AABB3.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Primitives/Polygon2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

void TransformVertexArray2D(int NumberOfVertices, Vertex_PCU* verts, float scale, float Orientation, Vec2 translation);

//--------------------------------------------------------------------------------------------------------------------------------------------
// APPEND FUNCITONS
//--------------------------------------------------------------------------------------------------------------------------------------------

void AppendLine2( std::vector<Vertex_PCU>& vertexArray , const Vec2& start , const Vec2& end , const Rgba8 color , const float thickness );
void AppendVertsForAABB2( std::vector<Vertex_PCU>& vertexArray , const AABB2& box , const Rgba8& tint );
void AppendVertsForAABB2( std::vector<Vertex_PCU>& vertexArray , const AABB2& box , const Rgba8& tint, const Vec2& uvAtMins, const Vec2& uvAtMaxs );

void AppendVertsForAABB3( std::vector<Vertex_PCU>& vertexArray , const AABB3& box , const Rgba8& tint );
void AppendVertsForAABB3( std::vector<Vertex_PCU>& vertexArray , const AABB3& box , const Rgba8& tint , const Vec2& uvAtMins , const Vec2& uvAtMaxs );

void AppendVertsForPolygon( std::vector<Vertex_PCU>& vertexArray , const Vec2* points , const unsigned int count , const Rgba8& tint );
void RotateDegreesPolygonAboutPoint( Polygon2D& polygon , Vec2 worldPosition , float orientationDegrees );

void AddCubeVerts( std::vector<Vertex_PCU>& cubeVertexArray , const Rgba8* tint );
uint* GetCubeIndices();

void CreateUVSphere( uint hCuts , uint vCuts , std::vector<Vertex_PCU>& sphereMeshVerts , std::vector<uint>& sphereIndices, float radius = 1.f , Vec3 center = Vec3::ZERO , const Rgba8& tint = WHITE );
//--------------------------------------------------------------------------------------------------------------------------------------------
