#!/bin/sh
#检测用户必须为ROOT
USER_ID=`id -u`
ROOT_UID=0
if [ $USER_ID -ne $ROOT_UID ]
then
	echo "[Permission denied],must run profile2.sh by root user!"
	exit 0
fi

#还原配置
PROFILE_FILE="/ivadata/profile/profile.tar.gz"
if [ -f "$PROFILE_FILE" ]
then
	echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
	echo "@@@@@@@@@@@@@@@@@@@@   RESTORE PROFILES   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
	echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
	echo "@@Old profile is Backup to iva_profile_bak_`date +%Y%m%d%02k%M`.tar.gz"
	cd ../
	echo "@@backup begin..."
	tar -zcvf iva_profile_bak_`date +%Y%m%d%02k%M`.tar.gz profiles
	echo "@@backup done..."
	echo "@@uppack new profile package file..."
	echo "@@uppack begin..."
	tar -zxvf $PROFILE_FILE
	echo "@@uppack done..."
	rm $PROFILE_FILE
	cd bin
else
	echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
	echo "@@@@@@@@@@@@@@@@@@@@   NO  RESTORE    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
	echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
fi




