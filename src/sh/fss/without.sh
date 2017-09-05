#!/bin/sh
#����û�����ΪROOT
USER_ID=`id -u`
ROOT_UID=0
if [ $USER_ID -ne $ROOT_UID ]
then
	echo "[Permission denied],must run without.sh by root user!"
	exit 0
fi

chmod 777 ./*.sh
chmod 777 ./iva*

WITHOUT=$1
case ${WITHOUT} in
1)
	echo "runall without ivaDatabase"
	;;
3)
	echo "runall without ivaRecord"
	;;
5)
	echo "runall without ivaVideoOut"
	;;
7)
	echo "runall without ivaVideoIn"
	;;
8)
	echo "runall without ivaOnvif"
	;;
9)
	echo "runall without ivaMaster"
	;;
10)
	echo "runall without ivaBoa"
	;;
*)
	echo "usage: ./without.sh [number]"
	echo "number 1---ivaDatabase"
	echo "       3---ivaRecord"
	echo "       5---ivaVideoOut"
	echo "       7---ivaVideoIn"
	echo "       8---ivaOnvif"
	echo "       9---ivaMaster"
	echo "       10--ivaBoa"
	exit 0
	;;
esac


#��̬��·��
LD_LIBRARY_PATH="../lib/:../Alg:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH

#�Ƿ񱣴���־ 0-����ӡ 1�������Ļ
SHOW_BOA_LOG=0
SHOW_MASTER_LOG=0
SHOW_VIDEOIN_LOG=0
SHOW_VIDEOOUT_LOG=0
SHOW_ONVIF_LOG=0
SHOW_DATABASE_LOG=0
SHOW_RECORD_LOG=0

#ִ�е�ǰ·���µ�һ����ִ�г��� $1-��ִ�г������� $2-�Ƿ񱣴���־ $3-��־���� $4-�Ƿ��ִ̨��
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

if [ $WITHOUT -ne 1 ]; then
	Run_Progress "ivaDatabase" $SHOW_DATABASE_LOG 1
fi

sleep 1

if [ $WITHOUT -ne 3 ]; then
	Run_Progress "ivaRecord" $SHOW_RECORD_LOG 1
fi

sleep 1

if [ $WITHOUT -ne 5 ]; then
	Run_Progress "ivaVideoOut" $SHOW_VIDEOOUT_LOG 1
fi

sleep 1

if [ $WITHOUT -ne 7 ]; then
	Run_Progress "ivaVideoIn" $SHOW_VIDEOIN_LOG 1
fi

sleep 1

if [ $WITHOUT -ne 8 ]; then
	Run_Progress "ivaOnvif" $SHOW_ONVIF_LOG 1
fi

sleep 1

if [ $WITHOUT -ne 9 ]; then
	Run_Progress "ivaMaster" $SHOW_MASTER_LOG 1
fi

sleep 1

if [ $WITHOUT -ne 10 ]; then
	Run_Progress "ivaBoa -c ." $SHOW_BOA_LOG 0
fi

sleep 1




