#pragma once
#include "Engine/Primitives/AABB2.hpp"
#include "Engine/Primitives/Disc2D.hpp"
#include "Engine/Primitives/OBB2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/EngineCommon.hpp"

//-----------------------------------------------------------------------------------------------------------------
// FORAWRD TYPE DECLARATIONS
//-----------------------------------------------------------------------------------------------------------------

class Polygon2D;
struct Vec2;
struct Vec3;
struct Vec4;

//-----------------------------------------------------------------------------------------------------------------
// ANGLE UTILITIES
//-----------------------------------------------------------------------------------------------------------------

float ConvertDegreesToRadians( float degrees );
float ConvertRadiansToDegrees( float radians );
float CosDegrees( float degrees );
float SinDegrees( float degrees );
float TanDegrees( float degrees );
float Atan2Degrees( float y, float x);

//-----------------------------------------------------------------------------------------------------------------
// BASIC 2D & 3D UTILITIES
//-----------------------------------------------------------------------------------------------------------------

float GetDistance2D( const Vec2& positionA, const Vec2& positionB );
float GetDistanceSquared2D( const Vec2& positionA, const Vec2& positionB );
float GetDistance3D( const Vec3& positionA, const Vec3& positionB );
float GetDistanceSquared3D( const Vec3& positionA, const Vec3& positionB );
float GetDistanceXY3D( const Vec3& positionA, const Vec3& positionB );
float GetDistanceXYSquared3D( const Vec3& positionA, const Vec3& positionB );
int	  GetTaxiCabLengthBetweenTwoPoints( const IntVec2& start , const IntVec2& finish );


//-----------------------------------------------------------------------------------------------------------------
// GEOMETRIC QUERY UTILITIES ( PRIMITIVES )
//-----------------------------------------------------------------------------------------------------------------

FloatRange GetRangeOnProjectedAxis( int numPoints , const Vec2* points , const Vec2* relativePos , const Vec2& axisNormal );

bool  DoDiscsOverlap( const Vec2& CenterA, float RadiusA, const Vec2& CenterB, float RadiusB );
bool  DoSpheresOverlap( const Vec3& CenterA, float RadiusA, const Vec3& CenterB, float RadiusB );
bool  DoAABBsOverlap( const AABB2& first , const AABB2& second );
bool  DoDiscAndAABBOverlap( const Vec2& center, const float radius, const AABB2& box );
bool  DoOBBsOverlap( const OBB2& first, const OBB2& second );

void  PushDiscOutOfAABB( Vec2& center , const float& radius ,const AABB2& box );
void  PushDiscOutOfPoint( Vec2& center , const float& radius , const Vec2& point );
void  PushMobileDiscOutOfFixedDisc( Vec2& MobileDiscCenter , const float& MobileDiscRadius , const Vec2& FixedDiscCenter , const float& FixedDiscRadius );
void  PushMobileDiscOutOfEachOther( Vec2& MobileDiscCenter1 ,  float MobileDiscRadius1 , Vec2& MObileDiscCenter2 ,  float MobileDiscRadius2 );

float GetAngularDistance( float startDegrees, float endDegrees );
float GetShortestAngularDisplacement( float startDegrees, float endDegrees );

bool  IsPointOnDisc2D( const Disc2D& disc , const Vec2& point );
bool  IsPointOnDisc2D( const Vec2& point , const Vec2& center , const float radius );
const Vec2  GetNearestPointOnDisc2D( const Disc2D& disc , const Vec2& point );
const Vec2  GetNearestPointOnDisc2D( const Vec2& point , const Vec2& center , const float radius );
const Vec2  GetNearestPointOnCircumferenceOfDisc2D( const Disc2D& disc , const Vec2& point );
const Vec2  GetNearestPointOnCircumferenceOfDisc2D( const Vec2& point , const Vec2& center , const float radius );

const Vec2  GetNearestPointOnAABB2D( const Vec2& referencePoint , const AABB2& box );
const Vec2  GetNearestPointOnInfiniteLine2D( const Vec2& referencePoint, const Vec2& somePointOnLine, const Vec2& anotherPointOnLine );
const Vec2  GetNearestPointOnLineSegment2D( const Vec2& referencePoint , const Vec2& startPoint , const Vec2& endPoint );
const Vec2  GetNearestPointOnCapsule2D( const Vec2& referencePoint , const Vec2& capsuleMidStart , const Vec2& capsuleMidEnd , const float capsuleRadius );
const Vec2  GetNearestPointOnOBB2D( const Vec2& referencePoint , const OBB2& box );

bool  IsPointOnCircumferenceOfDisc2D( const Disc2D& disc , const Vec2& point ); 
bool  IsPointOnCircumferenceOfDisc2D( const Vec2& point , const Vec2& center , const float radius );
bool  IsPointInsideAABB2D( const Vec2& referencePoint , const AABB2& box );
bool  IsPointInsideCapsule2D( const Vec2& referencePoint , const Vec2& capsuleMidStart , const Vec2& capsuleMidEnd , const float capsuleRadius );
bool  IsPointInsideOBB2D( const Vec2& referencePoint , const OBB2& box );
bool  IsPointInSector( const Vec2& pointOfObservation , const Vec2& observerLocation , const float forwardDegrees, const float apertureDegrees, const float maxDistance );
//bool  IsPointInSector( const Vec2& referencePoint, Vec2& center, const float& maxDistance/*radius*/, const Vec2& forwardDirection, const float& apertureDegrees );

