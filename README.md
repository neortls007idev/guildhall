MP2 And SD2 Checklist
------

## MP2 Checklist

Checklist for all MP2 assignments

------

### A01 Checklist

Use the branch name `mp2/turnin/a01` for grading, and you have the checklist copied to your `Root` folder in a file named `mp2.a01.md`

- [x] [05pts] Create a GitHub account
- [x] [05pts] Send me your Username
- [x] [10pts] Create Private Repots
    - [x] `Guildhall` repot
    - [ ] `Engine` repot if using submodules
- [x] [10pts] Make me a collaborator on all depots needed for class
    - [x] `Guildhall` Repot
    - [ ] `Engine` Repot if using Sbubmodules
- [x] [20pts] Properly setup [./example.gitignore](`.gitignore`) for all repots
    - [x] Not intermediate or temporary files should be pushed to the depot
    - [x] Only the `exe` in the `Run` folder exists.
- [x] [05pts] Make initial commit for all repots
- [x] [05pts] Make initial push for all repots
- [x] [10pts] Add a `mp2.a01.md` file to your `Root` folder...
    - [x] This should contain a copy of this checklist with tasks you've attempted marked
    - [x] Make sure this is pushed
- [x] [30pts] Create your `mp2/turnin/a01` branch and make sure it is pushed to remote.
- [x] Do a buddy build to make sure you can pull and build it yourself
- [x] Once assignment is done and everything compiles (and there were changes), merge `mp2/turnin/a01` back in to `master`
------

## SD2 Checklist

Checklist for all SD2 assignments
------

### A01 Checklist

#### Required Tasks
**Required Tasks** are tasks that the course relies on being complete, and will most likely be built upon on later assignments.  All required tasks must be attempted before a bonus task will be graded.

- [ ] *10pts*: `Window` class created in Engine to spec, with Protogame being updated;
- [ ] *20pts*: `RenderContext` changes
      - [ ] All broken `RenderContext` methods a stripped with an assert to todo added to the body.
      - [ ] `RenderContext` can be setup by giving it a window
- [ ] *05pts* `Camera::SetClearMode` added
- [ ] *05pts* `RenderContext::BeginCamera` should now clears depending on clear mode
      - [ ] For  now, clear the default swapchain on `RenderContext`
- [ ] *10pts* `SwapChain` implemented
    - [ ] Default `SwapChain` is added to your `RenderContext`, created during setup
    - [ ] Ability to get the backbuffer texture.
- [ ] *10pts*: `Texture` class added/modified
- [ ] *10pts*: `Texture::GetOrCreateView` will return a texture view;    
- [ ] *10pts*: Game code should cycle clear color each frame to show everything working.
- [ ] *20pts* Ability to create a debug context by pre defining `RENDER_DEBUG`
    - [ ] All builds should define `RENDER_DEBUG` for now
    - [ ] Be sure there are no leaked resources on shutdown (check `Output`)

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
- [ ] (X01.11 : 01pts) Allow change of window title at runtime
- [ ] (X01.12 : 02pts) Set a custom window icon
- [ ] (X01.13 : 02pts) Allow change of window icon at runtime
- [ ] (X01.14 : 02pts) Show window loading progress in task bar.

------
