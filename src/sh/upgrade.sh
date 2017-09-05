#!/bin/sh
#检测用户必须为ROOT
USER_ID=`id -u`
ROOT_UID=0
if [ $USER_ID -ne $ROOT_UID ]
then
	echo "[Permission denied],must run upgrade.sh by root user!"
	exit 0
fi

#升级操作
UPGRADE_FILE="/ivadata/upgrade/upgrade.tar.gz"
if [ -f "$UPGRADE_FILE" ]
then
	echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
	echo "@@@@@@@@@@@@@@@@@@@@   UPGRADE    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
	echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
	echo "@@Old Version is Backup to iva_bak_`date +%Y%m%d%02k%M`.tar.gz"
	cd ../../
	echo "@@backup begin..."
	tar -zcvf iva_bak_`date +%Y%m%d%02k%M`.tar.gz iva/*
	echo "@@backup done..."
	echo "@@uppack new version package file..."
	echo "@@uppack begin..."
	tar -zxvf $UPGRADE_FILE
	echo "@@uppack done..."
	rm $UPGRADE_FILE
	cd iva/bin
else
	echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
	echo "@@@@@@@@@@@@@@@@@@@@   NO  UPGRADE    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
	echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
fi




