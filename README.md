How to compile VLC with Moviesoap
============
You've got to create a dir `[vlc-root]/modules/moviesoap` and put these files there.

Then cd to moviesoap/patches and run ./apply_patches.sh.
	This will modify several files outside the moviesoap directory:
	 - <vlc-root>/configure.ac
	 - <vlc-root>/modules/Makefile.am
	 - <vlc-root>/modules/gui/qt4/Modules.am
	 - <vlc-root>/modules/gui/qt4/menus.cpp
