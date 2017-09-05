#!/bin/sh
#检测用户必须为ROOT
USER_ID=`id -u`
ROOT_UID=0
if [ $USER_ID -ne $ROOT_UID ]
then
	echo "[Permission denied],must run runall.sh by root user!"
	exit 0
fi

chmod 777 ./*.sh
chmod 777 ./iva*

#升级操作
./upgrade.sh

#还原配置
./profile2.sh

#创建取证图片和录像访问的软连接
./checkln.sh

#调用其他脚本，优化运行环境
./tk1_gpu_max_clock.sh
./cpu.sh

#动态库路径
LD_LIBRARY_PATH="../lib/:../Alg:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH

#是否保存日志 0-不打印 1输出到屏幕
SHOW_BOA_LOG=0
SHOW_MASTER_LOG=1
SHOW_VIDEOIN_LOG=0
SHOW_VIDEOOUT_LOG=0
SHOW_ONVIF_LOG=0
SHOW_DATABASE_LOG=0
SHOW_RECORD_LOG=0

#执行当前路径下的一个可执行程序 $1-可执行程序名字 $2-是否显示打印 $3-是否后台执行
Run_Progress()
{
	IVA=$1
	LOG_FLAG=$2
	DEAMON=$3
	echo "Start ${IVA}..."
	if [ $DEAMON -eq 1 ]
	then
		case ${LOG_FLAG} in
		1)
			./${IVA} &
			;;
		*)
			./${IVA} >/dev/null 2>&1 &
			;;
		esac
	else
		case ${LOG_FLAG} in
		1)
			./${IVA}
			;;
		*)
			./${IVA} >/dev/null 2>&1
			;;
		esac
	fi
}

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

#循环检查所有进程，有进程挂掉重启所有
while [ 1 ]; do
	appexe1=`ps -e | grep 'ivaBoa' | wc -l`
	appexe2=`ps -e | grep 'ivaMaster' | wc -l`
	appexe3=`ps -e | grep 'ivaVideoIn' | wc -l`
	appexe4=`ps -e | grep 'ivaRecord' | wc -l`
	appexe5=`ps -e | grep 'ivaOnvif' | wc -l`
	appexe6=`ps -e | grep 'ivaDatabase' | wc -l`
	appexe7=`ps -e | grep 'ivaVideoOut' | wc -l`
	
	if [ $appexe1 -eq 0 ] || [ $appexe2 -eq 0 ] || [ $appexe3 -eq 0 ] || [ $appexe4 -eq 0 ] || [ $appexe5 -eq 0 ] || [ $appexe6 -eq 0 ] || [ $appexe7 -eq 0 ]; then
		echo appexe1=$appexe1
		echo appexe2=$appexe2
		echo appexe3=$appexe3
		echo appexe4=$appexe4
		echo appexe5=$appexe5
		echo appexe6=$appexe6
		echo appexe7=$appexe7
		kill -KILL `ps -e | grep 'iva' | awk '{print $1}'` > /dev/null 2>&1
		
		for i in `ipcs -q | grep '^0x0000600' | awk '{print $2}'`
		do
			ipcrm -q $i
		done

		sleep 1
		Run_Progress "ivaDatabase" $SHOW_DATABASE_LOG 1
		sleep 1
		Run_Progress "ivaRecord" $SHOW_RECORD_LOG 1
		sleep 1
		Run_Progress "ivaVideoOut" $SHOW_VIDEOOUT_LOG 1
		sleep 1
		Run_Progress "ivaVideoIn" $SHOW_VIDEOIN_LOG 1
		sleep 1
		Run_Progress "ivaOnvif" $SHOW_ONVIF_LOG 1
		sleep 1
		Run_Progress "ivaMaster" $SHOW_MASTER_LOG 1
		sleep 1
		Run_Progress "ivaBoa -c ." $SHOW_BOA_LOG 0
		sleep 1

	fi
	sleep 2
done



