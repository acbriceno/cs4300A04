-------------------------------------------------------------------------------
COMP 4300 - Assignment 4
-------------------------------------------------------------------------------
Assignment Notes:
-------------------------------------------------------------------------------

Submission:

For this assignment you will zip and submit the entire 'src' folder of the
assignment. You are *NOT* allowed to add any files in addition to the ones
already in the src folder from the assignment zip file. This means that you
will not be submitting ANY changes you have made to the visual studio project,
if you used it for development. Put your level somewhere in the zip file.

YOU MUST INCLUDE THE FOLLOWING INFO IN A COMMENT AT THE TOP OF main.cpp:
- ALL students' full names, cs usernames, and student numbers
- If you did not get a specific feature to work, please explain what you tried 
  to do to get it to work, and partial marks may be given for effort.

-------------------------------------------------------------------------------
Program Specification
-------------------------------------------------------------------------------

In this assignment you will be writing the game that was presented in class.
This game must have the following features:

Assets:
- Entities in the game will be rendered using various Textures and Animations
  which we will be calling Assets (along with Fonts)
- Assets are loaded once at the beginning of the program and stored in the
  Assets class, which is stored by the GameEngine class
- All Assets are defined in assets.txt, with the syntax defined below

Player:
- The player Entity in the game is represented by NotLink, which has several
  different Animations: RunDown, RunUp, RunRight, StandDown, StandUp,
  StandRight, AtkUp, AtkDown, AtkRight. You must determine which direction
  and state the player is currently in and assign the correct Animation.
- The player is assigned the following animations in the direction facing.
  Please note that Left will be accomplished by mirroring Right (x scale -1)
  StandDir  - When no input (or both opposite) is being given to the player
  RunDir    - When move input is given to the player in a direction
  AttackDir - When the player's sword is currently visible from an attack
- The player moves with the following controls:
  Left: A key, Right: D key, Up: W Key, Down: S Key, Attack: Space Key
- The player can move up, left, down, right, at any time during the game
- The player can only move either horizontally or vertically at a given time
- If opposite directions are held, the player will be given a Stand anim
- The player can only attack once its current attack animation has finished
- The player collides with 'Tile' entities in the level (see level syntax) 
  whose bounding box blocks movement and cannot move through them. 
- The player will be given a CBoundingBox of a size specified in the level file

Other Entities:
- Each tile and npc in the game specifies two boolean values of whether it
  blocks movement and whether it blocks vision. If a tile blocks movement, 
  the player collides with it and cannot pass through. If an entity blocks
  vision then it does not affect enemy line of sight. See spec below.
- CBoundingBox has two new booleans to reflect these changes

Attacking:
- When the player attacks, a sword appears for 150ms and then disappears
  approximately 1 tile away from the player in the direction they are facing.
- The player's sword should be given a bounding box equal to the anim size
- When the sword collides with an enemy, it destroys the enemy
- When the player collides with an enemy, it respawns back at the start

NPC Entities:
- NPC entities will each be given an 'ai' behavior in the level file, either
  'Follow' or 'Patrol'. 
- 'Follow' means it will follow the player when the player is direct line of 
  sight, or head back to its original position when not in line of sight.
  When an entity is at its home position, it should not oscillate.
- 'Patrol' means it will move toward a series of patrol positions, looping
  back around to the start when the final one is reached. An npc has reached
  its patrol position if it is within a distance of 5 from it. 

Tiles:
- Tiles are Entities that define the level geometry and interact with players
- Tiles can be given any Animation that is defined in the Assets file
- Tiles will be given a CBoundingBox equal to the size of the animation
  tile->getComponent<CAnimation>()->animation.getSize()
- The current animation displayed for a tile can be retreieved with:
  tile->getComponent<CAnimation>()->animation.getName()

Drawing:
- The game window is given a fixed size of 1280x768 pixels (20x12 tiles)
- Rendering of entities is provided for you, as well as a 'debug' rendering
  mode which can be toggled with the 'F' key, and the 'R' key toggling
  texture rendering.
- You are required to change the camera position (window.view()) of the game
  window to alternate between two modes based on the m_follow variable:
  true:  Follow Mode - the camera is always centered on NotLink
  false: Room Mode   - the camera is centered on the middle of the room

Rooms:
- Each level is split up into individual 'rooms' given an (x,y) coordinate 
- Room (x,y) is defined by the rectangle (x*w, y*h, w, h) where w and h are
  the window width and height respectively.
