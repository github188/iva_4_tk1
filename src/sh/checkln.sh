#!/bin/sh
#检测用户必须为ROOT
USER_ID=`id -u`
ROOT_UID=0
if [ $USER_ID -ne $ROOT_UID ]
then
	echo "[Permission denied],must run checkln.sh by root user!"
	exit 0
fi

CheckLn()
{
	REAL_PATH=$1
	LINK_PATH=$2
	mkdir -p "$REAL_PATH"
	ln -s -f $REAL_PATH $LINK_PATH|| { rm -f $LINK_PATH && ln -s $REAL_PATH $LINK_PATH; };
	echo "ln -s -f $REAL_PATH $LINK_PATH"
}

cd ../web
CheckLn "/ivadata" "ivadata"
CheckLn "/mnt/usb" "usb"
CheckLn "/mnt/sd" "sd"
CheckLn "/mnt/hdd" "hdd"
cd ../bin

