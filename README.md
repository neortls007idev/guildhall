MP2 And SD2 Checklist
------

## SD2 Checklist

Checklist for all SD2 assignments
------

### A02 Checklist

### Core Task List [85%]

This is the first assignment where you must supplement your assignment with some extras to get full credit.  Core tasks
only cover 85 of the total 100 points.  Make a note of all extras you attempt in your assignment,
and remember you can submit extras from previous assignments.

- [ ] *15pts*: Get a Working Orthographic Camera
    - [ ] *05pts*: Implement `MakeOrthographicProjectionMatrixD3D` in `Engine/Math/MatrixUtils.hpp`, `.cpp`.  
    - [ ] *05pts*: Camera now owns a `UniformBuffer` for their matrices
    - [ ] *05pts*: Camera updates and binds their uniform buffer in `BeginCamera`
- [ ] *18pts*: Get Working Textures
    - [ ] *04pts*: Be able to create a `Texture` from an Image/File
    - [ ] *04pts*: `TextureView` supports shader resource views (srv)
    - [ ] *04pts*: Make a `Sampler` class
    - [ ] *03pts*: Default Samplers (Linear+Wrap, Point+Wrap)
    - [ ] *03pts*: `RenderContext::GetOrCreateTexture` works again.
- [ ] *08pts*: Blend States supported by shader.
    - [ ] *04pts*: Blend state is created/recreated and bound when shader is bound.
    - [ ] *04pts*: Only recreate blend state if changed since last binding
- [ ] *05pts*: Default Built-In Shader to use when `BindShader(nullptr)` is used
    - [ ] `Renderer::SetBlendState` should affect this shader;
- [ ] *05pts*: Render two textured quads on screen
    - [ ] *04pts*: One using an invert color shader
    - [ ] *01pts*: One default
- [ ] *34pts*: Dev Console
    - [x] *02pts*: User can open the dev console using the **tilde** key '\~'.  
                   You may use a custom key if you make a note in your assignment readme.
        - [ ] Upon opening console, be sure the input is cleared.
    - [x] *02pts*: Pressing tilde again closes the console.
    - [ ] *09pts* If console is open, Escape should...
        - [ ] *03pts*: Clear input if there is any text currently there
        - [ ] *03pts*: Close the console if console is open.
        - [ ] *03pts*: Normal game behaviour otherwise (for example, for now it will quit your application)
    - [ ] *14pts*: User can type commands **only** while console is open.
        - [ ] *02pts*: Typing should insert characters at the carot
        - [ ] *02pts*: The carot blinks (every quarter to half second usually looks okay)
        - [ ] *02pts*; Left & Right arrow keys should move carot forward and back along string
        - [ ] *02pts*: Delete should delete the character after the carot
        - [ ] *02pts*: Backspace should delete the character before the carot
        - [ ] *02pts*: Pressing `Enter` submits the command
        - [ ] *02pts* If it is an invalid command (it has not handler), print an error message
    - [ ] *02pts* Game input is ignored while dev console is open.
    - [ ] *03pts*: Support a `quit` command that quits the game.
    - [ ] *03pts*: Support a `help` command that lists all exposed commands

------

## Extras

- [x] *X01.00 : 05pts*:  Borderless Window Support *Press `Shift + B` Key*
- [x] *X01.01 : 05pts*:  Fullscreen Support.  See notes, the default D3D11 support for this will not count. *Press `Shift + F` Key* 
- [x] *X01.13 : 02pts*:  Allow change of window icon at runtime *Press `I` Key*
