

### A07 Checklist
**( NOTE :- Controls And Known bugs Listed at the Bottom after the Checklist )**
======

## Checklist
- [x] Be able to support up to at least 8 concurrent lights
- [x] Be able to support different light types...
    - [x] Point Light...
        - [x] Local
        - [x] Infinite
    - [x] Infinite Directional Light...
    - [x] Spot Light
       - [x] Local
       - [x] Infinite
    - *Have all these visible in the scene concurrently*
    - *Note: Local and Infinite are just attenuation values...*
- [x] Shader should be branchless using a unified lighting structure
      - ( DISCARD SHADER uses an IF)
- [x] Support linear fog...
      - [x] `RenderContext::EnableFog( float nearFog, float farFog, rgba nearFogColor, rgba farFogColor );`
      - [x] `RenderContext::DisableFog()` (shader is just 'always' going to do fog, so what do you set this to to make it not have an effect?)
- [x] Dissolve Shader Effect
    - [x] Support a `Material Block` uniform buffer with your `RenderContext`
        - see BindMaterialData(...);
    - [x] When binding a dissolve pattern to use, be sure to use a free slot.   
        - Suggest having a slot titled `USER_SLOT_START`, defined to likely 8.  That way when binding a texture
          a user can just say `USER_SLOT_START + idx` for the slot they want.  
          You can also move your common textures (diffuse, normal, et.al.) to a later slot if you like users starting at 0.
    - [x] Have your dissolve shader expose the following controls...
        - [x] A dissolve "depth" or value set to 0 to 1 to control how dissolved something is.
        - [x] A dissolve "range" to give a *burned edge* to the dissolve effect.  This is the range near the edge at which is interpolates between burn_start_color and burn_end_color.  
        - [x] A burn start color.
        - [x] A burn end color
        - *Note, the full range you will be moving through with this effect is not 0 to 1, but `(1 + 2 * dissolve_range)` (why?).  You can think of it kinda like a scroll bar.  Meaning - how does the `dissolve_value` or depth that is 0 to 1 actually affect our dissolve range we're using in the shader?*

======
- [x] *X07.10: 05%*: Triplanar Shader ( both LIT and UNLIT VERSION PRESENT )
- [x] *X07.13: 03%*: Fresnel or Highlight Shader
##General Notes
------

- 1. Support For GAMMA is Present but not using GAMMA at the moment. by that I mean GAMAA is not adjustable by the USER.
- 2. The game starts with only the 1st light enabled but all 8 lights are always rendered in the scene.
- 3. All 8 lights are Initialized with random colors.
- 4. At start only the 1st light which is White is enabled.
- 5. When Testing directional and spot light debug arrow might seem big if left at a spot and might block the view. recommended to test while moving objects with camera.
- 6. Fog is only enabled on the sphere and will only work when in Fresnel Shader or Lit Shader mode.( This is intentional )

------

======

### Known Errors/ Bugs
------

- *__NONE__*

------

======

### Control Scheme

- `WASD` for Camera Movement.
- Use Mouse to rotate in-place.
- while holding down `Shift` + `WASD` for increased camera movement speed.
- `O` (capitol letter 'Oh') to reset camera Position to origin. ( this does not rest the camera output Size ).
- `F8` to delete and reset the game.

------

**Specific Shader Testing( Also Present ON screen when shader is active)**

- 1. *__FRESNEL SHADER__*
      - 1. `Z` : decrease Fresnel Factor. ( Clamped 0 to 1 )
      - 2. `X` : increase Fresnel Factor. ( Clamped 0 to 1 )
      - 3. `C` : decrease Fresnel Power. ( Clamped 0 to 1 )
      - 4. `V` : increase Fresnel Power. ( Clamped 0 to 1 )
      - __Dev console command__ :- `UpdateFresnelShader` can be used to change `Fresnel Color`, `Factor` and `Power`.

- 2. *__DISSOLVE SHADER__*
      - 1. `Z` : decrease Burn Value. ( Clamped 0 to 1 )
      - 2. `X` : increase Burn Value. ( Clamped 0 to 1 )
      - 3. `C` : decrease Burn Edge Width. ( Clamped 0 to 1 + 2 * Current Burn Value )
      - 4. `V` : increase Burn Edge Width. ( Clamped 0 to 1 + 2 * Current Burn Value )
      - __Dev console command__ :- `UpdateDissolveShader` can be used to change `startColor`, `endColor` , `burnValue`, `burnEdgeWidth`.
      - __Dev console command__ :- `UpdateDissolveShaderPattern` can be used to change dissolve pattern. see dev console help.
      - A good test for this would be - UpdateDissolveShaderPattern patternTexPath =Data\Images\metalPlate\metal_plate_rough_1k.png
      ( You can just copy and paste this :) )
      *__WARNING :- Updating pattern can cause the program to crash in case texture is missing.__*


**Lighting Testing ( Also present on Screen )**

- To change current Light press `1-8` for the light number which you want to select.
- To Enable ( `Q` ) or Disable( `E` ) current light.
- `M` To cycle the light Type.
    - If light type is Directional / Spot Light - the directional angle changes with current camera rotation( forward vector ) so `F6` or `F7` can be used to change that.
    - Spot Light inner and outer angles are fixed and hard coded at the moment.

- To change Light Position
    - `F5` : Place Current Light at Origin.
    - `F6` : Place current light at current Camera Position.
    - `F7` : current Light Follows the Camera.
    - `F9` : Light moves in a fixed path.

- Switch/Cycle Current Shader Using `LEFT ARROW( <- ) KEY` & `RIGHT ARROW( -> ) KEY`.
      - Name of Current Shader Is displayed on screen using debug render always;
- `9` , `0`  : Change Ambient Light Intensity
      - (Clamped 0-1)
-  `-` , `+` : Change current Light Intensity
      - (UnClamped)
- `[`, `]`   : Change Global Specular Factor
      - (Clamped 0-1)
- `U` , `J`  : Change Global Specular Power
      - (Clamped 1-INFINITY)
- `T` , `R` , `G` , `Y` : Change current Light Attenuation to `ZERO` , `Constant` , `Linear` , `Quadratic`
------

**Lighting Devconsole Commands ( Also present in devconsole help )**
------

- `ChangeLightColor`            : Change Light Color at Index.
              - Ex - `ChangeLightColor` idx = 0 |color = 255 , 255 , 255 , 255
              - __If index is not passed light color of current selected light gets changed.__

- `ChangeLightAttenuation`      : Update Light Attenuation of Ambient Light.
              - Ex - `ChangeLightAttenuation` idx = 0 |attenuation = 1.f , 0.f , 2.3f
              - __If index is not passed light diffuse attenuation of current selected light gets changed.__

- `ChangeAmbientLightColor`     : Update Color of Ambient Light.
              - Ex - `ChangeAmbientLightColor`  color = 255 , 255 , 255 , 255

- `ChangeAmbientLightIntensity` : Update Intensity of Ambient Light.
              - Ex -  `ChangeAmbientLightIntensity` intensity = 0.3f

- `DisableFog` : Disables the Fog.
- `EnableFog`  : `EnableFog` can be used to change near and far fog distance and color.
------
