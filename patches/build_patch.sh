RELATIVE_LOCATION=vlc/modules/moviesoap/patches
VLC_LOCATION=../../../..


cd $VLC_LOCATION
diff -rbc vlc $RELATIVE_LOCATION/vlc > $RELATIVE_LOCATION/vlc.patch