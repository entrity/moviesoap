See the wiki
======

This README file isn't corrected/updated as easily as the wiki, so I suggest you look there for [How to Compile VLC with Moviesoap](https://github.com/Vaselinessa/moviesoap/wiki/How-to-Compile).

Just in case something goes wrong, however, it's a good idea to keep these instructions in the README:

###How to compile VLC with Moviesoap

You've got to create a dir `[vlc-root]/modules/moviesoap` and put these files there.

Then `cd` to moviesoap/patches and run `./apply_patch.sh`. This will modify several files outside the moviesoap directory:
* &lt;vlc-root&gt;/configure.ac
* &lt;vlc-root&gt;/modules/Makefile.am
* &lt;vlc-root&gt;/modules/gui/qt4/Modules.am
* &lt;vlc-root&gt;/modules/gui/qt4/menus.cpp

Then from the vlc-root directory, `./configure` and `make` as you normally would do.

----
Find more information at [http://moviesoap.org](http://moviesoap.org)