-------------------------------------------------------------------------------
COMP 4300 - Assignment 5
-------------------------------------------------------------------------------
Assignment Notes:
-------------------------------------------------------------------------------

Submission:

For this assignment you will zip and submit the entire 'src' folder of the
assignment. You are *NOT* allowed to add any files in addition to the ones
already in the src folder from the assignment zip file. This means that you
will not be submitting ANY changes you have made to the visual studio project,
if you used it for development. 

YOU MUST INCLUDE THE FOLLOWING INFO IN A COMMENT AT THE TOP OF main.cpp:
- ALL students' full names, cs usernames, and student numbers
- If you did not get a specific feature to work, please explain what you tried 
  to do to get it to work, and partial marks may be given for effort.

-------------------------------------------------------------------------------
Program Specification
-------------------------------------------------------------------------------

In this assignment you will be creating a Level Editor, using either Assignment
3 or Assignment 4 as the basis for your code. This level edit must have the
following features:

- The user should be able to mouse click on an entity in the level to select it
  and then move the mouse and left click somewhere else to reposition it. This
  should function in a similar way to the example shown in class.
- A new component 'CDraggable' should be created, and a new system 'sDrag'
  should be created. Any Entity which contains a CDraggable component should
  be click/draggable by the user as described above.
- The user should be able to insert a new Entity, or delete an existing Entity.
  Deleting an Entity should be done by pressing the Delete key while an Entity
  is currently selected. Inserting a new Entity should be done by pressing the
  Insert key, which places the new Entity on the mouse cursor.
- The user should be able to change the animation of an Entity while that 
  Entity is currently selected. Any animation defined in the assets file
  should be selectable by the user, and should be displayed to the user in some
  graphical way. For example, you could use the - or + key to cycle through
  possible animations, or a menu which allows you to select from available
  animations.
- The user should be able to enable a 'snap to grid' functionality which when
  enabled only allows the user to place Entities into the middle of a cell in
  a 64 x 64 pixel grid (similar to TilePos in Assignment 4). This snap to grid
  functionality should be able to be toggled by pressing the 'G' key, and the
  grid itself should be drawn on the screen when in snap mode.
- The user should be able to save the current level to a filename that is
  input by the user in some way inside the user interface, as well as be
  able to load a level from a filename specified by the user. You may get
  user to specify the filename via the console, but this will result in -5%
  
  
Marking Scheme:

- Existing entities Draggable:             30%
- CDraggable component / sDrag system:     10%
- New Entity creation / Entity deltion:    20%
- Entity animation selection from assets:  10%
- Snap to Grid:                            15%
- File Save / Load:                        15%

Due Date: Friday, November 30th @ 11:59pm
