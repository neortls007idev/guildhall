

### SD2 A05 Checklist
**( NOTE :- Controls Listed at the Bottom after the Checklist )**
------

C29 SD2 A05 Debug Render System

**( NOTE :- Controls Listed at the Bottom after the Checklist )**
======

## Overview
Implement the [./DebugRender.hpp](./DebugRender.hpp) file to spec.  Some lines are extras.

**Turn-In Branch Name: `sd2/turnin/a05`**

**Due Date: Mar 26, 2020**

### Goal
Some example code for how the system will be used in this assignment;

------

## Checklist [90%]
- [x] RenderContext
    - [x] `RasterState` moved off shader, and added to `RenderContext`
    - [x] `RenderContext::SetCullMode`
    - [x] `RenderContext::SetFillMode`
    - [x] `RenderContext::SetFrontFaceWindOrder`
    - [x] Create a default raster state to set when `BeginCamera` is called.
    - [x] Have a transient raster state to create/set when above calls are used.

- [x] World Rendering
    - [x] Points
    - [x] Lines
    - [x] Arrows
    - [x] Basis
    - [x] Quad
    - [x] Wire Box
    - [x] Wire Sphere
    - [x] Text
    - [x] Billboarded Text
    - [x] All world commands support all rendering modes;

- [x] Screen Rendering
    - [x] Points
    - [x] Lines
    - [x] Quads
    - [x] Textured Quads
    - [x] Text

- [x] Output
    - [x] Implement `DebugRenderWorldTo` to draw the debug objects into the passed camera's space.
    - [x] Implement `DebugRenderScreenTo` to add the screen-space debug calls to the passed in texture.
    - [x] Add a `DebugRenderWorldTo` call to your game after you render your scene
    - [x] Add a `DebugRenderScreenTo` call to your App before your present to render 2D objects

- [x] Controls
    - [x] Console command: `debug_render enabled=bool`
    - [x] Console command: `debug_add_world_point position=vec3 duration=float`
    - [x] Console command: `debug_add_world_wire_sphere position=vec3 radius=float duration=float`
    - [x] Console command: `debug_add_world_wire_bounds min=vec3 max=vec3 duration=float`
    - [x] Console command: `debug_add_world_billboard_text position=vec3 pivot=vec2 text=string`
    - [x] Console command: `debug_add_screen_point position=vec2 duration=float`
    - [x] Console command: `debug_add_screen_quad min=vec2 max=vec2 duration=float`
    - [x] Console command: `debug_add_screen_text position=vec2 pivot=vec2 text=string`

------

*_NOTE__ :- [1] Culling is not applied anywhere

            [2] Reasonable default values have been given to all console commands.*

------

## Extras
- [x] X03.06 : 05pts: Text Selection ( `shift + left and right arrow keys` )
- [x] X03.10 : 04pts: Cut, Copy & Paste from Window's Clipboard (Requires X03.04)
- [x] X03.24 : 02pts: Navigation Hotkeys, Home, End, Ctrl + Arrows
- [x] *X05.00: 02%*: `DebugAddWorldLineStrip`
- [x] *X05.00: 03%*: MeshUtils: `AddCylinderToIndexedVertexArray`
                     - void CreateCylinder ( ... ); // Created In indexed form

- [x] *X05.00: 03%*: MeshUtils: `AddConeToIndexedVertexArray`
                    - void CreateCone ( ... ); // Created In indexed form

*_NOTE__ :- AppendIndexedVerts( ... ) // general purpose ; used to append any two indexed meshes*
        - *_These functions can be found under VertexUtils.hpp_*

------

### Control Scheme

- `WASD` for Camera Movement.
- Use Mouse to rotate in-place.
- while holding down `Shift` + `WASD` for increased camera movement speed.
- `O` (capitol letter 'Oh') to reset camera Position to origin. ( this does not rest the camera output Size ).

**Debug Line Strip Mode Testing**
- `F8` key to delete and reset the game. *__NOTE__ :- Does not clear current debug objects*

- while holding down `Shift` + press `LeftMouseButton` : To Enter Line Strip Draw Mode

- `LeftMouseButton` : To Add point( point added will be current camera position ) for Line Strip Draw Mode.
                      A debug Point is drawn for 10 seconds at the position of last added point.

- while holding down `Shift` + press `RightMouseButton` : To Exit Line Strip Draw Mode and render the line strip for 20 seconds.
- `Esc` key can be used to exit the line strip mode without drawing the line strip.

*__NOTE__ :- Draw mode of line strip can be changed using `U` , `J` , `N` as explained below.*

*__NOTE__ :- All most multipliers can be changed using GameCommon.hpp.*

**3D Debug Objects Testing**

- To change Render Mode of the next object to add
    - `U` : RENDER_ALWAYS
    - `J` : RENDER_USE_DEPTH
    - `N` : RENDER_XRAY

- `1` : Add debug World Point at current Camera position
- `2` : Add debug World Line from current camera position to current camera position + Vec3( 1, 1 ,1 );
- `3` : Add debug World Arrow from current camera position to current camera transform position - GetCameraTransform().GetAsMatrix().GetKBasis3D();
        Essentially in the forward facing direction.
        *__NOTE__ :- the shaft and tip lerp to different colors is on purpose.*
- `4` : Add debug World Wire Bounds from current camera position to current camera position + GetCameraTransform().GetAsMatrix().GetJBasis3D();
- `5` : Add debug World Wire Sphere at current Camera position
- `6` : Add debug World Basis at current Camera position
- `7` : Add debug World Text (`formatted to` :- __" Hello %d %f" , 5 , 22.f__ ) at current Camera position

Makes sure you mouse position is correct for your current view - converting the client position (offset from top-left of window) to a world location.

------
