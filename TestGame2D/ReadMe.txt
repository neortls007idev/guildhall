

Known Issues

1. Health system is buggy; equal amount of health is not transferred to the next level.
2. If the player transfers to the next map in debug camera mode then F4 needs to be pressed twice to readjust the debug camera.
3. the gameover screen does not adjust well with map changes and may spawn off screen.
4. If noclip mode is used the bullets or entites tht move out of the map onlydie when switched to a new map.
5. Bullets can tunnel thorugh when player shoots between 2 diagonally located stone tiles.
6. the debug timescale options don't work with the sound system.
7. Mud tiles don't slow you down yet for some reason.
8. Bullets deflect boulders incorrectly.

=========================================================================

Intentional Behaviours:-

Enemy and Player Bullets can Destroy Each other.

==============================================================================

How to Use/Controls

- ESDF (Xbox Contorller left stick) to drive forward and turn the tank toward a direction
- IJKL (Xbox Contorller right stick) to turn the turret toward a direction
- Enter Key( Xbobx Contorller Start Button) to start the Game
- P to toggle Pause/Unpause while Playing
- ESC exits game.

- F1 (cheat) toggles debug drawing of inner (cyan) and outer (magenta) radii
- F3 (cheat) toggles noclip mode (disables physics for player tank only)
- F4 (cheat) toggles debug camera (vs. normal game camera), view entire map
- F8 (cheat) deletes and re-creates a new Game object, sets mode to Playing
- T (cheat) hold to sustain slow-motion (0.1x timescale)
- Y (cheat) hold to sustain fast-motion (4.0x timescale)

Keyboard keys ESDF and IJKL for moving tnak and turret still exist but you may not get the behaviour which is intended.

==============================================================================
Method 1(for new users): -
	
	1. Extract the zip folder to you desired location.
	2. Open the following path --> …\SD\Incursion\Run 
	3. Double-click Incursion_x64.exe to start the program.

Method 2(for advanced users):-

	1. Extract the zip folder to you desired location.
	2. Open the following path --> …\SD\Incursion
	3. Open the Incursion.sln using Visual Studio 2019.
	4. Press F7 key to build solution or go to Build --> Build Solution option using the Menu bar.
	5. Press Ctrl + F5 key to start the program without debugging or go to Debug --> Start without Debugging option using the Menu bar.

	NOTE:- 	
		* Visual Studio 2019 must be installed on the system.
		* In step 5 mentioned above, if you want you can execute the program with the debugger too.

Deep Learning
	
	The first important thing I learned this time with Incursion was that do not write code that is as smart as you are cause to debug your code you need to be smarter than your code which causes bugs to arise which you are not redy to deal with. The other important thing I learned was how there are different practices and standards and how you map them together( mapping spritesheet UVs to OpenGl Uvs) and why these standard need to be followed or what troubles can arise if we decide to go otherwise. I also realized there are streetsmart ways to code certain behaviours and knowing when and when not to use those streetsmart ways. Certain things that may simple to implement can be a lot harder to implement when you are trying to match the thought process of someone else( I knew how to map the texture to the spritesheets but doing it the way as shown in the class was a whole different challenge).



