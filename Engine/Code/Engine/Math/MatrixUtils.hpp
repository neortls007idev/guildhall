#pragma once
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/Vec3.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

const Mat44 CreateOrthoGraphicProjeciton( const Vec3& min , const Vec3& max );
const Mat44 MakePerpsectiveProjectionMatrixD3D( float fovDegrees , float aspectRatio , float nearZ , float farZ );
void MatrixTranspose( Mat44& mat );
void MatrixInvertOrthoNormal( Mat44& mat );
void MatrixInvert( Mat44& mat );

//--------------------------------------------------------------------------------------------------------------------------------------------