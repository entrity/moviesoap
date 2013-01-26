CURRENT_LOCATION=`pwd`
VLC_LOCATION=../../../..

cd $VLC_LOCATION
patch $@ -p0 -i $CURRENT_LOCATION/vlc.patch