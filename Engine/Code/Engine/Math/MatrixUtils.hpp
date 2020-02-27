#pragma once
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/Vec3.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

const Mat44		CreateOrthoGraphicProjeciton( const Vec3& min , const Vec3& max );
const Mat44		CreatePerpsectiveProjectionMatrixD3D( float fovDegrees , float aspectRatio , float nearZ , float farZ );
	  void		MatrixTranspose( Mat44& mat );
	  Mat44		MatrixInvertOrthoNormal( Mat44& mat );
	  void		MatrixInvert( Mat44& mat );
	  bool		IsMatrixOrtonormal( Mat44& mat );
//--------------------------------------------------------------------------------------------------------------------------------------------