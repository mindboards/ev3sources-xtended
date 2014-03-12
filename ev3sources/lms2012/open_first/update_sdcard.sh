#! /bin/bash
echo
echo -------------------------------------------------------------------------------
echo UPDATE SDCARD WITH NEWEST KERNEL, FILESYSTEM AND APPLICATION          TCP120709
echo -------------------------------------------------------------------------------
echo
sudo -v
echo
echo "  ...."checking.sdcard

current=${PWD}

VFATPART=`mount | awk '/LMS2012 / { print $3 }'`
EXT3PART=`mount | awk '/LMS2012_EXT/ { print $3 }'`

if [ -z "$VFATPART" -o -z "$EXT3PART" ]; then
  echo "Card not mounted or properly formatted"
  exit 1
fi

echo "  ...."erasing.sdcard
sudo rm -r $VFATPART/* $EXT3PART/*

echo "  ...."copying.kernel.to.sdcard
sudo cp uImage $VFATPART/uImage

echo "  ...."expanding filesystem to card
sudo tar -C $EXT3PART -jxf lmsfs.tar.bz2 

echo "  ...."copying.application.to.sdcard
sudo cp -r ~/projects/lms2012/lms2012/Linux_AM1808/* $EXT3PART/home/root/lms2012

echo "  ...."copying.testprograms.to.sdcard
sudo cp -r ~/projects/lms2012/lmssrc/Test/Test $EXT3PART/home/root/lms2012/prjs/
sudo mkdir -m 755 -p $EXT3PART/home/root/lms2012/3rdparty-vm
sudo cp ~/projects/lms2012/c_vireobridge/Linuxlib_AM1808/*.so $EXT3PART/home/root/lms2012/3rdparty-vm
sudo cp ~/projects/lms2012/c_robotcvm/Linuxlib_AM1808/*.so $EXT3PART/home/root/lms2012/3rdparty-vm

echo "  ...."writing.to.sdcard
sync

echo
echo REMOVE sdcard

echo
echo -------------------------------------------------------------------------------
echo

