#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include <vector>
#include "Engine/Primitives/AABB3.hpp"

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

//--------------------------------------------------------------------------------------------------------------------------------------------