#!/bin/sh
#����û�����ΪROOT
USER_ID=`id -u`
ROOT_UID=0
if [ $USER_ID -ne $ROOT_UID ]
then
	echo "[Permission denied],must run alg.sh by root user!"
	exit 0
fi

chmod 777 ./*.sh
chmod 777 ./iva*


#��̬��·��
LD_LIBRARY_PATH="../lib/:../Alg:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH

#�Ƿ񱣴���־ 0-����ӡ 1�������Ļ
SHOW_ANALY_LOG=1

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

Run_Progress "ivaAnaly" $SHOW_ANALY_LOG 1




