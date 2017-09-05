#!/bin/sh
#检测用户必须为ROOT
USER_ID=`id -u`
ROOT_UID=0
if [ $USER_ID -ne $ROOT_UID ]
then
	echo "[Permission denied],must run noalg.sh by root user!"
	exit 0
fi

chmod 777 ./*.sh
chmod 777 ./iva*

#升级操作
#./upgrade.sh

#还原配置
#./profile2.sh

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
SHOW_MASTER_LOG=0
SHOW_VIDEOIN_LOG=0
SHOW_VIDEOOUT_LOG=0
SHOW_DECODE_LOG=0
SHOW_ONVIF_LOG=0
SHOW_DATABASE_LOG=0
SHOW_UPLOAD_LOG=0
SHOW_RECORD_LOG=0

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

Run_Progress "ivaDatabase" $SHOW_DATABASE_LOG 1
sleep 1
Run_Progress "ivaUpload" $SHOW_UPLOAD_LOG 1
sleep 1
Run_Progress "ivaRecord" $SHOW_RECORD_LOG 1
sleep 1
Run_Progress "ivaVideoOut" $SHOW_VIDEOOUT_LOG 1
sleep 1
Run_Progress "ivaDecode" $SHOW_DECODE_LOG 1
sleep 1
Run_Progress "ivaVideoIn" $SHOW_VIDEOIN_LOG 1
sleep 1
Run_Progress "ivaOnvif" $SHOW_ONVIF_LOG 1
sleep 1
Run_Progress "ivaMaster" $SHOW_MASTER_LOG 1
sleep 1
Run_Progress "ivaBoa -c ." $SHOW_BOA_LOG 0
sleep 1



