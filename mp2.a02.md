## MP2 : A02 Checklist

- [x] *05pts*: Insantiate a physic system into the game.
- [ ] *10pts*: Be able to create game objects with a collider based on button press,
    - [ ] `1` should create random radius disc centered at cursor
- [-] *10pts*: Camera changes implemented
- [x] *05pts*: Can move around using `WASD` or equivalent
- [x] *10pts*: Can zoom in and out using mouse-wheel.
    - [x] Make sure this is clamped at a max and min "zoom" so zoom can't go negative.
- [x] *05pts*: Pressing 'O' (capitol letter 'Oh') should reset camera position and zoom to default (ie, move to origin)
- [ ] *05pts*: Be sure to render the objects in your scene, by default...
    - [ ] Render with a blue border
    - [ ] Render with 50% alpha white interior
- [ ] *10pts* If mouse is inside the object, render the border yellow.
- [ ] *15pts* Be able to drag them around
    - [ ] If the mouse is dragging the object, render the border dark green.
- [ ] *10pts* If the object is touching another object, render the entire interior at 50% alpha red instead of white.
    - [ ] Disc-vs-Disc collision should be same as MP1
- [ ] *10pts* If dragging, and `delete` or `backspace` is pressed, destroy object.
- [x] *05pts* Press `Escape` to quit.
- [x] Add `mp2.a02.md` readme to root folder

Makes sure you mouse position is correct for your current view - converting the client position (offset from top-left of window) to a world location.

------
