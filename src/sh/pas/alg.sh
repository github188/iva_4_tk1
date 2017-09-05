#!/bin/sh
#检测用户必须为ROOT
USER_ID=`id -u`
ROOT_UID=0
if [ $USER_ID -ne $ROOT_UID ]
then
	echo "[Permission denied],must run alg.sh by root user!"
	exit 0
fi

chmod 777 ./*.sh
chmod 777 ./iva*


#动态库路径
LD_LIBRARY_PATH="../lib/:../Alg:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH

#是否保存日志 0-不打印 1输出到屏幕
SHOW_ANALY_LOG=1

#执行当前路径下的一个可执行程序 $1-可执行程序名字 $2-是否保存日志 $3-日志名字 $4-是否后台执行
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

Run_Progress "ivaAnaly" $SHOW_ANALY_LOG 1




