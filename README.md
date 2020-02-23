MP2 And SD2 Checklist
------

## SD2 Checklist

Checklist for all SD2 assignments
------

### A03 Checklist

### Core Task List [85%]

This is the first assignment where you must supplement your assignment with some extras to get full credit.  Core tasks
only cover 85 of the total 100 points.  Make a note of all extras you attempt in your assignment,
and remember you can submit extras from previous assignments.

- [x] `Camera::SetProjectionPerspective( float fovDegrees, float nearZClip, float farZClip )` implemented
    - [ ] Set projection to `60 degrees`, and `-0.1` to `-100.0` for the clip planes.
- [ ] Camera now has a `Transform`
    - [ ] Create the `Transform` class
    - [ ] `Transform::SetPosition` implemented
    - [ ] `Transform::Translate` implemented
    - [ ] `Transform::SetRotationFromPitchRollYawDegrees`
        - [ ] When storing degrees, make sure they are stored in sane ranges...
            - [ ] Roll & Yaw is `-180` to `180`
            - [ ] Pitch is `-90` to `90`
- [ ] Camera now calculates `view` matrix from their transform.
    - [ ] `Transform::GetAsMatrix` implemented to calculate the camera's model matrix
    - [ ] `MatrixInvertOrthoNormal` implemented to invert the camera's model into a view matrix
        - [x] `MatrixIsOrthoNormal` check added
        - [x] `MatrixTranspose` added
- [ ] Draw a Quad at `(0, 0, -10)`, or 10 units in front of the origin (should be visible when you start)
- [ ] Allow player to move the camera by change the camera transform position
   - [ ] `W` & `S`: Forward & Back (movement is relative to where you're looking)
   - [ ] `A` & `D`: Left and Right (movement is relative to where you're looking)
   - [ ] `Space` & `C`: Up and Down Movement (movement is absolute (world up and world down)
   - [ ] `Left-Shift`: Move faster while held.
   - *Note:  If you want different controls, just make a note in your readme*
- [ ] Allow player to turn the camera using the mouse.
    - [ ] `InputSystem::HideSystemCursor` implemented
    - [ ] `InputSystem::ClipSystemCursor` implemented
    - [ ] `InputSystem::SetCursorMode` implemented
        - [ ] `ABSOLUTE` mode is what you currently have
        - [ ] `RELATIVE` move implemented
            - [ ] Move mouse to the center of the screen, and store off the cursor position
                - *Note:  Be sure to actually make the system call, not just assume where you moved it is where it went.  This can cause drifting.*
            - [ ] Each frame, get the cursor position, and calculate frame delta.
            - [ ] ...after which, reset to center of screen and reget the current position.
    - [ ] Game should be set to `RELATIVE` mode
        - [ ] `DevConsole` should unlock the mouse and set to `ABSOLUTE` mode
    - [ ] Associate `X` movement with `yaw`
    - [ ] Associate `Y` movement with `pitch`
        - [ ] Do not allow pitch above `85` degrees or below `95` degrees - no going upside down... yet...
        - *Note:  Up to you if you want inverted-y or not.*
- [ ] Support `RenderContext::SetModelMatrix`
    - [ ] Create a new uniform buffer for storing a model matrix (slot 2)
    - [ ] `SetModelMatrix` should update this uniform buffer
    - [ ] `BeginCamera` should `SetModelMatrix` to the `IDENTITY`, and be sure to bind the buffer.
- [ ] Be able to draw a cube mesh at `(1, 0.5, -12.0)`
    - [x] Create a `GPUMesh` class
        - [ ] Implement `IndexBuffer`
        - [ ] Be able to construct a mesh from a vertex and index array
        - [ ] Add `RenderContext::BindIndexBuffer`
        - [ ] Add `RenderContext::DrawIndexed`
        - [ ] Add `RenderContext::DrawMesh`
            - This should bind the vertex buffer, index buffer, and then `DrawIndexed`
    - [ ] Game creates a `cube mesh` around the origin with 2 unit sides.
    - [ ] Game has a `Transform` for the cube set at `(1, 0.5, -12.0f)`,
    - [ ] Cube transform sets `yaw` rotation to current time each frame
    - [ ] Game should `SetModelMatrix` to the cube transform matrix
- [ ] Support a depth buffer
    - [ ] `Texture::CreateDepthStencilBuffer` added
    - [ ] `Camera::SetDepthStencilBuffer` added
    - [ ] `RenderContext` now automatcially creates a depth buffer during init matching the swap chain's size
    - [ ] `RenderContext::GetDefaultBackbuffer` implemented to return this
    - [ ] `RenderContext::BeginCamera`, now binds the camera's back buffer as well.
        - [ ] **IMPORANT:  Do not bind the default one automatically if the camera doesn't have one set.  There are reasons a camera may not want a depth buffer!**
    - [ ] Camera's clear options should now store off the `depth` and `stencil` clear values.
    - [ ] If camera has a depth buffer and says it should clear depth, also clear the depth buffer.
------

## Extras

- [x] *X01.00 : 05pts*:  Borderless Window Support *Press `Shift + B` Key*
- [x] *X01.01 : 05pts*:  Fullscreen Support.  See notes, the default D3D11 support for this will not count. *Press `Shift + F` Key*
- [x] *X01.13 : 02pts*:  Allow change of window icon at runtime *Press `I` Key*
- [x] *X01.14 : 02pts*:  Show window loading progress in task bar.
    - Key binds for testing Taskbar progress  
      - *`Shift + T`* = reset the progress
      - *`Shift + P`* = Pause the progress
      - *`Shift + R`* = Resume/normal progress
      - *`Shift + E`* = Error progress mode
      - *`Shift + O`* = No progress/Progress Complete
      - *`Shift + I`* = Indeterminant state  
- [x] *X03.01 : 03pts*: Built-in error shader to use when a shader fails to compile.
- [x] *X02.10 : 02pts*:  Shader Reloading. `On key press R`, reload all shaders in your shader database
