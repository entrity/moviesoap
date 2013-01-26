# The purpose of this file is for collecting the files that need to be manually
# changed. You should do this if you don't have a working patch. Then you can
# use your manually changed files to make a patch file.
#
# After manually changing the files which this script shall copy into a directory
# tree beneath its own directory, run the file ./build_patch.sh to build a patch file.
#
# That patch file should be run from the parent directory of vlc-root. To apply that
# patch, you can just run the file ./apply_patch.sh




VLC_LOCATION=../../../..

# Ensure that a version of the necessary dirs from the vlc directory tree
# exists local to this 'patches' dir
mkdir -p vlc/modules/gui/qt4
mkdir -p vlc/src/config

# These are the files that need patching, relative to the directory above vlc-root
files_to_patch=( 
	vlc/configure.ac
	vlc/modules/Makefile.am
	vlc/modules/gui/qt4/Modules.am
	vlc/modules/gui/qt4/menus.cpp
	vlc/src/config/cmdline.c
	)

if [[ $1  && $1 = reverse ]]; then
	
	for patchable in ${files_to_patch[@]}; do
		if [ -e $patchable ]; then
			rm $patchable
		fi
		if [ -e $patchable.cpy ]; then
			mv $patchable.cpy $patchable
		fi
	done
	echo Your changes have been reversed

else

	for patchable in ${files_to_patch[@]}; do
		# only act if local file and exterior file are different
		if ! [ -e $patchable ] || ! diff $VLC_LOCATION/$patchable $patchable >/dev/null; then
			# rename existing dup files (if any)
			if [ -e $patchable ]; then		
				echo moving to backup: ./$patchable
				mv $patchable $patchable.cpy
			fi
			# copy files from vlc core
			echo copying to local tree: $patchable
			cp $VLC_LOCATION/$patchable $patchable
		fi
	done
	echo It is now appropriate to make your changes to the local files.

fi
