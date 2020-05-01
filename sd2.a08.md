

### A07 Checklist
**( NOTE :- Controls And Known bugs Listed at the Bottom after the Checklist )**
======

## Checklist

## Tasks
- [x] Be able to load `OBJ` Files
    - [x] Load OBJ as a single   vertex array
    - [x] At end of each group (or at very end), apply post steps..
      - [x] Invert -V
      - [x] Calculate Normals
      - [x] Calculate Tangents
  - [x] At end of import...
      -  [x] Apply transform
      -  [-] *Clean* (is an extra) ( does not work correctly triangles are missing )
      -  [x] Invert Winding
- [x] Post-Import Options supported
    - [x] Invert-V
        - Some programs us UV with V at the top, and some bottom.  On import
          be able to swap it out to the correct version for your engine.
    - [x] Invert Winding Order
        - Some programs use clockwise by default.  If you notice your mesh is
          inside out (you can see it from the inside, but not outside), you will
          want to invert the winding order of faces.
  - [x] Generate Normals (Flat only required)
        - Some OBJ files-  do not include normals.  If this import option is set,
          you will want - to generate normals if the file didn't include them.
          For **Flat Normals**, just calculate the normal for each face and assign it to
          each version that-defines the face.
  - [x] Generate Tangents-        
        - Use MikkT to generate tangents.  This requires the mesh have normals, so generate
          normals should also be true.
          Use [./mikktspace.h](./mikktspace.h) and [./mikktspace.c](./mikktspace.c) to
          do this step.  
  - [x] Post import transform from the authoring engines space to our space.
        - [x] Transform Positions (full)
        - [x] Normals (just direction, not scaled or translated)
        - [x] Tangents & Bitangents (just direction, not scaled or translated)
- [x] Have a lit model loaded in your scene to show it is working
- [x] Support a `Shader` or `ShaderState` class that is data driven
  - [x] Define a data format you like
  - [x] Define a class you like
  - [x] Implement `RenderContext::BindShader` or `BindShaderState` that
        binds the program and the additional state for you.
  - [x] Add `RenderContext::GetOrCreateShader(...)` to support a database of shaders
  - [x] Implement `RenderContext::BindShaderByName` or `RenderContext::BindShaderStateByName`
- [x] Support a `Material` class to make managing material data easier.
  - [x] Should encapsulate a `ShaderState` and relevant data needed for the material
  - [x] Define a data format you like.
  - [x] Define a `Material` class you can use that works with our data
      - [x] Must be able to support owning at least one UBO for material specific data?
      - [x] Can set material specific textures
      - [x] Can set material specific samplers
      - [x] Can set your engine specific constants (spec/tint)
  - [x] Can make a material from file
  - [x] Can call `RenderContext::BindMaterial` to bind the shader and all data assocated with the material


======
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

- If XML file data is set incorrectly game will not run.
- Resetting the game at runtime using F8 causes hard crash.
- Pressing `left alt` key causes some kind of pause effect ( can be toggled ).
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