//-----------------------------------------------------------------------------------------------------------------
// TRANFORM UTILITIES
//-----------------------------------------------------------------------------------------------------------------

const Vec2 TransformPosition2D( const Vec2& position, float UniformScale, float RotationDegrees, const Vec2& Translation );
const Vec2 TransformPosition2D( const Vec2& position , const Vec2& iBasis , const Vec2& jBasis , const Vec2& Translation );
const Vec3 TransformPosition3DXY( const Vec3& position, float ScaleXY, float ZRotationDegrees, const Vec2& TranslationXY );
const Vec3 TransformPosition3DXY( const Vec3& position , const Vec2& iBasisXY , const Vec2& jBasisXY , const Vec2& TranslationXY );

//-----------------------------------------------------------------------------------------------------------------

float Interpolate( float inBegin, float inEnd, float fractionOfInEnd );
float RangeMapFloat( float inBegin , float inEnd , float outBegin , float outEnd , float inValue );
float Clamp( float value , float min , float max );
float ClampZeroToOne( float value );
float Round( float value );
int	  RoundDownToInt( float value );
int   RoundToNearestInt( float value );
float GetFractionPart( float value );

//-----------------------------------------------------------------------------------------------------------------
//	GEOMETRIC VECTOR UTILITIES
//-----------------------------------------------------------------------------------------------------------------

float      GetTurnedToward( float currentDegrees, float goalDegrees, float maxDeltaDegrees );
float      DotProduct2D(const Vec2& first, const Vec2& second);
float      DotProduct3D( const Vec3& first , const Vec3& second );
float      DotProduct4D( const Vec4& first , const Vec4& second );
float      CrossProduct2D(const Vec2& first, const Vec2& second);
Vec3       CrossProduct3D(const Vec3& first, const Vec3& second);
float      GetProjectedLength2D( const Vec2& sourceVector , const Vec2& ontoVector );
const Vec2 GetProjectedOnto2D( const Vec2& sourceVector , const Vec2& ontoVector );
float	   GetAngleDegreesBetweenVectors2D( const Vec2& vectorA , const Vec2& vectorB );

// Triple Product = ( b * a Dot C )- ( a * b Dot C ) or a Cross ( B cross C ) = ( a Dot c ) * b - ( a Dot b ) c
const Vec2 TripleProduct( const Vec2& a , const Vec2& b , const Vec2& c );
const Vec2 AveragePoint( const Vec2* vertices , size_t count );
float	   GetFractionInRange(); // TO DO

//-----------------------------------------------------------------------------------------------------------------
//	STEPPING/EASING UTILITIES
//-----------------------------------------------------------------------------------------------------------------

float SmoothStart2( float t );
float SmoothStart3( float t );
float SmoothStart4( float t );
float SmoothStart5( float t );
float SmoothStop2( float t );
float SmoothStop3( float t );
float SmoothStop4( float t );
float SmoothStop5( float t );
float SmoothStep3( float t );

//--------------------------------------------------------------------------------------------------------------------------------------------
// POLYGON UTILS
//--------------------------------------------------------------------------------------------------------------------------------------------

const Vec2* GetLeftMostPointFromPointCloud( Vec2 const* points , uint pointCount );
const uint	GetIndexOfLeftMostPointFromPointCloud( Vec2 const* points , uint pointCount );
const Vec2* GetBottomMostPointFromPointCloud( Vec2 const* points , uint pointCount );
const uint	GetIndexOfBottomMostPointFromPointCloud( Vec2 const* points , uint pointCount );
const Vec2* GetRightMostPointFromPointCloud( Vec2 const* points , uint pointCount );
const uint	GetIndexOfRightMostPointFromPointCloud( Vec2 const* points , uint pointCount );
const Vec2* GetTopMostPointFromPointCloud( Vec2 const* points , uint pointCount );
const uint	GetIndexOfTopMostPointFromPointCloud( Vec2 const* points , uint pointCount );

// Get furthest vertex along a certain direction
size_t GetIndexOfFurthestPoint( const Vec2* vertices , size_t count , Vec2 direction );

//--------------------------------------------------------------------------------------------------------------------------------------------

float SignFloat( float val );

float CalculateAreaOfTriangle( const Vec2& vertex0 , const Vec2& vertex1 , const Vec2& vertex2 );
float CalculateAreaOfPolygon( const Polygon2D&  polygon );

//--------------------------------------------------------------------------------------------------------------------------------------------
// Minkowski sum support function for GJK

const Vec2 MinkowskiSumSupport( const Vec2* vertices1 , const size_t count1 , const Vec2* vertices2 , const size_t count2 , const Vec2 direction );
const bool GJKPolygonPolygonIntersectionTest( const Vec2* vertices1 , const size_t count1 , const Vec2* vertices2 , const size_t count2 );
const bool GJKPolygonPolygonIntersectionTest( const Polygon2D polygon1 , const Polygon2D polygon2 );
const Polygon2D GenerateMinkowskiDifferencePolygon( const Polygon2D* poly1 , const Polygon2D* poly2 );