- When NotLink moves to a new room, the camera should center to that room
  
Misc:
- The 'P' key should pause the game
- The 'F' key toggles wireframe / debug rendering
- The 'R' key toggles texture rendering
- The 'Y' key should toggle between follow camera or 'room' camera
- The 'ESC' key should go 'back' to the Main Menu, or quit if on the Main Menu

-------------------------------------------------------------------------------
Config Files
-------------------------------------------------------------------------------

There will be two configuration files in this assignment. The Assets config
file, and the Level configuration file.

-------------------------------------------------------------------------------
Assets File Specification
-------------------------------------------------------------------------------

There will be three different line types in the Assets file, each of which
correspond to a different type of Asset. They are as follows:

Texture Asset Specification:
Texture N P
  Texture Name     N        std::string (it will have no spaces)
  Texture FilePath P        std::string (it will have no spaces)

Animation Asset Specification:
Animation N T F S
  Animation Name   N        std::string (it will have no spaces)
  Texture Name     T        std::string (refers to an existing texture)
  Frame Count      F        int (number of frames in the Animation)
  Anim Speed       S        int (number of game frames between anim frames)
  
Font Asset Specification:
Font N P
  Font Name        N        std::string (it will have no spaces)
  Font File Path   P        std::string (it will have no spaces)
  
-------------------------------------------------------------------------------
Level Specification File
-------------------------------------------------------------------------------
  
Player Specification:
Player X Y BX BY S
  Spawn Position    X Y        int, int
  Bounding Box Size BX BY      int, int
  Speed             S          float

Tile Specification:
Tile Name RX RY TX TY BM BV
  Animation Name    Name      string
  Room Coordinate   RX RY     int, int
  Tile Position     TX TY     int, int
  Blocks Movement   BM        int (1 = true, 0 = false)
  Blocks Vision     BV        int (1 = true, 0 = false)
  
NPC Specification:
NPC Name RX RY TX TY BM BV AI ...
  Animation Name    Name      string
  Room Coordinate   RX RY     int, int
  Tile Position     TX TY     int, int
  Blocks Movement   BM        int (1 = true, 0 = false)
  Blocks Vision     BV        int (1 = true, 0 = false)
  AI Behavior Name  AI        string
  AI Parameters     ...       (see below)

AI = Follow
  ... = S
  Follow Speed      S         float (speed to follow player)

AI = Patrol
  ... = S N X1 Y1 X2 Y2 ... XN YN
  Patrol Speed      S         float
  Patrol Positions  N         int (number of patrol positions)
  Position 1-N      Xi Yi     int, int (Tile Position of Patrol Position i)

For Example:
NPC  Tektite  0  0 15 10 0 0 Patrol 2 4 15 10 15 7 17 7 17 10
- Spawn an NPC with animation name Tektie in room (0,0) with tile pos (15,10)
- This NPC does not block movement or vision
- The NPC has a Patrol AI with speed 2 and 4 positions, each in room (0,0)
  Positions: (15,10) (15,7) (17,7) (17,10)
 
-------------------------------------------------------------------------------
Assignment Hints
-------------------------------------------------------------------------------

You can use all of your collision code from Assignment 3 in this assignment.

I recommend approaching this assignment in the following order:

1. Implement WASD movement for the player such that it moves u/l/d/r when
   those keys are pressed. This will allow you to test rectangular collisions
   as soon as possible without worrying about implementing gravity just yet
2. Implement GameState_Play::loadLevel(), which will allow you to load the
   assets for the level given to you to test the rest of the assignment
3. Implement spawnPlayer() so that it correctly uses the configuration
   specified by the level file so you can start testing collisions
4. Implement the camera changes described above
5. Implement spawnSword() so that it spawns a sword in the correct position
   with a lifespan of 150ms
6. Implement player attacking / sword collision with NPCs
7. Implement player animations to correctly animate based on player state
8. Implement NPC Patrol behavior
11.Implement NPC Follow Behavior without vision
9. Implement Physics::LineIntersect() which will allow you to test whether or
   not two line segments AB and CD are intersections, and where they intersect
10.Implement Physics::EntityIntersect() which will allow you to test whether
   line segment AB intersects with any lines of the bounding box of Entity e
11.Implement NPC Follow Behavior using 10 to check line of sight
