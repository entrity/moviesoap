# INSTRUCTIONS:
# CD INTO THE ROOT DIRECTORY OF YOUR SOURCE TREE FOR VLC. FROM THERE, RUN THIS
# SHELL SCRIPT. E.G.: $ ./modules/moviesoap/cross-compile/compile.sh
#
# WHEN THE COMPILATION IS COMPLETE. THE INSTALLER SHOULD BE LOCATED AT
# <pwd>/win32/vlc-2.1.0-git-win32.exe

HOST=i686-w64-mingw32
TOPDIR=`pwd`
LOGFILE=$TOPDIR/moviesoap_cross_compile.log

echo vlc root is $TOPDIR
echo host is $HOST


# clear any existing logfile
> $LOGFILE

# function to write to logfile
log()
{
	echo $1 >> $LOGFILE
}

log "`date` host is $HOST"
log "------------------------------"

# BUILD 3RD-PARTY CONTRIBS
# UNCOMMENT THE FOLLOWING IF THIS IS THE FIRST TIME YOU ARE COMPILING THIS
# PARTICULAR DIRECTORY TREE OF VLC.

# git clone git://git.videolan.org/vlc.git vlc &&
# cd vlc &&
# VLC_ROOT=`pwd` &&
# log "`date` - vlc root is $TOPDIR" &&
# log "`date` - in vlc root" &&
# mkdir -p contrib/win32 &&
# cd contrib/win32 &&
# ../bootstrap --host=$HOST &&
# log "`date` - bootstrapped contribs" &&
# make fetch &&
# log "`date` - make fetch completed for contribs" &&
# make &&
# log "`date` - make completed for contribs" &&

# BOOTSTRAP, CONFIGURE, AND MAKE VLC

cd $TOPDIR &&
./bootstrap &&
log "`date` - bootstrap completed in vlc root" &&
mkdir -p win32 && cd win32 &&
../extras/package/win32/configure.sh --host=$HOST &&
log "`date` - configure completed for win32 build" &&
make &&
log "`date` - make completed" &&
make package-win32 &&
log "`date` - make package-win32 completed" &&

log "`date` - DONE"
