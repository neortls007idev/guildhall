MP2 And SD2 Checklist
------

## SD2 Checklist

Checklist for all SD2 assignments
------

### A01 Checklist

#### Required Tasks
**Required Tasks** are tasks that the course relies on being complete, and will most likely be built upon on later assignments.  All required tasks must be attempted before a bonus task will be graded.

- [x] *10pts*: `Window` class created in Engine to spec, with Protogame being updated;
- [x] *20pts*: `RenderContext` changes
      - [x] All broken `RenderContext` methods a stripped with an assert to todo added to the body.
      - [x] `RenderContext` can be setup by giving it a window
- [x] *05pts* `Camera::SetClearMode` added
- [x] *05pts* `RenderContext::BeginCamera` should now clears depending on clear mode
      - [x] For  now, clear the default swapchain on `RenderContext`
- [x] *10pts* `SwapChain` implemented
    - [x] Default `SwapChain` is added to your `RenderContext`, created during setup
    - [x] Ability to get the backbuffer texture.
- [x] *10pts*: `Texture` class added/modified
- [x] *10pts*: `Texture::GetOrCreateView` will return a texture view;    
- [-] *10pts*: Game code should cycle clear color each frame to show everything working.
- [x] *20pts* Ability to create a debug context by pre defining `RENDER_DEBUG`
    - [x] All builds should define `RENDER_DEBUG` for now
    - [x] Be sure there are no leaked resources on shutdown (check `Output`)

**How I will Grade**;  
1. Pull, and compile your program in `Debug` and `Release`.
2. Run it, and look for the cycling color.
3. Quit (`Escape`) and look for leaks in output
4. Check the code to make sure all window/context creation has been moved out of App
   - Also check that you are indeed making a debug context
5. Grade extras if applicable

#### Bonus Tasks
**Bonus Tasks** are tasks that are related to the main assignment, but are not required for finishing the course, hopefully to allow for some personalization of your own engine.

No required tasks will build off bonus tasks, but future bonus tasks may (and they will clearly label which ones they require).

These tasks *may be completed for any assignment in this semester*, but you must clearly state which extras you are turning in for which assignment.  You may resubmit an extra on multiple assignments to make up lost points.

Example, say for the `DevConsole` assignment, you attempt the extra `auto-complete`, worth 8 points. While testing, I notice that it crashes if backspace is pressed midway through an autocompleted command, but otherwise works.  You are awarded say 5/8 for that extra.  You may then fix that bug, resubmit, and get the remaining 3/8 on a later assignment.  After that though no more points may be acquired from that extra.  

Note, most bonus tasks are twice the work for half the points of a required task, and will not be graded at all if not all required tasks are attempted.  

- [ ] (X01.00 : 05pts) Borderless Window Support
- [ ] (X01.01 : 05pts) Fullscreen Support.  See notes, the default D3D11 support for this will not count.
- [ ] (X01.10 : 02pts) Window alignment options
- [x] (X01.11 : 01pts) Allow change of window title at runtime **( Press C key to test )**
- [ ] (X01.12 : 02pts) Set a custom window icon
- [ ] (X01.13 : 02pts) Allow change of window icon at runtime
- [ ] (X01.14 : 02pts) Show window loading progress in task bar.

------
