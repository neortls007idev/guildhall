#include "Engine/Math/MathUtils.hpp"
#include <cmath>
#include "Engine/Math/MathConstants.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"



//--------------------------------------------------------------------------------------------------------------------------------------------
// ANGLE UTILITIES
//--------------------------------------------------------------------------------------------------------------------------------------------

float ConvertDegreesToRadians( float degrees )
{
	return ( degrees * ( PI /180.f));
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float ConvertRadiansToDegrees( float radians )
{
	return ( radians * 180.f * PIINV );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float CosDegrees( float degrees )
{
	float radians = ConvertDegreesToRadians(degrees);
	return ( cos(radians) );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

float SinDegrees( float degrees )
{
	float radians = ConvertDegreesToRadians( degrees );
	return ( sin( radians ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float TanDegrees( float degrees )
{
	float radians = ConvertDegreesToRadians( degrees );
	return  tanf( radians );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float Atan2Degrees( float y, float x )
{
	return( ConvertRadiansToDegrees(atan2f(y,x)));
}

//--------------------------------------------------------------------------------------------------------------------------------------------
// BASIC 2D & 3D UTILITIES
//--------------------------------------------------------------------------------------------------------------------------------------------

float GetDistance2D( const Vec2& positionA, const Vec2& positionB )
{
	float DifferenceInX = ( positionB.x - positionA.x );
	float DifferenceInY = ( positionB.y - positionA.y );
	float FinalDistance = sqrtf( ( DifferenceInX * DifferenceInX ) + ( DifferenceInY * DifferenceInY ) );
	return FinalDistance;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float GetDistanceSquared2D( const Vec2& positionA, const Vec2& positionB )
{
	float DifferenceInX = ( positionB.x - positionA.x );
	float DifferenceInY = ( positionB.y - positionA.y );
	float FinalDistance = ( ( DifferenceInX * DifferenceInX) + (DifferenceInY * DifferenceInY ) );
	return FinalDistance;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float GetDistance3D( const Vec3& positionA, const Vec3& positionB )
{
	float DifferenceInX = ( positionB.x - positionA.x );
	float DifferenceInY = ( positionB.y - positionA.y );
	float DifferenceInZ = ( positionB.z - positionA.z );
	float FinalDistance = sqrtf( ( DifferenceInX * DifferenceInX ) + ( DifferenceInY * DifferenceInY ) + ( DifferenceInZ * DifferenceInZ ) );
	return FinalDistance;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float GetDistanceSquared3D( const Vec3& positionA, const Vec3& positionB )
{
	float DifferenceInX = ( positionB.x - positionA.x );
	float DifferenceInY = ( positionB.y - positionA.y );
	float DifferenceInZ = ( positionB.z - positionA.z );
	float FinalDistance = ( ( DifferenceInX * DifferenceInX ) + ( DifferenceInY * DifferenceInY ) + ( DifferenceInZ * DifferenceInZ ) );
	return FinalDistance;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float GetDistanceXY3D( const Vec3& positionA, const Vec3& positionB )
{
	float DifferenceInX = ( positionB.x - positionA.x );
	float DifferenceInY = ( positionB.y - positionA.y );
	float FinalDistance = sqrtf( ( DifferenceInX * DifferenceInX) + ( DifferenceInY * DifferenceInY ) );
	return FinalDistance;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float GetDistanceXYSquared3D( const Vec3& positionA, const Vec3& positionB )
{
	float DifferenceInX = ( positionB.x - positionA.x );
	float DifferenceInY = ( positionB.y - positionA.y );
	float FinalDistance = ( ( DifferenceInX * DifferenceInX ) + ( DifferenceInY * DifferenceInY) );
	return FinalDistance;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int GetTaxiCabLengthBetweenTwoPoints( const IntVec2& start , const IntVec2& finish )
{
	return ( abs( finish.x - start.x ) + abs( finish.y - start.y ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

FloatRange GetRangeOnProjectedAxis( int numPoints , const Vec2* points , const Vec2* relativePos , const Vec2& axisNormal )
{
	std::vector<Vec2> projectedPoints;
	for ( int index = 0; index < numPoints; index++ )
	{
		Vec2 displacementVector = points[ index ] - *relativePos;
		Vec2 projectedPoint = GetProjectedOnto2D( points[ index ] , axisNormal );
		projectedPoints.push_back( projectedPoint );
	}
	float minLength = projectedPoints[ 0 ].GetLength();
	float maxLength = projectedPoints[ projectedPoints.size() - 1 ].GetLength();
	for ( int index = 0; index < numPoints; index++ )
	{
		float length = projectedPoints[ index ].GetLength();
		if ( length < minLength )
		{
			minLength = length;
		}
		if ( length > maxLength )
		{
			maxLength = length;
		}
	}
	return FloatRange( minLength , maxLength );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------------
// GEOMETRIC QUERY UTILITIES
//--------------------------------------------------------------------------------------------------------------------------------------------

bool DoDiscsOverlap( const Vec2& CenterA, float RadiusA, const Vec2& CenterB, float RadiusB )
{
	float DistanceBetweenCenters = GetDistance2D( CenterB, CenterA );
	bool Is_Overlapping = false;
	if ( (DistanceBetweenCenters - RadiusB - RadiusA) <= 0 )
	{
		Is_Overlapping = true;
	}
	return Is_Overlapping;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool DoSpheresOverlap( const Vec3& CenterA, float RadiusA, const Vec3& CenterB, float RadiusB )
{
	float DistanceBetweenCenters = GetDistance3D( CenterB, CenterA );
	bool Is_Overlapping = false;
	if( (DistanceBetweenCenters - RadiusB - RadiusA) < 0 )
	{
		Is_Overlapping = true;
	}
	return Is_Overlapping;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool DoDiscAndAABBOverlap( const Vec2& center , const float radius , const AABB2& box )
{
	Vec2 displacementBetweenCenters ;
	Vec2 dimensionsOfAABB2D = box.GetDimensions();

// COLLAPSING THE AABB2 AND THE CIRCLE TO THE FIRST QUADRANT

	displacementBetweenCenters.x = abs( center.x - box.GetCenter().x );
	displacementBetweenCenters.y = abs( center.y - box.GetCenter().y );

// FOR THE EDGES OF THE AABB2

	if ( displacementBetweenCenters.x > ( ( dimensionsOfAABB2D.x * 0.5f ) + radius ) ) { return false; }
	if ( displacementBetweenCenters.y > ( ( dimensionsOfAABB2D.y * 0.5f ) + radius ) ) { return false; }

	if ( displacementBetweenCenters.x <= ( dimensionsOfAABB2D.x * 0.5f ) ) { return true; }
	if ( displacementBetweenCenters.y <= ( dimensionsOfAABB2D.y * 0.5f ) ) { return true; }

	float cornerDistanceSquared = ( displacementBetweenCenters.x - ( dimensionsOfAABB2D.x * 0.5f ) ) * ( displacementBetweenCenters.x - ( dimensionsOfAABB2D.x * 0.5f ) ) +
								  ( displacementBetweenCenters.y - ( dimensionsOfAABB2D.y * 0.5f ) ) * ( displacementBetweenCenters.y - ( dimensionsOfAABB2D.y * 0.5f ) );

	return ( cornerDistanceSquared <= ( radius * radius ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool DoOBBsOverlap( const OBB2& first , const OBB2& second )
{
	bool result = false;

	Vec2 firstOBBVerts[ 4 ];
	Vec2 secondOBBVerts[ 4 ];

	first.GetCornerPositions( firstOBBVerts );
	second.GetCornerPositions( secondOBBVerts );

	for ( int index = 0; index < 2 ; index++ )
	{
		Vec2	   axisToCheck = first.GetIBasisNormal();
		FloatRange firstOBBProjectedRange = FloatRange( -first.m_halfDimensions.x , first.m_halfDimensions.x );

		if ( index == 1 )
		{
			axisToCheck = first.GetJBasisNormal();
			firstOBBProjectedRange = FloatRange( -first.m_halfDimensions.y , first.m_halfDimensions.y );
		}

		FloatRange SecondOBBProjectedRange = GetRangeOnProjectedAxis( 4 , secondOBBVerts , &first.m_center , axisToCheck );

		result = firstOBBProjectedRange.DoesOverlap( SecondOBBProjectedRange );

		if ( !result )
		{
			return result;
		}
	}

	for ( int index = 0; index < 2; index++ )
	{
		Vec2 axisToCheck = second.GetIBasisNormal();
		FloatRange SecondOBBProjectedRange = FloatRange( -second.m_halfDimensions.x , second.m_halfDimensions.x );

		if ( index == 1 )
		{
			axisToCheck = second.GetJBasisNormal();
			SecondOBBProjectedRange = FloatRange( -second.m_halfDimensions.y , second.m_halfDimensions.y );
		}

		FloatRange firstOBBProjectedRange = GetRangeOnProjectedAxis( 4 , firstOBBVerts , &second.m_center , axisToCheck );

		result = SecondOBBProjectedRange.DoesOverlap( firstOBBProjectedRange );

		if ( !result )
		{
			return result;
		}
	}
	return result;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool IsPointInsideAABB2D( const Vec2& referencePoint , const AABB2& box )
{
// 	if ( referencePoint.x < box.m_mins.x && referencePoint.y < box.m_mins.y )
// 	{
// 		return false;
// 	}
// 	if ( referencePoint.x < box.m_mins.x && referencePoint.y > box.m_maxs.y )
// 	{
// 		return false;
// 	}
// 	if ( referencePoint.x > box.m_maxs.x&& referencePoint.y < box.m_mins.y )
// 	{
// 		return false;
// 	}
// 	if ( referencePoint.x > box.m_maxs.x&& referencePoint.y > box.m_maxs.y )
// 	{
// 		return false;
// 	}
// 	if ( referencePoint.x < box.m_mins.x && referencePoint.y > box.m_mins.y&& referencePoint.y < box.m_maxs.y )
// 	{
// 		return false;
// 	}
// 	if ( referencePoint.x > box.m_maxs.x&& referencePoint.y > box.m_mins.y&& referencePoint.y < box.m_maxs.y )
// 	{
// 		return false;
// 	}
// 	if ( referencePoint.x > box.m_mins.x&& referencePoint.x < box.m_maxs.x && referencePoint.y > box.m_maxs.y )
// 	{
// 		return false;
// 	}
// 	if ( referencePoint.x > box.m_mins.x&& referencePoint.x < box.m_maxs.x && referencePoint.y < box.m_mins.y )
// 	{
// 		return false;
// 	}

	FloatRange rangeOnX = FloatRange( box.m_mins.x , box.m_maxs.x );
	FloatRange rangeOnY = FloatRange( box.m_mins.y , box.m_maxs.y );

	if ( rangeOnX.minimum > rangeOnX.maximum )
	{
		float oldMaximum = 0.f;
		oldMaximum = rangeOnX.maximum;
		float oldMinimum = 0.f;
		oldMinimum = rangeOnX.minimum;
		rangeOnX.minimum = oldMaximum;
		rangeOnX.maximum = oldMinimum;
	}

	if ( rangeOnY.minimum > rangeOnY.maximum )
	{
		float oldMaximum = 0.f;
		oldMaximum = rangeOnY.maximum;
		float oldMinimum = 0.f;
		oldMinimum = rangeOnY.minimum;
		rangeOnY.minimum = oldMaximum;
		rangeOnY.maximum = oldMinimum;
	}

	bool result = false;

	if ( rangeOnX.IsInRange( referencePoint.x ) )
	{
		result = true;
	}
	else
	{
		return false;
	}

	if ( rangeOnY.IsInRange( referencePoint.y ) )
	{
		result = true;
		return result;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool IsPointInsideCapsule2D( const Vec2& referencePoint , const Vec2& capsuleMidStart , const Vec2& capsuleMidEnd , const float capsuleRadius )
{
	OBB2 box = OBB2( ( capsuleMidEnd - capsuleMidStart ) * 0.5f + capsuleMidStart , Vec2( ( capsuleMidEnd - capsuleMidStart ).GetLength() , capsuleRadius * 2.f ) , ( capsuleMidEnd - capsuleMidStart ).GetNormalized() );

	if ( IsPointOnDisc2D( Disc2D( capsuleMidStart , capsuleRadius ) , referencePoint ) )
	{
		return true;
	}
	else if ( IsPointOnDisc2D( Disc2D( capsuleMidEnd , capsuleRadius ) , referencePoint ) )
	{
		return true;
	}
	else if ( IsPointInsideOBB2D( referencePoint , box ) )
	{
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool IsPointInsideOBB2D( const Vec2& referencePoint , const OBB2& box )
{
	Vec2 pointLocalCoords = referencePoint - box.m_center;
	Vec2 jBasis = box.m_iBasis.GetRotated90Degrees();
	float pi = DotProduct2D( pointLocalCoords , box.m_iBasis );
	float pj = DotProduct2D( pointLocalCoords , box.m_iBasis.GetRotated90Degrees() );

	if ( pi > box.m_halfDimensions.x || pi < -box.m_halfDimensions.x || pj > box.m_halfDimensions.y || pj < -box.m_halfDimensions.y )
	{
		return false;
	}
	else
	{
		return true;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool  IsPointInSector( const Vec2& pointOfObservation , const Vec2& observerLocation , const float forwardDegrees , const float apertureDegrees , const float maxDistance )
{
	Vec2 displacementBetweenPointAndObserver = pointOfObservation - observerLocation;
	float diplacementOrientationDegrees = displacementBetweenPointAndObserver.GetAngleDegrees();
	float distanceBetweenPointAndObserver = displacementBetweenPointAndObserver.GetLength();
	if ( distanceBetweenPointAndObserver > maxDistance )
	{
		return false;
	}

	if ( abs(GetShortestAngularDisplacement(forwardDegrees,diplacementOrientationDegrees)) <= apertureDegrees*0.5f )
	{
		return true;
	}

	return false;
}


//--------------------------------------------------------------------------------------------------------------------------------------------
// TRANFORM UTILITIES
//--------------------------------------------------------------------------------------------------------------------------------------------

const Vec2 TransformPosition2D( const Vec2& position, float UniformScale, float RotationDegrees, const Vec2& Translation )
{
	Vec2 Temp = position;
	Temp *= UniformScale;
	Temp.RotateDegrees( RotationDegrees );
	Temp += Translation;
	return ( Temp ) ;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const Vec2 TransformPosition2D( const Vec2& position , const Vec2& iBasis , const Vec2& jBasis , const Vec2& Translation )
{
	Vec2 coordsOnNewCoordinateSystem = Vec2::ZERO;
	coordsOnNewCoordinateSystem.x = ( position.x * iBasis.x ) + ( position.y * jBasis.x ) + Translation.x;
	coordsOnNewCoordinateSystem.y = ( position.x * iBasis.y ) + ( position.y * jBasis.y ) + Translation.y;

	return coordsOnNewCoordinateSystem;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const Vec3 TransformPosition3DXY( const Vec3& position, float ScaleXY, float ZRotationDegrees, const Vec2& TranslationXY )
{
	Vec3 Temp = position;
	Temp.x *= ScaleXY;
	Temp.y *= ScaleXY;
	Vec3 Temp2 = Temp;
	Temp2 = Temp.GetRotatedAboutZDegrees(ZRotationDegrees);
	Temp2.x = Temp2.x + TranslationXY.x;
	Temp2.y = Temp2.y + TranslationXY.y;

	return( Temp2 );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

const Vec3 TransformPosition3DXY( const Vec3& position , const Vec2& iBasisXY , const Vec2& jBasisXY , const Vec2& TranslationXY )
{
	Vec3 coordsOnNewCoordinateSystem = Vec3::ZERO;
	coordsOnNewCoordinateSystem.x = ( position.x * iBasisXY.x ) + ( position.y * jBasisXY.x ) + TranslationXY.x;
	coordsOnNewCoordinateSystem.y = ( position.x * iBasisXY.y ) + ( position.y * jBasisXY.y ) + TranslationXY.y;
	coordsOnNewCoordinateSystem.z = position.z;

	return coordsOnNewCoordinateSystem;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float Interpolate(float inBegin, float inEnd, float fractionOfInEnd)
{
	return inBegin + fractionOfInEnd * (inEnd - inBegin);
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float RangeMapFloat(float inBegin, float inEnd, float outBegin, float outEnd, float inValue)
{
	float inDisplacement  = inValue - inBegin;
	float inRange		  = inEnd - inBegin;
	float fraction		  = inDisplacement / inRange;
	float outRange		  = outEnd - outBegin;
	float outDisplacement = fraction * outRange;
	float outValue		  = outBegin + outDisplacement;

	return outValue;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float Clamp( float value, float min, float max )
{
	if ( value < min )
	{
		return min;
	}
	if (value > max)
	{
		return max;
	}
	else
	{
		return value;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float ClampZeroToOne( float value )
{
	if ( value < 0.f )
	{
		return 0.f;
	}
	if ( value > 1.f )
	{
		return 1.f;
	}
	else
	{
		return value;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float Round( float value )
{
	return value;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int RoundDownToInt( float value )
{
	if ( value >= 0 )
	{
		return static_cast< int >( value );
	}
	else
	{
		return static_cast< int >( value - 1 );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int RoundToNearestInt( float value )
{
	return ( int ) value;
}

float GetFractionPart( float value )
{
	int integerPart = RoundToNearestInt( value );
	return value - integerPart;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float GetTurnedToward( float currentDegrees , float goalDegrees , float maxDeltaDegrees )
{
	float ShortestAngularDisplacement = GetShortestAngularDisplacement( currentDegrees , goalDegrees );

	if ( abs(ShortestAngularDisplacement) < maxDeltaDegrees)
	{
		return goalDegrees;
	}
	else
	{
		ShortestAngularDisplacement = Clamp( ShortestAngularDisplacement , -maxDeltaDegrees , +maxDeltaDegrees );
		return currentDegrees + ShortestAngularDisplacement;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float DotProduct2D( const Vec2& first , const Vec2& second )
{
	// a.b = b.a so order doesn't matter
	//float angleBetweenTheVectors = abs(first.GetAngleDegrees() - second.GetAngleDegrees());
	//return( first.GetLength() * second.GetLength() * CosDegrees( angleBetweenTheVectors ) );
	return ( ( first.x * second.x ) + ( first.y * second.y ) );
	/*
		A.B = (ax * bx) + (ay * by);
	*/
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float DotProduct3D( const Vec3& first , const Vec3& second )
{
	return ( ( first.x * second.x ) + ( first.y * second.y ) + ( first.z * second.z ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float DotProduct4D( const Vec4& first , const Vec4& second )
{
	return ( ( first.x * second.x ) + ( first.y * second.y ) + ( first.z * second.z ) + ( first.w * second.w ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float GetProjectedLength2D( const Vec2& sourceVector , const Vec2& ontoVector )
{
	return DotProduct2D( sourceVector , ontoVector.GetNormalized() );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const Vec2 GetProjectedOnto2D( const Vec2& sourceVector , const Vec2& ontoVector )
{
	float newLength = GetProjectedLength2D( sourceVector , ontoVector );
	Vec2 projectedVector = Vec2::MakeFromPolarDegrees( ontoVector.GetAngleDegrees() , newLength );
	return projectedVector;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float GetAngleDegreesBetweenVectors2D( const Vec2& vectorA , const Vec2& vectorB )
{
	return abs( vectorA.GetAngleDegrees() - vectorB.GetAngleDegrees() );
}


//--------------------------------------------------------------------------------------------------------------------------------------------

bool DoAABBsOverlap( const AABB2& first , const AABB2& second )
{
	if ( (second.m_maxs.x < first.m_mins.x && second.m_maxs.y < first.m_mins.y) || (first.m_mins.x < second.m_maxs.x  && first.m_mins.y < second.m_maxs.y) )
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PushDiscOutOfAABB( Vec2& center , const float& radius ,const AABB2& box )
{
	Vec2 nearestPoint = box.GetNearestPoint( center );
	PushDiscOutOfPoint( center , radius , nearestPoint );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PushDiscOutOfPoint( Vec2& center , const float& radius , const Vec2& point )
{
	if ( GetDistance2D( center , point) >= radius )
	{
		return;
	}
	Vec2 displacementBetweenCenterAndPoint = center - point;
	displacementBetweenCenterAndPoint.Normalize();
	Vec2 displacementRadius = displacementBetweenCenterAndPoint * radius;
	Vec2 pushOut = displacementRadius - ( center - point );
	center += pushOut;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PushMobileDiscOutOfFixedDisc( Vec2& MobileDiscCenter , const float& MobileDiscRadius , const Vec2& FixedDiscCenter , const float& FixedDiscRadius )
{
	if ( !DoDiscsOverlap( MobileDiscCenter , MobileDiscRadius , FixedDiscCenter , FixedDiscRadius ) )
	{
		return;
	}
	Vec2 displacementBetweenCenters = Vec2( MobileDiscCenter - FixedDiscCenter );
	float distanceBetweenCenters = displacementBetweenCenters.GetLength();
	float displacementToMove = MobileDiscRadius + FixedDiscRadius - distanceBetweenCenters;

	if ( displacementToMove >= 0 )
	{
		displacementBetweenCenters = Vec2::MakeFromPolarDegrees( displacementBetweenCenters.GetAngleDegrees() , displacementToMove );
		MobileDiscCenter += displacementBetweenCenters;
	}

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PushMobileDiscOutOfEachOther( Vec2& MobileDiscCenter1 ,  float MobileDiscRadius1  , Vec2& MobileDiscCenter2 ,  float MobileDiscRadius2 )
{
	Vec2 displacementBetweenCenters = Vec2( MobileDiscCenter2 - MobileDiscCenter1 );
	float distanceBetweenCenters = displacementBetweenCenters.GetLength();
	float displacementToMove = MobileDiscRadius1  + MobileDiscRadius2 - distanceBetweenCenters;

	if ( ( distanceBetweenCenters - ( MobileDiscRadius1 + MobileDiscRadius2 ) ) <= 0 )
	{
		displacementBetweenCenters.Normalize();
		//displacementBetweenCenters = Vec2::MakeFromPolarDegrees( displacementBetweenCenters.GetAngleDegrees() , displacementToMove ) * 0.5f;
		displacementBetweenCenters = displacementBetweenCenters * displacementToMove * 0.5f;
		MobileDiscCenter1 -= displacementBetweenCenters;
		MobileDiscCenter2 += displacementBetweenCenters;
	}
}

//-----------------------------------------------------------------------------------------------------------------

float GetAngularDistance( float startDegrees , float endDegrees )
{
	return ( endDegrees - startDegrees );
}

//-----------------------------------------------------------------------------------------------------------------

float GetShortestAngularDisplacement( float startDegrees , float endDegrees )
{
		startDegrees = fmod( startDegrees , 360.f );
		endDegrees   = fmod( endDegrees , 360.f );
		float angularDisplacement = endDegrees - startDegrees;
		if ( abs( angularDisplacement ) <= 180.f )
		{
			return angularDisplacement;
		}
		else
		{
			if ( angularDisplacement < 0 )
			{
				return 360.f + angularDisplacement;
			}
			else
			{
				angularDisplacement = angularDisplacement - 360.f;
				return angularDisplacement;
			}
		}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool IsPointOnDisc2D( const Disc2D& disc , const Vec2& point )
{
	Vec2 distanceBetweenCenterAndPoint = point - disc.m_center;

	if ( distanceBetweenCenterAndPoint.GetLength() <= disc.m_radius )
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool IsPointOnDisc2D( const Vec2& point , const Vec2& center , const float radius )
{
	Vec2 distanceBetweenCenterAndPoint = point - center;

	if ( distanceBetweenCenterAndPoint.GetLength() <= radius )
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const Vec2 GetNearestPointOnDisc2D( const Disc2D& disc , const Vec2& point )
{
	Vec2 distanceBetweenCenterAndPoint = point - disc.m_center;

	float lenghtOfdistanceBetweenCenterAndPoint = distanceBetweenCenterAndPoint.GetLength();

	if ( lenghtOfdistanceBetweenCenterAndPoint <= disc.m_radius )
	{
		return point;
	}
	// might be wrong and may need correction;

	Vec2 nearestPoint = ( disc.m_center + ( disc.m_radius * ( distanceBetweenCenterAndPoint / lenghtOfdistanceBetweenCenterAndPoint ) ) );

	return nearestPoint;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const Vec2 GetNearestPointOnDisc2D( const Vec2& point , const Vec2& center , const float radius )
{
	Vec2 distanceBetweenCenterAndPoint = point - center;

	float lenghtOfdistanceBetweenCenterAndPoint = distanceBetweenCenterAndPoint.GetLength();

	if ( lenghtOfdistanceBetweenCenterAndPoint <= radius )
	{
		return point;
	}
	// might be wrong and may need correction;

	Vec2 nearestPoint = ( center + ( radius * ( distanceBetweenCenterAndPoint / lenghtOfdistanceBetweenCenterAndPoint ) ) );

	return nearestPoint;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const Vec2 GetNearestPointOnCircumferenceOfDisc2D( const Disc2D& disc , const Vec2& point )
{
	// might be wrong and may need correction;

	Vec2 distanceBetweenCenterAndPoint = point - disc.m_center;
	Vec2 nearestPointOnCircumference = ( disc.m_center + ( disc.m_radius * ( distanceBetweenCenterAndPoint / distanceBetweenCenterAndPoint.GetLength() ) ) );
	return nearestPointOnCircumference;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const Vec2 GetNearestPointOnCircumferenceOfDisc2D( const Vec2& point , const Vec2& center , const float radius )
{
	// might be wrong and may need correction;

	Vec2 distanceBetweenCenterAndPoint = ( point - center ).GetNormalized();
	Vec2 nearestPointOnCircumference = ( center + ( radius * distanceBetweenCenterAndPoint ) );
	return nearestPointOnCircumference;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const Vec2 GetNearestPointOnAABB2D( const Vec2& referencePoint , const AABB2& box )
{
	if ( IsPointInsideAABB2D( referencePoint , box ) )
	{
		return referencePoint;
	}
	Vec2 nearestPoint = Vec2::ZERO;


	FloatRange rangeOnX = FloatRange( box.m_mins.x , box.m_maxs.x );
	FloatRange rangeOnY = FloatRange( box.m_mins.y , box.m_maxs.y );

	if ( rangeOnX.minimum > rangeOnX.maximum )
	{
		float oldMaximum = 0.f;
		oldMaximum = rangeOnX.maximum;
		float oldMinimum = 0.f;
		oldMinimum = rangeOnX.minimum;
		rangeOnX.minimum = oldMaximum;
		rangeOnX.maximum = oldMinimum;
	}

	if ( rangeOnY.minimum > rangeOnY.maximum )
	{
		float oldMaximum = 0.f;
		oldMaximum = rangeOnY.maximum;
		float oldMinimum = 0.f;
		oldMinimum = rangeOnY.minimum;
		rangeOnY.minimum = oldMaximum;
		rangeOnY.maximum = oldMinimum;
	}

	nearestPoint.x = Clamp( referencePoint.x , rangeOnX.minimum , rangeOnX.maximum );
	nearestPoint.y = Clamp( referencePoint.y , rangeOnY.minimum , rangeOnY.maximum );
	return nearestPoint;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const Vec2 GetNearestPointOnInfiniteLine2D( const Vec2& referencePoint , const Vec2& somePointOnLine , const Vec2& anotherPointOnLine )
{
	Vec2 direction = anotherPointOnLine - somePointOnLine;
	Vec2 tempVec = referencePoint - somePointOnLine;
	Vec2 nearestPoint = GetProjectedOnto2D( tempVec , direction );
	nearestPoint = nearestPoint + somePointOnLine;
	return nearestPoint;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const Vec2 GetNearestPointOnLineSegment2D( const Vec2& referencePoint , const Vec2& startPoint , const Vec2& endPoint )
{
	Vec2 displacementFromStartToReference = referencePoint - startPoint; // AP
	Vec2 displacementFromStartToEnd = endPoint - startPoint; // AB

	float magnitudeAB = displacementFromStartToEnd.GetLengthSquared();
	float ABAPproduct = DotProduct2D( displacementFromStartToReference , displacementFromStartToEnd );
	float distance = ABAPproduct / magnitudeAB; //The normalized "distance" from start to closest point

	if ( distance <= 0 )     //Check if projection is over the line
	{
		return startPoint;

	}
	else if ( distance >= 1 )
	{
		return endPoint;
	}
	else
	{
		return startPoint + ( displacementFromStartToEnd * distance );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const Vec2 GetNearestPointOnCapsule2D( const Vec2& referencePoint , const Vec2& capsuleMidStart , const Vec2& capsuleMidEnd , const float capsuleRadius )
{
	if ( IsPointInsideCapsule2D( referencePoint , capsuleMidStart , capsuleMidEnd , capsuleRadius ) )
	{
		return referencePoint;
	}

	Vec2 nearestPoint = Vec2::ZERO;
	Vec2 displacementFromStartToPoint = referencePoint - capsuleMidStart;
	Vec2 displacementFromStartToEnd = capsuleMidEnd - capsuleMidStart;
	Vec2 displacementFromEndToPoint = referencePoint - capsuleMidEnd;

	if ( DotProduct2D(displacementFromStartToPoint,displacementFromStartToEnd) < 0 )
	{
		return GetNearestPointOnDisc2D( referencePoint , capsuleMidStart , capsuleRadius );
	}
	else if ( DotProduct2D(displacementFromEndToPoint,displacementFromStartToEnd) > 0 )
	{
		return GetNearestPointOnDisc2D( referencePoint , capsuleMidEnd , capsuleRadius );
	}
	else
	{
		Vec2 startToPointProjectedOntoStartToEnd = GetProjectedOnto2D( displacementFromStartToPoint , displacementFromStartToEnd );
			 nearestPoint = capsuleMidStart + startToPointProjectedOntoStartToEnd;
		Vec2 displacementNearestPointToPoint = referencePoint - nearestPoint;
		displacementNearestPointToPoint.ClampLength( capsuleRadius );
		nearestPoint += displacementNearestPointToPoint;
		return nearestPoint;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const Vec2 GetNearestPointOnOBB2D( const Vec2& referencePoint , const OBB2& box )
{
	//Get Point in Local Space
	Vec2 referencePointLocalCoords = referencePoint - box.m_center;
	Vec2 jBasis = box.m_iBasis.GetRotated90Degrees();
	float pi = DotProduct2D( referencePointLocalCoords , box.m_iBasis );
	float pj = DotProduct2D( referencePointLocalCoords , box.m_iBasis.GetRotated90Degrees() );
	float ni = Clamp( pi , -box.m_halfDimensions.x , box.m_halfDimensions.x );
	float nj = Clamp( pj , -box.m_halfDimensions.y , box.m_halfDimensions.y );
	Vec2 nearestPoint = box.m_center + ( box.m_iBasis * ni ) + ( jBasis * nj );
	return nearestPoint;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float SmoothStart2( float t )
{
	return t * t;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float SmoothStart3( float t )
{
	return t * t * t;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float SmoothStart4( float t )
{
	return t * t * t * t;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float SmoothStart5( float t )
{
	return t * t * t * t * t;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float SmoothStop2( float t )
{
	return t * ( 2 - t );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float SmoothStop3( float t )
{
	float scale = 1 - t;
	return ( 1 - ( scale * scale * scale ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float SmoothStop4( float t )
{
	float scale = 1 - t;
	return ( 1 - ( scale * scale * scale * scale ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float SmoothStop5( float t )
{
	float scale = 1 - t;
	return ( 1 - ( scale * scale * scale * scale * scale ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float SmoothStep3( float t )
{
	return ( ( t * t ) * ( 3 - ( 2 * t ) ) );
}