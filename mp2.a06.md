

### A02 Checklist
**( NOTE :- Controls Listed at the Bottom after the Checklist )**
------

```markdown
MP2.A06 Rotation
======

## Overview
Add in rotation.

**Turn-In Branch Name: `mp2/turnin/a06`**

**Due Date: Mar 12, 2020**

### Goal

------

## Checklist
- [x] Angular Update - Add members to `Rigidbody2D`
    - [x] Add `float m_rotationInRadians`
    - [x] Add `float m_angularVelocity`
    - [x] Add `float m_frameTorque`
    - [x] Add `float m_moment`
    - [x] Add appropriate getter/setter methods
- [x] `Rigidbody2D::CalculateMoment` to calculate moment based on attached collider and mass
    - [x] `virtual float Collider2D::CalculateMoment( float mass )`
    - [x] Implement `DiscCollider::CalculateMoment`
    - [x] Implement `PolygonCollider::CalculateMoment`
    - [x] When setting **mass** or **collider** on a rigidbody - recalculate your moment.
        - *Note: When setting mass, you can do less work by taking advantage of the ratio of new mass to old mass should match the ratios of the moments*.
- [x] Rotating a `Rigidbody2D` properly updates the **world shape** of the collider.
    - [x] Add controls to rotate a selected object
        - Suggest **R** and **F**
    - [x] Add controls to update the rotational velocity of an object
        - Suggest **T**, **G**, and **V** to increase, decrease, and reset
- [x] `Rigidbody2D` now do angular calculations during their update.
    - [x] Use torque to compute an angular acceleration
    - [x] Apply angular acceleration to update angular velocity
    - [x] Apply angular velocity to update rotation
- [x] Update `Rigidbody2D::GetImpactVelocity` to take into account rotational velocity.
- [x] Update `ApplyImpulseAt` to apply impulse to torque based on positions.
- [x] Update impulse calculations to take into account rotational forces, see PDF in lnks
- [x] Update tooltip to show...
    - [x] Moment of Inertia
    - [x] Current Rotation (degrees)
    - [x] Current Angular Velocity

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
