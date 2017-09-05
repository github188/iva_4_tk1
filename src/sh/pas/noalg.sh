#!/bin/sh
#����û�����ΪROOT
USER_ID=`id -u`
ROOT_UID=0
if [ $USER_ID -ne $ROOT_UID ]
then
	echo "[Permission denied],must run noalg.sh by root user!"
	exit 0
fi

chmod 777 ./*.sh
chmod 777 ./iva*

#��������
#./upgrade.sh

#��ԭ����
#./profile2.sh

#����ȡ֤ͼƬ��¼����ʵ�������
./checkln.sh

#���������ű����Ż����л���
./tk1_gpu_max_clock.sh
./cpu.sh

#��̬��·��
LD_LIBRARY_PATH="../lib/:../Alg:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH

#�Ƿ񱣴���־ 0-����ӡ 1�������Ļ
SHOW_BOA_LOG=0
SHOW_MASTER_LOG=0
SHOW_VIDEOIN_LOG=0
SHOW_VIDEOOUT_LOG=0
SHOW_DECODE_LOG=0
SHOW_ONVIF_LOG=0
SHOW_DATABASE_LOG=0
SHOW_UPLOAD_LOG=0
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



