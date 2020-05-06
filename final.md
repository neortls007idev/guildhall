

### A07 Checklist
**( NOTE :- Controls And Known bugs Listed at the Bottom after the Checklist )**
======

## Checklist

## Tasks
- [x] `NamedProperties`
    - [ ] Switch EventArgs to use NamedProperties instead NamedStrings
    - [x] Add ability to subscribe methods to your event system.
    - [x] Add ability to unsubscribe an object from the event system (unsubscribes all methods on that object)

- [x] Color Transform (ex: Grayscale) Effect
    - [x] Create/Recycle a color target matching your swapchain's output.
    - [x] Render game as normal
    - [x] Create/Recycle another match target
    - [x] Apply an effect on the original image, outputting to this new texture
        - [x] Effect applies a per-pixel color transform using a matrix.
        - [x] Make it so you can specify a "strength" for this transform, where 0
              has no effect, and 1 is the full effect so you can blend between them.
        - [x] Be able to specify a tint and tint power that pixels trend toward (useful for fades)
              - `0` would have no effect
              - `1` the resultant color would be the tint
        - [x] **Optional Challenge: Have all the above be done with a single mat44.**
    - [x] Copy this final image to the swapchain before present

- [x] Bloom Effect
    - [x] Set your normal color target, and a secondary "bloom" target
        - [x] Camera can set set tertiary render targets
        - [x] Shader has a secondary output specified
    - [x] When done, be able to blur the bloom target
        - [x] Create/Recycle a matching color/render target
        - [x] Can use any blur algorithm you can find online, some suggestions...
            - [x] Doing a Hardcoded 11x11 Kernel single pass Guassian Blur.
            - Simple single pass box blur
            - Single pass guassian blur
            - Multiple pass guassian blur
    - [x] Take the result of the blur, and the normal color output, and combine them
          into the final image.
    - [x] Be able to toggle blur on-and-off `F2` to see it working or not
        - [x] Disabling the blur just doesn't run the blur and composite steps;

- [x] Texture Pool
    - [x] Be able to ask your `RenderContext` for a temporary render target of a given resolution and size.
        - [x] Search for an existing free texture first, and return it if you find one.
        - [x] If there are none available, create and return a new one.
    - [x] Be able to relinquish back these temporary textures back to the `RenderContext` when you're done with them.
        - This will allow them to be reused.
    - [x] Add a `RenderContext::GetTotalTexturePoolCount()` to tell you how many textures have been created this way.
    - [x] Add a `RenderContext::GetTexturePoolFreeCount()` to tell you how many are currently in the pool to be recycled
    - [x] Debug render these counts to the screen to you can make sure you're properly recycling during this assignment.
        - At any given time you likely will not have more than 3 textures in use at once, meaning your pool should never exceed that count.  This can really depend on your scene though.  For eaxmple, in this assignment for bloom...
          1. Camera color output
          2. Camera bloom target
          3. Temporaries
             - Blurring secondary output
             - Composite output

======
##General Notes
------

- 1. Support For GAMMA is Present but not using GAMMA at the moment. by that I mean GAMAA is not adjustable by the USER.
- 2. The game starts with only the 1st light enabled but all 8 lights are always rendered in the scene.
- 3. All 8 lights are Initialized with random colors.
- 4. At start only the 1st light which is White is enabled.
- 5. When Testing directional and spot light debug arrow might seem big if left at a spot and might block the view. recommended to test while moving objects with camera.
- 6. Fog is only enabled on the sphere and will only work when in Fresnel Shader or Lit Shader mode.( This is intentional )
- 7. Tone Map tint power clamped from 0 to infinity on purpose.
- 8. Tone Map tint power is a uniform scale on the tone map matrix so if tone shader is and power is 0 you will see nothing.

------

======

### Known Errors/ Bugs
------

- If XML file data is set incorrectly game will not run.
- Pressing `left alt` key causes some kind of pause effect ( can be toggled ).
------

======

### Control Scheme

- `WASD` for Camera Movement.
- Use Mouse to rotate in-place.
- while holding down `Shift` + `WASD` for increased camera movement speed.
- `O` (capitol letter 'Oh') to reset camera Position to origin. ( this does not rest the camera output Size ).

------

**Specific Shader Testing( Also Present ON screen when shader is active)**

- 1. *__TONEMAP SHADER__*
      - 1. `N`                  : Increase TINT Power. ( Clamped 0 to INFINITY )
      - 2. `B`                  : decrease TINT Power. ( Clamped 0 to INFINITY )
      - 3. `Shift + UP ARROW`   : Increase TINT Strength. ( Clamped 0 to 1 )
      - 4. `Shift + DOWN ARROW` : decrease TINT Strength. ( Clamped 0 to 1 )
      - 5. `UP/DOWN ARROW`      : Cycle the Tint to ALPHA TINT / GRAY SCALE / SEPHIA.
      - 6. `F3`                 : Toggle Tint

- 2. *__FRESNEL SHADER__*
      - 1. `Z` : decrease Fresnel Factor. ( Clamped 0 to 1 )
      - 2. `X` : increase Fresnel Factor. ( Clamped 0 to 1 )
      - 3. `C` : decrease Fresnel Power. ( Clamped 0 to 1 )
      - 4. `V` : increase Fresnel Power. ( Clamped 0 to 1 )
      - __Dev console command__ :- `UpdateFresnelShader` can be used to change `Fresnel Color`, `Factor` and `Power`.

- 3. *__DISSOLVE SHADER__*
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
