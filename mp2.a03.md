

### A02 Checklist
**( NOTE :- Controls Listed at the Bottom after the Checklist )**
------
### PROJECT NAME :- PhysicsSandbox

I may mark tasks as **CHALLENGE** if I believe them to be more difficult relative to the rest of the assignment.  This is just to hint to you that you may want to save those tasks for the end if you have time, or skip altogether if not.  

- [x] *25pts*: Add a `PolygonCollider2D`
    - [x] *05pts*: Can construct from counter-clockwise list of points
        - [x] `GUARANTEE_OR_DIE` points represent a convex polygon
    - [x] **CHALLENGE** - Can construct from a point cloud (gift wrapping algorithm)
    - [x] *05pts*: Implement `Contains`
    - [x] *05pts*: Implement `GetClosestPoint`
    - [x] *05pts*: Implement `Intersects`, but only vs disc, return `false` if intersecting with another polygon for now.
        - [x] Be sure disc now checks against both discs and polygons using `GetType`
    - [x] *05pts*: Can render it - should work as before.
- [x] *25pts*: Can construct `GameObject` with a `PolygonCollider`
    - [x] Press `2` to enter a polygon draw mode, place first point where cursor is when `2` was pressed
    - [] Draw a line from last point added to cursor
        - [] Line drawn `RED` if not a valid point to add (ie, would maintain convexity)
        - [] Line drawn `BLUE` if it is a valid point.
    - [x] When pressing `Left Mouse Button`, add a point if it is a legal (will maintain convex polygon)
    - [x] When pressing `Right Mouse Button`, end, closing off the loop.
    - [x] Pressing `Escape` while in this mode should cancel the creation.
    - [x] Construct object with polygon as its collider, exiting creation mode
        - [x] Pick 'center', or position of rigidbody by either
            - Average of all points
            - Center of box that encapsulates all points
            - **CHALLENGE**: Center of mass, assuming uniform density
    - [x] *If you did the giftwrapping algorithm, you can just add points and construct a polygon that would work for them*
- [x] *05pts*: `Rigidbody2D` can `Enable` or `Disable` itself, taking it out of the simulation, but still rendering
    - [x] Objects being dragged should set themselves to disabled, and re-enable when unselected.
- [x] *05pts*: When drawing, draw the location of the `Rigidbody` as an `X`, red if disabled, blue if enabled.
- [x] *10pts*: `Physics2D::Update` loop implemented
    - [x] Apply all global forces
    - [x] Simulate all rigidbodies
    - [x] Cleanup destroyed objects (should be happening from A01)
- [x] *10pts*: Add `Rigidbody2D::SetVelocity` to change the velocity of the object
    - [x] When releasing an object, set its velocity based on mouse movement (ie, be able to throw an object)
- [x] *05pts*: `Rigidbody2D` can `SetSimulationMode` to either `STATIC`, `KINEMATIC`, or `DYNAMIC`
    - [x] `STATIC` objects do not apply forces or simulate.
    - [x] `KINEMATIC` objects do not apply forces, but will simulate.  
    - [x] `DYNAMIC` objects apply forces and simulate
    - [x] While selected, `1`, `2`, and `3` should switch the object to `STATIC`, `KINEMATIC`, or `DYNAMIC` respectively.
- [x] *05pts*: `Physics2D::SetSceneGravity` implemented
    - [x] Applies a flat acceleration force to every dynamic rigidbody in the scene beginning of frame.
    - [x] Use `+` and `-` keys to adjust gravity amount
    - [x] Current gravity should be visible on the screen (hotkey hint here is nice)
- [x] *05pts*: Objects that fully fall off the sides of the screen should appear on the opposite side, maintaining velocity
- [x] *05pts*: Implement `Rigidbody2D::GetVelocity`
    - [x] Objects that falling off the bottom of the screen, reverse its velocity along Y (bounce it)
    - [x] Only bounce if it is falling down (-y)

------

### Control Scheme

- `WASD` for Camera Movement.
- `O` (capitol letter 'Oh') to reset camera Position to origin. ( this does not rest the camera output Size ).
- `Right Mouse Button` To spawn New Game Objects ( circular disc type ).
- `Scroll Mouse Wheel Up` and `Down Movement` for Zooming in and out.
- `Left Click` to select any Game Object and `click again` to Deselect the object.
- Select an object and hit `delete` or `backspace` key to delete the currently selected object.
- `F8` key to delete and reset the game.
- Press `2` while any Game Object is not selected to enter *Polygon Draw mode*.
- Use `+` or `-` ( non numpad keys ) to change the Scene Gravity.

**Polygon Draw Mode Info ( When in Draw Mode )**
- use `Backspace` to delete last added point. Can be used to delete all points if you continuously press `Backspace` in the reverse order of insertion. If no points are left on screen you can still create the polygon as you have technically not exited the draw mode yet.
- use `Esc` to cancel the polygon construction and exit out of the draw mode.
- use `Right Mouse button` to confirm the polygon construction and exit of the draw mode.
- If number of points are less then 3 and `Right Mouse button` is pressed then you exit out of draw mode.
- *When you first enter the draw mode, the first point is the place where the mouse is currently located and this point can me outside the client space if cursor is outside the client space and the window is active when you enter the draw mode, this first point can also be deleted using `BackSpace`*
- *__NOTE__ :- Center of the polygon is considered as average of all valid vertices of polygon.*
- Objects that overlap the cursor will still be highlighted but new disc objects cannot be spawned while in this mode and also no object can be selected while in this mode.

**Object Selection**
-  While selected, `1`, `2`, and `3` should switch the object to `STATIC`, `KINEMATIC`, or `DYNAMIC` respectively.
- *__NOTE__ :- Once you select an object you are essentially changing its velocity over the next 30 frames so when you change it to `KINEMATIC` make sure you drag around the cursor or it shall feel as if the object is now `STATIC` as velocity due to no cursor drag would be essentially ZERO.*
- By default all objects are created in `DYNAMIC` mode.
- Selected object is not influenced by physics simulations i.e. the gravity and movement but collisions will still be visible.
- Select an object and drag around the cursor and deselect the object to throw it around ( might feel slow also recommended to use an actual mouse rather than the trackpad ).
- *__NOTE__ :- may feel a bit more finicky when selecting polygons in comparison to the discs.*

Makes sure you mouse position is correct for your current view - converting the client position (offset from top-left of window) to a world location.
------
