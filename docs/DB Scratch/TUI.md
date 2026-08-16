# Rendering

###### All things here render what they receive, and should not affect the properties or structure in any way. 

Screen is rendered through a 2D buffer (main buffer) , this should be a class named Screen as it can have related methods later on.
-  Gets terminals rows and columns.
-  constructor inits buffer, etc
-  destructor destroy buffer, 

Infinite loops re-render this buffer.

For components a Base class can be made, components could be deriving from this class.

Components :- Text, Box

Internally each component contains:
-  functions to update the main buffer with its content
-  height, width, x, y
-  takes style (strictly a struct weather it be text, box).
-  *some other things...*

A fixed ratio scaling cannot be used as this is terminal not screen, a terminal contains cells in which only characters can be filled and given style, so the placement of components depends on how user defines the structure.

Finally, the rendering should be done by writing to stdout file via  `write()`, also prepend \033[H

# Input Handler

	Operate at 1byte at a time

This changes a state of all component present on screen and detects mouse click. (only component)

Main terminal cursor renders rows from buffer, so it is not available to work with, so only mouse clicks can change states/make elements active and keyboard can change text boxes...for now.

A structure can be made out of current elements on screen which will be given to input handler so that it can change the whole structure and return changed structure to render in main loop.

**Tracks a state which defines which components is active and is being updated.**

Different streams can exists, now keyboard/mouse are on stdin, a new file can be made for database.

# Structure / AST

Can be a good place to access/manipulate elements to be rendered
# Behavior/Components

#### Text-box Behaviors

- Supports backspace, Enter(\n) as submit inner text , adding printable characters to inner text

#### Table Behaviors

-  Supports Nothing only inner text

#### Folder/ Item Navigator

-  Supports Arrow keys, Navigation keys

