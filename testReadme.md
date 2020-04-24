
## Tasks
- [ ] Be able to load `OBJ` Files
    - [ ] Load OBJ as a single   vertex array
    - [ ] At end of ea- ch group (or at very end), apply post steps..
      - [ ] Invert- -V
      - [ ] Calculat- Normals
      - [ ] Calculate-Tange-ts
  - [ ] At end of impo-t...
      -  [ ] Apply transform
      -  [ ] *Clean* (is an extra)
      -  [ ] Invert Winding
- [ ] Post-Import Options su- ported
    - [ ] Invert-V
        - Some programs us UV with V at the top, and some bottom.  On import
          be able to swap it out to the correct version for your engine.
    - [ ] Invert Winding Order
        - Some programs use c- ockwise by default.  If you notice your mesh is
          inside out (you can se- e it from the inside, but not outside), you will
          want to invert the win- ding order of faces.
  - [ ] Generate Normals (Flat o- nly required)
        - Some OBJ files-  do not include normals.  If this import option is set,
          you will want - to generate normals if the file didn't include them.
          For **Flat Norma-s**, just calculate the normal for each face and assign it to
          each version that-defines the face.
  - [ ] Generate Tangents-        - Use MikkT to generate tangents.  This requires the mesh have normals, so generate
          normals should also be true.
          Use [./mikktspace.h](./mikktspace.h) and [./mikktspace.c](./mikktspace.c) to
          do this step.  
  - [ ] Post import transform from the authoring engines space to our space.
        - [ ] Transform Positions (full)
        - [ ] Normals (just direction, not scaled or translated)
        - [ ] Tangents & Bitangents (just direction, not scaled or translated)
- [ ] Have a lit model loaded in your scene to show it is working
- [ ] Support a `Shader` or `ShaderState` class that is data driven
  - [ ] Define a data format you like
  - [ ] Define a class you like
  - [ ] Implement `RenderContext::BindShader` or `BindShaderState` that
        binds the program and the additional state for you.
  - [ ] Add `RenderContext::GetOrCreateShader(...)` to support a database of shaders
  - [ ] Implement `RenderContext::BindShaderByName` or `RenderContext::BindShaderStateByName`
- [ ] Support a `Material` class to make managing material data easier.
  - [ ] Should encapsulate a `ShaderState` and relevant data needed for the material
  - [ ] Define a data format you like.
  - [ ] Define a `Material` class you can use that works with our data
      - [ ] Must be able to support owning at least one UBO for material specific data?
      - [ ] Can set material specific textures
      - [ ] Can set material specific samplers
      - [ ] Can set your engine specific constants (spec/tint)
  - [ ] Can make a material from file
  - [ ] Can call `RenderContext::BindMaterial` to bind the shader and all data assocated with the material
