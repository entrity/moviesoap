Installation
============
You've got to create a dir `[vlc-root]/modules/moviesoap` and put these files there.

Several files outside this dir must also be modified:
 - <root>/configure.ac
 - <root>/modules/Makefile.am
 - <root>/modules/gui/qt4/Modules.am
 - <root>/modules/gui/qt4/menus.cpp
 
Look in the dir `extra` (in this bundle) to see modified versions of these files. Look for lines with comments including the name "Markham" or "moviesoap." Modify your files accordingly. (It will probably work to just copy the modified versions to aforementioned locations.)
