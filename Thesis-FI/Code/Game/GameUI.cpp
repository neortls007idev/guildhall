#include "Game/Game.hpp"
#include "ThirdParty/ImGUI/imgui.h"
#include "ThirdParty/ImGUI/ImGuiFileDialog.h"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DebugUI()
{
	ImGui::NewFrame();
	if( ImGui::Begin( "UI Controls" ) )
	{
		if( ImGui::CollapsingHeader( "Shaders" ) )
		{
			if( ImGui::TreeNode( "Currently Active Shader" ) )
			{
				ImGui::RadioButton( "Lighting and Shadow"		, &m_currentShaderIndex , LIT_SHADOW );
				ImGui::RadioButton( "Lighting"					, &m_currentShaderIndex , LIT );
				ImGui::RadioButton( "UnLit"					, &m_currentShaderIndex , UNLIT );
				ImGui::RadioButton( "UV Debugger"				, &m_currentShaderIndex , UV );
				ImGui::RadioButton( "Normals Debugger"			, &m_currentShaderIndex , NORMAL );
				ImGui::RadioButton( "Tangents Debugger"		, &m_currentShaderIndex , TANGENT );
				ImGui::RadioButton( "BiTangents Debugger"		, &m_currentShaderIndex , BITANGENT );
				ImGui::RadioButton( "Surface Normal Debugger"	, &m_currentShaderIndex , SURFACE_NORMAL );
				ImGui::RadioButton( "Dissolve Shader"			, &m_currentShaderIndex , DISSOLVE );
				ImGui::RadioButton( "Triplanar UnLit Shader"	, &m_currentShaderIndex , TRIPLANAR_UNLIT );
				ImGui::RadioButton( "Triplanar Lit Shader"		, &m_currentShaderIndex , TRIPLANAR_LIT );

				m_currentShader = m_lightShaders[ m_currentShaderIndex ];
				ImGui::TreePop();
				ImGui::Separator();
			}
		}

		if( ImGui::CollapsingHeader( "Lights" ) )
		{
			if( ImGui::CollapsingHeader( "GlobalSettings" ) )
			{
				ImGui::ColorEdit3( "Ambient Light Color"		, ( float* ) &m_lights.ambientLight );
				ImGui::SliderFloat( "Ambient Light Intensity"	, &m_lights.ambientLight.w , 0.f , 1.f );
				ImGui::SliderFloat( "Global Specular Power"	, &m_lights.SPECULAR_POWER , 1.f , 10000.f );
				ImGui::SliderFloat( "Global Specular Factor"	, &m_lights.SPECULAR_FACTOR , 0.f , 1.f );
				ImGui::Separator();
			}

			if( ImGui::TreeNode( "Currently Actively selected light( For additional movement options )" ) )
			{
				ImGui::RadioButton( "1"	, &m_currentLightIndex , 0 ); ImGui::SameLine();
				ImGui::RadioButton( "2"	, &m_currentLightIndex , 1 ); ImGui::SameLine();
				ImGui::RadioButton( "3"	, &m_currentLightIndex , 2 ); ImGui::SameLine();
				ImGui::RadioButton( "4"	, &m_currentLightIndex , 3 ); ImGui::SameLine();
				ImGui::RadioButton( "5"	, &m_currentLightIndex , 4 ); ImGui::SameLine();
				ImGui::RadioButton( "6"	, &m_currentLightIndex , 5 ); ImGui::SameLine();
				ImGui::RadioButton( "7"	, &m_currentLightIndex , 6 ); ImGui::SameLine();
				ImGui::RadioButton( "8"	, &m_currentLightIndex , 7 );

				ImGui::TreePop();
				ImGui::Separator();
			}
			
			for( uint lightIndex = 0; lightIndex < TOTAL_LIGHTS ; lightIndex++ )
			{
				std::string lightName = "Light ";
				lightName.append( ToString( ( int ) lightIndex + 1 ) );
				if( ImGui::TreeNode( lightName.c_str() ) )
				{
					bool lightStatus = true;
					if( m_lights.lights[ lightIndex ].intensity >= 0.001f && m_lights.lights[ lightIndex ].intensity <= 0.001f )
					{
						lightStatus = false;
					}

					ImGui::Checkbox( "isEnabled" , &lightStatus );

					//ImGui::ColorEdit3( "Light position" , ( float* ) &uiTestColor );
					if( ImGui::TreeNode( "Movement" ) )
					{
						ImGui::InputFloat3( "Light World Position" , ( float* ) &m_lights.lights[ lightIndex ].worldPosition );
						if( ImGui::Button( "Set Position to Origin" ) )
						{
							SetLightPositionToOrigin( lightIndex );
						}

						if ( ImGui::Button("Set Position to Camera Position") )
						{
							SetLightPositionToCameraPos( lightIndex );
						}
						
						//ImGui::RadioButton( "Follows the Camera" , 1 );
						//ImGui::RadioButton( "Moves in a fixed path" , 0 );

						ImGui::TreePop();
						ImGui::Separator();
					}

					if( ImGui::TreeNode( "Light Type" ) )
					{
						ImGui::RadioButton( "Point Light"		 , &m_lightType[ lightIndex ] , POINT_LIGHT );
						ImGui::RadioButton( "Spot Light"		 , &m_lightType[ lightIndex ] , SPOT_LIGHT );
						ImGui::RadioButton( "Directional Light" , &m_lightType[ lightIndex ] , DIRECTIONAL_LIGHT );
						
						ImGui::Text( "Direction Factor = %.3f"	 , m_lights.lights[ lightIndex ].directionFactor );
						ImGui::InputFloat3( "Light Direction"	 , ( float* ) &m_lights.lights[ lightIndex ].direction );
						//ImGui::SliderAngle( "Dot Inner Angle" , &m_lights.lights[ lightIndex ].dotInnerAngle , -360.f , 360.f );
						//ImGui::SliderAngle( "Dot Outer Angle" , &m_lights.lights[ lightIndex ].dotOuterAngle , -360.f , 360.f );
						if( ImGui::Button( "Set Direction to Camera Forward Vector" ) )
						{
							SetLightDirectionToCameraForwardVector( lightIndex );
						}
						
						UpdateLightData( lightIndex );
						ImGui::TreePop();
						ImGui::Separator();
					}

					ImGui::ColorEdit3( "Light Color" , ( float* ) &m_lights.lights[ lightIndex ].color );
					ImGui::SliderFloat( "Light Intensity" , &m_lights.lights[ lightIndex ].intensity , -100.f , +100.f );

					if( ImGui::TreeNode( "Attenuation" ) )
					{
						ImGui::InputFloat3( "Diffuse Attenuation Factor"  , ( float* ) &m_lights.lights[ lightIndex ].attenuation );
						ImGui::InputFloat3( "Specular Attenuation Factor" , ( float* ) &m_lights.lights[ lightIndex ].specularAttenuation );

						ImGui::TreePop();
						ImGui::Separator();
					}

					ImGui::TreePop();
					ImGui::Separator();
				}
			}
		}

		if( ImGui::CollapsingHeader( "Effects" ) )
		{
			ImGui::Checkbox( "Tone Map" , &m_isToneMapShaderActive );
			ImGui::Text( "Current Tone Map Shader" );
			ImGui::RadioButton( "No Tone"		, &m_currentToneMap , NO_TONE ); ImGui::SameLine();
			ImGui::RadioButton( "GrayScale"	, &m_currentToneMap , GRAYSCALE ); ImGui::SameLine();
			ImGui::RadioButton( "Sephia"		, &m_currentToneMap , SEPHIA );
			ImGui::SliderFloat( "Tone Map Strength" , &m_toneMapTransform.Tw , 0.f , 1.f );
			ImGui::SliderFloat( "Tone Map Power" , &m_tonePower , 0.f , 200.f );

			if( m_isToneMapShaderActive )
			{
				UpdateToneMapData();
			}

			ImGui::Separator();

			ImGui::Checkbox( "Bloom Blur" , &m_isblurShaderActive );

			ImGui::Separator();

			if( ImGui::TreeNode( "Fog Data" ) )
			{
				ImGui::Checkbox( "Is Fog Active" , &m_isblurShaderActive );
				ImGui::SliderFloat( "Near Distance" , &m_fogData.nearFog , -100.f , 100.f );
				ImGui::SliderFloat( "Far Distance" , &m_fogData.farFog , -100.f , 100.f );
				ImGui::ColorEdit3( "Near Color" , ( float* ) &m_fogData.nearFogColor );
				ImGui::ColorEdit3( "Far Color" , ( float* ) &m_fogData.farFogColor );

				ImGui::TreePop();
				ImGui::Separator();
			}

			if( ImGui::TreeNode( "Fresnel Shader Data" ) )
			{
				ImGui::Checkbox( "Is Active"			, &m_isFresnelShaderActive );
				ImGui::SliderFloat( "Fresnel Power"	, &m_fresnelShaderData.fresnelPower  , 0.f , 1.f );
				ImGui::SliderFloat( "Fresnel Factor"	, &m_fresnelShaderData.fresnelfactor , 0.f , 1.f );
				ImGui::ColorEdit3( "Fresnel Color"		, ( float* ) &m_fresnelShaderData.fresnelColor );

				ImGui::TreePop();
				ImGui::Separator();
			}
			
			if( ImGui::TreeNode( "Dissolve Shader Data" ) )
			{
				ImGui::SliderFloat( "Burn Value" , &m_dissolveShaderData.burnValue , 0.f , 1.f );
				ImGui::SliderFloat( "Burn Edge Width" , &m_dissolveShaderData.burnEdgeWidth , 0.f , 1 + 2 * m_dissolveShaderData.burnValue );
				ImGui::ColorEdit3( "Start Color" , ( float* ) &m_dissolveShaderData.startColor );
				ImGui::ColorEdit3( "End Color" , ( float* ) &m_dissolveShaderData.endColor );

				ImGui::TreePop();

				if( ImGui::Button( "Open File Dialog For Setting Dissolve Pattern" ) )
				{
					igfd::ImGuiFileDialog::Instance()->OpenDialog( "Select Dissolve Pattern" , "Choose File" , ".png,.jpg,.jpeg" , "." );
				}

				// display
				if( igfd::ImGuiFileDialog::Instance()->FileDialog( "Select Dissolve Pattern" ) )
				{
				  // action if OK
					if( igfd::ImGuiFileDialog::Instance()->IsOk == true )
					{
						std::string filePathName = igfd::ImGuiFileDialog::Instance()->GetFilePathName();
						std::string filePath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath();
						m_dissolveShaderPatternTexture = g_theRenderer->GetOrCreateTextureFromFile( filePathName.c_str() );
						// action
						ImGui::Separator();
					}
					// close
					igfd::ImGuiFileDialog::Instance()->CloseDialog( "Select Dissolve Pattern" );
				}
			}
		}
	}
	ImGui::End();
	ImGui::Begin( "Debug Data",0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground );
	ImGui::Text( "RTVs Created = %d" , g_theRenderer->GetTotalRenderTargetPoolSize() );
	ImGui::Text( "RTVs In Free Pool = %d" , g_theRenderer->GetTexturePoolFreeCount() );
	if ( m_frameRate >= 50.f )
	{
		ImGui::TextColored( ImVec4( 0.f , 1.f , 0.f , 1.f ) , "Frame Rate = %.3f" , m_frameRate );
	}
	else
	{
		ImGui::TextColored( ImVec4( 1.f , 0.f , 0.f , 1.f ) , "Frame Rate = %.3f" , m_frameRate );
	}
		
	ImGui::End();
}

//--------------------------------------------------------------------------------------------------------------------------------------------