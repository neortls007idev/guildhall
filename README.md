MP2 And SD2 Checklist
------

## SD2 Checklist

Checklist for all SD2 assignments
------

### A02 Checklist

#### Required Tasks

- [x] *05pts*: When creating a texture from a handle (`ID3D11Texture2D*`), use `ID3D11Texture2D::GetDesc` to cache off the texure dimensions.
- [x] *05pts*: `Camera::SetColorTarget( Texture* tex )` implemented.
    - [x] If color target is `nullptr` when `BeginCamera` is called, default to the swapchain's backbuffer.
- [x] *20pts*: Be able to create a `Shader` by compiling *Vertex* and *Pixel* shader stages.
    - [x] Can compile a shader stage
    - [x] Properly print out compilation errors.
    - [x] Create a raster state
- [x] *10pts*: Implement `RenderContext::BindShader( Shader* shader )` to set the active shader.  
    - [x] Assert that you are currently drawing (between a `BeginCamera` and `EndCamera` call)
    - [x] Binds the vertex and pixel shader stages to the correct place in the pipeline.
    - [x] Bind the raster state
- [x] *10pts*: Implement `RenderContext::GetOrCreateShader( char const* filename )`
- [x] *05pts*: Implement `RenderContext::BindShader( char const* filename )` helper method so we can set by string.
- [x] *05pts*: Implement `RenderContext::Draw( int vertexCount, int startIndex = 0 )` to RenderContext
- [x] *10pts*: Implement `RenderBuffer` class for allocating GPU memory
- [x] *10pts*: Implement `VertexBuffer` specialization of `RenderBuffer` for handling vertex data
    - [x] Add `buffer_attribute_t` description of your `VertexPCT`
- [x] *05pts*: Implement `RenderContext::BindVertexBuffer( VertexBuffer* vbo )`
    - [x] Keep track of currently bound vertex buffer and don't rebind if no change is made.
- [x] *15pts*: Implement `RenderContext::DrawVertexArray`
    - [x] `RenderContext` should own a vertex buffer (`RenderBuffer* m_immediateVBO`) to use for `DrawVertexArray`
    - [x] Copy vertices to immediate vbo.
    - [x] Bind the vertex buffer to input slot 0 (i)
    - [x] `GetOrCreate` an input layout from current shader/vertex buffer.  Bind if different.
    - [x] Call `Draw`

------
