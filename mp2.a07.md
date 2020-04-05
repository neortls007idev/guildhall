

### A07 Checklist
**( NOTE :- Controls And Known bugs Listed at the Bottom after the Checklist )**
------

## Checklist
- [x] Remove screen wrap and bottom of screen bounce.
    - [x] Add some static world geometry (at least a floor) to catch falling objects.
    - [x] Destroy objects that fall out of world bounds
        - [ ] Option A: Define a world bounds and draw it so users can see where objects should destroy at.
        - [x] Option B: Destroy objects that leave screen either left/right/bottom, but NOT top.
- [x] Polygon-v-Polygon Intersection Check (using GJK)
- [x] Polygon-v-Polygon Manifold (normal/penetration) - object push out correctly
- [x] Polygon-v-Polygon Contacts - Objects generate one or two contact points.
    - [x] Manifold now returns a contact edge (two points) instead of just a single contact point.  For cases where only one contact exists,
          return an edge with the start and end point being the same.
- [x] *Optional*: Apply Impulse using contact points
    - [x] *Optional*: Calculate impulse using the center of the contact edge.
    - [ ] *Optional*: When applying impulse, use the point on the edge closest to each body's center of mass.
- [x] *Optional*: To make friction/bounce more stable - calculate normal impulse first, apply it, and then generate tangent impulse, and apply it.

------

##General Notes

-[1] Floor Object is now present and does not move with the camera and cannot to destroyed.( drawn in orange. )
-[2] All objects other than the floor object now spawn in dynamic mode.
-[3] Initial friction is set to 1.0 , Initial Bounciness is set to 0.8, Initial Linear drag is set to 0.1.

------
### Known Errors/ Bugs

- [1] contact points are imperfect and at time polygons may intersect.
- [2] might be just me but non - uniformly dense polygon rotations seem weird.
- [3] Using a temp hack in polygon collider update as a fix - but even with 0 rotation polygons seem to be having a sheering effect on them - to be discussed with forseth.
- [4] static disc vs polygon collision seem to be having a sticky effect. - to be discussed with forseth.

------

### Control Scheme

- `WASD` for Camera Movement.
- `O` (capitol letter 'Oh') to reset camera Position to origin. ( this does not rest the camera output Size ).
- `Right Mouse Button` To spawn New Game Objects ( circular disc type ).
- `Scroll Mouse Wheel Up` and `Down Movement` for Zooming in and out.
- `HoldDown  Left Mouse Button` to select any object and `release` the button to release the object.
- Select an object and hit `delete` or `backspace` key to delete the currently selected object.
- `F8` key to delete and reset the game.
- Press `2` while any Game Object is not selected to enter *Polygon Draw mode*.
- Use `+` or `-` ( non numpad keys ) to change the Scene Gravity.

**Physics Controls**

Most controls are present in the hover-over tooltip.

 `[` , `]` : Decrement , Increment Mass by 1.


 `1` `STATIC` , `2` `DYNAMIC` , `3` `KINEMATIC` : Update Simulation Mode

`Left Arrow Key` , `Right Arrow Key` : Decrement , Increment Friction( defaulted to 0.8 ) by 0.05.

 `:`  ,  `\` : Decrement , Increment Drag by 0.5 * deltaSeconds.

`Scroll Mouse Wheel Up` and `Down Movement` : Decrement , Increment Bounciness.
`R`  ,  `F` : Decrement , Increment Rotation In Degrees by 10 * deltaSeconds.
`T` , `G` , `V` : Decrement , Increment , Reset Angular Velocity In Degrees
`K`  ,  `L` : Decrement , Increment Angular Drag by 0.5 * deltaSeconds

*__NOTE__ :- If angular velocity is manually provided to a static object then it will rotate in place once object is deselected.*

*__NOTE__ :- All most multipliers can be changed using GameCommon.hpp.*

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
- *__NOTE__ :- Once you select an object you are essentially changing its velocity over the last 30 frames till deselection, so when you change it to `KINEMATIC` make sure you drag around the cursor or it shall feel as if the object is now `STATIC` as velocity due to no cursor drag would be essentially ZERO.*
- By default all objects are created in `DYNAMIC` mode.
- Selected object is not influenced by physics simulations i.e. the gravity and movement but collisions will still be visible.
- Select an object and drag around the cursor and deselect the object to throw it around ( might feel slow also recommended to use an actual mouse rather than the trackpad ).
- *__NOTE__ :- may feel a bit more finicky when selecting polygons in comparison to the discs.*

Makes sure you mouse position is correct for your current view - converting the client position (offset from top-left of window) to a world location.

------
