imgmgr
======

Lua application for Windows that adds some file system tools.  I intended to use it primarily for scripting deep file find/replace/modify operations.

This code includes lpng, ljpeg, Lua 5.2.3 and zib.

Sample lua scripts are located in the folder with the application source.

To use this for creating sprite sheets:
======

1)Install ImageMagick.

2)Build this program.

3)Copy it into your path (or add the output folder to your path).

4)Copy the makeSpriteSheet.lua script to the root folder where your individual frame images exist.

5)Run imgmgr makeSpriteSheet.lua

6)Find spriteSheet.png in the folder where you ran the script.
