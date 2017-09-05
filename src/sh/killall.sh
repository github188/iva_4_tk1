#!/bin/sh
#检测用户必须为ROOT
USER_ID=`id -u`
ROOT_UID=0
if [ $USER_ID -ne $ROOT_UID ]
then
	echo "[Permission denied],must run killall.sh by root user！"
	exit 0
fi

startnum=`ps -e | grep 'runall.sh' | wc -l`
#echo startnum=$startnum
if [ $startnum -ne 0 ]
then
	echo "kill killiva.sh"
	kill -9 `ps -e | grep 'runall.sh' | awk '{print $1}'`
fi

ivanum=`ps -e | grep 'iva' | wc -l`
#echo ivanum=$ivanum
if [ $ivanum -ne 0 ]
then
	echo "kill iva*"
	kill -9 `ps -e | grep 'iva' | awk '{print $1}'`
fi

ipcsnum=`ipcs -q | grep '^0x0000600' | wc -l`
#echo ipcsnum=$ipcsnum
if [ $ipcsnum -ne 0 ]
then
	echo "ipcrm message queue"
	for i in `ipcs -q | grep '^0x0000600' | awk '{print $2}'`
	do
		ipcrm -q $i
	done
fi

appnum=`ps -e | grep 'iva' | wc -l`
#echo appnum=$appnum
if [ $appnum -eq 0 ]
then
	echo "[OK] All iva processes have been killed."
else
	echo "[ERROR],Some process cannot be killed!"
	exit 0
fi

ipcsnum=`ipcs -q | grep '^0x0000600' | wc -l`
#echo ipcsnum=$ipcsnum
if [ $ipcsnum -eq 0 ]
then
	echo "[OK] All iva message queue have been killed."
else
	echo "[ERROR],Some message queue cannot be killed!"
	exit 0
fi

#sleep 1
#rm /tmp/alg_h264_tmp.cache
#echo 1 > /proc/sys/vm/drop_caches
#sleep 1
