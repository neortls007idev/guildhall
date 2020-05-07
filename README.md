MP2.Final : Fleshing Out the System
======

## Overview
Just going to flesh out the physics system so it can easily be used for a game.


## Checklist

- [x] Be able to attach user data to a rigidbody and/or collider to hook the physics system up to game code.

- [x] Add collision/contact events
    - [x] `OnOverlapBegin`
    - [x] `OnOverlapStay`
    - [x] `OnOverlapLeave`

- [x] Add Trigger Volumes
    - [x] `OnTriggerEnter`
    - [x] `OnTriggerStay`
    - [x] `OnTriggerLeave`

- [x] Support Physics Layers
    - [x] Be able to specify which layer a rigid body and/or collider belongs to.
    - [x] Be able to enable or disable collision between two given layers.
    - [x] Only process collisions if the two objects are allowed to interact
    - [x] Only process triggers if the two objects are on the same layer

- [ ] **Optional**: Support axis locks.
    - [ ] Support `X` and `Y` axis locks, only allowing movement in those directions.
    - [ ] Support `Rotation` lock preventing the object from rotating.

------
## Next Steps - The Future is here soon
This assignmetn just fleshes out the features of the system, but still has a lot of room for improvement.

If you find yourself using it in DFS or Ludum Dares, some areas of improvement are...
- `Transform2D`
  - Have a transform object that a rigidbody can watch/modify instead of having to copy back out.
  - That or allow transforms be parent/child relationships so that your game object can just child itself to the rigidbody.
- Optimization
  - Better Memory Management
  - Scene Management
- Queries
  - `GetOverlappingObjects` - Ask the environment for all objects touching a shape with certain properties
  - `Raycast2D` - Be able to raycast against first hit, or all hit in the scene.
- Effectors
  - Objects in a scene that don't collider, but instead apply some other kind of force/behaviour on interaction
    - 1-Way Platforms
    - Gravity Wells
    - Conveyer Belts
- Constraints/Joints
  - Be able to connect rigidbodies together with a constraint/joint - usedful for ragdolling
    - `FixedJoint` - maintains rotation and dispacement between two objects
    - `SpringJoint` - maintains rotation, and behaves long a spring along the displacement vector
    - `HingeJoint` - Maintain distance, but allow rotation between two objects.
    - ...

======
##General Notes

1. Floor Object is now present and does not move with the camera and cannot to destroyed.( drawn in orange. )
2. All objects other than the floor object now spawn in dynamic mode.
3. Initial friction is set to 1.0 , Initial Bounciness is set to 0.8, Initial Linear drag is set to 0.1.

------
### Known Errors/ Bugs

1. contact points are imperfect and at time polygons may intersect.
2. might be just me but non - uniformly dense polygon rotations seem weird.
3. Using a temp hack in polygon collider update as a fix - but even with 0 rotation polygons seem to be having a sheering effect on them - to be discussed with forseth.
4. static disc vs polygon collision seem to be having a sticky effect. - to be discussed with forseth.

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

`Shift` + `Scroll Mouse Wheel Up` and `Down Movement` : Decrement , Increment Bounciness.
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
