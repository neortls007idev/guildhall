#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Matrix4x4.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr uint16_t TOTAL_OMNI_LIGHTS = 8;
constexpr uint16_t TOTAL_SPOT_LIGHTS = 16;

//--------------------------------------------------------------------------------------------------------------------------------------------

struct lightDataT
{
	Vec3        worldPosition				= Vec3::ZERO;
	uint		lightType					= 0;										   // lightType
//	uint		pad00						= 9999999;									   // this is not required, but know the GPU will add this padding to make the next variable 16-byte aligned

	Vec3        color						= Vec3::ONE;
	float       intensity					= 1.0f;                                        // rgb and an intensity

	Vec3        direction					= Vec3::UNIT_VECTOR_ALONG_K_BASIS;             // direction light is point, default (0,0,1)
	float       directionFactor				= 0.f;                                         // do we consider this a point light (0.0f) or a directional light (1.0f).  Determine how we calcualte the incident light vector, default (0.0f)

	Vec3        attenuation					= Vec3::UNIT_VECTOR_ALONG_J_BASIS;             // intensity falloff , attenuation for diffuse light, default (0,0,1)
	float       dotInnerAngle				= -1.0f;                                       // cone light inner angle (default -1.0f) - angle at which cone lights begin to stop affecting an object

	Vec3        specularAttenuation			= Vec3::UNIT_VECTOR_ALONG_K_BASIS;             // attenuation for specular lighting (constant,linear,quadratic), default (0,0,1)
	float       dotOuterAngle				= -1.0f;                                       // cone light outer angle (default -1.0f) - angle at which cone lights stop affecting the object completely
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct dirLightDataT
{
	Vec3        color						= Vec3::ONE;
	float       intensity					= 1.0f;                                        // rgb and an intensity

	Vec3        direction					= Vec3::UNIT_VECTOR_ALONG_K_BASIS;             // direction light is point, default (0,0,1)
	float		shadowFactor				= 0.f;										   // 0 or 1 for if the light shadow is accounted for
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct pointLightDataT
{
	Vec3        worldPosition				= Vec3::ZERO;
	float		shadowFactor				= 0.f;										   // 0 or 1 for if the light shadow is accounted for

	Vec3        color						= Vec3::ONE;
	float       intensity					= 1.0f;                                        // rgb and an intensity

	Vec3        attenuation					= Vec3::UNIT_VECTOR_ALONG_J_BASIS;             // intensity falloff , attenuation for diffuse light, default (0,0,1)
	uint		pad03						= 9999999;									   // this is not required, but know the GPU will add this padding to make the next variable 16-byte aligned

	Vec3        specularAttenuation			= Vec3::UNIT_VECTOR_ALONG_K_BASIS;             // attenuation for specular lighting (constant,linear,quadratic), default (0,0,1)
	uint		pad04						= 9999999;									   // this is not required, but know the GPU will add this padding to make the next variable 16-byte aligned
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct spotLightDataT
{
	Vec3        worldPosition				= Vec3::ZERO;
	float		shadowFactor				= 0.f;										   // 0 or 1 for if the light shadow is accounted for

	Vec3        color						= Vec3::ONE;
	float       intensity					= 1.0f;                                        // rgb and an intensity

	Vec3        direction					= Vec3::UNIT_VECTOR_ALONG_K_BASIS;             // direction light is point, default (0,0,1)
	float       directionFactor				= 0.f;                                         // do we consider this a point light (0.0f) or a directional light (1.0f).  Determine how we calcualte the incident light vector, default (0.0f)

	Vec3        attenuation					= Vec3::UNIT_VECTOR_ALONG_J_BASIS;             // intensity falloff , attenuation for diffuse light, default (0,0,1)
	float       dotInnerAngle				= -1.0f;                                       // cone light inner angle (default -1.0f) - angle at which cone lights begin to stop affecting an object

	Vec3        specularAttenuation			= Vec3::UNIT_VECTOR_ALONG_K_BASIS;             // attenuation for specular lighting (constant,linear,quadratic), default (0,0,1)
	float       dotOuterAngle				= -1.0f;                                       // cone light outer angle (default -1.0f) - angle at which cone lights stop affecting the object completely
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct shaderLightDataT
{
	Vec4				ambientLight;										// rgb and an intensity
	dirLightDataT		globalDirLight;
	pointLightDataT		omniPointLights[ TOTAL_OMNI_LIGHTS ];
	spotLightDataT		spotLights[ TOTAL_SPOT_LIGHTS ];
		
	// all 0 to 1 and help influence the lighting equation
	float				DIFFUSE_FACTOR		= 1;							// default: 1  - scales diffuse lighting in equation (lower values make an object absorb light
	float				SPECULAR_FACTOR		= 1;							// default: 1  - scales specular lighting in the equation (higher values create a "shinier" surface)
	float				SPECULAR_POWER		= 32;							// default: 32 - controls the specular falloff (higher values create a "smoother" looking surface)
	float				padding00			= 0.f;					
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct lightView_t
{
	Mat44 LIGHT_PROJECTION;
	Mat44 LIGHT_VIEW;
};

//--------------------------------------------------------------------------------------------------------------------------------------------