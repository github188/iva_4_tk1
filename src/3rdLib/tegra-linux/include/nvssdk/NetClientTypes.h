#ifndef NETCLIENTTYPES_H
#define NETCLIENTTYPES_H

#include "GlobalTypes.h"

#ifdef WIN32
#else
#define __stdcall
#endif

#define HEAVY_MODE 0
#define LIGHT_MODE 1

#define LEN_8   8
#define LEN_10	10
#define LEN_16  16
#define LEN_32  32
#define LEN_64  64
#define LEN_63  63
#define LEN_128 128
#define LEN_256 256
#define LEN_1300 1300 //����һ��Э�鲻����ȫ���͵�Э�飬�����ַ������ֶθ���������65�ֽ�����MAX��1300/65 = 20.ʣ��140�ֽڱ�������int�ͱ���

#define TD_FALSE 0
#define TD_TRUE  1


#define MAX_TIMESEGMENT_TYPE 2
#define MAX_TIMESEGMENT_INDEX 8

#define TD_NULL                 0L
#define TD_NULL_PTR             TD_NULL

#define TD_SUCCESS              0
#define TD_FAILURE              (-1)

#define  TD_LITTLE_ENDIAN       1234      
#define  TD_BIG_ENDIAN          4321      


#ifdef _MSC_VER
typedef __int64                 TD_S64;
typedef unsigned __int64        TD_U64;
#endif

#if defined __INTEL_COMPILER || defined __GNUC__
typedef long long               TD_S64;
typedef unsigned long long      TD_U64;
#endif

#define MAX_LOCAL_CHANNEL_NUM	64	
#define MAX_DIGIT_CHANNEL_NUM	64
#define	MAX_CHANNEL_NUM_TDWY		(MAX_LOCAL_CHANNEL_NUM + MAX_DIGIT_CHANNEL_NUM)	   //֧��3511
#define MAX_TOTAL_CHANNEL_NUM	MAX_CHANNEL_NUM_TDWY
#define MAX_VOLUME_CTRL_TYPE 5

#define MAX_CONNECTION		32
#define MAX_PORT_NUM        512
#define MAX_DHINFO_PARAM_LEN 1024
#define MAX_DOWNLOAD_NUM    5
#define MAX_COM_NUM			16	   //��󴮿���
#define MAX_INTERTALK_NUM	1      //���˫��Խ���
#define MAX_PICTURECHANNEL_NUM  	1      //���ͼƬͨ����
#define MAX_CAPTURECHANNEL_NUM  	1      //���ץ��ͨ����
#define MAX_TRACECHANNEL_NUM        1

#define MAX_STREAM_TYPES		5		//�������������
#define BASIC_STREAM_TYPES		2		//��������������
#define MAX_EVENT_STREAM_TYPES  (MAX_STREAM_TYPES - BASIC_STREAM_TYPES)//�¼�������������������������

#define MIN_ITS_CHANNEL_NUM		5		//��ͨ������� ʹ��ͨ��4 ��ͼƬ��ͨ�����ò�������ͨ�������ṹ����С���� 4���� ��С��

#define MAX_RECV_CHANNEL    1024
#define MAX_ENCODER         1024
#define LENGTH_RECV_ORDER   1024

#define LENGTH_USER_NAME_BASE64		50
#define LENGTH_PASSWORD_BASE64		50

#define LENGTH_USER_NAME	16
#define LENGTH_PASSWORD		16
#define LENGTH_CHN_TITLE    32
#define LENGTH_CHANNEL_TITLE  64
#define LENGTH_DEVICE_NAME  32
#define LENGTH_IPV4			16	
#define LENGTH_IPV6			32
#define LENGTH_IPV6_V1		64
#define LENGTH_SUB_MASK		16

//max dev port
#define MAX_NET_PORT_NUM				65535

//connect mode
#define NETMODE_TCP     1   //Tcp connect
#define NETMODE_UDP     2   //Udp connect
#define NETMODE_MC      3   //Multicast connect
#define NVS_ACTIVE      4   //Positive Mode
#define NETMODE_P2P		5   //P2P connect

//Video resolution
#define QCIF			0   //QCif 			176*144
#define HCIF			1   //HCif			352*144	
#define FCIF			2	//Cif			352*288	
#define HD1				3	//HD1			720*288
#define FD1_TDWY				4	//D1			720*576
#define MD1				5	//MD1			720*288
#define QVGA			6   //QVGA			320x240
#define VGA				7   //VGA			640*480
#define HVGA			8	//HVGA			640*240
#define HD_720P			9   //720p			1280*720
#define HD_960P			10	//960P			1280*960
#define HD_200W			11  //200W			1600*1200
#define HD_1080P		12	//1080P			1920*1080
#define HD_QXGA			13	//QXGA			2048*1536
#define QHD				0x100	//QHD		960*540
#define VZ_960H			0x200	//960H		960*576
#define VZ_5MA			0x300	//5.0MP		2448*2048
#define VZ_5M			0x400	//5M(2)		2560*1920
#define VZ_5MB			0x410	//5M(3)		2592*1944
#define VZ_5MC			0x420	//5M(4)		2592*2048
#define VZ_5MD			0x430	//5M(5)		2528*2128
#define VZ_5ME			0x500	//5M		2560*2048
#define VZ_6M			0x600	//6M		2752*2208
#define VZ_6MA			0x610	//6M		3008*2008
#define VZ_6MB			0x620	//6M		3408*2008



//��Ƶ��ת
#define VZ_QCIF_REVERSE			(0x100000+QCIF)
#define VZ_HCIF_REVERSE			(0x100000+HCIF)
#define VZ_FCIF_REVERSE			(0x100000+FCIF)
#define VZ_HD1_REVERSE			(0x100000+HD1)
#define VZ_FD1_REVERSE			(0x100000+FD1_TDWY)
#define VZ_MD1_REVERSE			(0x100000+MD1)
#define VZ_QVGA_REVERSE			(0x100000+QVGA)
#define VZ_VGA_REVERSE			(0x100000+VGA)
#define VZ_HVGA_REVERSE			(0x100000+HVGA)
#define VZ_HD_720P_REVERSE		(0x100000+HD_720P)
#define VZ_HD_960P_REVERSE		(0x100000+HD_960P)
#define VZ_HD_200W_REVERSE		(0x100000+HD_200W)
#define VZ_HD_1080P_REVERSE		(0x100000+HD_1080P)
#define VZ_HD_QXGA_REVERSE		(0x100000+HD_QXGA)
#define VZ_QHD_REVERSE			(0x100000+QHD)
#define VZ_VZ_960H_REVERSE		(0x100000+VZ_960H)
#define VZ_VZ_5MA_REVERSE		(0x100000+VZ_5MA)
#define VZ_VZ_5M_REVERSE		(0x100000+VZ_5M)
#define VZ_VZ_5MB_REVERSE		(0x100000+VZ_5MB)
#define VZ_VZ_5MC_REVERSE		(0x100000+VZ_5MC)
#define VZ_VZ_5MD_REVERSE		(0x100000+VZ_5MD)

//Stream type
#define MAIN_STREAM   		0      //Basic stream(hight quality)
#define SUB_STREAM    		1      //Extended stream(low bitrate)
#define CHANNEL_INTERTALK	2		//˫��Խ�
#define CHANNEL_DOWNLOAD    3		//�ļ�����
#define STREAM_DOWNLOAD     CHANNEL_DOWNLOAD
#define CHANNEL_PICTURE     4		//ͼƬץ��    
#define CHANNEL_SIMUCAP  	5		//ģ��ץ����
#define CHANNEL_TRACK  	    6		//�켣��
#define SNAPSHOT_STREAM  	8		//SnapShot stream
#define FLAG_THREE_STREAM  256		//������
#define FLAG_CONID_THREE_STREAM	 8 	//��λ��ʶ������

#define CHANNEL_THREE_STREAM	254	//������

#define DOWNLOAD_CHANNEL_TAG  (250)
#define DISTINGUISH_FILE_CHAN	64

#define LAN_VIDEO MAIN_STREAM     //for old compatible
#define WAN_VIDEO SUB_STREAM    

//Device type
#define NVS_T       0           //T serials NVS
#define NVS_S       1           //S serials NVS
#define NVS_TPLUS   2           //T+ serials NVS

//Product Model
#define TC_NVSS					0x0000
#define TC_NS621S				0x0010			
#define TC_NS621S_USB			0x0011			
#define TC_NS224S				0x0020			
#define TC_NC621S				0x0030			
#define TC_NC8000S				0x0040			
#define TC_NC8001S				0x0041			
#define TC_NC8100S				0x0042			
#define TC_NC8101S				0x0043			
#define TC_NC8200S				0x0044			
#define TC_NC8700S2				0x0045
#define	TC_NC9100S1_MP_IR       0x0046	
#define TC_NC8250				0x0050			
#define TC_NC9010S2_MP			0x0051			
#define TC_NC9000S2_2MP			0x0052			
#define TC_NC9010N1_2MP			0x0053			
#define TC_NC9000S3_3MP			0x0054			
#define TC_NH9106N1_2MPIR		0x0055			
#define	TC_NS921_N1_2MP			0x0056	
#define	TC_NS921S3_HD_V1        0x0056			
#define TC_CC9118_MP_E			0x0057			
#define TC_NH9206N1_MPIR_IVS	0x0058			
#define TC_NC9100S3_3MP_IR30    0x0059          
#define TC_NT9054S2_MPIR		0x005A	
#define	TC_NC9000S3_2MP_E       0x005B			
#define	TC_NC9200S3_MP_E_IR15   0x005C
#define TC_NH9206S3_2MP_IVS		0x005D	
#define	TC_NC9000S3_MP_E        0x005E
#define TC_NS621S2				0x0060			
#define TC_NS622S2				0x0061			
#define TC_NS324S2				0x0062			
#define TC_NS628S2 				0x0064	
#define	TC_FS0013ERS_HD         0x0070
#define TC_NC9100S3E_MP_IR30	0x0080
#define TC_NC9000S3E_MP			0x0081
#define TC_NC9200S3E_MP_IR		0x0082
#define TC_NC9100S3E_2MP_IR30	0x0083
#define TC_NC9000S3E_2MP		0x0084
#define TC_NC9200S3E_2MP_IR		0x0085
#define TC_NC921S3E_MP_HD_C		0x0086
#define TC_NH9406S3E_MPIR       0x0087
#define TC_NH9406S3E_2MPIR		0x0088
#define TC_NC9320S3E_2MP_E		0x0089
#define TC_NC9400S3E_MP_C_IR30	0x008A
#define TC_NC9200S3E_2MP_E_I3	0x008B
#define TC_NC9320S3E_MP_E		0x0090
#define TC_NC9000S3E_5MP_UE		0x0091
#define	TC_NC9001S3E_3MP_E		0x0092	
#define	TC_NC9201S3E_3MP_EI_IR	0x0093
#define	TC_NH9806S6_3MPIR_I		0x0094
#define	TC_CC9033_3MP			0x0095
#define TC_NC9401S3E_4MP_I5_E	0x0096
#define TC_NC9401S3E_4MP_I_E	0x0097
#define TC_NC9001S3E_2MP_ES		0x0098
#define TC_NH9904S6_2MP			0x0099
#define TC_NH9806S6_2MPIR_S_A	0x009A
#define TC_NC9320S3E_2MP_D		0x009B

#define TC_NC9010I_2MP			0x00A1			
#define TC_NC9000I_5MP			0x00A2			
#define TC_21X_R2				0x00A3			
#define TC_21X_R3				0x00A4			
#define TC_21X_R5				0x00A5
#define TC_217_T2				0x00A6
#define TC_217_T3				0x00A7
#define TC_218_T5	            0x00A8
#define	TC_218_T3H				0x00A9	
#define	TC_217_T5				0x00AA	
#define	TC_217_T3H				0x00AB
#define	TC_237_5MPH				0x00AC
#define	TC_238_5MPH				0x00AD
#define	TC_227_2MP_S_Z			0x00AE
#define	TC_217_1MP_S			0x00AF
#define TC_NH9X06S6_XMPIR_A		0x00D0
#define TC_NH9X06S6_XMPIR_T		0x00D1
//end

#define TC_2816AN_SH			0x0100			
#define TC_2808AN_S_3511		0x0110			
#define TC_2816AN_SD			0x0120			
#define TC_2816AN_SD_E          0x0121			
#define TC_2808AN_S_3515		0x0130			
#define TC_2804AN_MX			0x0131			
#define TC_2816AN_S				0x0140			
#define TC_H802BHX_T			0x0141			
#define TC_2832AN_SS            0x0142			
#define TC_H804BE_HD            0x0143			
#define TC_1002S2_3C			0x0150			
#define TC_H804N_HD				0x0151			
#define	TC_2802AN_SDI			0x0160			
#define TC_H908NC_H_V50			0x0161			
#define TC_2800AN_SF_L_H		0x0170			
#define TC_2800AN_SF_H			0x0171			
#define TC_2804AN_SA			0x0172			
#define TC_2800AN_SF_L			0x0173			
#define TC_2800AN_SF			0x0174			
#define TC_H804BE_P				0x0175	
#define TC_2808AN_SF_L			0x0176
#define	TC_2804AN_SQ        	0x0177
#define TC_T800GE				0x0188

#define TC_2804AN_SR_L			0x0178
#define TC_H804I_HD_BP			0x0179
#define TC_H904B_HD_V6_X		0x017A
#define TC_H908NC_HD			0x017B

#define TC_2800AN_R16_S2		0x017C
#define	TC_2804AN_M				0x017D
#define TC_2800AN_R4_S1			0x017E
#define TC_2800AN_R4_S2			0x017F
#define TC_2800AN_R8_S2			0x017F

#define TC_H804N_HD_P			0x0180
#define TC_2800AN_R16_S2_V2_0   0x0181
#define TC_2800AN_R16_S4	    0x0181
#define TC_2800AN_R32_S4        0x0181

#define TC_2816AN_SR_V2_0		0x0182
#define TC_2832AN_SR_V2_0		0x0182

#define TC_2800AN_R4_S1_V2_0	0x0183

#define TC_ND921S2_MP			0x0400			
#define	TC_ND921S2_2MP			0x0410			
#define	TC_ND921S2_SDI			0x0411			
#define	TC_2804AN_SDI			0x0500	
#define	TC_2864AN_SR			0x0520
#define TC_7816W				0x0600
#define TC_H904BIX_HD_I			0x0184

#define TC_T238_5MP_S_Z			0x00B0
#define TC_T238_6MP				0x00B2
#define TC_T237_6MP				0x00B3
#define TC_T238_6MP_B			0x00B4
#define TC_T237_7MP				0x00B5
#define TC_T238_7MP				0x00B6
//
#define TC_NR2160M7_E16			0x0190
#define TC_NR2080M7_E16			0x0190
#define TC_NR2080M7_E8			0x0190
#define TC_NR2040M7_E8			0x0190
#define TC_NR2020M7_E8			0x0190
#define TC_NR2080M7_S8			0x0190
#define TC_NR2040M7_S8			0x0190
#define TC_NR2020M7_S8			0x0190
#define TC_NR2040M7_S4			0x0191
#define TC_NR2020M7_S4			0x0191	
#define TC_NR2020M7_S2			0x0192

#define TC_T237_2MP_S			0x00E0

#define TC_NH9406S6_MPIR_S		0x009E
#define TC_NH9606S6_2MPIR_S		0x009F

//Encode type
#define H263		11
#define H264		21
#define MP4			31
#define MJPEG		41
#define MODE_H265	23

//	SendStringToServer
#define SEND_STRING_CMD		0
#define SEND_STRING_DATA	1
#define SEND_UTF8_STRING	2

//H264 Decod mode
#define H264DEC_DECTWO	0		//decode 2 field
#define H264DEC_DECTOP	1		//decode top field ��low CPU cost��

//Message define
#define WCM_RECVMESSAGE         1   //Received string
#define WCM_ERR_ORDER           2   //Occur error in command socket
#define WCM_ERR_DATANET         3   //Occur error in data socket
#define WCM_TIMEOUT_DATANET     4   //Process stream timeout
#define WCM_ERR_PLAYER          5   //Occur error in player
#define WCM_ERR_USER            6   //User define error
#define WCM_LOGON_NOTIFY        7   //Logon notify
#define WCM_VIDEO_HEAD          8   //Received video head
#define WCM_VIDEO_DISCONNECT    9   //the connection were disconnect by mediacenter

//Message of Mediacenter
#define WCM_CLIENT_CONNECT      10  //There a client connected to Mediacenter
#define WCM_CLIENT_DISCONNECT   11  //A client disconnected from Mediacenter
#define WCM_CLIENT_LOGON        12  //A client logon/logogg the Mediacenter

#define WCM_RECORD_ERR          13  //Occur when recording file

//Directory Service Mode
#define WCM_DSM_REGERR          14  //Error on proxy regist to Directory server
#define WCM_DSM_ENCODERADD      15  //A NVS regist on the proxy
#define WCM_DSM_ENCODERDEL      16  //A NVS's regist connection disconnected.
//#define	WCM_DSM_ENCODERCHANGE	28	// A NVS'S IP changed.

//Message of Download logfile
#define WCM_LOGFILE_FINISHED    17  //When logfile download finished

//Message of query file
#define WCM_QUERYFILE_FINISHED  18  //Query finished record files

#define WCM_DWONLOAD_FINISHED   19  //Download finished
#define WCM_DWONLOAD_FAULT		20  //Download fault
#define WCM_REBUILDINDEX        22  //Finished of rebuild index file

//Message of talk
#define WCM_TALK				23  //Begin to talk

//Message of DiskManager
#define WCM_DISK_FORMAT_PROGRESS	24
#define WCM_DISK_FORMAT_ERROR		25
#define WCM_DISK_PART_ERROR			26

//Message of Backup Kernel
#define	WCM_BUCKUP_KERNEL			27

//Message of LocalstoreClearDisk
#define	WCM_LOCALSTORE_PROGRESS		28
#define WCM_DOWNLOAD_INTERRUPT		29	//  Download interrupt    
#define WCM_QUERYLOG_FINISHED		30  //	Query finished log

#define WCM_INTERTALK_READY			31	//	Intertalk is ready for now ...
#define WCM_CONTROL_DEVICE_RECORD	32	//	Control device record (start, stop, package)

#define WCM_RECORD_OVER_MAX_LIMIT	33	//	The size of record file is too big, and we should start to capture a new file.
#define WCM_ERR_DATANET_MAX_COUNT	34	//	Max DataNet

#define WCM_CDBURN_STATUS			35
#define WCM_NEED_DECRYPT_KEY		36	//	Need decrypt key for video
#define WCM_DECRYPT_KEY_FAILED		37	//	decrypt key is not matched
#define WCM_DECRYPT_SUCCESS			38	//	decrypt success

#define WCM_ERR_DIGITCHANNEL_NOT_ENABLED	39	//	The digital channel is not enabled
#define WCM_ERR_DIGITCHANNEL_NOT_CONNECTED	40	//	The digital channel is not connected

#define WCM_ENCRYPT_CLEARED			41   //	clear encrypt
#define WCM_FTP_UPDATE_STATUS       42   //FTP_UPDATE_STATUS
#define	WCM_BUCKUP_IMAGE			43
#define WCM_REBUILDINDEX_PROGRESS   44   //rebuild index file progress

#define	WCM_ILLEGAL_RECORD			45   //record notify when there is illegal patking

#define WCM_QUERY_ATMFILE_FINISHED  46  //Query ATM record files finished

#define WCM_AUTOTEST_INFO			47  //�Զ�������@130109

#define WCM_LASTERROR_INFO			48	//�豸���صĴ�����

#define	WCM_LOCALSTORE_LOCK_FILE	49  //�ļ��ӽ���

#define WCM_DISK_QUOTA				50	//�������

#define WCM_CONNECT_INFO			51	//������Ϣ

#define WCM_DOWNLOAD_VOD_TRANS_FAILED		52	//Vod Transform Failed

#define WCM_USER_CHANGE             100          //�û���Ϣ�ı�
#define WCM_UNSUPPORT_STREAM        101          //�豸��֧�ָ�·����
#define WCM_BROADCAST_MSG			102          //ͨ�ù㲥��Ϣ
#define WCM_VCA_SUSPEND				103          //��ͣ���ܷ���֪ͨ��Ϣ
#define	WCM_ITS_ILLEGAL_RECORD		104			 //Υ��¼�����״̬
#define WCM_BACKUP_SEEKWORKDEV		105			 //�鲥�����������������֪ͨ��Ϣ
#define WCM_IPSAN_DISCOVERY			108			 //IPSAN�豸����

#define WCM_RAID_STATUS				109			//����״̬��Ϣ
#define WCM_HDD_STATUS				110			//�������״̬��Ϣ
#define WCM_VIRTUALDISK_STATUS		111			//�������״̬��Ϣ
#define WCM_HOTBACK_DEV_STATUS		112			//�������㲥�ȱ���״̬
#define WCM_WORD_DEV_STATUS			113			//�ȱ����㲥������״̬
#define WCM_ALARM_INFORMATION       114         //��������ʵʱ����
#define WCM_ALARM_OUT_STATUS		115         //��������˿�״̬
#define WCM_ALARM_SCHEDULE_STATUS   116         //����״̬
#define WCM_EXPORT_CONFIG_FINISHED  117         //�����������
#define WCM_ALM_OUTPORT_SET			118		    //���ñ�������˿ڵ�״̬����·���Ǳ�·��
#define WCM_VOD_PLAY_FINISHED		119		    //vod�������
#define WCM_VOD_PLAY_INTERRUPT		120		    //vod�����ж�
#define WCM_PSECH_STATE				121		    //pse channel state
#define WCM_EMAIL_TEST_RESULT		122		    //Email test result
#define WCM_DISK_SMART_INFO			123		    //
#define WCM_ITS_TRAFFICFLOWREPORT	124		    //
#define WCM_CHANNEL_TALK			125
#define WCM_GET_CREATEFREEVO		126
#define WCM_USER_MODIFYPSW			127			//�޸�����
#define WCM_DISK_OPERATION          128         //���̲���
#define WCM_DOWNLOAD_SUCCESS		129
#define WCM_DOWNLOAD_FAULT			130
#define WCM_GIGITAL_CHN_BATCH		131			//digital channel batch
#define WCM_LIFEMONITOR_HBREAL		132			//heart rate and blood oxygen
#define WCM_LIFEMONITOR_GRAMREAl	133			//waveform data
#define WCM_PREVIEWREC_DEVAMPLITUDE	134
#define WCM_QUERYFILE_FAULT			135			//Query fault record files
#define WCM_QUERY_REPORT_FORM		136			//Query report form
#define WCM_START_PLAY				137			//inner StartPlay
#define WCM_NEED_DECRYPT_KEY_PLAYBACK		138			//LPARAM_NEED_DECRYPT_KEY
#define WCM_DECRYPT_KEY_FAILED_PLAYBACK		139			//LPARAM_DECRYPT_KEY_FAILED
#define WCM_DECRYPT_SUCCESS_PLAYBACK		140			//LPARAM_DECRYPT_SUCCESS
#define WCM_VOD_STREAM_END					141			//Vod stream end

//Number of Show Video
#define MAX_DRAW_NUM	2
#define MAX_OSDTYPE_NUM         10
#define MAX_OSD_REALTIME_NUM	16  //��������ʹ��

//User 
#define AUT_BROWSE            1     //Only can preview video
#define AUT_BROWSE_CTRL       2     //Can preview and control device
#define AUT_BROWSE_CTRL_SET   3     //Preview, control device, modify settings
#define AUT_ADMIN             4      //Super user, No.0 user is super user,
                             //User name is Admin, which can not be modified,
                             //Can add, delete, modify user's authority

typedef int						ALARMTYPE;

//Logon state

#define LOGON_DSMING       3    //DSM��Directory Sevices Mode��connecting
#define LOGON_RETRY        2    //Retry
#define LOGON_SUCCESS      0    //Logon successfully
#define LOGON_ING          1    //Being logon
#define LOGON_FAILED       4   //Fail to logon
#define LOGON_TIMEOUT      5   //Logon timeout
#define NOT_LOGON          6   //Not logon
#define LOGON_DSMFAILED    7   //DSM connection failure
#define LOGON_DSMTIMEOUT   8   //DSM connection 
#define LOGON_SUCCESS_LIGHT	9  //Light Logon successfully

//Player state

#define    PLAYER_STOP          0    //Not require to play
#define    PLAYER_WAITTING      0x01 //Require to play but not play, waiting for data
#define    PLAYER_PLAYING       0x02 //Playing
#define    PLAYER_CARDWAITTING  0x04 //Require to hardware decode
#define    PLAYER_CARDPLAYING   0x08  //Being hardware decode output

typedef int VIDEO_NORM;
#define VIDEO_MODE_PAL 0x00
#define VIDEO_MODE_NTSC 0x01
#define VIDEO_MODE_AUTO 0x02

#define OSDTYPE_TIME	0x01
#define OSDTYPE_TITLE	0x02
#define OSDTYPE_LOGO	0x04
#define OSDTYPE_ITS		0x05

#define BRIGHTNESS 0
#define CONTRAST   1
#define SATURATION 2
#define HUE        3

#define PRE_VDOQUALITY 0
#define PRE_FRAMERATE  2

#define NO_STREAM  0
#define ONLY_VIDEO 1
#define ONLY_AUDIO 2
#define VIDEO_AUDIO 3

#define  DISABLE 0
#define  ENABLE  1

#define LOW 0
#define HIGH 1

#define TALK_BEGIN_OK			0
#define TALK_BEGIN_ERROR		1
#define TALK_ERROR				2
#define TALK_END_OK				3

#define TALK_STATE_NO_TALK			0			//	û�жԽ�
#define TALK_STATE_ASK_BEGIN		1			//	�Ѿ�����ʼ�Խ�
#define TALK_STATE_TALKING			2			//	���ڶԽ�
#define TALK_STATE_ASK_STOP			3			//	�Ѿ�����ֹͣ�Խ�
#define TALK_STATE_RESUME			4			//  2010-4-26-16:21 @yys@,	�����Զ��ָ��Խ�������

#define  PRO_UPGRADE_OK 0
#define  PRO_UPGRADE_ERROR -1
#define  PRO_UPGRADE_READY -2

#define WEB_UPGRADE_OK 0
#define WEB_UPGRADE_ERROR -1
#define WEB_UPGRADE_READY -2
//����IE
#define WEB_UPGRADE_REDAY	WEB_UPGRADE_READY

//��Ƶ�������豸����
typedef int							PARATYPE;
#define PARA_VIDOEPARA				0
#define PARA_VIDEOPARA				0 
#define PARA_VIDEOPARA				0 
#define PARA_VIDEOQUALITY			1     
#define PARA_FRAMERATE				2     
#define PARA_IFRAMERATE				3     
#define PARA_STREAMTYPE				4     
#define PARA_MOTIONDETECTAREA		5
#define PARA_THRESHOLD          	6    
#define PARA_MOTIONDETECT			7     
#define PARA_OSDTEXT				8    
#define PARA_OSDTYPE				9     
#define PARA_ALMINMODE				10    
#define PARA_ALMOUTMODE				11    
#define PARA_ALARMINPUT				12    
#define PARA_ALARMOUTPUT			13    
#define PARA_AlMVDOCOVTHRESHOLD		14    
#define PARA_ALMVIDEOCOV			15    
#define PARA_RECORDCOVERAREA		16    
#define PARA_RECORDCOVER			17    
#define PARA_ALMVDOLOST				18    
#define PARA_DEVICETYPE				19    
#define PARA_NEWIP					20
#define PARA_AUDIOCHN				21    
#define PARA_IPFILTER				22    
#define PARA_COVERAREA				23    
#define PARA_VIDEOHEADER			24
#define PARA_VIDEOSIZE				25    
#define PARA_BITRATE				26    
#define PARA_DATACHANGED			27
#define PARA_PARSECHANGED			28   

#define PARA_APPIFRAMERATE			29    
#define PARA_APPFRAMERATE			30    
#define PARA_APPVIDEOQUALITY		31    
#define PARA_APPVIDEOSIZE			32    
#define PARA_APPSTREAMTYPE			33    
#define PARA_APPBITRATE				34    
#define PARA_APPCOVERAREA			35    
#define PARA_APPVIDEOHEADER			36    

#define PARA_REDUCENOISE			37    
#define PARA_BOTHSTREAMSAME			38    
#define PARA_PPPOE					39    

#define PARA_ENCODETYPE				40    
#define PARA_PREFERMODE				41    

#define PARA_LOGFILE				42    
#define PARA_SETLOGOOK				43    

#define PARA_STORAGE_SCHEDULE   	44   
#define PARA_STORAGE_TASKSTATE		45    
#define PARA_STORAGE_ALARMSTATE		46    
#define PARA_STORAGE_PRERECORD		47    
#define PARA_STORAGE_RECORDRULE		49    
#define PARA_STORAGE_EXTNAME		50    
#define PARA_STORAGE_MAPDEVICE		51    
#define PARA_STORAGE_MOUNTUSB		52    
#define PARA_STORAGE_DISKSTATE		53    
#define PARA_AUDIOCODER				54    
#define PARA_NPMODE					55    
#define PARA_STORAGE_RECORDSTATE	56    

#define PARA_PU_MANAGERSVR			57    
#define PARA_PU_DEVICEID			58    
#define PARA_STREAMCLIENT			59    
#define PARA_DATEFORMAT				60    
#define PARA_HTTPPORT				61    

#define PARA_CMOS_SCENE				62    
#define PARA_SMTP_INTO				63    
#define PARA_SMTP_ENABLE			64    
#define PARA_SENSORFLIP				65    

#define PRAM_WIFI_PARAM				66
#define PARA_WIFI_SEARCH_RESULT		67   

#define PARA_PRIVACYPROTECT			68
#define PARA_NTP					69
#define PARA_ALARTHRESHOLD			70
#define PARA_OSDALPHA				71
#define PARA_WORDOVERLAY			72

#define PARA_ALARMSERVER			73
#define PARA_DHCP_BACK				74
#define PARA_WIFIDHCP				75
#define PARA_DIAPHANEITY			76
#define PARA_PTZ_PROTOCOL			77
#define PARA_3D						78
#define PARA_DISK_USAGE				79
#define PARA_VIDEOPARA_SCHEDULE		80
#define PARA_ALARM_INPORT			81
#define PARA_ALARM_OUT				82
#define PARA_INPORT_SCHEDULE		83
#define PARA_OUTPORT_SCHEDULE		84
#define PARA_MOTION_SCHEDULE		85
#define PARA_VDOLOST_SCHEDULE		86
#define PARA_VDOLOST_LINKREC		87
#define PARA_MOTIONDEC_LINKREC		88
#define PARA_ALMINPORT_LINKREC		89
#define PARA_VDOLOST_LINKSNAP		90
#define PARA_MOTIONDEC_LINKSNAP		91
#define PARA_ALMINPORT_LINKSNAP		92
#define PARA_VDOLOST_LINKPTZ		93
#define PARA_ALMINPORT_LINKPTZ		94
#define PARA_VDOLOST_LINKSNDDIS		95
#define PARA_MOTIONDEC_LNKSNDDIS	96
#define PARA_ALMINPORT_LNKSNDDIS	97
#define PARA_ALMINPORT_LNKOUT		98
#define PARA_VDOLOST_LNKOUT			99
#define PARA_MOTIONDEC_LNKOUT		100
#define PARA_OUTPORT_DELAY			101
#define PARA_UPNP_ENABLE			102
#define PARA_SYSINFO_RESULT			103
#define PARA_DDNSCHANGED 			104	   
#define PARA_FUNCLIST				105	
#define PARA_OUTPORT_STATE			106
#define PARA_ZTEINFO				107
#define PARA_ONLINESTATE			108
#define PARA_DZINFO					109
#define PARA_COMSERVER				110
#define PARA_VENCTYPE				111  //��Ƶ����ı� ������
#define PARA_PTZAUTOPBACK			112
#define PARA_3GDEVICESTATUS			113
#define PARA_3GDIALOG				114
#define PARA_3GMESSAGE				115
#define PARA_HDCAMER				116
#define PARA_HXLISTENPORTINFO		117
#define PARA_3GTASKSCHEDULE			118
#define PARA_3GNOTIFY				119
#define PARA_COLORPARA				120
#define PARA_EXCEPTION				121
#define PARA_CAHNNELNAME			122
#define PARA_CUSTOMRECTYPE			123
#define PARA_FTP_UPDATE				124
#define PARA_FTP_UPDATE_STATUS		125
#define PARA_CHNPTZFORMAT			126
#define PARA_3GVPDN					127
#define PARA_VIDEOCOVER_SCHEDULE	128
#define PARA_CHNPTZCRUISE			129
#define PARA_VDOCOVER_LINKPTZ		130

#define PARA_VCA_CHANNEL			131
#define PARA_VCA_VIDEOSIZE			132
#define PARA_VCA_ADVANCED			133
#define PARA_VCA_TARGET				134
#define PARA_VCA_ALARMSTAT			135
#define PARA_VCA_RULESET			136
#define PARA_VCA_RULEEVENT0			137
#define PARA_VCA_RULEEVENT1			138
#define PARA_VCA_RULEEVENT2			139
#define PARA_VCA_ALARMSCHEDULE		140
#define PARA_VCA_SCHEDULEENABLE		141
#define PARA_VCA_ALARMLINK			142

#define PARA_FTP_SNAPSHOT			143
#define PARA_FTP_LINKSEND			144
#define PARA_FTP_TIMEDSEND			145

#define PARA_DVR3G_POWERDELAY		146
#define PARA_DVR3G_SIMNUM			147
#define PARA_DVR3G_GPSINFO			148
#define PARA_DVR3G_GPSFILTER		149

#define PARA_CDBURN_MODE			150					//	��¼��ʽ����
#define PARA_VIDEOENCRYPT			151
#define PARA_PREIVEWREOCRD			152

#define PARA_DIGITALCHANNEL			153
#define PARA_NPMODE_EX				154

#define	PARA_SIP_VIDEOCHANNEL		155			//	SIP��Ƶͨ������
#define PARA_SIP_ALARMCHANNEL		156			//	SIP����ͨ������

#define PARA_VIDEOCOMBINE			157        //	�໭��ϳ�
#define PARA_PLATFORM_RUN			158			//	PlatForm Run

#define PARA_ITS_CHNLENABLE         159       //
#define PARA_ITS_CHNLLOOP           160       //
#define PARA_ITS_CHNLTIME           161       //
#define PARA_ITS_CHNLSPEED          162       //
#define PARA_ITS_CHNLRECO           163       //
#define PARA_ITS_CHNLVLOOP          164       //

#define PARA_ITS_LPOPTIM_FIRSTHZ    165       //
#define PARA_ITS_LPOPTIM_NPENABLE   166       //
#define PARA_ITS_LPOPTIM_OTHER      167       //

#define PARA_ITS_TIMERANGE			168         //���ʱ������� 
#define PARA_ITS_TIMERANGEPARAM     169			//ʱ��β�������
#define PARA_ITS_TIMERANGE_AGCBLOCK			170     //ʱ��β������ʹ��
#define PARA_ITS_TIMERANGE_FLASHLAMP		171     //ʱ��������ʹ��
#define PARA_ITS_DETECTMODE					172     //������ģʽ����
#define PARA_ITS_BLOCK						173		//��������������
#define PARA_ITS_LOOPMODE					174		//��Ȧ����ģʽ	
#define PARA_ITS_DEVICETYPE					175		//�����������
#define PARA_ITS_SWITCHTIME					176     //������Ƶ���ⴥ��ģʽ�л�ʱ�������������ʱץ��ʱ�䣨��Ҫ�����״
#define PARA_JPEGQUALITY					177     //����JPEGͼƬ����	

#define PARA_VCA_RULEEVENT9					178		//���ܷ�������ʶ�����
#define PARA_VCA_RULEEVENT10				179		//���ܷ�����Ƶ��ϲ���

#define PARA_ITS_TEMPLATENAME				180		//�������ģ����������

#define PARA_ITS_RECOPARAM					181		//��ͨ-����ʶ���㷨����

#define PARA_ITS_DAYNIGHT					182		//��ͨ-����ҹ��ʼʱ������
#define PARA_ITS_CAMLOCATION				183		//��ͨ-���λ��
#define PARA_ITS_WORKMODE					184		//��ͨ-�������ģʽ
#define PARA_TIMEZONE_SET                   185     //����ʱ��
#define PARA_LANGUAGE_SET                   186     //��������

#define PARA_CHANNELENCODEPROFILE_SET       187     //ͨ������

#define PARA_EXCEPTION_HANDLE               188    //�쳣����

#define PARA_ITS_CHNLIGHT					189		//������Ӧ���źŵ�
#define PARA_ITS_CAPTURE					190		//������Ӧ��ץ��λ��
#define PARA_ITS_RECOGNIZE					191		//�Ƿ�����ʶ����								
#define PARA_IMG_DISPOSAL					192		//ͼƬЧ��������
#define PARA_WATERMARK						193		//�Ƿ�����ˮӡ����
#define PARA_SYSTEM_TYPE					194		//ϵͳ����
#define PARA_ITS_LIGHT_INFO					195		//�źŵƶ�Ӧ�Ĳ���

//IPC 3MP
#define PARA_CHECKBADPIXEL_STATUS           196     //������״̬
#define PARA_DEVICE_STATE                   197     //�豸ʵʱ״̬
#define PARA_CHECKIRIS_STATUS               198     //��Ȧ���״̬

#define PARA_ITS_ILLEGALPARK				199     //����Υ��ͣ������
#define PARA_ITS_ILLEGALPARKPARAM           200    //����Υ��ͣ���궨�������

#define PARA_LAN_IPV4						201     //����IPv4��ַ
#define PARA_LAN_IPV6						202     //����IPv6��ַ
#define PARA_LAN_WORKMODE					203     //���繤��ģʽ
#define PARA_PWM_STATUS                     204     //�����PWM����ֵ

#define PARA_AUDIOMIX	                    205     //�����ϳɲ���
#define PARA_AUDIOLOST_ALARMSCHEDULE		206		//��Ƶ��ʧ��������ģ��
#define PARA_AUDIOLOST_ALARMLINK			207		//��Ƶ��ʧ��������ģ��
#define PARA_AUDIOLOST_ALARMSCHENABLE		208		//��Ƶ��ʧ��������ģ��

#define PARA_ITS_SWITCH_INTERVAL			209		//����ⴥ������Ƶ�Զ��л�ʱ����
#define PARA_ITS_VIDEODETECT				210		//�������ͼ����
#define PARA_ITS_REFERENCLINES				211		//���������������
#define PARA_ITS_DETECTAREA					212		//������������������
#define PARA_ITS_RECOTYPE					213		//��������㷨��������
#define PARA_METHODMODE				        214     //������ƵԴ��ʽ�Ļ�ȡ��ʽ

#define PARA_LOCALSTORE_CHANNELPARA	        215     //ͨ����ش洢����


#define PARA_WIFIWORKMODE				    216      //����WIFI����ģʽ
#define PARA_WIFIAPDHCPPARA					217      //����DHCP�������ò��� Ip��Χ ����ʱ���
#define PARA_WIFIAPPARA						218      //����WIFI_AP���������Ϣ

#define PARA_ITS_ENABLEPARAM				219		 //���ý�ͨ��ع���ʹ��
#define PARA_ITS_REDLIGHTDETECTAREA			220		 //���ú�Ƽ���������
#define PARA_ATM_INFO						221		 //����ATM��Ϣ
#define PARA_AUDIO_SAMPLE					222		 //������Ƶ������
#define PARA_IPCPnP							223		 //��������ͨ�����弴��
#define PARA_WIFIDHCPAP						224		 //����APģʽDHCP����
#define PARA_MIXAUDIO   			        225		 //����
#define PARA_DOME_TITLE   			        226		 //	�������
#define PARA_HARDWARE_VOLTAGE               227      //����8127�����ѹ

#define PARA_ALARMTRIGGER_AUDIOLOSE	        228      //������Ƶ��ʧ��������ֵ

#define PARA_VIDEOCOVER_ALARMSCHEDULE		229		 //��Ƶ�ڵ���������ģ��
#define PARA_VIDEOCOVER_ALARMLINK			230		 //��Ƶ�ڵ�������������
#define PARA_VIDEOCOVER_ALARMSCHENABLE		231		 //��Ƶ�ڵ�����ģ��ʹ��
#define PARA_HOLIDAY_PLAN_SCHEDULE			232		 //����ģ��
#define PARA_VCA_RULEEVENT12				233
#define PARA_VCA_RULEEVENT13				234
#define PARA_VDOLOST_LNKSINGLEPIC			235
#define PARA_ALMINPORT_LNKSINGLEPIC			236
#define PARA_MOTIONDEC_LNKSINGLEPIC			237
#define PARA_VDOCOVER_LNKSINGLEPIC			238
#define PARA_CHANNEL_TYPE				    239		//ͨ������
#define PARA_OTHER_ALARMSCHEDULE			240 	//������������ģ��
#define PARA_OTHER_ALARMLINK				241		//������������ģ��
#define PARA_OTHER_ALARMSCHENABLE			242		//������������ģ��
#define PARA_PORTSET						243		//�˿����ò����ı�
#define PARA_DISKGROUP						244		
#define PARA_DISKQUOTA						245			
#define PARA_NEW_IP							246

//add by wd @20130603
#define  PARA_ITS_CROSSINFO					247     //·����Ϣ
#define  PARA_ITS_AREAINFO					248     //��������
#define  PARA_JPEGSIZEINFO					249    //���ץ�ķֱ���	
#define  PARA_DEVSTATUS						250
//add end 

#define  PARA_HD_TEMPLATE_INDEX				251		//����ģ����
#define	 PARA_STREAM_DATA					252		//�û��Զ������ݲ���
#define  PARA_VCA_RULEEVENT14_LEAVE_DETECT	253		//��ڼ����Ϣ
#define  PARA_DOME_PTZ						254		//���PTZ�����ı���Ϣ
#define  PARA_ITS_TEMPLATEMAP				255     //ʱ�������������ģ��ӳ���ϵ
#define  PARA_GPS_CALIBRATION				256     //�Ƿ�����GPSУʱ
#define  PARA_ALARM_THRESHOLD				257     //��ѹ������ֵ
#define  PARA_SHUTDOWN_THRESHOLD			258     //��ѹ�ػ���ֵ
#define  PARA_SMART_SCHEDULE				259		//����¼��ģ��
#define  PARA_VIDEO_SCHEDULE				260
#define  PARA_ITS_CHNCARSPEED				261		//�����ڳ����ٶȲ���

#define  PARA_DOME_SYETEM				    262		//���ϵͳ����
#define  PARA_DOME_MENU						263		//����˵�����
#define  PARA_DOME_WORK_SCHEDULE			264		//�������ģ�����
#define  PARA_INTERESTED_AREA 				265		//����Ȥ�������
#define  PARA_APPEND_OSD				    266		//�����ַ����Ӳ���
#define  PARA_AUTO_REBOOT					267		//NVR�Զ���������ʱ��
#define	 PARA_IP_FILTER						268		//IP��ַ����
#define	 PARA_DATE_FORMAT					269		//����ʱ���ʽ
#define  PARA_NETCONNECT_INFO_MTU			270     //MTU�������ײ�socket���ͷְ������Ԫ��
#define  PARA_LANSET_INFO					271     //�������ò���
#define  PARA_DAYLIGHT_SAVING_TIME			272		//����ʱ
#define  PARA_NET_OFF_LINE					273
#define	 PARA_PICTURE_MERGE					274		//ͼ��ϳ�
#define  PARA_SNAP_SHOT_INFO			    275		//ǰ��ץ�Ĳ���
#define	 PARA_IO_LINK_INFO					276
#define  PARA_COMMAND_ITS_FOCUS				278		//�۽��������
#define  PARA_VCA_RULEEVENT7				279		//��Ʒ������ʧ
#define	 PARA_VCA_RULEEVENT11				280		//�����¼�
#define	 PARA_DEV_COMMONNAME  				281		//�Զ����豸ͨ�ýӿڵı�����64�ֽڣ�

#define  PARA_APPVENCTYPE				    282		//��Ƶ����ı� ������
#define  PARA_THIRD_VENCTYPE				283		//��Ƶ����ı� ������
#define	 PARA_ITS_DEV_COMMONINFO  			284		//ITS�豸ͨ����Ϣ��Ϣ
#define  PARA_ITS_COMPOUND_PARAM			285		//���û�ϴ�����ز���

#define  PARA_DEV_AUDIO_VOLUME				286		//�豸��Ƶ���Ʒ�ʽ�������ı�
#define  PARA_FILE_APPEND_INFO				287		//¼���ļ�������Ϣ
#define	 PARA_VCA_RULEEVENT3				288		//���ܷ����ǻ��¼�
#define	 PARA_VCA_RULEEVENT4				289		//���ܷ���ͣ���¼�	
#define	 PARA_VCA_RULEEVENT5				290		//���ܷ��������¼�
#define  PARA_VCA_RULEEVENT8				291		//���ܷ����������¼�
#define  PARA_ANYSCENE						292		//��������
#define  PARA_ANYCRUISETYPE					293		//����Ӧ��Ѳ��
#define  PARA_ANYCRUISE_TIMER				294		//����Ӧ�ö�ʱѲ��ģ��
#define  PARA_ANYCRUISE_TIMERANGE			295		//����Ӧ��ʱ���Ѳ��ģ��
#define  PARA_TRACK_ASTRICT_LOCATION		296		//������λ
#define  PARA_FACE_DETECT_ARITHMETIC		297		//��������㷨
#define  PARA_PERSON_STATISTIC_ARITHMETIC	298		//����ͳ���㷨
#define  PARA_TRACK_ARITHMETIC				299		//���ܸ����㷨
#define  PARA_TRACK_ZOOMX					300		//���ٱ���ϵ��--ʵʱ����
#define  PARA_COVER_ALARM_AREA				301		//��Ƶ�ڵ������������
#define  PARA_3D_PRAVICY_INFO				302     //3D��˽�ڵ���������ı�	

#define  PARA_WORKDEV_BACKUP_ENABLE			303		//�������ȱ�ʹ��
#define  PARA_BACKUPDEV_INFO				305		//�ȱ���������Ϣ
#define  PARA_IPSAN_DISKINFO				306		//IPSAN������Ϣ
#define  PARA_RAID_WORKMODE					307		//RAID����ģʽ��Ϣ  
#define  PARA_RAIDARRAY_INFO				308	    //RAID�б���Ϣ     
#define  PARA_VIRTUAL_DISK_INFO			    309	    //���������Ϣ    
#define  PARA_RAID_ENABLE					310		//RAID����ʹ��״̬
#define  PARA_HOTBACKUP_WORKMODE			311		//�ȱ�����ģʽ��Ϣ  
#define  PARA_ITS_SECURITY_CODE				312		//���÷�α��λ��
#define  PARA_ITS_LIGHT_SUPPLY				313		//�������Ȳ���
#define  PARA_ITS_CAPTURE_CPUNT				314		//ץ������
#define  PARA_ITS_ILLEGAL_PARK_INFO			315		//Υ��ͣ���궨״̬��Ϣ
#define  PARA_ITS_LINE_PRESS_PERMILLAGE		316		//���ó����ڳ�������ѹ��ǧ�ֱȲ���
#define  PARA_ITS_WORDOVERLAY				317     //ITS�ַ�����
#define  PARA_RTMP_URL_INFO					318     //����RTMP�б���Ϣ
#define  PARA_RTSP_LIST_INFO				319     //RTSP�б���Ϣ
#define  PARA_DEV_FORBID_CHN				320     //��ֹͨ����
#define  PARA_ALARM_IN_CONFIG				321     //����ͨ�����øı���Ϣ
#define  PARA_ALARM_IN_LINK					322     //����ͨ�������ı���Ϣ
#define  PARA_ALARM_SCHEDULE				323     //����ģ��ı���Ϣ
#define  PARA_ALARM_IN_OSD					324     //�ַ����Ӹı���Ϣ
#define  PARA_COM_DEVICE					 	325     //�����豸���øı���Ϣ
#define  PARA_DH_ALARM_HOST		             	326     //���������������ı���Ϣ
#define  PARA_DH_WORK_MOD		             	327		//������������ģʽ
#define  PARA_DH_DEV_TEST		             	328		//��������ģʽ
#define  PARA_ALARM_IN_OFFLINE_TIME_INTERVEL 	329     //�ѻ�ɨ�����ı���Ϣ
#define  PARA_ELENET_METER					 	330     //Electronic Net Meters Param
#define  PARA_DH_ADD_ALARM_HOST					331		//��ӱ�������
#define  PARA_DH_DEVICE_ENABLE					332		//�����豸ʹ��
#define  PARA_ALM_OUT_LHP						333		//����������ʽ�������·�������Ǳ�·����
#define  PARA_OUTPUT_COLOR_TYPE					334		//����ɫ���������
#define  PARA_ITS_RADER_CFG_INFO				335		//ITS Roadway rader area config info
#define  PARA_VCA_RULEEVENT15_WATER_DETECT		336		//water level monitor change message
#define  PARA_CHANGE_VIDEO_INPUT_MODE			337		//change videoinput mode
#define  PARA_CHANGE_WATER_STEADY_PERIOD		338		//water steady period
#define  PARA_CHANGE_WATER_SNAP_INFO    		339		//water steady period

#define  PARA_STORAGE_ALARMPRE   				340
#define  PARA_FAN_TEMP_CONTRO					341
#define  PARA_MODIFYAUTHORITY					342
#define  PARA_VCA_VIDEODIAGNOSE					343    	//VCA video diagnose
#define  PARA_VCA_AUDIODIAGNOSE					344    	//VCA audio diagnose
#define  PARA_VCA_TRIPWIRE300W					345    	//VCA Tripwire For 300W

#define  PARA_PSE_CHANNEL_MODE					346
#define  PARA_PSE_BARCODE						347
#define  PARA_VCA_RULEEVENT14_LEAVE_DETECTEX	348		//VCA Leave Detect For 300W
#define  PARA_FAN_WHITELIGHT					349
#define  PARA_APERTURE_TYPE						350
#define  PARA_IDENTI_CODE						351

#define  PARA_EXCEPTION_INFO					353
#define  PARA_NET_CLIENT_VO_VIEW_SEGMENT		354 	//Custom Picture segmentation way
#define  PARA_NET_CLIENT_PREVIEWREC 			355		//Vc and VGA/HDMI1 or HDMI2 Output Homology
#define  PARA_NET_CLIENT_PRE_MODE 				356		//Preview mode
#define  PARA_ITS_TRAFFICFLOW					357 	//ITS Lane Traffic Statistics
#define  PARA_ITS_TRAFFICFLOWREPORT 			358 	//ITS Traffic Flow Report
#define  PARA_ITS_ILLEGALTYPE					359 	//ITS Illegal Type
#define  PARA_ITS_PICMERGEOVERLAY				360		//ITS Pictrue Merge Over Lay
#define  PARA_NET_CLIENT_AUDIO_PONTICELLO		361 	//Audio Ponticello
#define  PARA_VCA_FACEMOSAIC 					362		//Face Mosaic
#define  PARA_ITS_FTP_UPLOAD					363
#define  PARA_ITS_SIGNAL_LIGHT					364
#define  PARA_DISK_SMART						365
#define	 PARA_OSD_DOT_MATRIX					366
#define	 PARA_OSD_VECTOR						367
#define  PARA_FRAMERATE_LIST					368
#define  PARA_MAX_LANTRATE						369
#define  PARA_ITS_CARLOGO_OPTIM					370
#define  PARA_NET_CLIENT_CREATEFREEVO			371
#define  PARA_EIS								373
#define  PARA_SVC								374
#define  PARA_SERVICE_SNMP						375
#define  PARA_ALM_IN_LHP						376		//����������ʽ�����뿪·�������Ǳ�·����
#define  PARA_NET_CLIENT_PTZ_LIST				377
#define  PARA_ITS_CHNL_DELAY_DISTANCE			378
#define  PARA_DEV_TRENDS_ROI					379
#define  PARA_NET_CLIENT_COLORPARA_LIST			380
#define  PARA_NET_EXCEPTION_LINKOUTPORT			381     //�쳣����˿�����
#define  PARA_NET_WIRELESSMODULE			    382     //��ȡ�豸֧������ģ��
#define  PARA_DEV_EVENT							383     //�¼�ģ��ʹ��
#define  PARA_DEV_TELNET						384     //telnetʹ��
#define	 PARA_ILLEGAL_ALARMLINK					385
#define  PARA_VDOLOST_LNKMAIL					386
#define	 PARA_ALMINPORT_LNKMAIL					387
#define  PARA_MOTIONDEC_LNKMAIL					388
#define  PARA_NET_CLIENT_SCENETIMEPOINT			389		//����ҵ��ʱ���
#define  PARA_VCA_RULE_RIVERCLEAN				390		//VCA����(�ӵ����)
#define  PARA_VCA_RULE_DREDGE					391		//VCA����(���ɵ�ж)
#define  PARA_VCA_RIVER_ADVANCE					392		//VCA�ӵ����߼�����
#define  PARA_H323_LOCAL_Param					393		//H323 Local param
#define  PARA_H323_GK_Param						394		//H323 GK param
#define  PARA_COMMON_ENABLE_VIDEO_REVERSE		395		//video reverse
#define  PARA_LIFEMONITOR_SET_CONFIG			396		//set config
#define  PARA_OSD_EXTRA_INFO					397		//osd extra info
#define  PARA_SNAPSHOT_VIDEOSIZE				398		//snapshot video size
#define  PARA_ITS_IPDCAPMODEL					399		//set capture model
#define  PARA_ITS_IPDCARPOSITION				400     //set manul capture car potion
#define  PARA_DEV_DISKPOPSEAL					401     //disk pop & seal state
#define  PARA_NETSERVER_SERVERINFO				402		//server info
#define  PARA_COMMONENABLE_IllegalPark			403     //ITS Illegal Park
#define  PARA_COMMONENABLE_PUBLIC_NETWORK		404		//public network state
#define  PARA_DEV_VCA_CARCHECK					405     //Vca Car Check Enable
#define  PARA_VCA_SCENE_RECOVERY				406     //Vca Scene recovery
#define  PARA_VIDEO_PARAM						407     //video param
#define  PARA_STREAM_SMOOTH						408		//stream smooth

#define  MAX_SCENETIME_COUNT					8      //����ҵ��ʱ���
#define  MAX_SCENETIME_TYPE_COUNT				2    //����ҵ��ʱ���

typedef int							IDENTIFY;
#define CLIENT						0
#define PROXY						1
#define CLIENT_PROXY				2

// enum DECDATATYPE
// {
//     T_AUDIO8,T_YUV420,T_YUV422
// };
typedef int DECDATATYPE;
#define T_AUDIO8 0
#define T_YUV420 1
#define T_YUV422 2

typedef struct
{
    int     m_iPara[10];
    char    m_cPara[33];
}STR_Para;


#define MAX_DEVSTATUS_TYPE	6
//add by wd 20130621
#define MAX_DEVSTATUS_NUM	4
typedef struct 
{
	int     iType;
	int     iStatus[MAX_DEVSTATUS_NUM];
}STR_DevStatus;
//add end


//Schedle time
typedef struct
{
    unsigned short      iStartHour;
    unsigned short      iStartMin;
    unsigned short      iStopHour;
    unsigned short      iStopMin;
    unsigned short      iRecordMode; //iEnable;
}NVS_SCHEDTIME,*PNVS_SCHEDTIME;

typedef struct
{
	int		 iSize;
	int      iStartHour;
	int      iStartMin;
	int      iStopHour;
	int      iStopMin;
	int      iRecordMode; 
}NVS_SCHEDTIME_Ex,*PNVS_SCHEDTIME_Ex;

typedef struct
{
    unsigned short      m_u16Brightness;
    unsigned short      m_u16Hue;
    unsigned short      m_u16Contrast;
    unsigned short      m_u16Saturation;
    NVS_SCHEDTIME strctTempletTime;
}STR_VideoParam;

typedef struct
{
    char    cEncoder[16];       //NVS IP��
    char    cClientIP[16];      //Client IP��
    int     iChannel;           //Channel Number��
    int     iStreamNO;          //Stream type
    int     iMode;              //Network mode��1��TCP���� 2��UDP����3��multicast��
}LINKINFO, *PLINKINFO;

typedef struct
{
    int             iCount;                     //Connect Count��
    unsigned int    uiID[MAX_ENCODER];          //ID
}UIDLIST, *PUIDLIST;

typedef struct
{
    char        m_cHostName[32];    //NVS host name
    char        m_cEncoder[16];     //NVS (IP)
    int         m_iRecvMode;        //Network mode
    char        m_cProxy[16];       //Proxy (IP)
    char        m_cFactoryID[32];   //ProductID
    int         m_iPort;            //NVS port
    int         m_nvsType;          //NVS type(NVS_T or NVS_S or DVR ...eg)

	int         m_iChanNum;         //encoder channel num
    int         m_iLogonState;      //encoder logon state 090414 zyp add
    int         m_iServerType;      //������ģʽ�豸SERVER_ACTIVE�����Ǳ���ģʽ�豸SERVER_NORMAL
}ENCODERINFO,*PENCODERINFO;

typedef struct
{
    int             m_iServerID;        //NVS ID,NetClient_Logon ����ֵ
    int             m_iChannelNo;       //Remote host to be connected video channel number (Begin from 0)
    char            m_cNetFile[255];    //Play the file on net, not used temporarily
    char            m_cRemoteIP[16];    //IP address of remote host
    int             m_iNetMode;         //Select net mode 1--TCP  2--UDP  3--Multicast
    int             m_iTimeout;         //Timeout length for data receipt
    int             m_iTTL;             //TTL value when Multicast
    int             m_iBufferCount;     //Buffer number
    int             m_iDelayNum;        //Start to call play progress after which buffer is filled
    int             m_iDelayTime;       //Delay time(second), reserve
    int             m_iStreamNO;        //Stream type
    int             m_iFlag;            //0���״������¼���ļ���1������¼���ļ�
    int             m_iPosition;        //0��100����λ�ļ�����λ�ã�-1�������ж�λ
    int             m_iSpeed;           //1��2��4��8�������ļ������ٶ�
}CLIENTINFO;

//Version message structure, with which to transmit SDK version message
typedef struct
{
    unsigned short  m_ulMajorVersion;
    unsigned short  m_ulMinorVersion;
    unsigned short  m_ulBuilder;
    char*           m_cVerInfo;
}SDK_VERSION;

//OSD param
typedef struct
{
    unsigned char   ucCovTime;
    unsigned char   ucCovWord;
    int             iCovYear;
    int             iCovMonth;
    int             iCovDay;
    int             iCovHour;
    int             iCovMinute;
    int             iCovSecond;
    unsigned char   ucPosition;
    char            cCovWord[32];
}OSD_PROPERTY;

//Hardware decode to show external data parameter
typedef struct
{
    int     m_iChannelNum;      //Decode card hardware channel number
    int     m_iVideoWidth;      //Video width
    int     m_iVideoHeight;     //Video height
    int     m_iFrameRate;       //Frame rate
    int     m_iDisPos;          //Video display position
    int     m_iAudioOut;        //whether to output audio
}DecoderParam;

typedef struct
{
    unsigned long nWidth;    //Video width, audio data is 0��
    unsigned long nHeight;   //Video height, audio data is 0��
    unsigned long nStamp;    //Time stamp(ms)��
    unsigned long nType;     //Audio type��T_AUDIO8,T_YUV420����
    unsigned long nFrameRate;//Frame rate��
    unsigned long nReserved; //reserve
}FRAME_INFO;

typedef struct __tagExtraInfo
{
	int		m_iInfoLen;
	void*	m_pvExtInfo;	
}FRAME_EXT_INFO, *LPFRAME_EXT_INFO;

typedef int RAWFRAMETYPE;
#define VI_FRAME 0
#define VP_FRAME 1
#define AUDIO_FRAME 5


typedef struct
{
    unsigned int nWidth;    //Video width, audio data is 0
    unsigned int nHeight;   //Video height, audio data is 0
    unsigned int nStamp;    //Time stamp(ms)
    unsigned int nType;     //RAWFRAMETYPE, I Frame:0,P Frame:1,B Frame:2,Audio:5
    unsigned int nEnCoder;  //Audio or Video encoder(Video,0:H263,1:H264, 2:MP4. Audio,��ѹ��:0,G711_A:0x01,G711_U:0x02,ADPCM_A:0x03,G726:0x04)
    unsigned int nFrameRate;//Frame rate
    unsigned int nAbsStamp; //Abstract Time(s)
}RAWFRAME_INFO;
   
//�����ֵĻص�����           
typedef void (*PROXY_NOTIFY)(int _ulLogonID,int _iCmdKey, char* _cData,  int _iLen,void* _iUser);

//������յ�ԭʼ����,����������һ֡���ݣ������ڴ���ת��
typedef void (*NVSDATA_NOTIFY)(unsigned int _ulID,unsigned char *_cData,int _iLen,void* _iUser);

#define STREAM_INFO_SYSHEAD     1  //ϵͳͷ����
#define STREAM_INFO_STREAMDATA  2  //��Ƶ�����ݣ�����������������Ƶ�ֿ�����Ƶ�����ݣ�
//������һ֡����,������д¼���ļ�,�����Ͳ����������š�Ҫ����_ulStreamType������
typedef void (__stdcall *FULLFRAME_NOTIFY)(unsigned int _ulID,unsigned int _ulStreamType,unsigned char *_cData,int _iLen,void* _iUser);
typedef void (__stdcall *FULLFRAME_NOTIFY_V4)(unsigned int _ulID, unsigned int _ulStreamType, unsigned char *_cData, int _iLen, void* _iUser, void* _iUserData); //_iUser���ļ�ͷ���ݣ�_iUserData���û�����
//δ����ǰ�ı�׼����,��h264����
typedef void (__stdcall *RAWFRAME_NOTIFY)(unsigned int _ulID,unsigned char* _cData,int _iLen, RAWFRAME_INFO *_pRawFrameInfo, void* _iUser);

//�������ݣ��������ز���
typedef void (__stdcall *RECV_DOWNLOADDATA_NOTIFY)(unsigned int _ulID, unsigned char* _ucData,int _iLen, int _iFlag, void* _lpUserData);


//===========================================================================
//  storage  add 2007.6.13
//===========================================================================

#define MAX_DAYS            7
#define MAX_TIMESEGMENT     4
#define MAX_PAGE_SIZE       20
#define MAX_PAGE_LOG_SIZE	20
#define MAX_BITSET_COUNT    16  //֧��512������

#define MAX_DH_TIMESEGMENT  8		//DH����ʱ���

typedef int RECORD_TYPE;
#define RECORD_ALL 0xFF
#define RECORD_MANUAL 1
#define RECORD_TIME	  2
#define RECORD_ALARM  3
#define RECORD_OTHER  4

 typedef int RECORD_STATE;
#define REC_STOP 0
#define REC_MANUAL 1
#define REC_TIME 2
#define REC_ALARM 3

typedef int AUDIO_ENCODER;
#define   G711_A              0x01  /* 64kbps G.711 A, see RFC3551.txt  4.5.14 PCMA */
#define   G711_U              0x02  /* 64kbps G.711 U, see RFC3551.txt  4.5.14 PCMU */
#define   ADPCM_DVI4          0x03  /* 32kbps ADPCM(DVI4) */
#define   G726_16KBPS         0x04  /* 16kbps G.726, see RFC3551.txt  4.5.4 G726-16 */
#define   G726_24KBPS         0x05  /* 24kbps G.726, see RFC3551.txt  4.5.4 G726-24 */
#define   G726_32KBPS         0x06  /* 32kbps G.726, see RFC3551.txt  4.5.4 G726-32 */
#define   G726_40KBPS         0x07  /* 40kbps G.726, see RFC3551.txt  4.5.4 G726-40 */
#define   MPEG_LAYER2         0x0E  /* Mpeg layer 2*/
#define   AMR_NB			  0x15
#define	  MPEG4_AAC			  0x16	/* MPEG-4 aac HEv2 ADTS*/
#define   ADPCM_IMA           0x23  /* 32kbps ADPCM(IMA) */
#define   MEDIA_G726_16KBPS   0x24  /* G726 16kbps for ASF ... */
#define   MEDIA_G726_24KBPS   0x25  /* G726 24kbps for ASF ... */
#define   MEDIA_G726_32KBPS   0x26  /* G726 32kbps for ASF ... */
#define   MEDIA_G726_40KBPS   0x27   /* G726 40kbps for ASF ... */

//Record time parameter
typedef struct
{
    unsigned short iYear;   /* Year */
    unsigned short iMonth;  /* Month */
    unsigned short iDay;    /* Day */
    unsigned short iHour;   /* Hour */
    unsigned short iMinute; /* Minute */
    unsigned short iSecond; /* Second */
} NVS_FILE_TIME,*PNVS_FILE_TIME;

//Record File Property
typedef struct
{
    int             iType;          /* Record type 1-Manual record, 2-Schedule record, 3-Alarm record*/
    int             iChannel;       /* Record channel 0~channel defined channel number*/
    char            cFileName[250]; /* File name */
    NVS_FILE_TIME   struStartTime;  /* File start time */
    NVS_FILE_TIME   struStoptime;   /* File end time */
    int             iFileSize;      /* File size */
}NVS_FILE_DATA,*PNVS_FILE_DATA;

typedef struct
{
	int				iSize;
	NVS_FILE_DATA	tFileData;		//�ļ�������Ϣ
	int			    iLocked;		//�ӽ���״̬
}NVS_FILE_DATA_EX,*PNVS_FILE_DATA_EX;

//Reserch parameter
typedef struct
{
    int             iType;          /* Record type 0xFF-All, 1-Manual record, 2-Schedule record, 3-Alarm record*/
    int             iChannel;       /* Record channel 0xFF-All, 0~channel-defined channel number*/
    NVS_FILE_TIME   struStartTime;  /* Start time of file */
    NVS_FILE_TIME   struStopTime;   /* End time of file */
    int             iPageSize;      /* Record number returned by each research*/
    int             iPageNo;        /* From which page to research */
    int             iFiletype;      /* File type, 0-All, 1-AVstream, 2-picture*/
    int             iDevType;       /* �豸���ͣ�0-���� 1-������Ƶ������ 2-��������� 0xff-ȫ��*/
}NVS_FILE_QUERY, *PNVS_FILE_QUERY;

typedef struct  
{
	NVS_FILE_QUERY	fileQuery;
	char			cOtherQuery[65];
	int				iTriggerType;		//�������� 3:�˿ڱ���,4:�ƶ�����,5:��Ƶ��ʧ����,0x7FFFFFFF:��Ч
	int				iTrigger;			//�˿ڣ�ͨ������
}NVS_FILE_QUERY_EX, *PNVS_FILE_QUERY_EX;

//Net storage device
typedef struct
{
    char cDeviceIP[16];
    char cStorePath[250];
    char cUsername[16];
    char cPassword[16];
    int  iState;
}NVS_NFS_DEV,*PNVS_NFS_DEV;

//Diskinfo
//disk manager
#define DISK_SATA_NUM		8
#define DISK_USB_NUM		4
#define	DISK_NFS_NUM		1
#define	DISK_ESATA_NUM		1
#define	DISK_SD_NUM			1
#define DISK_SATA_EX_NUM	8

#define DISK_VD_NUM			16
#define DISK_IPSAN_NUM		8

#define DNO_ESATA	32	// esata disk first no
#define DNO_SD		50	// SD disk first no

#define DISK_USB		8	  //USB���ʹ��� 8~12
#define DISK_SATA_EX	1008  //SATA���ʹ��� 1008~1015
#define	DISK_IPSAN		3000  //IPSAN���� 3000~3008
#define DISK_VD			2000  //��������̺� 2000~2015

typedef int DISK_STATUSTYPE;
#define DISK_ZERO 0
#define DISK_NOTFORMAT 1
#define DISK_FORMATED  2
#define DISK_CANUSE    3
#define DISK_READING   4


typedef struct
{
    unsigned int  m_u32TotalSpace; // �ܴ�С
    unsigned int  m_u32FreeSpace;  // ʣ��ռ�
    unsigned short  m_u16PartNum;    // ������  1��4
    unsigned short  m_u16Status;     // ״̬ 0���޴��̣�1��δ��ʽ����2���Ѹ�ʽ����3���ѹ��أ�4����д��
    unsigned short  m_u16Usage;      // ��; 0��¼��1������
    unsigned short  m_u16Reserve;    //	����
}NVS_DISKINFO, *PNVS_DISKINFO;

//���ṹ��
#define  CMD_DISK_QUOTA_MAX_USED  			1
#define  CMD_DISK_QUOTA_TOTAL				2
#define  CMD_DISK_QUOTA_CURRENT_USED		3
typedef struct
{
	int  iSize;												//�ṹ���С
	int  iRecordQuota; 										//¼������λ��GB��
	int  iPictureQuota;										//ͼƬ����λ��GB��
}DISK_QUOTA;

typedef struct
{
	int  iSize;												//�ṹ���С
	char cRecordQuota[65]; 										//����¼������λ��GB��
	char cPictureQuota[65];										//����ͼƬ����λ��GB��
}DISK_QUOTA_USED;
//����ṹ��
#define MAX_DISK_GROUP_NUM			8
typedef struct
{
	int		iSize;								//�ṹ���С 
	int		iGroupNO;							//�����ţ�0-7
	unsigned int	uiDiskNO[2];				//���̱�ţ���λ��ʾ��ֻ��ѡ��0-7//����32����С���󣬶�Ӧbitλ�ӵ͵���
	unsigned int	uiChannelNO[4];				//ͨ�����,��λ��ʾ��ͨ���Ŵ�С���󣬶�Ӧ//bitλ�ӵ͵���������128ͨ��
	unsigned int	uiDiskNoEx[4];				//�������̱��
	unsigned int	uiChannelNOEx[12];			//ͨ�����,��λ��ʾ��ͨ���Ŵ�С���󣬶�Ӧ//bitλ�ӵ͵���������512ͨ��
}DISK_GROUP;

//����Ȩ����Ϣ�ṹ��
#define MAX_MODIFY_AUTHORITY_NUM    26				//���Ȩ�޸���
typedef struct 
{
	int iLevel;									// Ȩ�޺�
	unsigned int uiList[4];						//ͨ���б�����
}AUTHORITY_INFO;
//����Ȩ����Ϣ�ṹ����չ��֧�ִ�·��
typedef struct 
{
	int		iSize;
	char	cUserName[LEN_32];							//�û���
	int		iLevel;										//Ȩ�޺�
	unsigned int uiList[MAX_BITSET_COUNT];				//ͨ���б�����
}AUTHORITY_INFO_V1;
//�û�Ȩ�޽ṹ��
typedef struct
{
	int iNeedSize;				//�û�ʹ��ʱ��iNeedSize����һ��������Ҫ�޸ĵ�Ȩ����,��󲻳���10
	AUTHORITY_INFO  strAutInfo[MAX_MODIFY_AUTHORITY_NUM];
}USER_AUTHORITY;

//�û�Ȩ�����ṹ��
#define MAX_AUTHORITY_GROUP_NUM		4	//Ȩ�����������
typedef struct
{
	int iSize;												//�ṹ���С 
	int iGroupNO;											//Ȩ�����ţ�1��ͨ�û���2��Ȩ�û���
	//3�����û���4����Ա
	unsigned int uiList[2];									//��Ȩ��������Ӧ��Ȩ�޺ţ���λ��ʾ��
	//Ȩ�ޱ�Ŵ�С���󣬶�Ӧbitλ�ӵ͵���
	char  cReserved[256];									//Ԥ��
}GROUP_AUTHORITY;

typedef struct
{
    NVS_DISKINFO		m_strctSATA[DISK_SATA_NUM];
    NVS_DISKINFO		m_strctUSB[DISK_USB_NUM];
    NVS_DISKINFO		m_strctNFS[DISK_NFS_NUM];
    NVS_DISKINFO		m_strctESATA[DISK_ESATA_NUM];
    NVS_DISKINFO		m_strctSD[DISK_SD_NUM];
	NVS_DISKINFO		m_strctSATA_EX[DISK_SATA_EX_NUM];
	NVS_DISKINFO		m_tVD[DISK_VD_NUM];			//����VD���2000~2015
	NVS_DISKINFO		m_tIPSAN[DISK_IPSAN_NUM];	//����IPSAN���3000~3007
}NVS_STORAGEDEV, *PNVS_STORAGEDEV;

typedef int SMTP_AUTHTYPE;
#define AUTH_OFF			0
#define AUTH_PLAIN			1
#define AUTH_CRAM_MD5		2
#define AUTH_DIGEST_MD5 	3
#define AUTH_GSSAPI			4
#define AUTH_EXTERNAL		5
#define AUTH_LOGIN			6
#define AUTH_NTLM			7
#define SMTP_AUTHTYPE_LAST	8

typedef int SCENE_TYPE;
#define AUTO			0
#define SCENE1			1
#define SCENE2			2
#define SCENE3 			3
#define SCENE4			4
#define SCENE5			5
#define SCENE6			6
#define SCENE7			7
#define SCENE8			8
#define SCENE9			9
#define SCENE10			10
#define SCENE11			11
#define SCENE_TYPE_LAST	12

//SMTP alarm info
typedef struct
{
    char            cSmtpServer[32];    //smtp server
    unsigned short  wdSmtpPort;         //smtp server port
    char            cEmailAccount[32];  //mail account
    char            cEmailPassword[32];  //mail password
    int             iAuthtype;           //authtype
    char            cEmailAddress[32];  //mailbox address
    char            cMailSubject[32];   //mail subject
}SMTP_INFO,*PSMTP_INFO;
//add SMTP alarm info by gjf
typedef struct SMTP_INFOEX
{
	SMTP_INFO	smtpInfo;
	char		cMailAddr[3][32];		//	����3����ַ
	int			iScrityType;			//	0-retain��1-not scrity��2-SSL��3-TLS
}*PSMTP_INFOEX;
//add end 
//---------------------------------------------------------------------------

typedef struct
{
    char cESSID[33];            //ESSID
    char cEncryption[16];       //wifi encrypttion flag; 0 no encryption��1 digital encryption
}WIFI_INFO, *PWIFI_INFO;

typedef struct
{
    char    m_cDDNSUserName[32];
    char    m_cDDNSPwd[32];
    char    m_cDDNSNvsName[32];
    char    m_cDDNSDomain[32];
    int     m_iDDNSPort;
    int     m_iDDNSEnable;
}DDNS_INFO, *PDDNS_INFO;

typedef struct
{
    char    cESSID[33];         //ESSID
    char    cWifiSvrIP[16];     //wifiIP
    char    cWifiMask[16];          //wifi mask
    char    cWifiGateway[16];       //wifi gateway
    char    cWifiDNS[16];           //wifi DNS
    char    cWifiKeyType[16];       //wifi KeyType��Hex��ASCII
    char    cWifiPassword[128];     //wifi password, @yys@, 32->128, 2010-07-05
    char    cEncryption[16];        //wifi encrypttion flag; NULL no encryption��"WEP" encryption
    char    cWifiKeyNum[2];         //wifi KeyNum (1,2,3,4)
    int     iPwdFormat;
    int     iHaveCard;
    int     iCardOnline;
}NVS_WIFI_PARAM, *PNVS_WIFI_PARAM;

//����IE
//wifiap
//typedef struct
//{
//	char	cESSID[64];			    //ESSID!
//	char	cWifiSvrIP[16];		    //wifiapIP
//	char	cWifiMask[16];			//wifiap mask
//	char	cWifiGateway[16];		//wifiap gateway
//	char	cWifiDNS[16];			//wifiap DNS
//	char	cWifiKeyType[16];		//wifiap KeyType��Hex��ASCII
//	char	cWifiPassword[128];		//wifiap password,
//	char	cEncryption[16];		//wifiap encrypttion flag; NULL no encryption��"WEP" encryption
//	char	cWifiKeyNum[2];         //wifiap KeyNum (1,2,3,4)
//	int		iPwdFormat;
//	int		iHaveCard;
//	int		iCardOnline;
//}NVS_WIFIAP_PARAM, *PNVS_WIFIAP_PARAM;

//����SDK4.0��3.3
typedef struct NVS_IPAndID
{
    char *m_pIP;
    char *m_pID;
	union
	{
		unsigned int *m_puiLogonID;
		int  *m_piLogonID;
	};
}*pNVS_IPAndID;

typedef struct			//���Ƶ�ͨ����Ϣ
{
	char    m_cParam1[64];
	char    m_cParam2[64];
	char    m_cParam3[64];
	char    m_cParam4[64];
	char    m_cParam5[64];
	char    m_cParam6[64];
	char    m_cParam7[64];
	char    m_cParam8[64];
	char    m_cParam9[64];
	char    m_cParam10[64];
	char    m_cParam11[64];
	char    m_cParam12[64];
	char    m_cParam13[64];
	char    m_cParam14[64];
	char    m_cParam15[64];
	char    m_cParam16[64];
	char    m_cParam17[64];
	char    m_cParam18[64];
	char    m_cParam19[64];
	char    m_cParam20[64];
}DZ_INFO_PARAM, *PDZ_INFO_PARAM;


#define MAX_PARAM_NUM 32

typedef struct STRCT_ParamPackage
{
	char			m_strParam[MAX_PARAM_NUM][256];
	int				m_iParam[MAX_PARAM_NUM];
	void*           m_lpVoid[MAX_PARAM_NUM];
	
}*pSTRCT_ParamPackage;
//--------------------------------------------------------

#ifndef HIWORD
#define HIWORD(l)   ((unsigned short) (((unsigned int) (l) >> 16) & 0xFFFF))
#endif

#ifndef LOWORD
#define LOWORD(l)   ((unsigned short) (l))
#endif


#define SERVER_NORMAL  0
#define SERVER_ACTIVE  1
#define SERVER_DNS		2

#define DATA_RECEIVE_NOT  0      //û�н���
#define DATA_RECEIVE_WAIT 1      //�ȴ�����
#define DATA_RECEIVE_HEAD 2      //�Ѿ��յ���Ƶͷ�����ǻ�û���յ�����								 
#define DATA_RECEIVE_ING  3      //���ڽ���ing
#define DATA_RECEIVE_DIGITALCHANNEL_FAILED  4      //����ͨ��δ����

#define INFO_USER_PARA     0
#define INFO_CHANNEL_PARA  1
#define INFO_ALARM_PARA    2
#define INFO_VIDEO_HEAD    3
#define INFO_LOGON_PARA    4
#define INFO_FINISH_PARA   5

typedef void (*CMDSTRING_NOTIFY)(char *_cBuf,int _iLen,void* _pUserData); 
  
#define PROTOCOL_COUNT			64				//	���֧�ֵ�Э�����
#define PROTOCOL_NAME_LENGTH	32				//	����Э�����Ƶ���󳤶�
#define MAX_PROTOCOL_TYPE		3
typedef struct
{
	int     iType;							   //������ 0�����豸֧�ֵ�Э��, 1��������ʹ�õ�Э��
	int		iCount;					           //������֧�ֵ�Э��������
	char 	cProtocol[PROTOCOL_COUNT][PROTOCOL_NAME_LENGTH];		//��Э�����ƣ����֧�ָ�Э�飬ÿ��Э��������ֽڣ�
} st_NVSProtocol;  

#ifdef WIN32
typedef struct
{
	char m_cIP[32];
	char m_cDeviceID[64];
	int  m_iLogonID;
	int  m_iChannel;
	int  m_iStreamNO;
	int  m_iDrawIndex;
}st_ConnectInfo,*pst_ConnectInfo;
#else
typedef struct
{
	char m_cIP[32];
	char m_cDeviceID[64];
	int  m_iLogonID;
	int  m_iChannel;
	int  m_iStreamNO;
	int  m_iDrawIndex;
	char m_cDownLoadFile[256];
}st_ConnectInfo,*pst_ConnectInfo;
#endif


#define CLIENT_DEFAULT 0       //���Ӵ����Ĭ�Ͽͻ���
#define CLIENT_DECODER 1       //�Խ��������⴦��

//#define MAX_DATA_CHAN MAX_CHANNEL_NUM_TDWY		//����������
#define MAX_FILE_CHAN 5

#define DATA_CMD  1
#define DATA_DATA 2
typedef void (*INNER_DATA_NOTIFY)(unsigned int _ulID,void *_pBlock,int _iType,void* _iUser);

#define REC_FILE_TYPE_STOP      (-1)
#define REC_FILE_TYPE_NORMAL	0
#define REC_FILE_TYPE_AVI		1
#define REC_FILE_TYPE_ASF		2
#define REC_FILE_TYPE_AUDIO     3
#define REC_FILE_TYPE_RAWAAC    4
#define REC_FILE_TYPE_VIDEO     5


typedef struct					//  2010-1-26-18:09 @yys@
{
	int			iChannelNo;		//	ͨ����
	int			iLogType;		//	��־����
	int			iLanguage;		//	��������
	NVS_FILE_TIME	struStartTime;		//	��ʼʱ��
	NVS_FILE_TIME	struStopTime;		//	����ʱ��
	int			iPageSize;		//	ҳ��С
	int			iPageNo;		//	ҳ���
}NVS_LOG_QUERY, *PNVS_LOG_QUERY;

typedef struct  
{
	int				iChannel;
	int				iLogType;
	NVS_FILE_TIME	struStartTime; 	/* File start time */
	NVS_FILE_TIME	struStoptime; 	/* File end time */
	char			szLogContent[129];					
}NVS_LOG_DATA,*PNVS_LOG_DATA;

#define DOWNLOAD_TYPE_NOTHING		0
#define DOWNLOAD_TYPE_ERROR			1
#define DOWNLOAD_TYPE_PIC			2
#define DOWNLOAD_TYPE_VIDEO			3
#define DOWNLOAD_TYPE_TIMESPAN		4

//Inner Player Start
//play back control CMD
#define		PLAY_CONTROL_PLAY				1				//play start
#define		PLAY_CONTROL_PAUSE				2				//play pause
#define     PLAY_CONTROL_SEEK				3
#define		PLAY_CONTROL_FAST_FORWARD		4				//play fast
#define		PLAY_CONTROL_SLOW_FORWARD		5				//play slow
#define     PLAY_CONTROL_OSD				6				//Add OSD
#define		PLAY_CONTROL_GET_PROCESS		7				//get process
#define		PLAY_CONTROL_START_AUDIO		8				//PlayAudio
#define		PLAY_CONTROL_STOP_AUDIO			9				//Stop Audio

#define     PLAY_CONTROL_SET_VOLUME			10				//SetVolume
#define     PLAY_CONTROL_GET_VOLUME			11				//Get Volume

#define     PLAY_CONTROL_STEPFORWARD		12				//stepforward
#define		PLAY_CONTROL_START_CAPTUREFILE	13				//Capture File
#define		PLAY_CONTROL_STOP_CAPTUREFILE	14				//Capture File

#define		PLAY_CONTROL_START_CAPTUREPIC	15				//Capture Pic
#define		PLAY_CONTROL_PLAYRECT			16				//Play Rect
#define		PLAY_CONTROL_GETUSERINFO		17				//Get UserInfo
#define		PLAY_CONTROL_SETDECRYPTKEY		18				//SetVideoDecryptKey
#define		PLAY_CONTROL_GETFILEHEAD		19				//get video param
#define		PLAY_CONTROL_SETFILEHEAD		20				//set file header
#define		PLAY_CONTROL_GETFRAMERATE		21				//get frame rate


//PlayBack Type
#define PLAYBACK_TYPE_FILE				1
#define PLAYBACK_TYPE_TIME				2

struct PlayerParam
{
	int				iSize;
	char			strFileName[LEN_128];	//previous device file name
	int				iLogonID;				//remote file use
	int				iChannNo;				//remote time use
	NVS_FILE_TIME	tBeginTime;
	NVS_FILE_TIME	tEndTime;
	//up is first edition(lc) old struct
	int				iPlayerType;
	//down is new function 
	char			cLocalFilename[LEN_256];//local record file name
	int				iPosition;			    //file position,by 0%��100%;continue download��the pos of the file request
	int				iSpeed;					//1��2��4��8��control file play speed, 0-pause
	int				iIFrame;			    //juest send I frame 1,just play I frame;0,all play				
	int				iReqMode;				//need data mode 1,frame mode;0,stream mode					
	int				iRemoteFileLen;		    //if local file is not NULL��this param set to NULL
	int				iVodTransEnable;		//enable
	int				iVodTransVideoSize;	    //Video resolution
	int				iVodTransFrameRate;     //frame rate
	int				iVodTransStreamRate;    //stream rate
};

typedef struct _tagPlayBackOsd
{
	int iSize;
	int iX;
	int iY;
	char cOsdText[96];
	int iColor;		//fefault -1 red
}tPlayBackOsd;

typedef struct _tagPlaybackProcess
{
	int iSize;
	int iPlayByFileOrTime;
	int iPlayedFrameNum;
	int iTotalFrame;
	int iProcess;
	unsigned int uiCurrentPlayTime;
}tPlaybackProcess;

typedef struct _tagPlaybackVolume
{
	int iSize;
	unsigned short usVolume;
}tPlaybackVolume;

//typedef struct _tagPlaybackVlume
//{
//	int iSize;
//	int iSetOrGet;
//	int iVolume;
//}tPlaybackVlume;

typedef struct _tagPlaybackCapture
{
	int		iSize;
	char	cFileName[LEN_256];	
	int		iRecFileType;
}tPlaybackCapture;

typedef struct _tagDecryptKey
{
	int		iSize;
	char	cKey[LEN_128];
	int		ikeySize;
}tDecryptKey;

//typedef struct _tagVideoParam
//{
//	int iSize;
//	int iWidth;
//	int iHeight;
//	int iFrameRate;
//}tVideoParam;

//Inner Player End

typedef struct  
{
	int iPreset[32];				//Ԥ��λ�� ȡֵ��Χ��1-255	
	int	iStayTime[32];				//ͣ��ʱ�� ȡֵ��Χ��1-60	
	int	iSpeed[32];					//�ٶ�	   ȡֵ��Χ��0-100	
}st_PTZCruise,*Pst_PTZCruise;

/************************************************************************
*  CD-Burn �������                                                                    
************************************************************************/
#define		CDBURN_CMD_SET_MIN					0
#define		CDBURN_CMD_SET_START				(CDBURN_CMD_SET_MIN+0)				//	��ʼ��¼
#define		CDBURN_CMD_SET_STOP					(CDBURN_CMD_SET_MIN+1)				//	ֹͣ��¼
#define		CDBURN_CMD_SET_MODE					(CDBURN_CMD_SET_MIN+2)				//	���ÿ�¼ģʽ
#define		CDBURN_CMD_SET_RESUME				(CDBURN_CMD_SET_MIN+3)				//	������¼
#define		CDBURN_CMD_SET_MAX					(CDBURN_CMD_SET_MIN+4)

//#define		CDBURN_CMD_GET_MIN					(CDBURN_CMD_SET_MAX)			//	�����������չ������
#define		CDBURN_CMD_GET_MIN					(3)									//	Get��������
#define		CDBURN_CMD_GET_MODE					(CDBURN_CMD_GET_MIN+0)				//	�õ���¼ģʽ
#define		CDBURN_CMD_GET_CDROMLIST			(CDBURN_CMD_GET_MIN+1)				//	�õ���ǰ�����б�
#define		CDBURN_CMD_GET_CDROMCOUNT			(CDBURN_CMD_GET_MIN+2)				//	�õ���ǰ�����ĸ���
#define		CDBURN_CMD_GET_STATUS				(CDBURN_CMD_GET_MIN+3)				//	�õ���ǰ��¼״̬
#define		CDBURN_CMD_GET_MAX					(CDBURN_CMD_GET_MIN+4)

#define		CDBURN_MAX_COUNT				32			//	�������Ĺ�����

#define		CDBURN_STATUS_FREE				0		//	����δʹ��
#define		CDBURN_STATUS_BUSY				1		//	���ڿ�¼
#define		CDBURN_STATUS_BAD				2		//	��������
#define		CDBURN_STATUS_FULL				3		//	�����Ѿ���
#define		CDBURN_STATUS_PACK				4		//	�������ڴ��
#define     CDBURN_STATUS_BEGINFAILED       102     //  ��ʼ��¼ʧ�� 

#define		MAX_DVDNAME_LEN					31

struct TCDBurnStatus
{
	int				iCDRomID;					//	����ID
	int				iState;						//	��ǰ״̬
	unsigned long	ulTotal;					//	�ܴ�С
	unsigned long	ulFree;						//	�����С
	union
	{
		char			cReserved[4];				//	Ԥ��4���ֽ�
		int			iReserved;
	};	
};

//Burn file mode:0-original mode,1-single file mode
#define BURN_FILE_ORIGINAL_MODE			0
#define BURN_FILE_SINGLE_FILE_MODE		1

struct TCDBurnPolicy
{
	int				iCDRomID;								//	CD Rom ID,only use single burn

	int				iMode;									//	0-single burn, 1-mutil burn, 2-circle burn
	int				iChannelBits[CDBURN_MAX_COUNT];			//	channel list,bits
	int				iDeviceBits[CDBURN_MAX_COUNT];			//	device list,bits
	//Expand 20121025
	int				iDiskType;								//	disk type
	int				iBurnSpeed;								//	burn speed
	int				iBurnHour;								//	burn time
	int				iResSpace;								//	reserve space
	char			cDVDName[MAX_DVDNAME_LEN+1];			//	DVD name 
	//Expand 20150427
	int				iBurnFileMode;							//Burn file mode:0-original mode,1-single file mode
};

struct TCBurnStopMode
{
	int             iDeviceNum;                            //�������
	int             iFlagByBits;                            //bit0: 1�����̣�0���̣�bit1��1�����̣� 0����
};       

/************************************************************************
*	��Ƶ���� Video Encrypt & Decrypt                                                                      
************************************************************************/
struct TVideoEncrypt
{
	int			iChannel;					//	����ͨ��, -1����ʾ����ͨ��
	int			iType;						//	��������, -- 0�������ܣ�1��AES����������չ
	int			iMode;						//	�����㷨�򷽷���Ŀǰ�ò���
	char		cPassword[17];				//	��������
};

struct TVideoDecrypt
{
	int			iChannel;							//	����ͨ��
	char		cDecryptKey[17];					//	������Կ
};

/************************************************************************
   Digital Channel Param  20110303                                                                   
************************************************************************/
#define DC_CMD_GET_MIN				0
#define DC_CMD_GET_ALL				(DC_CMD_GET_MIN+0)
#define DC_CMD_GET_IPCPnP			(DC_CMD_GET_MIN+2)
#define DC_CMD_GET_MAX				(DC_CMD_GET_MIN+3)

#define DC_CMD_SET_MIN				(0)
#define DC_CMD_SET_ALL				(DC_CMD_SET_MIN+1)
#define DC_CMD_SET_IPCPnP			(DC_CMD_SET_MIN+2)
#define DC_CMD_SET_MAX				(DC_CMD_SET_MIN+3)
/************************************************************************
   ����ͨ�����弴�ù���ģʽ
***********************************************************************/
#define DIGITAL_CHANNEL_PLUGANDPLAY_WORKMODE_MIN         0
#define DIGITAL_CHANNEL_PLUGANDPLAY_WORKMODE_OFF        (DIGITAL_CHANNEL_PLUGANDPLAY_WORKMODE_MIN + 0)//�ر�
#define DIGITAL_CHANNEL_PLUGANDPLAY_WORKMODE_AUTO_ADD   (DIGITAL_CHANNEL_PLUGANDPLAY_WORKMODE_MIN + 1)//�Զ����
#define DIGITAL_CHANNEL_PLUGANDPLAY_WORKMODE_AUTO_FIND  (DIGITAL_CHANNEL_PLUGANDPLAY_WORKMODE_MIN + 2)//�Զ�����(��ʾ�û��ֶ����)
#define DIGITAL_CHANNEL_PLUGANDPLAY_WORKMODE_MAX        (DIGITAL_CHANNEL_PLUGANDPLAY_WORKMODE_MIN + 0)

//  [12/15/2012]
#define	MAX_RTSPURL_LEN				(127)
#define MAX_RTSPURL_LEN_EX			(63)

//	Э������
#define SERVERTYPE_PRIVATE			(0)
#define SERVERTYPE_ONVIF			(1)
#define SERVERTYPE_PUSH				(2)
#define SERVERTYPE_RTSP				(3)

struct TDigitalChannelParam
{
	int iChannel;							//	[����ͨ����]--ȡֵ��Χ���豸���Ͷ�������¼ʱ����ͻ��˷�������ͨ����
	int	iEnable;							//	[ʹ��]--0�����ø�����ͨ����1�����ø�����ͨ����Ĭ��Ϊ����
	int iConnectMode;						//	[���ӷ�ʽ]--0��IP��1��������2������ģʽ��Ĭ��ΪIP
	int	iDevChannel;						//	[ǰ���豸ͨ����]--ȡֵ��Χ��ǰ���豸����������Ĭ��Ϊ0
	int iDevPort;							//	[ǰ���豸�˿ں�]--81��65535��Ĭ��Ϊ3000
	int iStreamType;						//	[��������]--0����������1����������Ĭ��Ϊ������
	int iNetMode;							//	[��������]--1��TCP��2��UDP��3���鲥���ݶ�ΪTCP,���ɸ���
	int iPtzAddr;							//	[PTZ��ַ]--0��256��Ϊ����ʹ��Ĭ��ֵ

	char cIP[33];							//	[���Ӳ���]--iConnectMode=0ʱ��IP��ַ��iConnectMode=1ʱ��������iConnectMode=2ʱ���豸ID��Ĭ��Ϊ�գ�����32���ַ�
	char cProxyIP[33];						//	[����IP]--iConnectMode=2ʱ�����ֶ���Ч��Ĭ��Ϊ�գ�����32���ַ�
	char cPtzProtocolName[33];				//	[PTZЭ��]--��¼�豸ʱ����ͻ��˷����豸֧�ֵ�PTZЭ���б�Ϊ����ʹ��Ĭ��ֵ������32���ַ�
	char cUserName[17];						//	[�û���]--��¼ǰ���豸���û���������Ϊ�գ�����16���ַ�
	char cPassword[17];						//	[����]--��¼ǰ���豸�����룬����Ϊ�գ�����16���ַ�
	char cEncryptKey[17];					//	[��Ƶ��������]--ǰ���豸��Ƶ���ܵ����룬Ϊ�����ʾ�����ܣ�����16���ַ�
	int iServerType;						//  ǰ���豸�����õ�����Э��(Miracle�����ֶ�)
											//  0:˽��Э��
											//  1:OnvifЭ�飻
											//  2:Push��
											//	3:RTSP
	union
	{
		struct{
			char cRTSPUrl[MAX_RTSPURL_LEN+1];		//	RTSP url
			char cIPv6[LENGTH_IPV6_V1];				//	IPv6 ��ַ
		};
		char cRTSPUrlEx[4*MAX_RTSPURL_LEN_EX+4];
	};
	
	char cMucIp[LEN_64];	//muticast IP address
	int  iMucPort;			//muticast port
};

/************************************************************************
*	�õ��豸ͨ�������Բ���                                                                     
************************************************************************/
#define		CHANNEL_TYPE_LOCAL			0			//	����ģ��ͨ��
#define		CHANNEL_TYPE_DIGITAL		2			//	����ͨ��
#define		CHANNEL_TYPE_COMBINE		3			//	�ϳ�ͨ��

#define		CHANNEL_TYPE_MAINSTREAM		0			//	������
#define     CHANNEL_TYPE_SUBSTREAM      1           //	������
#define		CHANNEL_TYPE_THIRDSTREAM	4			//	������

#define GENERAL_CMD_MIN						0
#define GENERAL_CMD_GET_CHANNEL_TYPE		(GENERAL_CMD_MIN+1)			//	�õ����豸��ĳ��ͨ��������(��)
#define GENERAL_CMD_CHANNEL_TYPE			(GENERAL_CMD_MIN+1)			//	�õ����豸��ĳ��ͨ��������(��)
#define GENERAL_CMD_CHANTYPE_LIST			(GENERAL_CMD_MIN+2)			//	�õ����豸�ĸ�ͨ���ɱ༭������
#define GENERAL_CMD_ALARM_IN_CHANNEL_TYPE   (GENERAL_CMD_MIN+3)	//	�õ����豸�ĸ���������ͨ������
#define GENERAL_CMD_MAX						(GENERAL_CMD_MIN+4)

/************************************************************************
* VCA ������ݽṹ�����ϲ����ʹ��,�����CMDIDʹ��enum���(sizeof(enum))��
	�����ǵ�CB��VC�ļ��������⣬ͳһʹ��#defineʵ��
************************************************************************/
#define VCA_MAX_RULE_NUM				8				//	ÿ��ͨ�����������������
#define VCA_MAX_EVENT_NUM				14				//	�����¼�����
#define VCA_MAX_TRIPWIRE_NUM			8				//	���ĵ����߸���
#define VCA_MAX_DBTRIPWIRE_NUM			4				//	����˫���߸���
														//
#define VCA_MAX_OSC_REGION_NUM			3				//	�����Ʒ��ʧ�������

#define VCA_MAX_RULE_NAME_LEN			17				//	�������Ƶĳ���

typedef int vca_EEventType;

#define VCA_EVENT_MIN							0						//	�¼�ID������ "[0,9)"
#define VCA_EVENT_TRIPWIRE						(VCA_EVENT_MIN+0)		//	�����¼�
#define VCA_EVENT_DBTRIPWIRE					(VCA_EVENT_MIN+1)		//	˫�����¼�
#define VCA_EVENT_PERIMETER						(VCA_EVENT_MIN+2)		//	�ܽ��¼�
#define VCA_EVENT_LOITER						(VCA_EVENT_MIN+3)		//	�ǻ��¼�
#define VCA_EVENT_PARKING						(VCA_EVENT_MIN+4)		//	ͣ���¼�
#define VCA_EVENT_RUN							(VCA_EVENT_MIN+5)		//	�����¼�
#define VCA_EVENT_HIGH_DENSITY					(VCA_EVENT_MIN+6)		//	��Ա�ܶ��¼�
#define VCA_EVENT_ABANDUM						(VCA_EVENT_MIN+7)		//	�������¼�
#define VCA_EVENT_OBJSTOLEN						(VCA_EVENT_MIN+8)		//	�������¼�
#define VCA_EVENT_FACEREC						(VCA_EVENT_MIN+9)		//	����ʶ���¼�
#define	VCA_EVENT_VIDEODETECT					(VCA_EVENT_MIN+10)		//	��Ƶ����¼�
#define	VCA_EVENT_TRACK							(VCA_EVENT_MIN+11)		//	�����¼�
#define	VCA_EVENT_FLUXSTATISTIC					(VCA_EVENT_MIN+12)		//	����ͳ���¼�
#define	VCA_EVENT_CROWD						    (VCA_EVENT_MIN+13)		//	��Ⱥ�ۼ��¼�
#define	VCA_EVENT_LEAVE_DETECT					(VCA_EVENT_MIN+14)		//	��ڼ���¼�
#define	VCA_EVENT_WATER_LEVEL_DETECT			(VCA_EVENT_MIN+15)		//	ˮλ����¼�
#define	VCA_EVENT_AUDIO_DIAGNOSE				(VCA_EVENT_MIN+16)		//	Audio Diagnose
#define VCA_EVENT_FACE_MOSAIC					(VCA_EVENT_MIN+17)		//  Face Mosaic
#define VCA_EVENT_RIVERCLEAN					(VCA_EVENT_MIN+18)		//  �ӵ����
#define VCA_EVENT_DREDGE						(VCA_EVENT_MIN+19)		//  ���ɵ�ж
#define VCA_EVENT_ILLEAGEPARK					(VCA_EVENT_MIN+20)		//	Υ��ͣ��
#define	VCA_EVENT_MAX							(VCA_EVENT_MIN+21)		//	��Ч�¼�ID���������


#define VCA_CMD_SET_MIN							1									//	VCA������������
#define VCA_CMD_SET_CHANNEL						(VCA_CMD_SET_MIN+ 0)				//	����VCAͨ���ż�ʹ��
#define VCA_CMD_SET_VIDEOSIZE					(VCA_CMD_SET_MIN+ 1)				//	����VCAͨ������Ƶ��С
#define VCA_CMD_SET_ADVANCE_PARAM				(VCA_CMD_SET_MIN+ 2)				//	����VCA�߼�����
#define VCA_CMD_SET_TARGET_PARAM				(VCA_CMD_SET_MIN+ 3)				//	����VCA���Ӳ�������ɫ����
#define VCA_CMD_SET_ALARM_STATISTIC				(VCA_CMD_SET_MIN+ 4)				//	����VCA��������Ϊ��
#define VCA_CMD_SET_RULE_COMMON					(VCA_CMD_SET_MIN+ 5)				//	����VCA���򳣹����
#define VCA_CMD_SET_RULE0_TRIPWIRE				(VCA_CMD_SET_MIN+ 6)				//	����VCA���򣨵����ߣ�
#define VCA_CMD_SET_RULE1_DBTRIPWIRE			(VCA_CMD_SET_MIN+ 7)				//	����VCA����˫���ߣ�
#define VCA_CMD_SET_RULE2_PERIMETER				(VCA_CMD_SET_MIN+ 8)				//	����VCA�����ܽ磩
#define VCA_CMD_SET_ALARM_SCHEDULE				(VCA_CMD_SET_MIN+ 9)				//	����VCA����ģ��
#define VCA_CMD_SET_ALARM_LINK					(VCA_CMD_SET_MIN+10)				//	����VCA��������˿�
#define VCA_CMD_SET_SCHEDULE_ENABLE				(VCA_CMD_SET_MIN+11)				//	����VCA����ʹ��
#define VCA_CMD_SET_RULE9_FACEREC				(VCA_CMD_SET_MIN+12)				//	����VCA��������ʶ��
#define VCA_CMD_SET_RULE10_VIDEODETECT			(VCA_CMD_SET_MIN+13)				//	����VCA������Ƶ��ϣ�
#define VCA_CMD_SET_RULE8_ABANDUM		        (VCA_CMD_SET_MIN+14)				//	����VCA���������
#define VCA_CMD_SET_RULE7_OBJSTOLEN             (VCA_CMD_SET_MIN+15)                //  ����VCA���򣨱����
#define VCA_CMD_SET_RULE11_TRACK	            (VCA_CMD_SET_MIN+16)                //  ����VCA���򣨸��٣�
#define	VCA_CMD_CALL_TRACK_NO					(VCA_CMD_SET_MIN+17)				//  ������ʼ����λ
#define VCA_CMD_SET_RULE12_FLUXSTATISTIC	    (VCA_CMD_SET_MIN+18)				//	����VCA��������ͳ��)
#define VCA_CMD_SET_RULE13_CROWD				(VCA_CMD_SET_MIN+19)				//	����VCA������Ⱥ�ۼ�)
#define VCA_CMD_SET_CHANNEL_ENABLE				(VCA_CMD_SET_MIN+20)				//	����VCAͨ��ʹ��(0:���������ܷ��� 1:���ñ���ͨ�����ܷ��� 2:����ǰ��ͨ�����ܷ���)
#define VCA_CMD_SET_RULE14_LEAVE_DETECT			(VCA_CMD_SET_MIN+21)				//	����VCA������ڼ��)
#define VCA_CMD_SET_RULE15_WATER_LEVEL			(VCA_CMD_SET_MIN+22)				//	����VCA����ˮλ���)
#define VCA_CMD_SET_MAX							(VCA_CMD_SET_MIN+23)				//	VCA������������


#define VCA_CMD_GET_MIN							(16)								//	VCA��ȡ��������,Ϊ�˼���VIDEODETECT֮ǰ�ĺ�
#define VCA_CMD_GET_CHANNEL						(VCA_CMD_GET_MIN+ 0)				//	��ȡVCAͨ���ż�ʹ��
#define VCA_CMD_GET_VIDEOSIZE					(VCA_CMD_GET_MIN+ 1)				//	��ȡVCAͨ������Ƶ��С
#define VCA_CMD_GET_ADVANCE_PARAM				(VCA_CMD_GET_MIN+ 2)				//	��ȡVCA�߼�����
#define VCA_CMD_GET_TARGET_PARAM				(VCA_CMD_GET_MIN+ 3)				//	��ȡVCA���Ӳ�������ɫ����
#define VCA_CMD_GET_ALARM_STATISTIC				(VCA_CMD_GET_MIN+ 4)				//	��ȡVCA��������
#define VCA_CMD_GET_RULE_COMMON					(VCA_CMD_GET_MIN+ 5)				//	��ȡVCA���򳣹����
#define VCA_CMD_GET_RULE0_TRIPWIRE				(VCA_CMD_GET_MIN+ 6)				//	��ȡVCA���򣨵����ߣ�
#define VCA_CMD_GET_RULE1_DBTRIPWIRE			(VCA_CMD_GET_MIN+ 7)				//	��ȡVCA����˫���ߣ�
#define VCA_CMD_GET_RULE2_PERIMETER				(VCA_CMD_GET_MIN+ 8)				//	��ȡVCA�����ܽ磩
#define VCA_CMD_GET_ALARM_LINK					(VCA_CMD_GET_MIN+ 9)				//	��ȡVCA��������˿�
#define VCA_CMD_GET_ALARM_SCHEDULE				(VCA_CMD_GET_MIN+10)				//	��ȡVCA����ģ��
#define VCA_CMD_GET_SCHEDULE_ENABLE				(VCA_CMD_GET_MIN+11)				//	��ȡVCA����ʹ��
#define VCA_CMD_GET_RULE9_FACEREC				(VCA_CMD_GET_MIN+12)				//	��ȡVCA��������ʶ��
#define VCA_CMD_GET_RULE10_VIDEODETECT			(VCA_CMD_GET_MIN+13)				//	��ȡVCA������Ƶ��ϣ�
#define VCA_CMD_GET_RULE8_ABANDUM			    (VCA_CMD_GET_MIN+14)				//	����VCA���������
#define VCA_CMD_GET_RULE7_OBJSTOLEN             (VCA_CMD_GET_MIN+15)				//  ����VCA���򣨱����
#define VCA_CMD_GET_RULE11_TRACK	            (VCA_CMD_GET_MIN+16)                //  ����VCA���򣨸��٣�
#define VCA_CMD_GET_RULE12_FLUXSTATISTIC	    (VCA_CMD_GET_MIN+18)				//	����VCA��������ͳ��)
#define VCA_CMD_GET_RULE13_CROWD				(VCA_CMD_GET_MIN+19)				//	����VCA������Ⱥ�ۼ�)
#define VCA_CMD_GET_CHANNEL_ENABLE				(VCA_CMD_GET_MIN+20)				//	��ȡVCAͨ��ʹ��(0:���������ܷ��� 1:���ñ���ͨ�����ܷ��� 2:����ǰ��ͨ�����ܷ���)
#define VCA_CMD_GET_STATISTIC_INFO				(VCA_CMD_GET_MIN+21)				//  ��ȡVCAͨ��������ͳ������
#define VCA_CMD_GET_RULE14_LEAVE_DETECT			(VCA_CMD_GET_MIN+22)				//  ��ȡVCA������ڼ�⣩
#define VCA_CMD_GET_RULE15_WATER_LEVEL			(VCA_CMD_GET_MIN+23)				//	����VCA����ˮλ���)
#define VCA_CMD_GET_VCALIST						(VCA_CMD_GET_MIN+24)				//	Get VCA Type List
#define VCA_CMD_GET_MAX							(VCA_CMD_GET_MIN+25)				//	VCA��ȡ���������

#define VCA_CMD_FACEMOSAIC						(75)	//FACEMOSAIC
#define VCA_CMD_MIN 100
#define VCA_CMD_TARGET_PARAM					(VCA_CMD_MIN + 0)					//����VCA���Ӳ�������ɫ����
#define VCA_CMD_ADVANCE_PARAM					(VCA_CMD_MIN + 1)					//����VCA�߼�����
#define VCA_CMD_RULE_PARAM						(VCA_CMD_MIN + 2)					//����VCA���򳣹����
#define VCA_CMD_ALARM_STATISTIC					(VCA_CMD_MIN + 3)					//����VCA��������Ϊ��
#define VCA_CMD_TRIPWIRE						(VCA_CMD_MIN + 4)					//����VCA���򣨵����ߣ�
#define VCA_CMD_PERIMETER						(VCA_CMD_MIN + 5)					//����VCA�����ܽ磩
#define VCA_CMD_LINGER							(VCA_CMD_MIN + 6)					//����VCA�����ǻ���
#define VCA_CMD_PARKING							(VCA_CMD_MIN + 7)					//����VCA����ͣ����
#define VCA_CMD_RUNNING							(VCA_CMD_MIN + 8)					//����VCA���򣨱��ܣ�
#define VCA_CMD_CROWD							(VCA_CMD_MIN + 9)					//����VCA������Ⱥ�ۼ���
#define VCA_CMD_DERELICT						(VCA_CMD_MIN + 10)					//����VCA���������
#define VCA_CMD_STOLEN							(VCA_CMD_MIN + 11)					//����VCA���򣨱����
#define VCA_CMD_MULITTRIP						(VCA_CMD_MIN + 12)					//����VCA����˫���ߣ�
#define VCA_CMD_VIDEODIAGNOSE					(VCA_CMD_MIN + 13)					// set VCA rule (video diagnose for 300W)
#define VCA_CMD_AUDIODIAGNOSE					(VCA_CMD_MIN + 14)					// set VCA rule (audio diagnose for 300W)
#define VCA_CMD_TRIPWIRE_EX						(VCA_CMD_MIN + 15)					// set VCA rule (Tripwire for 300W)
#define VCA_CMD_RULE14_LEAVE_DETECT				(VCA_CMD_MIN + 16)					// set VCA rule (leave detect for 300w)
#define VCA_CMD_CHANNEL_ENABLE                  (VCA_CMD_MIN + 17)                  //ͨ��ʹ��
#define VCA_CMD_FACEREC							(VCA_CMD_MIN + 18)                  //����VCA��������ʶ��
#define VCA_CMD_TRACK							(VCA_CMD_MIN + 19)                  //����VCA���򣨹켣��
#define VCA_CMD_VIDEODETECTION                  (VCA_CMD_MIN + 20)                  //����VCA������Ƶ��ϣ�
#define VCA_CMD_VIDEOSIZE						(VCA_CMD_MIN + 21)                  //�������ܷ�����Ƶ�����С
#define VCA_CMD_RIVERCLEAN						(VCA_CMD_MIN + 22)					//����VCA���򣨺ӵ���⣩
#define VCA_CMD_DREDGE							(VCA_CMD_MIN + 23)					//����VCA���򣨵��ɵ�ж��
#define VCA_CMD_RIVERADV						(VCA_CMD_MIN + 24)					//����VCA�ӵ����߼�����
#define VCA_CMD_SCENEREV						(VCA_CMD_MIN + 25)					//���ܷ��������ָ�ʱ��
#define VCA_CMD_MAX								(VCA_CMD_MIN + 26)                  //���

#define VCA_ALARMLINK_MIN                        0
#define VCA_ALARMLINK_AUDIO	                    (VCA_ALARMLINK_MIN)                 //����������ʾ
#define VCA_ARARMLINK_SCREEMSHOW                (VCA_ALARMLINK_MIN + 1)             //������Ļ��ʾ
#define VCA_ALARMLINK_OUTPORT                   (VCA_ALARMLINK_MIN + 2)             //��������˿�
#define VCA_ALARMLINK_RECODER                   (VCA_ALARMLINK_MIN + 3)             //����¼��
#define VCA_ALARMLINK_PTZ                       (VCA_ALARMLINK_MIN + 4)             //����PTZ
#define VCA_ALARMLINK_CAPTUREPIC                (VCA_ALARMLINK_MIN + 5)             //����ץ��
#define VCA_ALARMLINK_SINGLEPIC					(VCA_ALARMLINK_MIN + 6)				//����������
#define VCA_ALARMLINK_WHITELIGHT				(VCA_ALARMLINK_MIN + 7)				//White Light
#define VCA_ALARMLINK_MAX                       (VCA_ALARMLINK_MIN + 8)

#define VCA_ALARMTYPE_MIN                       0         
#define VCA_ALARMTYPE_VIDEOLOST                 (VCA_ALARMTYPE_MIN)                 //��Ƶ��ʧ
#define VCA_ALARMTYPE_PORT                      (VCA_ALARMTYPE_MIN + 1)				//�˿�
#define VCA_ALARMTYPE_MOVE						(VCA_ALARMTYPE_MIN + 2)				//�ƶ����
#define VCA_ALARMTYPE_COVER						(VCA_ALARMTYPE_MIN + 3)				//��Ƶ�ڵ�
#define VCA_ALARMTYPE_VCA						(VCA_ALARMTYPE_MIN + 4)				//���ܷ���
#define VCA_ALARMTYPE_MAX						(VCA_ALARMTYPE_MIN + 5)

#define VCA_LINKPTZ_TYPE_MIN                    0
#define VCA_LINKPTZ_TYPE_NOLINK                 (VCA_LINKPTZ_TYPE_MIN )          //������
#define VCA_LINKPTZ_TYPE_PRESET                 (VCA_LINKPTZ_TYPE_MIN + 1)		 //Ԥ��λ
#define VCA_LINKPTZ_TYPE_TRACK                  (VCA_LINKPTZ_TYPE_MIN + 2)		 //�켣
#define VCA_LINKPTZ_TYPE_PATH                   (VCA_LINKPTZ_TYPE_MIN + 3)		 //·��
#define VCA_LINKPTZ_TYPE_MAX                    (VCA_LINKPTZ_TYPE_MIN + 4)

/************************************************************************/
/* ���ܷ�����Ƶ�������                                                 */
/************************************************************************/
#define	VCA_AVD_NOISE 				(1<<0)   	//	�������
#define VCA_AVD_CLARITY				(1<<1)		//	���������
#define VCA_AVD_BRIGHT_ABMNL		(1<<2)		//	�����쳣���
#define VCA_ADV_COLOR				(1<<3)		//	ƫɫ���
#define VCA_ADV_FREEZE				(1<<4)		//	���涳�����
#define VCA_ADV_NOSIGNAL			(1<<5)		//	�źŶ�ʧ���
#define VCA_ADV_CHANGE				(1<<6)		//	�����任���
#define VCA_ADV_INTERFERE			(1<<7)		//	��Ϊ�������
#define VCA_ADV_PTZ_LOST_CTL		(1<<8)		//	PTZʧ�����

/************************************************************************/
/* ���ܷ�����Ƶ�������                                                 */
/************************************************************************/
#define VCA_AUDIO_MIN                0
#define	VCA_AUDIO_DROP 				(1<<(VCA_AUDIO_MIN))	//	Voice Drop
#define VCA_AUDIO_UNNORMAL			(1<<(VCA_AUDIO_MIN + 1))//	Voice Unnormal
#define VCA_AUDIO_NOISE				(1<<(VCA_AUDIO_MIN + 2))//	Noise Restrain
#define VCA_AUDIO_ECHO				(1<<(VCA_AUDIO_MIN + 3))//	Echo Restrain
#define VCA_AUDIO_SIGNAL			(1<<(VCA_AUDIO_MIN + 4))//	Signal Unnormal
#define VCA_AUDIO_MAX               VCA_AUDIO_MIN + 5

//����
#define	MAX_BITSET_NUM		4
#define	MAX_ALARM_LINKTYPE  6

// typedef enum __tagLinkPTZType
// {
// 	LINKPTZ_TYPE_MIN = 0,
// 	LINKPTZ_TYPE_NOLINK = LINKPTZ_TYPE_MIN,			//������
// 	LINKPTZ_TYPE_PRESET,						//Ԥ��λ
// 	LINKPTZ_TYPE_TRACK,							//�켣
// 	LINKPTZ_TYPE_PATH							//·��
// }ELinkPTZType, *PELinkPTZType;	
typedef int ELinkPTZType;
typedef int __tagLinkPTZType;
typedef int * PELinkPTZType ;
#define LINKPTZ_TYPE_MIN	0
#define LINKPTZ_TYPE_NOLINK LINKPTZ_TYPE_MIN
#define LINKPTZ_TYPE_PRESET 1
#define LINKPTZ_TYPE_TRACK  2
#define LINKPTZ_TYPE_PATH	3

//ʱ���
typedef struct __tagAlarmScheduleParam
{
	int				iBuffSize;

	int				iWeekday;								//	�����յ���������0-6��
	int				iParam1;
	int				iParam2;
	NVS_SCHEDTIME	timeSeg[MAX_DAYS][MAX_TIMESEGMENT];		//	ʱ���

	void*			pvReserved;									
	int				iSceneID;//�������(0~15) 20140305��չ
}TAlarmScheduleParam, *PTAlarmScheduleParam;

//����ģ��ʹ��
typedef struct __tagAlarmScheEnableParam
{
	int				iBuffSize;

	int				iEnable;						//	����ģ��ʹ��
	int				iParam1;						//	ȡֵ��iAlarmType����
	int				iParam2;						//	ȡֵ��iAlarmType����
	int				iParam3;						//	����

	void*			pvReserved;									
	int				iSceneID;//�������(0~15) 20140305��չ
}TAlarmScheEnableParam, *PTAlarmScheEnableParam;


struct vca_TPoint
{
	int iX;
	int	iY;
} ;					//	sizeof = 2*4 = 8

struct vca_TLine
{
	vca_TPoint 	stStart;
	vca_TPoint 	stEnd;
} ;						//	sizeof = 2*8 = 16

struct vca_TPolygon
{
	int 		iPointNum;
	vca_TPoint 	stPoints[VCA_MAX_POLYGON_POINT_NUM];   
} ;						//	sizeof = 16*8+4 = 260 

struct vca_TPolygonEx
{
	int 		iPointNum;
	vca_TPoint 	stPoints[VCA_MAX_POLYGON_POINT_NUMEX];   
} ;						//	sizeof = 32*8+4 = 260

struct vca_TDisplayParam
{
	int				iDisplayRule;			//	�Ƿ���ʾ���򣬱�����ߣ��ܽ���	
	int				iDisplayStat;			//	�Ƿ���ʾ��������
	int				iColor;					//	����ʱ����ɫ
	int				iAlarmColor;			//	����ʱ����ɫ
};	
				
struct vca_TAlarmTimeSegment
{
	int		iStartHour;					
	int		iStartMinute;
	int		iStopHour;
	int		iStopMinute;
	int		iEnable;
};				//	����ʱ���

struct vca_TAlarmSchedule
{
	int						iWeekday;					//	�����յ���������0-6��
	vca_TAlarmTimeSegment	timeSeg[7][4];				//	ʱ���
};			//	��������ģ��

struct vca_TLinkPTZ
{
	unsigned short	usType;								//	0��������ͨ����1Ԥ��λ��2�켣��3·��
	unsigned short	usPresetNO;							//	Ԥ��λ��
	unsigned short	usTrackNO;							//	�켣��
	unsigned short	usPathNO;							//	·����
};

//	��������
struct vca_TAlarmLink
{
	int						iLinkType;					//	0������������ʾ��1��������Ļ��ʾ��2����������˿ڣ�3������¼��4������PTZ��5������ץ��
	int						iLinkChannel;
	
	int						iLinkSet[6];				//	0,1,2,3,5
	vca_TLinkPTZ			ptz[MAX_CHANNEL_NUM_TDWY];		//	4
};					

struct vca_TScheduleEnable
{
	int						iEnable;					//	�Ƿ�ʹ��	
	int						iParam;						//	����������
};

// �����߲���
struct vca_TTripwire
{
	int					iValid;					//	�Ƿ���Ч,�����ɹ����е�iEventID�ж��ĸ��¼���Ч���ɣ�
												//	��������й��򶼲���ЧʱiEventID�ܻ�ָ��һ���¼����ϲ��޷��ж��Ƿ������Ч��ֻ�ܼ�һ���¼���Ч�ֶ�
	int					iTargetTypeCheck;		//	Ŀ����������
	int					iMinDistance;			//	��С����,Ŀ���˶����������ڴ���ֵ
	int					iMinTime;				//	���ʱ��,Ŀ���˶����������ڴ���ֵ	
	int					iType;					//	��ʾ��Խ����
	int					iDirection;				//	�����߽�ֹ����Ƕ�
	vca_TLine			stLine;					//	��������
}; 

struct vca_TPerimeter
{
	int					iValid;					//	�Ƿ���Ч,�����ɹ����е�iEventID�ж��ĸ��¼���Ч���ɣ�
												//	��������й��򶼲���ЧʱiEventID�ܻ�ָ��һ���¼����ϲ��޷��ж��Ƿ������Ч��ֻ�ܼ�һ���¼���Ч�ֶ�
	int					iTargetTypeCheck;		//	�����˳�
	int					iMode;					//	���ģʽ
	int 				iMinDistance;			//	��С����
	int 				iMinTime;				//	���ʱ��		
	int					iType;					//	�Ƿ�����������
	int 				iDirection;				//	��ֹ����Ƕ�(��λ: �Ƕ�)
	vca_TPolygon		stRegion;				//	����Χ	
} ;		// �ܽ����

struct vca_TFaceRec
{
	int					iValid;					//	�Ƿ���Ч,�����ɹ����е�iEventID�ж��ĸ��¼���Ч���ɣ�
												//	��������й��򶼲���ЧʱiEventID�ܻ�ָ��һ���¼����ϲ��޷��ж��Ƿ������Ч��ֻ�ܼ�һ���¼���Ч�ֶ�
	vca_TPolygon		stRegion;				//	����Χ
};		//����ʶ�����

struct vca_TVideoDetection
{
	int					iValid;					//	�Ƿ���Ч,�����ɹ����е�iEventID�ж��ĸ��¼���Ч���ɣ�
												//	��������й��򶼲���ЧʱiEventID�ܻ�ָ��һ���¼����ϲ��޷��ж��Ƿ������Ч��ֻ�ܼ�һ���¼���Ч�ֶ�
	int					m_iCheckTime;
};

struct vca_tOSC
{
	int                 iValid;					//	�Ƿ���Ч,�����ɹ����е�iEventID�ж��ĸ��¼���Ч���ɣ�
	int                 iColor;					//������ɫ
	int					iAlarmColor;			//����ʱ������ɫ
	int					iMinTime;				//��������������ڵ����ʱ��
	int 				iMinSize;				//��С���سߴ�
	int 				iMaxSize;				//������سߴ�
	int 				iSunRegionCount;		//�Ӷ���θ���
	vca_TPolygon 		stMainRegion;			//�����������
	vca_TPolygon 		stSubRegion[VCA_MAX_OSC_REGION_NUM];	//�Ӷ��������
};

typedef	struct vca_tTrack
{
	int					m_iStartPreset;			//��ʼ���ٵ�
	vca_TPolygon		m_TrackRegion;			//��������
	int					m_iScene;				//������/��/С
	int					m_iWeekDay;				//����
	NVS_SCHEDTIME		m_timeSeg[MAX_DAYS][MAX_TIMESEGMENT];		//����ʱ���
}vca_tTrack, *LPvca_tTrack;

typedef	struct vca_tFluxStatistic
{
	int					iValid;					//	�Ƿ���Ч,�����ɹ����е�iEventID�ж��ĸ��¼���Ч���ɣ�
												//	��������й��򶼲���ЧʱiEventID�ܻ�ָ��һ���¼����ϲ��޷��ж��Ƿ������Ч��ֻ�ܼ�һ���¼���Ч�ֶ�
	vca_TPolygon		stRegion;				//	����Χ	
}vca_tFluxStatistic, *LPvca_tFluxStatistic;

typedef	struct vca_tCrowd
{
	int					iValid;					//	�Ƿ���Ч,�����ɹ����е�iEventID�ж��ĸ��¼���Ч���ɣ�
												//	��������й��򶼲���ЧʱiEventID�ܻ�ָ��һ���¼����ϲ��޷��ж��Ƿ������Ч��ֻ�ܼ�һ���¼���Ч�ֶ�
	vca_TPolygon		stRegion;				//	����Χ	
}vca_tCrowd, *LPvca_tCrowd;

#define  MAX_RULE_REGION_NUM	4
typedef struct vca_tLeaveDetect
{
	int					iValid;					//	���¼�����Ƿ���Ч,0����Ч 1����Ч
	int					iLeaveAlarmTime;		//	��ڱ���ʱ��,60~3600,��λ���� ,Ĭ��ֵ��120S
	int					iRuturnClearAlarmTime;	//	�ظ�����ʱ��,10~300,��λ����   Ĭ��ֵ��15S
	int					iAreaNum;				//	������������,1��4
	vca_TPolygon		stRegion[MAX_RULE_REGION_NUM];			//	����Χ	
}vca_tLeaveDetect, *LPvca_tLeaveDetect;

//��ˮλ��⣩RULEEVENT15
#define MAX_REFERPOINT_NUM   10
typedef struct tagWaterRange //ˮλ��Χ
{
	int iStart;	//��ʼ��Χ
	int iEnd;    //������Χ
}WaterRange;

typedef struct tagReferPoint //ˮλ�ο�
{
	int iPointX;	//������
	int iPointY;    //������
	int iValue;		//����ϵĶ���
}ReferPoint;

typedef struct tagWaterPresetInfo
{
	int                 iSize;
	int					iPresetId;					//	Ԥ��λID
	WaterRange			stGaugeRange;				//	ˮλ��Χ iStart:iEnd
	RECT				rcGaugeRect;				//	ˮ������Խ�������
	RECT				rcAssistRect;				//  ��������Խ�������
	RECT				rcGaugeLine;				//	ˮ�߲ο�������
	int					iReferNum;					//  �ο������
	ReferPoint			stReferPoint[MAX_REFERPOINT_NUM]; //  �ο���1��2, 3
}WaterPresetInfo;

#define	EVENT_BUF_RESERVED		2048			//Ԥ����С
#define MAX_WATER_SAMPLE_POINT 4
typedef union vca_UEventParam
{	
	char cBuf[EVENT_BUF_RESERVED];							//	������Ԥ���ռ��Ϊ2048
	vca_TTripwire		tripwire;
	vca_TPerimeter		perimeter;
	vca_TFaceRec		m_stFaceRec;
	vca_TVideoDetection	m_stVideoDetection;
	vca_tOSC            m_stOSC;
	vca_tTrack			m_stTrack;
	vca_tFluxStatistic  m_stFluxStatic;
	vca_tCrowd          m_stCrowd;
	vca_tLeaveDetect	m_stLeaveDetect;
}vca_UEventParam, *LPvca_UEventParam;
	
// �������ò���
struct vca_TRuleParam
{
	char 				cRuleName[VCA_MAX_RULE_NAME_LEN];	// ��������
	int					iValid;								// �ù����Ƿ���Ч

	vca_TDisplayParam	stDisplayParam;						//	��ʾʱ�Ĳ����趨

	int					iAlarmCount;						//	��������
	vca_TAlarmSchedule	alarmSchedule;						//	��������ģ�����
	vca_TAlarmLink		alarmLink;							//	�����������ò���
	vca_TScheduleEnable	scheduleEnable;						//	����ʹ��

	vca_EEventType		iEventID;							// ��Ϊ�����¼����� 
	vca_UEventParam		events;								// ��Ϊ�����¼�����
};	

// ���ܷ������ò���
struct vca_TConfig
{
	int		iVideoSize;						// ��Ƶ��С
	int		iDisplayTarget;					// �Ƿ����Ŀ���
	int		iDisplayTrace;					// �Ƿ���ӹ켣
	int		iTargetColor;					// Ŀ�����ɫ
	int		iTargetAlarmColor;				// ����ʱĿ�����ɫ
	int		iTraceLength;					// �켣����
};	  

// ���ܷ����߼�����
struct vca_TAdvParam
{
	int		iEnable; 					//�Ƿ����ø߼�����
	int		iTargetMinSize; 			//Ŀ����С������
	int		iTargetMaxSize; 			//Ŀ�����������
	int   	iTargetMinWidth;  			//�����
	int		iTargetMaxWidth;  			//��С���
	int		iTargetMinHeight;  			//���߶�
	int		iTargetMaxHeight;  			//��С�߶�
	int     iTargetMinSpeed;			//Ŀ����С�����ٶ�(-1Ϊ��������)
	int     iTargetMaxSpeed;			//Ŀ����������ٶ�(-1Ϊ��������)
	int 	iTargetMinWHRatio;			//��С�����
	int 	iTargetMaxWHRatio;			//��󳤿��
	int		iSensitivity;				//�����ȼ���
};	

struct TStatisticInfo
{
	int					iSize;								//�ṹ���С
	int					iFluxIn;							//��������
	int					iFluxOut;							//�뿪����
};

//���ܷ���ͨ������
struct vca_TVCAChannelParam
{
	int					iEnable;					//	�Ƿ�ʹ�ܴ�ͨ�����ܷ���
	vca_TConfig			config;  					//	���ܷ������ò���    	
	vca_TAdvParam		advParam;					//	���ܷ����߼�����

	int					iRuleID;
	vca_TRuleParam	rule[VCA_MAX_RULE_NUM]; 		//	�������ò���	
};

//	VCA���õĲ����ܼ�, 
struct vca_TVCAParam
{
	int						iEnable;								//	�Ƿ�ʹ�����ܷ���
	int						iChannelBit;							//	���ܷ���ͨ����Bit��

	int						iChannelID;								//	VCAͨ��
	vca_TVCAChannelParam	chnParam[MAX_CHANNEL_NUM_TDWY];				//	ÿ��ͨ���Ĳ�������
};					

struct vca_TAlarmInfo
{
	int				iID;				//	������ϢID�����ڻ�ȡ�������Ϣ
	int				iChannel;			//	ͨ����
	int				iState;				//	����״̬
	int				iEventType;			//	�¼�����  0�������� 1��˫���� 2���ܽ��� 3���ǻ� 4��ͣ�� 5������ 
													//6����������Ա�ܶ� 7�������� 8�������� 9������ʶ�� 10:��Ƶ��� 
													//11:���ܸ��� 12������ͳ�� 13����Ⱥ�ۼ�  14����ڼ�� 15����Ƶ���
	int				iRuleID;			//	����ID

	unsigned int	uiTargetID;			//	Ŀ��ID
	int				iTargetType;		//	Ŀ������
	RECT			rctTarget;			//	Ŀ��λ��
	int				iTargetSpeed;		//	Ŀ���ٶ�
	int				iTargetDirection;	//	Ŀ�귽��
	int             iPresetNo;    //  Ԥ��λID
	unsigned int	iWaterLevelNUm;		//  ˮ���߶���
	char			cWaterPicName[LEN_64];//  ����ͼƬ·�� 
	int             iPicType;             // 0:�ؼ� 1����ͨ
	int             iDataType;            // 0:ʵʱ 1���ѻ�
};

/************************************************************************
* FTP ��ʱ�������                                                                     
************************************************************************/

//	FTP���Э��
#define FTP_CMD_SET_MIN				0
#define FTP_CMD_SET_SNAPSHOT		(FTP_CMD_SET_MIN+0)
#define FTP_CMD_SET_LINKSEND		(FTP_CMD_SET_MIN+1)
#define FTP_CMD_SET_TIMEDSEND		(FTP_CMD_SET_MIN+2)
#define FTP_CMD_SET_UPDATE			(FTP_CMD_SET_MIN+3)
#define FTP_CMD_SET_MAX				(FTP_CMD_SET_MIN+4)

//#define FTP_CMD_GET_MIN				(FTP_CMD_SET_MAX)		//	�����������չ������
#define FTP_CMD_GET_MIN				(4)
#define FTP_CMD_GET_SNAPSHOT		(FTP_CMD_GET_MIN+0)
#define FTP_CMD_GET_LINKSEND		(FTP_CMD_GET_MIN+1)
#define FTP_CMD_GET_TIMEDSEND		(FTP_CMD_GET_MIN+2)
#define FTP_CMD_GET_UPDATE			(FTP_CMD_GET_MIN+3)
#define FTP_CMD_GET_MAX				(FTP_CMD_GET_MIN+4)

#define FTP_COMMON_CMD_MIN			(FTP_CMD_GET_MAX)
#define FTP_COMMON_CMD_SNAPSHOT_EX	(FTP_COMMON_CMD_MIN + 0)
#define FTP_COMMON_CMD_MAX			(FTP_COMMON_CMD_MIN + 1)

struct FTP_SNAPSHOT
{
	int		iChannel;			//	ͨ�� 
	int		iEnable;			//	ʹ��
	int		iQValue;			//	����
	int		iInterval;			//	ʱ����
	int		iPictureSize;       //  ץ��ͼƬ��С	0x7fff�������Զ��������Ӧ�ֱ��ʴ�С
	int		iPicCount;			//  ץ��ͼƬ����
};

typedef struct tagFtpSnapshot
{
	int		iSize;
	int		iChannelNo;			//	channel number 
	int		iType;			
	int		iQValue;			//	quality[0-100]
	char	cInterval[LEN_16];	//	capture time interval [0,3600]
	int		iPictureSize;       //  capture picture size	0x7fff��automatic
	int		iPicCount;			//  capture picture number
}FtpSnapshot, *pFtpSnapshot;

struct FTP_LINKSEND
{
	int		iChannel;			//	ͨ��
	int		iEnable;			//	ʹ��
	int		iMethod;			//	��ʽ
};

struct FTP_TIMEDSEND	
{
	int		iChannel;			//	ͨ��
	int		iEnable;			//	ʹ��
	int		iFileType;			//	�ļ�����
	int		iHour;				//	ʱ��(hour)
	int		iMinute;			//	ʱ��(Minute)
	int		iSecond;			//	����(Second)
};

struct FTP_UPDATE
{
	char	cFtpAddr[32];		//	����ʹ�õ�Ftp��ַ
	char	cFileName[32];		//	����ʹ�õ��ļ���
	char	cUserName[16];		//	����ʹ�õ��û���
	char	cPassword[16];		//	����ʹ�õ�����
	int		iPort;				//	�˿�
	int		iUsage;				//	��־λ��;
};

/************************************************************************
* 3G DVR �������
************************************************************************/
#define		DVR3G_CMD_SET_MIN					0
#define		DVR3G_CMD_SET_POWERDELAY			(DVR3G_CMD_SET_MIN+0)				//	��ʱ���ػ�
#define		DVR3G_CMD_SET_SIMNUM				(DVR3G_CMD_SET_MIN+1)				//	SIM����
#define		DVR3G_CMD_SET_GPSOVERLAY			(DVR3G_CMD_SET_MIN+2)				//	GPS��Ϣ
#define		DVR3G_CMD_SET_GPSFILTER				(DVR3G_CMD_SET_MIN+3)				//	GPS����
#define		DVR3G_CMD_SET_FTPUPLOAD_MODE		(DVR3G_CMD_SET_MIN+4)				//	Ftp�Զ��ϴ��ķ�ʽ
#define		DVR3G_CMD_SET_VPDN					(DVR3G_CMD_GET_MIN+5)				//	VPDN
#define		DVR3G_CMD_SET_MAX					(DVR3G_CMD_SET_MIN+6)

//#define		DVR3G_CMD_GET_MIN					(DVR3G_CMD_SET_MAX)					//	�����������չ������
#define		DVR3G_CMD_GET_MIN					(5)									//  Get��������
#define		DVR3G_CMD_GET_POWERDELAY			(DVR3G_CMD_GET_MIN+0)				//	��ʱ���ػ�
#define		DVR3G_CMD_GET_SIMNUM				(DVR3G_CMD_GET_MIN+1)				//	SIM����
#define		DVR3G_CMD_GET_GPSOVERLAY			(DVR3G_CMD_GET_MIN+2)				//	GPS��Ϣ
#define		DVR3G_CMD_GET_GPSFILTER				(DVR3G_CMD_GET_MIN+3)				//	GPS����
#define		DVR3G_CMD_GET_FTPUPLOAD_MODE		(DVR3G_CMD_GET_MIN+4)				//	Ftp�Զ��ϴ��ķ�ʽ
#define		DVR3G_CMD_GET_VPDN					(DVR3G_CMD_GET_MIN+5)				//	VPDN
#define		DVR3G_CMD_GET_MAX					(DVR3G_CMD_GET_MIN+6)

struct TDVR3GInfo
{
	int		iPowerDelayOnTime;						//	��ʱ������ʱ�䣬��������Χ0~999�룬 0��ʾ��ʱ������ʹ�ܡ�
	int		iPowerDelayOffTime;						//	��ʱ�ػ���ʱ�䣬������ ��Χ0~999�룬0��ʾ��ʱ�ػ���ʹ�ܡ�	
	int		iGpsOverlay[MAX_CHANNEL_NUM_TDWY];			//	ʹ�ܵ���GPS��Ϣ����ͨ�����
	int		iGpsFilter;								//	bitset, 0-��ת��, 65535-����. GPGGA-bit 1��GPRMC-bit 2��GPVTG-bit 3��GPGLL-bit 4��GPGSA- bit 5, GPGSV-bit 6������λ��ʱ������
	char	cSimNum[33];							//	SIM���ţ������Ϊ32λ��
};

struct TDVR3GInfoEx
{
	TDVR3GInfo	dvr3GInfo;
	int			iFtpUploadMode;					//	�ϴ���ʽ
	char		cReserved[64];					//	Ԥ���ֶ�
};

#define DVR3G_VPDN_MAX_LEN 32

struct TDVR3GVPDN
{
	int iSize;
	char cDialNumber[DVR3G_VPDN_MAX_LEN];	//�������
	char cAccount[DVR3G_VPDN_MAX_LEN];		//�˺�
	char cPassword[DVR3G_VPDN_MAX_LEN];		//����
};

/************************************************************************
* �໭��ϳ�                                                                     
************************************************************************/
struct TVideoCombine 
{
	int     iPicNum;     		// �ϳɻ�����
	int		iRec;  				// �ϳ�λ��
	char	cChannelID[300];   	// �ϳ�ͨ����Ϣ
};

#define MAX_VIDEO_COMBINE_NUM		2      //��Ƶ�ϳ�ÿ�������Ŀ
#define MAX_VIDEO_COMBINE_BLOCK     4	   //��Ƶ�ϳ����ֶ���
#define MAX_VO_NUM					3	   //Max Video Output Num
#define MAX_VC_NUM					2 	   //Max Video Combine Num
typedef struct tagVideoCombine 
{
	int		iSize;				// the size of Struct
	int     iPicNum;     		// combine picture number
	int		iRec;  				// combine location
	char	cChannelID[300];   	// combine information
	int		iVoDevId;			// input device number, 0-reserved��1-VGA/HDMI0, 17-HDMI1, 33-CVBS 49-VC, default is VC
}VideoCombine, *pVideoCombine;


typedef struct tagRectEx
{
	int 			iX;
	int				iY;					// coordinates
	int				iWidth;
	int				iHeight;			// width and height
}RectEx, *pRectEx;

typedef struct tagPicParam
{
	int				ChnNo;				// Channel No
	tagRectEx		RectParam;
}PicParam, *pPicParam;

#define  MAX_VIEW_NUM				128		//Max View Number

//Create Free
typedef struct tagVoViewSegment
{
	int				iSize;				// the size of Struct
	int    	 		iVoDevId;     		// output Device ID
	int				iPicCnt;  		    // Picture Count= iPicCnt/1000 - 1
	tagPicParam		tPicPar[MAX_VIEW_NUM];
}VoViewSegment,*pVoViewSegment;

//VC Affinal
typedef struct tagVcAffinal 
{
	int				iSize;				// the size of Struct
	int             iVcId;	            // vcid
	int    	 		iVoDevId;     		// output Device ID
}VcAffinal,*pVcAffinal;

//Preview Mode  
typedef struct tagPreviewMode  
{
	int				iSize;				// the size of Struct
	int				iType;  			// Video Input Type
}PreviewMode, *pPreviewMode;

/************************************************************************
* ����                                                                     
************************************************************************/
#define MAX_MIX_NUM 300
typedef struct __tagAudioMix
{	
	int		iEnable;					//����ʹ��
	int		iChannel;					//��Ƶ�ϳ�ͨ����ͨ����
	char	cMixChannels[MAX_MIX_NUM];	//��Դ���
	void*	pvReserved;					//��������,ͬʱҲ��Ϊ����TVideoCombine�Ĵ�С����
}TAudioMix,	*PTAudioMix;


/************************************************************************
* SIPЭ�����                                                                     
************************************************************************/
#define SIP_CMD_GET_MIN						0
#define SIP_CMD_GET_ALARMCHANNEL			(SIP_CMD_GET_MIN+0)
#define SIP_CMD_GET_VIDEOCHANNEL			(SIP_CMD_GET_MIN+1)
#define SIP_CMD_GET_MAX						(SIP_CMD_GET_MIN+2)

//#define SIP_CMD_SET_MIN						(SIP_CMD_GET_MAX)		//	�����������չ������
#define SIP_CMD_SET_MIN						(2)
#define SIP_CMD_SET_ALARMCHANNEL			(SIP_CMD_SET_MIN+0)
#define SIP_CMD_SET_VIDEOCHANNEL			(SIP_CMD_SET_MIN+1)
#define SIP_CMD_SET_MAX						(SIP_CMD_SET_MIN+2)

struct TSipVideoChannel
{
	int		iChannelNo;
	char	cChannelID[33];
	int		iLevel;
	int		iPtzTime;
};

struct TSipAlarmChannel
{
	int		iChannelNo;
	char	cChannelID[33];
	int		iLevel;
};

#define CHARSET_LENGTH          32              //  ��ʾ�ַ����ַ�������
#define LANGUAGE_COUNT			255				//	���֧�ֵ����Ը���
#define LANGUAGE_NAME_LENGTH	32				//	�����������Ƶ���󳤶�
typedef struct
{
	int    iCount;
	char   cLanguage[LANGUAGE_COUNT][LANGUAGE_NAME_LENGTH];
} st_NVSLanguageList;

typedef struct
{
	unsigned long nWidth;
	unsigned long nHeight;
	unsigned long nStamp;
	unsigned long nType;
	unsigned long nFrameRate;
	unsigned long nReserved;
}PSPACK_INFO;

//2012-04-22
/************************************************************************
* �������ݻص��ṹ�����                                                                     
************************************************************************/
#define 	DTYPE_MIN			0
#define 	DTYPE_H264_MP		(DTYPE_MIN + 0)
#define     DTYPE_RAW_AUDIO		(DTYPE_MIN + 1)
#define 	DTYPE_PS			(DTYPE_MIN + 2)
#define 	DTYPE_MAX			(DTYPE_MIN + 2)

typedef void (*RAWH264_MP_NOTIFY)(unsigned int _ulID, unsigned char* _cData, 
								  int _iLen, RAWFRAME_INFO *_pRawFrameInfo, void* _iUser);

typedef void (*RAW_AUDIO_NOTIFY)(unsigned int _ulID, unsigned char* _cData, int _iLen, int Type, void* _pvUserData);
//PS���ص��봿��Ƶ�ص�һ��
typedef RAW_AUDIO_NOTIFY	PS_STREAM_NOTIFY;

//PS����װ�Ļص����� 
typedef void (*PSPACK_NOTIFY)(unsigned long _ulID, unsigned char* _ucData, int _iSize, PSPACK_INFO* _pPsPackInfo, void* _pContext);

/************************************************************************/
/*                   �����ݻص�                                         */
/************************************************************************/

//������Ϣ�ص�
typedef void (*ALARM_NOTIFY_V4)(int _ulLogonID, int _iChan, int _iAlarmState,ALARMTYPE _iAlarmType,void* _iUser);

//�豸�����ı�ص�   
typedef void (*PARACHANGE_NOTIFY_V4)(int _ulLogonID, int _iChan, PARATYPE _iParaType,STR_Para* _strPara,void* _iUser);

//���ص�����,����֪ͨ�豸�����ߡ���Ƶ���ݵ������Ϣ��_iWparam����WCM_LOGON_NOTIFY����Ϣ����
typedef void (*MAIN_NOTIFY_V4)(int _ulLogonID,int _iWparam, int _iLParam,void* _iUser); 

//���������ص�
typedef void (*COMRECV_NOTIFY_V4)(int _ulLogonID, char *_cData, int _iLen,int _iComNo,void* _iUser); 

//����������Ƶ����
typedef void (*DECYUV_NOTIFY_V4)(unsigned int _ulID,unsigned char *_cData, int _iLen, const FRAME_INFO *_pFrameInfo, void* _iUser);

#ifndef WIN32
#define HDC int
#endif

typedef int (*CBK_DRAW_FUNC)(long _lHandle,HDC _hDc,long _lUserData);


#define UPGRADE_PROGRAM			1		//	��������
#define UPGRADE_WEB				2		//	��ҳ����

typedef void (*UPGRADE_NOTIFY_V4)(int _iLogonID, int _iServerState, void* _iUserData);


typedef void (__stdcall *RECVDATA_NOTIFY)(unsigned long _ulID,unsigned char* _ucData,int _iLen);		//	������ʹ�ô˻ص�����
typedef void (__stdcall *RECVDATA_NOTIFY_EX)(unsigned long _ulID, unsigned char* _ucData,int _iLen, int _iFlag, void* _lpUserData);

#ifdef WIN32
typedef void (__stdcall *LOGON_NOTIFY)(int _iLogonID, int _iLogonState);
typedef void (__stdcall *ALARM_NOTIFY)(int _iLogonID, int _iCh, void* _vContext,ALARMTYPE _iAlarmType);
typedef void (__stdcall *PARACHANGE_NOTIFY)(int _iLogonID, int _iCh, PARATYPE _iParaType,LPVOID _strPara);
typedef void (__stdcall *MAIN_NOTIFY)(int _iLogonID,int _iWParam,int _iLParam, void* _lpNoitfyUserData);
typedef void (__stdcall *ALARM_NOTIFY_EX)(int _iLogonID, int _iChannel, void* _vContext,ALARMTYPE _iAlarmType, void* _lpNoitfyUserData);
typedef void (__stdcall *PARACHANGE_NOTIFY_EX)(int _iLogonID, int _iChannel, PARATYPE _iParaType,LPVOID _strPara, void* _lpNoitfyUserData);
typedef void (__stdcall *COMRECV_NOTIFY)(int _iLogonID, char *_buf, int _length,int _iComNo);
typedef void (__stdcall *DECYUV_NOTIFY)(unsigned long _ulID, unsigned char* _ucData, int _iSize, FRAME_INFO *_pFrameInfo, void* _pContext);
//�����ں˳���
typedef void (__stdcall *PROUPGRADE_NOTIFY)(int _iLogonID,int _iServerState);
//������ҳ
typedef void (__stdcall *WEBUPGRADE_NOTIFY)(int _iLogonID,int _iServerState);    
#else
typedef void (*ALARM_NOTIFY)(int _ulLogonID, int _iChan, int _iAlarmState,ALARMTYPE _iAlarmType,void* _iUser);
typedef void (*PARACHANGE_NOTIFY)(int _ulLogonID, int _iChan, PARATYPE _iParaType,STR_Para* _strPara,void* _iUser);
typedef void (*MAIN_NOTIFY)(int _ulLogonID,int _iWparam, int _iLParam,void* _iUser); 
typedef void (*COMRECV_NOTIFY)(int _ulLogonID, char *_cData, int _iLen,int _iComNo,void* _iUser);
typedef void (*DECYUV_NOTIFY)(unsigned int _ulID,unsigned char *_cData, int _iLen, const FRAME_INFO *_pFrameInfo, void* _iUser);
//�����ں˳���
typedef void (*PROUPGRADE_NOTIFY)(int _iLogonID,int _iServerState,void* _iUser);
//������ҳ
typedef void (*WEBUPGRADE_NOTIFY)(int _iLogonID,int _iServerState,void* _iUser);    
#endif

//ͨ����ش洢����
#define STORAGE_CMD_MIN 0
#define STORAGE_CMD_STORAGE_RULE STORAGE_CMD_MIN+0
#define STORAGE_CMD_MAX STORAGE_CMD_MIN+1

#define STORAGE_RULE_MIN 0
#define STORAGE_RULE_RECORD_AUDIO STORAGE_RULE_MIN+0
#define STORAGE_RULE_STORAGE_TIME STORAGE_RULE_MIN+1
#define STORAGE_RULE_EXTRACT_FRAME STORAGE_RULE_MIN+2
#define STORAGE_RULE_REDUNDANCE_RECORD STORAGE_RULE_MIN+3
#define STORAGE_RULE_SUB_RECORD STORAGE_RULE_MIN+4
#define STORAGE_RULE_SUB_STORAGE_TIME STORAGE_RULE_MIN+5
#define STORAGE_RULE_MAX STORAGE_RULE_MIN+6
typedef struct STORAGE_RULE
{
	int			iSize;		//Size of the structure,must be initialized before used
	int			iType;		//�������� 0:�Ƿ��¼��Ƶ 1:����ʱ��(��λ:��) 2:�Ƿ�����֡���� 3���Ƿ�������¼��
	int			iValue;		//����ֵ iType = 0��2��3ʱ��0:����¼�򲻿��� 1:��¼����;
							//iType = 1ʱ����ΧΪ[0,60]��0��ʾ��ǿ��ִ�й��ڲ������������󸲸ǡ�
}*PSTORAGE_RULE;


//************************************************************************/
//*							��������
//************************************************************************/
#define MAX_LAN_NUM						2
#define MAX_WIFICARD_NUM				2

#define LAN_CMD_SET_MIN					0
#define LAN_CMD_SET_IPV4				(LAN_CMD_SET_MIN+0)
#define LAN_CMD_SET_IPV6				(LAN_CMD_SET_MIN+1)
#define LAN_CMD_SET_WORKMODE			(LAN_CMD_SET_MIN+2)
//#define LAN_CMD_SET_LANNUM			(LAN_CMD_SET_MIN+3)
#define LAN_CMD_SET_DHCP				(LAN_CMD_SET_MIN+4)
#define LAN_CMD_SET_WIFIDHCPMODE		(LAN_CMD_SET_MIN+5)
#define LAN_CMD_SET_WIFIWORKMODE		(LAN_CMD_SET_MIN+6)
#define LAN_CMD_SET_WIFIAPDHCPPARA		(LAN_CMD_SET_MIN+7)
#define LAN_CMD_SET_WIFIAPPARA			(LAN_CMD_SET_MIN+8)
#define LAN_CMD_SET_WIFIPARA			(LAN_CMD_SET_MIN+9)
#define LAN_CMD_SET_MAX					(LAN_CMD_SET_MIN+10)

#define LAN_CMD_GET_MIN					0
#define LAN_CMD_GET_IPV4				(LAN_CMD_GET_MIN+0)
#define LAN_CMD_GET_IPV6				(LAN_CMD_GET_MIN+1)
#define LAN_CMD_GET_WORKMODE			(LAN_CMD_GET_MIN+2)
#define LAN_CMD_GET_LANNUM				(LAN_CMD_GET_MIN+3)
#define LAN_CMD_GET_DHCP				(LAN_CMD_GET_MIN+4)
#define LAN_CMD_GET_WIFIDHCPMODE		(LAN_CMD_GET_MIN+5)
#define LAN_CMD_GET_WIFIWORKMODE		(LAN_CMD_GET_MIN+6)
#define LAN_CMD_GET_WIFIAPDHCPPARA		(LAN_CMD_GET_MIN+7)
#define LAN_CMD_GET_WIFIAPPARA			(LAN_CMD_GET_MIN+8)
#define LAN_CMD_GET_WIFIPARA			(LAN_CMD_GET_MIN+9)
#define LAN_CMD_GET_MAX					(LAN_CMD_GET_MIN+10)

typedef struct LANPARAM_IPV4 
{
	int iSize;						//sizeof(LANPARAM_IPV4)
	int iLanNo;						//������� 0:Lan1 1:Lan2	
	int iLanType;					//�������� Ԥ������0���ɡ�
	//Ĭ��10M/100M/1000M����Ӧ�����ɸ��ġ�
	char cIP[LENGTH_IPV4];			//IPV4 IP��ַ			
	char cMask[LENGTH_IPV4];		//IPV4 ��������
	char cGateway[LENGTH_IPV4];		//IPV4 Ĭ������
	char cDNS[LENGTH_IPV4];			//IPV4 ��ѡDNS������
	char cBackDNS[LENGTH_IPV4];		//IPV4 ����DNS������
	char cReserved[64];				//Ԥ������0���ɡ�
	//�����Ժ������������Ե���չ������MTU					
} *PLANPARAM_IPV4;

typedef struct LANPARAM_IPV6 
{
	int iSize;						//sizeof(LANPARAM_IPV6)
	int iLanNo;						//������� 0:Lan1 1:Lan2	
	int iPrefixLen;					//IPV6 ����ǰ׺����
	char cIP[LENGTH_IPV6];			//IPV6 IP��ַ	
	char cGateway[LENGTH_IPV6];		//IPV6 Ĭ������
	char cDNS[LENGTH_IPV6];			//IPV6 ��ѡDNS������
	char cBackDNS[LENGTH_IPV6];		//IPV6 ����DNS������
	char cReserved[LEN_64];				//Ԥ������0���ɡ�
	//����IPV6��صĻ������Կ���ͨ�����ֶ�ʵ�֡�					
} *PLANPARAM_IPV6;

typedef struct LANPARAM_IPV6_V1 
{
	int iSize;						//sizeof(LANPARAM_IPV6)
	int iLanNo;						//������� 0:Lan1 1:Lan2	
	int iPrefixLen;					//IPV6 ����ǰ׺����
	char cIP[LENGTH_IPV6_V1];			//IPV6 IP��ַ	
	char cGateway[LENGTH_IPV6_V1];		//IPV6 Ĭ������
	char cDNS[LENGTH_IPV6_V1];			//IPV6 ��ѡDNS������
	char cBackDNS[LENGTH_IPV6_V1];		//IPV6 ����DNS������
	char cReserved[LEN_64];				//Ԥ������0���ɡ�
	//����IPV6��صĻ������Կ���ͨ�����ֶ�ʵ�֡�					
} *PLANPARAM_IPV6_V1;

typedef struct LANPARAM_WORKMODE 
{
	int iSize;						//sizeof(LANPARAM_WORKMODE)
	int iWorkMode;					//����ģʽ	0:���ؾ��� 1:�����ݴ� 2:��ַ�趨					
	int iMainLanNo;					//��������� 0:Lan1 1:Lan2					
} *PLANPARAM_WORKMODE;

typedef struct LANPARAM_DHCP 
{
	int iSize;						//sizeof(LANPARAM_WORKMODE)
	int iLanNo;						//������� 0:Lan1 1:Lan2	
	int iEnable;					//DHCPʹ�� 0:ͣ�� 1:���� 
} *PLANPARAM_DHCP;
//add wifi&wifiap
typedef struct WIFIPARAM_DHCP       //cb@120711
{
	int iSize;						//sizeof(WIFIPARAM_DHCP)
	int iWifiDHCPMode;				//��������ģʽ 0:��������0 DHCP�� 1:��������0 apģʽDHCP	
	int iEnable;					//DHCPʹ�� 0:ͣ�� 1:���� 
	int iReserved;                  //����չ���� PS��Ŀǰֻ��һ����������������DHCP����
} *PWIFIPARAM_DHCP;

typedef struct WIFIAPDHCPPARA_DHCP       
{
	int iSize;						//sizeof(WIFIAPDHCPPARA_DHCP)
	int iDHCPStart;					//DHCP���䷶Χ��ʼ	
	int iDHCPEnd;					//DHCP���䷶Χ���� 
	int iDHCPLease;                //DHCP����
} *PWIFIAPDHCPPARA_DHCP;

typedef struct ITS_OSD
{
	int iX;
	int iY;
	int iEnable;
} *PITS_OSD;

// typedef	enum __tagAlarmLinkType
// {
// 	ALARM_LINKTYPE_AUDIO = 0,
// 	ALARM_LINKTYPE_VIDEO,
// 	ALARM_LINKTYPE_OUTPORT,
// 	ALARM_LINKTYPE_RECORD,
// 	ALARM_LINKTYPE_PTZ,
// 	ALARM_LINKTYPE_SNAPSHOT
// }EAlarmLinkType, *PEAlarmLinkType;
typedef int __tagAlarmLinkType;
typedef int EAlarmLinkType;
typedef int *PEAlarmLinkType;

//modify by zhy 2013.03.06
//////////////////////////////////////General AlarmLinkType////////////////////////////////////
#define ALARMLINKTYPE_MIN                               0  
#define ALARMLINKTYPE_LINKSOUND			(ALARMLINKTYPE_MIN + 0)		//����������ʾ
#define ALARMLINKTYPE_LINKDISPLAY		(ALARMLINKTYPE_MIN + 1)		//������Ļ��ʾ
#define ALARMLINKTYPE_LINKOUTPORT		(ALARMLINKTYPE_MIN + 2)     //��������˿� 
#define ALARMLINKTYPE_LINKRECORD     	(ALARMLINKTYPE_MIN + 3)     //����¼��  
#define ALARMLINKTYPE_LINKPTZ	     	(ALARMLINKTYPE_MIN + 4)     //����PTZ  
#define ALARMLINKTYPE_LINKSNAP   	    (ALARMLINKTYPE_MIN + 5)     //����ץ�� 
#define ALARMLINKTYPE_LINKSINGLEPIC		(ALARMLINKTYPE_MIN + 6)		//����������
#define ALARMLINKTYPE_LINKEMAIL			(ALARMLINKTYPE_MIN + 7)		//�����ʼ�

//����IE
#define ALARM_LINKTYPE_AUDIO		ALARMLINKTYPE_LINKSOUND			//����������ʾ
#define ALARM_LINKTYPE_VIDEO		ALARMLINKTYPE_LINKDISPLAY		//������Ļ��ʾ
#define ALARM_LINKTYPE_OUTPORT		ALARMLINKTYPE_LINKOUTPORT		//��������˿� 
#define ALARM_LINKTYPE_RECORD		ALARMLINKTYPE_LINKRECORD		//����¼��  
#define ALARM_LINKTYPE_PTZ			ALARMLINKTYPE_LINKPTZ			//����PTZ  
#define ALARM_LINKTYPE_SNAPSHOT		ALARMLINKTYPE_LINKSNAP   		//����ץ��
#define ALARM_LINKTYPE_SINGLEPIC	ALARMLINKTYPE_LINKSINGLEPIC		//����������
#define ALARM_LINKTYPE_EMAIL		ALARMLINKTYPE_LINKEMAIL			//�����ʼ�

typedef struct __tagAlarmLink
{
	int				iLinkSetSize;
	int				iLinkSet[MAX_ALARM_LINKTYPE*MAX_BITSET_NUM];	//����¼�� ץ�� ���
	//ȡֵ��iLinkType����,MAX_BITSET_NUM������չ����0 Type��0+MAX_ALARM_LINKTYPEΪͬһ���������͵Ĳ���������Ϊ��չ
	//iLinkType=0,1ʱ��iLinkSet�ж�Ӧλ�����ݱ�ʾʹ��iEnable��0����ʹ�ܣ�1��ʹ�ܡ�
	//iLinkType=2,3,5ʱ��iLinkSet�ж�Ӧλ�����ݱ�ʾ��λʹ��iEnableByBits��
	//�����λ�����λÿһλ��ʾһ������Ƶͨ��/����˿ڵ�ʹ�ܡ�
	//iLinkType=4ʱ��iLinkSe��Ӧλ������t��ʾ������ͨ����

	int				iPtzSize;										//PTZ
	struct			vca_TLinkPTZ		ptz[MAX_CHANNEL_NUM_TDWY];		//iLinkType=4ʱ�洢����PTZ�Ĳ���
}TAlarmLink, *PTAlarmLink;							//	������������

typedef struct __tagAlarmLinkParam
{
	int				iBuffSize;

	int				iAlarmTypeParam;				//	�������Ͳ���
	//	ȡֵ��iAlarmType����������iAlarmTypeΪ���ܷ�������iAlarmTypeParam��ʾiRuleID
	int				iReserved;						//	ȡֵ��iAlarmType����������iAlarmTypeΪ���ܷ�������iAlarmTypeParam��ʾiEventID
	int				iLinkType;						//  ��չ +100 
	union
	{

		int iChannelSet[4];							//�����������ô˽ṹ ��֧����չ����Ҫʹ��ԭ�ӻ�ȡ��ʽ
		int iEnable;								//����������32·ʹ��					
		struct										//���˵����ⶼʹ������ṹ��
		{
			TAlarmLink		stLinkContent;			//��������
			void*			pvReserved;		
		};
	};
}TAlarmLinkParam, *PTAlarmLinkParam;


typedef struct __tagAlarmLinkParam_V1
{
	int				iAlarmTypeParam;				//	�������Ͳ���
	//	ȡֵ��iAlarmType����������iAlarmTypeΪ���ܷ�������iAlarmTypeParam��ʾiRuleID
	int				iReserved;						//	ȡֵ��iAlarmType����������iAlarmTypeΪ���ܷ�������iAlarmTypeParam��ʾiEventID
	int				iLinkType;						//  ��չ +100 
	int				iLinkParamSize;
	union
	{
		int iEnable;
		int iChannelSet[MAX_BITSET_COUNT];						//�����������ô˽ṹ
		struct  
		{
			int				iPtzNo;				
			unsigned short	usType;					//	0��������ͨ����1Ԥ��λ��2�켣��3·��
			unsigned short	usTypeNO;				//	Ԥ��λ��/�켣��/·����
		};
	};
}TAlarmLinkParam_V1, *PTAlarmLinkParam_V1;

/************************************************************************/
/* ͨ�ñ�������															*/
/************************************************************************/
//����
// typedef enum __tagAlarmSetCmd
// {
// 	CMD_SET_ALARMSCHEDULE = 0,
// 	CMD_SET_ALARMLINK,
// 	CMD_SET_ALARMSCH_ENABLE
// }EAlarmSetCmd, *PEAlarmSetCmd;
typedef int __tagAlarmSetCmd;
typedef int EAlarmSetCmd;
typedef int * PEAlarmSetCmd;
#define CMD_SET_ALARMSCHEDULE	0
#define CMD_SET_ALARMLINK		1
#define CMD_SET_ALARMSCH_ENABLE 2
#define CMD_SET_ALARMTRIGGER	3
#define CMD_SET_ALARMLINK_V1	4
#define CMD_SET_ALARMLINK_V2	5

// typedef enum __tagAlarmGetCmd
// {
// 	CMD_GET_ALARMSCHEDULE = 0,
// 	CMD_GET_ALARMLINK,
// 	CMD_GET_ALARMSCH_ENABLE
// }EAlarmGetCmd, *PEAlarmGetCmd;
typedef int __tagAlarmGetCmd;
typedef int EAlarmGetCmd;
typedef int * PEAlarmGetCmd;
#define CMD_GET_ALARMSCHEDULE	0
#define CMD_GET_ALARMLINK		1
#define CMD_GET_ALARMSCH_ENABLE 2
#define CMD_GET_ALARMTRIGGER	3
#define CMD_GET_ALARMLINK_V1	4
#define CMD_GET_ALARMLINK_V2	5

//����������ʶ
#define ATPI_AUDIO_LOST			0		//��Ƶ��ʧ������������
#define AUDIO_LOST_PARAM_COUNT	1		

//������ֵ����
typedef struct __tagAlarmTriggerParam
{
	int				iBuffSize;
	int				iType;				//	��������
	int				iID;				//	����������ʶ
	int				iValue;				//	����ֵ
}TAlarmTriggerParam, *PTAlarmTriggerParam;

//SDK current use               //���ֻ����֪ͨ������������ʹ����Э��һ�µĺ궨��
#define ALARM_VDO_MOTION		0
#define ALARM_VDO_REC			1
#define ALARM_VDO_LOST			2
#define ALARM_VDO_INPORT		3
#define ALARM_VDO_OUTPORT		4
#define ALARM_VDO_COVER 		5
#define ALARM_VCA_INFO			6			//VCA������Ϣ
#define ALARM_AUDIO_LOST		7
#define ALARM_EXCEPTION		    8
//����������������Ϊ��Э��һ�£�
#define ALARM_NORMAL						0x7FFFFFFF //״̬�����ޱ���		
#define ALARM_ANALOG_UPPER_LIMIT_ON			10//ģ�������ޱ���
#define ALARM_ANALOG_LOWER_LIMIT_ON			11//ģ�������ޱ���
#define ALARM_TEMPERATURE_UPPER_LIMIT_ON	12//�¶����ޱ���
#define ALARM_TEMPERATURE_LOWER_LIMIT_ON	13//�¶����ޱ���
#define ALARM_HUMIDITY_UPPER_LIMIT_ON		14//ʪ�����ޱ���
#define ALARM_HUMIDITY_LOWER_LIMIT_ON		15//ʪ�����ޱ���
#define	ALARM_VDO_INPORT_OFF			    (3+256) //�˿�����
#define ALARM_ANALOG_UPPER_LIMIT_OFF		(10+256)//ģ������������ 
#define ALARM_ANALOG_LOWER_LIMIT_OFF		(11+256)//ģ������������ 
#define ALARM_TEMPERATURE_UPPER_LIMIT_OFF	(12+256)//�¶��������� 
#define ALARM_TEMPERATURE_LOWER_LIMIT_OFF	(13+256)//�¶��������� 
#define ALARM_HUMIDITY_UPPER_LIMIT_OFF		(14+256)//ʪ���������� 
#define ALARM_HUMIDITY_LOWER_LIMIT_OFF		(15+256)//ʪ���������� 
//////////////////////////////////////ALARM CLEAR TYPE////////////////////////////////////CMD_GET_ALARMSCHEDULE
//from now on, after code 10  alarm use the style sending directly��not switch code in SDK��chenbin@140719
#define  ALARM_TYPE_MIN                          0
#define  ALARM_TYPE_VIDEO_LOST					 (ALARM_TYPE_MIN)       //��Ƶ��ʧ
#define  ALARM_TYPE_PORT_ALARM                   (ALARM_TYPE_MIN + 1)   //�˿ڱ���            
#define  ALARM_TYPE_MOTION_DETECTION			 (ALARM_TYPE_MIN + 2)   //�ƶ����
#define  ALARM_TYPE_VIDEO_COVER					 (ALARM_TYPE_MIN + 3)   //��Ƶ�ڵ�
#define	 ALARM_TYPE_VCA							 (ALARM_TYPE_MIN + 4)	//���ܷ���
#define  ALARM_TYPE_AUDIOLOST					 (ALARM_TYPE_MIN + 5)	//��Ƶ��ʧ
#define  ALARM_TYPE_TEMPERATURE                  (ALARM_TYPE_MIN + 6)	//��ʪ��
#define  ALARM_TYPE_ILLEGAL_DETECT               (ALARM_TYPE_MIN + 7)	//Υ�¼��
#define  ALARM_TYPE_EXCPETION				     (ALARM_TYPE_MIN + 100) //�쳣����
#define  ALARM_TYPE_ALL							 (ALARM_TYPE_MIN + 255) //ȫ��
#define  ALARM_TYPE_MAX                          (ALARM_TYPE_ALL + 1)
#define  ALARM_TYPE_PORT_ALARM_OFF				 (ALARM_TYPE_MIN + 1 + 256)//�˿ڱ�������
///////////////////////////////////////ExceptionType///////////////////////////////////

#define  EXCEPTION_TYPE_MIN						0
#define  EXCEPTION_TYPE_DISK_FULL				(EXCEPTION_TYPE_MIN + 0)  //������			
#define  EXCEPTION_TYPE_NO_DISK					(EXCEPTION_TYPE_MIN + 1)  //�޴���
#define  EXCEPTION_TYPE_DISK_IO_ERROR			(EXCEPTION_TYPE_MIN + 2)  //���̶�д����
#define  EXCEPTION_TYPE_NOALLOW_ACCESS			(EXCEPTION_TYPE_MIN + 3)  //�Ƿ�����
#define  EXCEPTION_TYPE_IP_COLLISION			(EXCEPTION_TYPE_MIN + 4)  //IP��ͻ
#define  EXCEPTION_TYPE_NETWORK_INTERRUPT		(EXCEPTION_TYPE_MIN + 5)  //�����ж�
#define  EXCEPTION_TYPE_SYSTEM_BUSY				(EXCEPTION_TYPE_MIN + 6)  //ϵͳæ	
#define  EXCEPTION_TYPE_NO_REDUNDANCY_DISK		(EXCEPTION_TYPE_MIN + 7)  //��������
#define  EXCEPTION_TYPE_ABNORMAL_VOLTAGE		(EXCEPTION_TYPE_MIN + 8)  //��ѹ�쳣
#define  EXCEPTION_TYPE_MAC_COLLISION			(EXCEPTION_TYPE_MIN + 9)  //MAC��ͻ
#define  EXCEPTION_TYPE_RAID                    (EXCEPTION_TYPE_MIN + 10) //�����쳣
#define  EXCEPTION_TYPE_HOTBACKUP               (EXCEPTION_TYPE_MIN + 11) //�ȱ��쳣
#define  EXCEPTION_TYPE_OVERLOAD				(EXCEPTION_TYPE_MIN + 12) //PSE overload
#define  EXCEPTION_TYPE_DISK_OVERLOAD			(EXCEPTION_TYPE_MIN + 13) //Disk overload
#define  EXCEPTION_TYPE_RECODE					(EXCEPTION_TYPE_MIN + 14) //recode
#define  EXCEPTION_TYPE_MAX						(EXCEPTION_TYPE_MIN + 15)	
//////////////////////////////////////Exception Handle////////////////////////////////////
#define  EXCEPTION_HANDLE_ENABLE                 0x01         //ʹ���쳣���� 
#define  EXCEPTION_HANDLE_DIALOG				 0x02         //�Ƿ񵯳���ʾ�Ի���
#define  EXCEPTION_HANDLE_LINK_BELL              0x04         //�Ƿ�����������
#define  EXCEPTION_HANDLE_LINK_NETCLIENT         0x08         //�Ƿ�֪ͨ����ͻ���
#define  EXCEPTION_HANDLE_LINK_EMAIL			 0x10         //�Ƿ������ʼ�

//////////////////////////////////////AlarmLinkType////////////////////////////////////
#define ALARMLINKTYPE_MIN                               0  
#define ALARMLINKTYPE_VIDEOLOST_LINKRECORD     	(ALARMLINKTYPE_MIN + 0)   //��Ƶ��ʧ��������¼��    
#define ALARMLINKTYPE_VIDEOLOST_LINKSNAP   	    (ALARMLINKTYPE_MIN + 1)   //��Ƶ��ʧ��������ץ�� 
#define ALARMLINKTYPE_VIDEOLOST_LINKOUTPORT     (ALARMLINKTYPE_MIN + 3)   //��Ƶ��ʧ��������˿� 
#define ALARMLINKTYPE_MOTIONDETECT_LINKRECORD   (ALARMLINKTYPE_MIN + 4)   //�ƶ���ⱨ������¼�� 
#define ALARMLINKTYPE_MOTIONDETECT_LINKSNAP     (ALARMLINKTYPE_MIN + 5)   //�ƶ���ⱨ������ץ��
#define ALARMLINKTYPE_MOTIONDETECT_LINKOUTPORT 	(ALARMLINKTYPE_MIN + 6)   //�ƶ������������˿� 
#define ALARMLINKTYPE_PORTALARM_LINKRECORD     	(ALARMLINKTYPE_MIN + 7)   //�˿ڱ�������¼��
#define ALARMLINKTYPE_PORTALARM_LINKSNAP       	(ALARMLINKTYPE_MIN + 8)   //�˿ڱ�������ץ��
#define ALARMLINKTYPE_PORTALARM_LINKOUTPORT    	(ALARMLINKTYPE_MIN + 9)   //�˿ڱ�����������˿�
#define ALARMLINKTYPE_MAX					   	(ALARMLINKTYPE_PORTALARM_LINKOUTPORT + 1)

/************************************************************************/
/*  ���ܽ�ͨ ���Ӿ���*/
/************************************************************************/

#define SN_ENCRYPT_TYPE_MASK					0x01
#define SN_ENCRYPT_TYPE_RECOGNIZE_ARITHMETIC	0x00			//ʶ���㷨��������
#define SN_ENCRYPT_TYPE_PROGRAM					0x01			//�����������

#define SYSTEM_TYPE_MASK					0x04					
#define SYSTEM_TYPE_DEBUG					0x00				//����
#define SYSTEM_TYPE_GATE					0x03				//����
#define SYSTEM_TYPE_ECOP					0x04				//�羯

#define SN_REG_MASK							0x02				
#define SN_REG_RECOGNIZE_DOG				0x01				//ʶ��
#define SN_REG_ENCRYPT_DOG					0x02				//���ܽ�ͨ���ܹ�

//IPC 3MP
#define MAX_CHANNEL_TYPE		2
#define MAX_REALTYPE_NUM        2  //ʵʱ�����������࣬0���������ʵʱ����ֵ��1�����ʵʱ�����Ϣ

struct STRCT_REAL_STATE 
{
	int        m_iTypes[MAX_REALTYPE_NUM];  //ÿ��type��typeֵ
	int        m_iValues[MAX_REALTYPE_NUM]; //ÿ��type��Ӧ��ʵʱ����
};

//ITScb@121010
//���������չ
////CMD����
///************************************************************************
//*	�������ʱ���                                                                     
//************************************************************************/
//#define ITS_CMD_SET_MIN                         0
//#define ITS_CMD_SET_TIMERANGE_ENABLE			0                           //����ʱ���ʹ��
//#define ITS_CMD_SET_TIMERANGE_AGCBLOCK			1							//����ʱ��β������ʹ��
//#define ITS_CMD_SET_TIMERANGE_FLASHLAMP			2							//����ʱ��������ʹ��
//
//#define ITS_CMD_GET_TIMERANGE_ENABLE			3							//��ȡʱ���ʹ��
//#define ITS_CMD_GET_TIMERANGE_AGCBLOCK			4							//��ȡʱ��β������ʹ��
//#define ITS_CMD_GET_TIMERANGE_FLASHLAMP			5							//��ȡʱ��������ʹ��
///************************************************************************/
///*   ���ܽ�ͨ��������                                                                 
///************************************************************************/
//#define ITS_ROADWAY_CMD_SET_MIN     0
//#define ITS_ROADWAY_CMD_SET_ENABLE  (ITS_ROADWAY_CMD_SET_MIN + 0)	    //���ó���ʹ��
//#define ITS_ROADWAY_CMD_SET_LOOP    (ITS_ROADWAY_CMD_SET_MIN + 1)		//���ó�����Ȧ
//#define ITS_ROADWAY_CMD_SET_TIME    (ITS_ROADWAY_CMD_SET_MIN + 2)		//���ó���ʱ��
//#define ITS_ROADWAY_CMD_SET_SPEED   (ITS_ROADWAY_CMD_SET_MIN + 3)		//���ó����ٶ�
//#define ITS_ROADWAY_CMD_SET_RECO    (ITS_ROADWAY_CMD_SET_MIN + 4)		//���ò⵽ʶ������
//#define ITS_ROADWAY_CMD_SET_VLOOP   (ITS_ROADWAY_CMD_SET_MIN + 5)		//����������Ȧ
//#define ITS_ROADWAY_CMD_SET_LIGHT   (ITS_ROADWAY_CMD_SET_MIN + 6)		//���ó����źŵ�
//#define ITS_ROADWAY_CMD_SET_CAPTURE (ITS_ROADWAY_CMD_SET_MIN + 7)		//���ó���ץ��
//#define ITS_ROADWAY_CMD_SET_REFERENCELINE	(ITS_ROADWAY_CMD_SET_MIN + 8)		//���ó�������
//#define ITS_ROADWAY_CMD_SET_MAX     (ITS_ROADWAY_CMD_SET_MIN + 9)		//
//
//#define ITS_ROADWAY_CMD_GET_MIN     0
//#define ITS_ROADWAY_CMD_GET_ENABLE  (ITS_ROADWAY_CMD_GET_MIN + 0)	    //��ó���ʹ��
//#define ITS_ROADWAY_CMD_GET_LOOP    (ITS_ROADWAY_CMD_GET_MIN + 1)		//��ó�����Ȧ
//#define ITS_ROADWAY_CMD_GET_TIME    (ITS_ROADWAY_CMD_GET_MIN + 2)		//��ó���ʱ��
//#define ITS_ROADWAY_CMD_GET_SPEED   (ITS_ROADWAY_CMD_GET_MIN + 3)		//��ó����ٶ�
//#define ITS_ROADWAY_CMD_GET_RECO    (ITS_ROADWAY_CMD_GET_MIN + 4)		//��ò⵽ʶ������
//#define ITS_ROADWAY_CMD_GET_VLOOP   (ITS_ROADWAY_CMD_GET_MIN + 5)		//���������Ȧ
//#define ITS_ROADWAY_CMD_GET_LIGHT   (ITS_ROADWAY_CMD_GET_MIN + 6)		//��ó����źŵ�
//#define ITS_ROADWAY_CMD_GET_CAPTURE (ITS_ROADWAY_CMD_GET_MIN + 7)		//��ó���ץ��
//#define ITS_ROADWAY_CMD_GET_REFERENCELINE	(ITS_ROADWAY_CMD_GET_MIN + 8)	//���ó�������
//#define ITS_ROADWAY_CMD_GET_MAX     (ITS_ROADWAY_CMD_GET_MIN + 9)		//
///************************************************************************/
///*	���ܽ�ͨ�����Ż�
///************************************************************************/
//#define ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_MIN                   0
//#define ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_FIRST_HZ				(ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_MIN + 0)	    //���ó����Ż��׺���
//#define ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_NOPLATE_ENABLE		(ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_MIN + 1)		//�����Ƿ��������Ƴ�
//#define ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_OTHER					(ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_MIN + 2)		//���ô��������ƺͲ����ų���
//#define ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_MAX					(ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_MIN + 3)		//
//
//#define ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_MIN                   0										   		
//#define ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_FIRST_CHARACTER       (ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_MIN + 0)		//��ó����Ż��׺���
//#define ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_NOPLATE_ENABLE		(ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_MIN + 1)		//����Ƿ��������Ƴ�
//#define ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_OTHER					(ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_MIN + 2)		//��ô��������ƺͲ����ų���
//#define ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_MAX					(ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_MIN + 3)		//
///************************************************************************/
///*  ��ͨΥ�¼����  
///************************************************************************/
//#define ITS_EXTRAINFO_CMD_SET_MIN								 0
////Υͣ��Ŀ
//#define ITS_ILLEGALPARK_CMD_SET									(ITS_EXTRAINFO_CMD_SET_MIN + 0)	    //����Υ��ͣ������
//#define ITS_ILLEGALPARKPARM_CMD_SET								(ITS_EXTRAINFO_CMD_SET_MIN + 1)		//����Υ��ͣ���궨�������
//#define ITS_LICENSEOPTIMIZEOTHER_CMD_SET						(ITS_EXTRAINFO_CMD_SET_MIN + 2)		//���ô��������ƺͲ����ų���
//#define ITS_OPTIM_CMD_SET										(ITS_EXTRAINFO_CMD_SET_MIN + 3)		//���ó����Ż��׺���
//#define ITS_RECOPARAM_CMD_SET									(ITS_EXTRAINFO_CMD_SET_MIN + 4)		//����ʶ���㷨����
//#define ITS_ROADWAYENABLE_CMD_SET								(ITS_EXTRAINFO_CMD_SET_MIN + 5)		//���ó���
//#define ITS_CAMLOCATION_CMD_SET									(ITS_EXTRAINFO_CMD_SET_MIN + 6)		//�����豸��Ϣ
//#define ITS_EXACTCONFIRM_CMD_SET								(ITS_EXTRAINFO_CMD_SET_MIN + 7)		//���þ�ȷ��λ��Ϣ
////�������
//#define ITS_VIDEODECTECT_CMD_SET								(ITS_EXTRAINFO_CMD_SET_MIN + 8)		//ͼ�������
//#define ITS_DETECTAREA_CMD_SET									(ITS_EXTRAINFO_CMD_SET_MIN + 10)	//�켣��������������
//#define ITS_RECOTYPE_CMD_SET									(ITS_EXTRAINFO_CMD_SET_MIN + 11)	//�㷨���Ͳ���
////��ͨһ���
//#define ITS_REDLIGHTDETECTAREA_CMD_SET							(ITS_EXTRAINFO_CMD_SET_MIN + 12)	//���ú�Ƽ���������
//#define ITS_ENABLE_CMD_SET										(ITS_EXTRAINFO_CMD_SET_MIN + 13)	//���ý�ͨ��ع���ʹ��Э��
//
//#define ITS_EXTRAINFO_CMD_SET_MAX								(ITS_EXTRAINFO_CMD_SET_MIN + 15)	//
//
//#define ITS_EXTRAINFO_CMD_GET_MIN								 0
////Υͣ��Ŀ
//#define ITS_ILLEGALPARK_CMD_GET									(ITS_EXTRAINFO_CMD_GET_MIN + 0)	    //���Υ��ͣ������
//#define ITS_ILLEGALPARKPARM_CMD_GET								(ITS_EXTRAINFO_CMD_GET_MIN + 1)		//���Υ��ͣ���궨�������
//#define ITS_LICENSEOPTIMIZEOTHER_CMD_GET						(ITS_EXTRAINFO_CMD_GET_MIN + 2)		//��ô��������ƺͲ����ų���
//#define ITS_OPTIM_CMD_GET										(ITS_EXTRAINFO_CMD_GET_MIN + 3)		//��ó����Ż��׺���
//#define ITS_RECOPARAM_CMD_GET									(ITS_EXTRAINFO_CMD_GET_MIN + 4)		//���ʶ���㷨����
//#define ITS_ROADWAYENABLE_CMD_GET								(ITS_EXTRAINFO_CMD_GET_MIN + 5)		//��ó���
//#define ITS_CAMLOCATION_CMD_GET									(ITS_EXTRAINFO_CMD_GET_MIN + 6)		//����豸��Ϣ
////�������
//#define ITS_VIDEODECTECT_CMD_GET								(ITS_EXTRAINFO_CMD_GET_MIN + 7)		//ͼ�������
//#define ITS_DETECTAREA_CMD_GET									(ITS_EXTRAINFO_CMD_GET_MIN + 9)		//�켣��������������
//#define ITS_RECOTYPE_CMD_GET									(ITS_EXTRAINFO_CMD_GET_MIN + 10)	//�㷨���Ͳ���
//#define ITS_CAMRATYPE_CMD_GET									(ITS_EXTRAINFO_CMD_GET_MIN + 11)	//�������
////��ͨһ���
//#define ITS_REDLIGHTDETECTAREA_CMD_GET							(ITS_EXTRAINFO_CMD_SET_MIN + 12)	//��ú�Ƽ���������
//#define ITS_ENABLE_CMD_GET										(ITS_EXTRAINFO_CMD_SET_MIN + 13)	//��ý�ͨ��ع���ʹ��Э��
//#define ITS_VERSIONPRO_CMD_GET									(ITS_EXTRAINFO_CMD_SET_MIN + 14)	//�������汾��Ϣ
//
//#define ITS_EXTRAINFO_CMD_GET_MAX								(ITS_EXTRAINFO_CMD_GET_MIN + 15)	//
////CMD����END ���鱣��ԭλ��ע�ͣ��Ա���ز�����ͬһ��λ�ã���߿ɶ���

/************************************************************************
*	��������������                                                                   
************************************************************************/
#define MAX_ITS_BLOCK        5  

struct ITS_TBlock  
{
	int iX;					
	int iY;
};

struct ITS_TOTAL_TBlock
{
	ITS_TBlock block[MAX_ITS_BLOCK];
};
/************************************************************************
*	�������ʱ���                                                                     
************************************************************************/
#define ITS_CMD_SET_TIMERANGE_ENABLE			0                           //����ʱ���ʹ��
#define ITS_CMD_SET_TIMERANGE_AGCBLOCK			1							//����ʱ��β������ʹ��
#define ITS_CMD_SET_TIMERANGE_FLASHLAMP			2							//����ʱ��������ʹ��

#define ITS_CMD_GET_TIMERANGE_ENABLE			3							//��ȡʱ���ʹ��
#define ITS_CMD_GET_TIMERANGE_AGCBLOCK			4							//��ȡʱ��β������ʹ��
#define ITS_CMD_GET_TIMERANGE_FLASHLAMP			5							//��ȡʱ��������ʹ��

#define MAX_ITS_TIMERANGE                8

struct ITS_TTimeRange 
{
	int iEnable;								//�Ƿ����ø�ʱ��Σ�1-���ã�0-������
	int iTimeRange;								//ʱ�䷶Χ�� bit24-bit31:��ʼСʱ
	//					bit16-bit23:��ʼ����
	//					bit8-bit15:����Сʱ
	//					bit0-bit7:��������	
};

struct ITS_TOTAL_TTimeRange 
{
	ITS_TTimeRange timerange[MAX_ITS_TIMERANGE];
};
/************************************************************************
*	�������ʱ���ͨ�ò���                                                                     
************************************************************************/
#define MAX_ITS_TEMPLATE                8
#define MAX_TIME_RANGE_PARAM  			43
struct ITS_TTimeRangeParam 
{
	int iType;							// 0-�������� 1-�����ع� 2-���ð�ƽ�� 3-��������
	int iAutoEnable[MAX_TIME_RANGE_PARAM];	//  1��1-�Զ���0-�ֶ���
	//	2������˵����iTypeΪ12ʱ��0-�չ�ģʽ��1-ӫ���ģʽ��2-�����а�ƽ�⣩��
	//	3��iTypeΪ255ʱ������1-�Զ���0-�ֶ���
	//  4��iTypeΪ8ʱ1-�Զ�����ͨģʽ����0-�ֶ� 2-�Զ���ר��ģʽ��3-ITS���3D���뼶��   //add by wd @20130619
	int iParam1[MAX_TIME_RANGE_PARAM];		//0--��Ȧ���ڣ��ֶ��£��������ֵ���Զ��£�1������Ȧ��0�رչ�Ȧ��
	//1--����̬��1��������0����ر�
	//2--���ⲹ����1��������0����ر�
	//3--�ع�ʱ�䣺�ֶ��£��������ֵ���Զ��£���������ֵ
	//4--���ŵ��ڣ��ֶ��£��������ֵ���Զ��£���������ֵ
	//5--������ڣ��ֶ��£��������ֵ���Զ��£���������ֵ
	//6--gamma���ڣ��ֶ��£��������ֵ���Զ��£���������ֵ
	//7--��ȵ��ڣ��ֶ��£��������ֵ���Զ��£���������ֵ
	//8--������ڣ��ֶ��£��������ֵ���Զ�(��ģʽ)�£���������ֵ���Զ�(ר��ģʽ)�£�����2D��������ֵ   ITS���3D���뼶��3-�ߣ�2-�У�1-�ͣ�0-��� //add by wd @20130619
	//9--�ع����򣺴������Ͻ�X����
	//10--���ⲹ�����򣺴������Ͻ�X����
	//11--AF�������ã��������Ͻ�X����
	//12--Ŀ�����ȵ��ڣ��ֶ��£��������ֵ���Զ��£���������ֵ
	//13--��ƽ����ڣ��ֶ��£������ƽ���R������0-255���Զ��£������ƽ���R����У��ϵ������Χ100-200������Ϊ5
	//14--jpegͼ������
	//15--lutʹ��
	//16--�Զ����ȵ���ʹ��
	int iParam2[MAX_TIME_RANGE_PARAM];		//0--��Ȧ���ڣ�δʹ��
	//1--����̬��δʹ��
	//2--���ⲹ����δʹ��
	//3--�ع�ʱ�䣺�ֶ��£�δʹ�ã��Զ��£���������ֵ
	//4--���ŵ��ڣ��ֶ��£�δʹ�ã��Զ��£���������ֵ
	//5--������ڣ��ֶ��£�δʹ�ã��Զ��£���������ֵ
	//6--gamma���ڣ��ֶ��£�δʹ�ã��Զ��£���������ֵ
	//7--��ȵ��ڣ��ֶ��£�δʹ�ã��Զ��£���������ֵ
	//8--������ڣ��ֶ��£�δʹ�ã��Զ�(��ģʽ)�£�δʹ�ã��Զ�(ר��ģʽ)�£�����3D��������ֵ
	//9--�ع����򣺴������Ͻ�Y����
	//10--���ⲹ�����򣺴������Ͻ�Y����
	//11--AF�������ã��������Ͻ�Y����
	//12--Ŀ�����ȵ��ڣ��ֶ��£��������ֵ���Զ��£���������ֵ
	//13--��ƽ����ڣ��ֶ��£������ƽ���G������0-255���Զ��£������ƽ���G����У��ϵ������Χ100-200������Ϊ5
	//14--jpegͼ������
	//15--lutʹ��
	//16--�Զ����ȵ���ʹ��
	int iParam3[MAX_TIME_RANGE_PARAM];		//0--��Ȧ���ڣ�δʹ��
	//1--����̬��δʹ��
	//2--���ⲹ����δʹ��
	//3--�ع�ʱ�䣺�ֶ��£�δʹ�ã��Զ��£���������ֵ
	//4--���ŵ��ڣ��ֶ��£�δʹ�ã��Զ��£���������ֵ
	//5--������ڣ��ֶ��£�δʹ�ã��Զ��£���������ֵ
	//6--gamma���ڣ��ֶ��£�δʹ�ã��Զ��£���������ֵ
	//7--��ȵ��ڣ��ֶ��£�δʹ�ã��Զ��£���������ֵ
	//8--������ڣ��ֶ��£�δʹ�ã��Զ��£�δʹ��
	//9--�ع����򣺴������½�X����
	//10--���ⲹ�����򣺴������½�X����
	//11--AF�������ã��������½�X����
	//12--Ŀ�����ȵ��ڣ��ֶ��£��������ֵ���Զ��£���������ֵ
	//13--��ƽ����ڣ��ֶ��£������ƽ���B������0-255���Զ��£������ƽ���B����У��ϵ������Χ100-200������Ϊ5
	//14--jpegͼ������
	//15--lutʹ��
	//16--�Զ����ȵ���ʹ��
	int iParam4[MAX_TIME_RANGE_PARAM];		//0--��Ȧ���ڣ�δʹ��
	//1--����̬��δʹ��
	//2--���ⲹ����δʹ��
	//3--�ع�ʱ�䣺δʹ��
	//4--���ŵ��ڣ�δʹ��
	//5--������ڣ�δʹ��
	//6--gamma���ڣ�δʹ��
	//7--��ȵ��ڣ�δʹ��
	//8--������ڣ�δʹ��
	//9--�ع����򣺴������½�Y����
	//10--���ⲹ�����򣺴������½�Y����
	//11--AF�������ã��������½�Y����
	//12--Ŀ�����ȵ��ڣ�δʹ��
	//13--��ƽ����ڣ�δʹ��
	//14--jpegͼ������
	//15--lutʹ��
	//16--�Զ����ȵ���ʹ��
};

struct ITS_TOTAL_TTimeRangeParam 
{
	ITS_TTimeRangeParam timerangeparam[MAX_ITS_TEMPLATE +1];
};

struct ITS_TTemplateMap 
{
	int iTemplateID;					//ģ����
	char cTemplateName[64];				//ģ������
};

struct ITS_TOTAL_TTemplateMap 
{
	ITS_TTemplateMap templatemap[MAX_ITS_TIMERANGE];
};

typedef struct ITS_ILLEGALRECORD
{
	long m_lCarID;
	int  m_iRecordFlag;
	int  m_iIllegalType;
	int  m_iPreset;
}*pITS_ILLEGALRECORD;

/************************************************************************
*	�����������ģʽ                                                                    
************************************************************************/
struct ITS_CamWorkMode 
{
	int iWorkMode;							//����ģʽ  0-����ģʽ   1-����ģʽ
	int iInterval;								//ʱ����	
};

/************************************************************************/
//*   ���ܽ�ͨ��������                                                                 
/************************************************************************/

//��9��ʼGET/SETʹ��ͳһ�ĺ궨�壬����ʱע���޸�ITS_ROADWAY_CMD_GET_MAX/ITS_ROADWAY_CMD_SET_MAX��ֵ
#define ITS_ROADWAY_CMD_SET_MIN				0
#define ITS_ROADWAY_CMD_SET_ENABLE			(ITS_ROADWAY_CMD_SET_MIN + 0)	    //���ó���ʹ��
#define ITS_ROADWAY_CMD_SET_LOOP			(ITS_ROADWAY_CMD_SET_MIN + 1)		//���ó�����Ȧ
#define ITS_ROADWAY_CMD_SET_TIME			(ITS_ROADWAY_CMD_SET_MIN + 2)		//���ó���ʱ��
#define ITS_ROADWAY_CMD_SET_SPEED			(ITS_ROADWAY_CMD_SET_MIN + 3)		//���ó����ٶ�
#define ITS_ROADWAY_CMD_SET_RECO			(ITS_ROADWAY_CMD_SET_MIN + 4)		//���ò⵽ʶ������
#define ITS_ROADWAY_CMD_SET_VLOOP			(ITS_ROADWAY_CMD_SET_MIN + 5)		//����������Ȧ
#define ITS_ROADWAY_CMD_SET_LIGHT			(ITS_ROADWAY_CMD_SET_MIN + 6)		//���ó����źŵ�
#define ITS_ROADWAY_CMD_SET_CAPTURE			(ITS_ROADWAY_CMD_SET_MIN + 7)		//���ó���ץ��
#define ITS_ROADWAY_CMD_SET_REFERENCELINE	(ITS_ROADWAY_CMD_SET_MIN + 8)		//���ó�������
#define ITS_ROADWAY_CMD_CHNLCARSPEED        (ITS_ROADWAY_CMD_SET_MIN + 9)		//����/��ȡ ������ͬ�������͵��ٶȼ�����
#define ITS_ROADWAY_CMD_CHNLDELAYDIS        (ITS_ROADWAY_CMD_SET_MIN + 10)		//����/��ȡ �����ӳ�ץ�ľ���
#define ITS_ROADWAY_CMD_SET_MAX				(ITS_ROADWAY_CMD_SET_MIN + 11)		

#define ITS_ROADWAY_CMD_GET_MIN				0
#define ITS_ROADWAY_CMD_GET_ENABLE			(ITS_ROADWAY_CMD_GET_MIN + 0)	    //��ó���ʹ��
#define ITS_ROADWAY_CMD_GET_LOOP			(ITS_ROADWAY_CMD_GET_MIN + 1)		//��ó�����Ȧ
#define ITS_ROADWAY_CMD_GET_TIME			(ITS_ROADWAY_CMD_GET_MIN + 2)		//��ó���ʱ��
#define ITS_ROADWAY_CMD_GET_SPEED			(ITS_ROADWAY_CMD_GET_MIN + 3)		//��ó����ٶ�
#define ITS_ROADWAY_CMD_GET_RECO			(ITS_ROADWAY_CMD_GET_MIN + 4)		//��ò⵽ʶ������
#define ITS_ROADWAY_CMD_GET_VLOOP			(ITS_ROADWAY_CMD_GET_MIN + 5)		//���������Ȧ
#define ITS_ROADWAY_CMD_GET_LIGHT			(ITS_ROADWAY_CMD_GET_MIN + 6)		//��ó����źŵ�
#define ITS_ROADWAY_CMD_GET_CAPTURE			(ITS_ROADWAY_CMD_GET_MIN + 7)		//��ó���ץ��
#define ITS_ROADWAY_CMD_GET_REFERENCELINE	(ITS_ROADWAY_CMD_GET_MIN + 8)		//��ó�������
#define ITS_ROADWAY_CMD_GET_MAX				(ITS_ROADWAY_CMD_GET_MIN + 11)		

#define MAX_ROADWAY_COUNT					4          //��󳵵���
#define MAX_LOOP_COUNT						4          //�����������Ȧ��
#define DAY_OR_NIGHT						2          //0---���죬1---����

#define LIGHT_COUNT_EX                      8                   //�źŵ��� 
#define LIGHT_COUNT							4					//�źŵ���
#define LIGHT_COM_COUNT						4					//�źŵƴ�����
#define CAPTURE_PLACE_COUNT					3					//������Ӧ��ץ��λ����Ŀ

#define CAPTURE_TYPE_MASK					0x01FF				// ��������8λ [4/19/2012 hanyongqiang]
#define CAPTURE_TYPE_GATE					0x01				//����
#define CAPTURE_TYPE_RED_LIGHT				0x02				//����� 
#define CAPTURE_TYPE_REVERSE				0x04				//����
#define CAPTURE_TYPE_OVERSPEED				0x08				//����

//���������չ
#define	MAX_ITS_REFLINE_NUM		25
#define MAX_ITS_DETECTMODE		5


struct TITSRoadwayInfo 
{
	int iRoadwayID;
	int  iEnable;
	char cChannelName[32];
	char cChannelDir[32];
};

#define MAX_ROADWAY_CHANNEL_NAME 50
struct TITSRoadwayInfoEx
{
	int iRoadwayID;
	int  iEnable;
	char cChannelName[MAX_ROADWAY_CHANNEL_NAME+1];
	char cChannelDir[LEN_32];
};

//add by wd @20130531 ���ó�����Ϣ ��չ
typedef struct __tagTITSRoadwayInfo_V1
{
	int  iBufSize;					//��������ṹ��ĳ���(���Զ���)
	int  iRoadwayID;				//������� ���֧��4��������0-3
	int  iEnable;					//����ʹ�ܱ�־  1-ʹ�ܣ�0-��ʹ��
	char cChannelName[MAX_ROADWAY_CHANNEL_NAME+1];			//��������  ���Ȳ�����50�ַ�
	char cChannelDir[LEN_32];		//��������  ���Ȳ�����31�ַ�
	int	 iChannelDownUp;			//����������  0-���У�1-����
	//add by wd 20130619 ��չ�ֶ� ����
	int  iUses;						//������; 0-��ͨ������1-�ǻ���������2-��������,3-С����������
	int	 iSceneID;					// ����ID(0~15) 20140203��չ
	NVS_FILE_TIME  stStartTime;	
	NVS_FILE_TIME  stStopTime;		//Time RangeS
	NVS_FILE_TIME  stStartTime2;	
	NVS_FILE_TIME  stStopTime2;
	NVS_FILE_TIME  stStartTime3;	
	NVS_FILE_TIME  stStopTime3;	
	NVS_FILE_TIME  stStartTime4;	
	NVS_FILE_TIME  stStopTime4;
	int  iBorderEnable;				//�߽糵�� ��λʹ�ܣ�bit0-��߽磬bit1-�ұ߽硣Ĭ�϶�Ϊ0
	char cNewChannelID[LEN_64];		//Roadway new channel ID, the lenth less than 63 bits
} TITSRoadwayInfo_V1, *PTITSRoadwayInfo_V1;
//add ended

struct TITSRoadWayLoop
{
	int iRoadwayID;
	int iComportNo;
	int iSpeedLoopNo;
	int iCaptureLoopNo;
	int iSceneID;// ����ID(0~15) 20140203��չ
	int iSecCaptureLoopNo;//�ڶ�ץ����Ȧ��,20150304��չ
};

struct TITSRoadwayTime
{
	int iRoadwayID;
	int iLoopMaxTime;
	int iLoopMinTime;
	int iLoopDelayTime;
	int iSceneID;// ����ID(0~15) 20140203��չ
};

struct TITSRoadwaySpeed
{
	int iRoadwayID;
	int iLoopDistance;
	int iSpeedLimit;
	int iSpeedModify;
	int iSceneID;// ����ID(0~15) 20140203��չ
};

struct TITSRoadwayReco
{
	int iRoadwayID;
	int iRoadwayRange;
	int iSceneID;// ����ID(0~15) 20140203��չ
	int iChnNo;
};

struct TITSRoadwayVLoop
{
	int iRoadwayID;
	int iLoopID;
	int iDayNight;
	int iPoint1[DAY_OR_NIGHT][MAX_LOOP_COUNT];
	int iPoint2[DAY_OR_NIGHT][MAX_LOOP_COUNT];
	int iPoint3[DAY_OR_NIGHT][MAX_LOOP_COUNT];
	int iPoint4[DAY_OR_NIGHT][MAX_LOOP_COUNT];
	int iSceneID;// ����ID(0~15) 20140203��չ
	int iSensitivity;//sensibility(0~100) defult50 20141219
	int iChnNo;
};

struct TITSRoadwayLight			//�źŵ�
{
	int iRoadwayID;
	int iComNo;				//�źŵƶ�Ӧ�Ĵ��ںţ���Χ0-2������0��1ΪRS232���ڣ�2ΪRS485���ڣ�
	int iFloodID;			//����Ʊ��,��λ					
	char cLightID[32];	//�����źŵ�,��ʽ4,0,1,2,3����(��һλ��ʾһ�������ƣ��������4·��Ƽ��������Χ0-3��)					
};

//���������չ
typedef struct __tagTITSRoadwayLight_V1
{
	TITSRoadwayLight m_stLight;
	int				 m_iCompelRedlight;		//ǿ�ƺ��
	int iSceneID;					// ����ID(0~15) 20140203��չ
	int iFlashLampType;				//����ƿ��Ʒ�ʽ  0���������� 1��������
}TITSRoadwayLight_V1, *PTITSRoadwayLight_V1;

struct TITSRoadwayCapture			//ץ��λ��
{
	int iRoadwayID;                         //�������   ���֧��4��������0-3
	int iType;								//����ץ������
	//bit0--����
	//bit1--����� 
	//bit2--���� 
	//bit3--����
	//bit4--��ֹ����ʻץ�� 
	//bit5--��ֹ����ʻץ�� 
	//bit6--ѹ����ץ��
	//bit7--�ǻ�����ץ��
	//bit8--�����涨������ʻ (����������ʻ)
	//bit9--��ֱֹ��    //add by wd @20130619 ��չ
	//bit10--��������
	//bit11--ѹ������
	//bit12--��ת���򴳺��
	//bit13--Υ��ͣ��  //add end
	//bit14--��ϴ�������Ƶץ�ĳ��ٹ���ʹ��
	char cPlace[CAPTURE_PLACE_COUNT][64];	//����������Ȧʱ��ץ��λ��
	//0������ץ��
	//1�������������Ȧʱץ��
	//2�����뿪������Ȧʱץ��
	//3��������ץ����Ȧʱץ��
	//4�����뿪ץ����Ȧʱץ��
	//5������ʱץ��
	//�����ͽ��У�����1,2,3,4
	int iChannelTpye;                       //���
	//0--��ת����
	//1--��ת����
	//2--ֱ�г���
	//3--��תֱ�г���
	//4--��תֱ�г���
	//5--�ǻ������� ---Ԥ��                 //modify by wd @20130619 
	//6--��ת��ת����
	//7--��ת�����Ӵ�ת��
	//8--ֱ��+��ת+��ת����
	int iRecognizePlace;                   //ʶ�����
	//0--ʶ���һ��
	//1--ʶ��ڶ���
	//2--��ʶ���һ����ʶ��ڶ���,ȡ���ŶȸߵĽ��
	//3--ʶ��ڶ���,���δʶ������,��ʶ���һ��
	//4--ʶ���һ��,���δʶ������,��ʶ��ڶ���
	//5��ʶ�������                        
	int iSceneID;							// ����ID(0~15) 20140203��չ
	int iRedLightCapType;					//0-������ץ�ģ�Ĭ�ϣ�1-������ץ�� 2-������+����
	int iTrailCapPlace;						//Trail Capture Place
};

struct ITS_RecoParamEx					//�������ʶ���㷨��չ����   
{
	int iMaxPlateNO;					//�������
	int iMaxPlateWidth;					//����ƿ��
	int iMinPlateWidth;					//��С���ƿ��
	int iRoadwayID;						//�������
	int iEnable;						//ʹ�ܱ�־	0������ʶ��1������ʶ��3����������ʶ�𣬲���������ʶ��				
	int iEngineNum;						//ʶ���������	�ݲ�֧�֣���0����	
	int iAreaType;						//ʶ����������
	int iPlateType;						//ʶ��������
	int iSpeed;							//�����ٶȡ�ע�������ٶ�ָ���ǳ�������Ƶ�����е��ƶ��ٶȣ����ǳ�����ʵ����ʹ�ٶȡ�	��Χ1-10					
	int iMaxSkip;						//����ڵ�֡��
	int iMinValid;						//��С�������Ŷ�
	int iFramePercent;					//ʶ�����
	int iMinCapDiff;					//ץͼ������ֵ
	char cCaliInfo[76];					//���Ʊ궨  20120229��չ
	//20121030��չ
	int	iPlateCharType;					//����ÿһλ��ʶ���ַ�����,
	//��ǰ����Ϊ7λ��ʹ�ú�14λ��ʾÿλ���Ƶ��ַ����ͣ�ÿ��λ��ʾһλ�ַ�����
	//00-���� 01-��ĸ 10-���� 11-��ĸ������
	int iConfid;						//�������Ŷ���ֵ
};

typedef struct tagTITSRecoParam
{
	int iSize;
	ITS_RecoParamEx tRecoPara;
	int iCarNumType;
	int iAngle;	
}TITSRecoParam, *pTITSRecoParam;

//ITS���߲���---�������v3.2
typedef struct __tagTITS_RefLine
{
	int			m_iLineID;
	int			m_iEnable;
	int			m_iLineType;
	vca_TLine	m_stLine1;
	vca_TLine	m_stLine2;				//Ԥ��
	int			m_iDistance;
}TITS_RefLine, *PTITS_RefLine;

typedef struct __tagTITS_ReflineAarry
{
	int			m_iRoadwayID;
	int			m_iLineNum;
	TITS_RefLine m_stLines[MAX_ITS_REFLINE_NUM];
	int iSceneID;// ����ID(0~15) 20140203��չ
}TITS_RefLineAarry, *PTITS_ReflineArray;


#define MAX_VEHICLE_TYPE 3//�����������
typedef struct __tagTITS_ChnCarSpeedPara
{
	int iCarWayNo;						//������ţ����֧��4��������0-3
	int iCarType;						//�������ͣ�0-����⣬1-С����2-��
	int iSpeedHighLimit;				//�����������ޣ�m/s������ʱ����1000��ת���ɱ���3ΪС����
	int iSpeedLowLimit;					//�����������ޣ�m/s������ʱ����1000��ת���ɱ���3ΪС����
	int iHighExceedPercent;				//����ִ���ٷֱȣ�0-100
	int iLowExceedPercent;				//����ִ���ٷֱȣ�0-100
	int iSceneID;						// ����ID(0~15) 20140203��չ
	float fAbnormalHighSpeed;			//�쳣���� �ٶ����Ʋ�������λ����/��(3λС����float��)
	float fAbnormalLowSpeed;			//�쳣���� �ٶ����Ʋ�������λ����/��(3λС����float��)
}TITS_ChnCarSpeedPara, *PTITS_ChnCarSpeedPara;


struct TITSRoadwayParam
{
	int iRoadWayID;

	//modified by wd @20130531 
	//TITSRoadwayInfoEx info;
	TITSRoadwayInfo_V1 RoadWayInfo;
	//modified end

	TITSRoadWayLoop loop;
	TITSRoadwayTime time;
	TITSRoadwaySpeed speed;
	TITSRoadwayReco reco;
	TITSRoadwayVLoop vloop;
	TITSRoadwayCapture capture;
	TITSRoadwayCapture captureNew;
	ITS_RecoParamEx recopara;
	TITSRecoParam   recopara_ex;
	TITSRoadwayLight_V1	light;
	TITS_RefLineAarry   m_stLineArray;	
};

//ShangHai BK  |zyb add 20150304
typedef struct tagTITSRoadwayDelayDistance
{
	int				iSize;
	int				iSceneId;	//����ID(0~15)
	int				iLaneId;	//�������, ���֧��4��������0-3
	int				iSignalDelayDistance;	//ץ���ź��ӳ�ץ�ľ���(cm)��ʱ��(ms)
	int				iDelayCapDistanceS;		//�ڶ���ץ���ӳپ���(cm)��ʱ��(ms)
	int				iDelayCapDistanceT;		//������ץ���ӳپ���(cm)��ʱ��(ms)
	int				iDelayCapDistanceFourth;//������ץ���ӳپ���(cm)��ʱ��(ms)
	int				iDelayCapDistanceFifth;	//������ץ���ӳپ���(cm)��ʱ��(ms)
	int				iDelayCapTypeFirst;		//��1��ץ���ӳ����� 1��ʱ��
	int				iDelayCapTypeSec;		//��2��ץ���ӳ����� 1��ʱ��
	int				iDelayCapTypeTrd;		//��3��ץ���ӳ����� 1��ʱ��
	int				iDelayCapTypeFourth;	//��4��ץ���ӳ����� 1��ʱ�� 
	int				iDelayCapTypeFifth;		//��5��ץ���ӳ����� 1��ʱ��
}TITSRoadwayDelayDistance, *pTITSRoadwayDelayDistance;
//add end

/************************************************************************/
//*	���ܽ�ͨ�����Ż�
/************************************************************************/

#define ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_MIN                   0
#define ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_FIRST_HZ				(ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_MIN + 0)	    //���ó����Ż��׺���
#define ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_NOPLATE_ENABLE		(ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_MIN + 1)		//�����Ƿ��������Ƴ�
#define ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_OTHER					(ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_MIN + 2)		//���ô��������ƺͲ����ų���
#define ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_MAX					(ITS_LICENSEPLATE_OPTIMIZE_CMD_SET_MIN + 3)		//

#define ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_MIN                   0										   		
#define ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_FIRST_CHARACTER       (ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_MIN + 0)		//��ó����Ż��׺���
#define ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_NOPLATE_ENABLE		(ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_MIN + 1)		//����Ƿ��������Ƴ�
#define ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_OTHER					(ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_MIN + 2)		//��ô��������ƺͲ����ų���
#define ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_MAX					(ITS_LICENSEPLATE_OPTIMIZE_CMD_GET_MIN + 3)		//

struct TITSLicensePlateOptimizeFirstHZInfo
{
	char cModifyChar[9];
	int iMinCharConfid;
	char cModifyAlpha[10];
	int iMinAlphaConfig;
};

struct TITSLicesePlateOptimizeOther    //�������ƺͲ����ų���
{
	int iType;                         //���� 0�����������֣�1�������ź���
	int iCount;                        //�׺�����Ŀ ���֧��8��
	char cModifyChar[8][3];            //�׺���    Ӧ���ó���ʡ�����׺��ֻ򲻿��ܳ��ֵĳ����׺��֡�������4���ֽ�
};
typedef struct _TITSLicensePlateOptimizeFirstHZInfo_V1
{
	char cModifyChar[9];
	int iMinCharConfid;
	char cModifyAlpha[10];
	int iMinAlphaConfig;
	int iSize;
	int iSceneId;
	int iChnNo;
}TITSLicensePlateOptimizeFirstHZInfo_V1, *PTITSLicensePlateOptimizeFirstHZInfo_V1;

#define MAX_OPTIMIZEOTHERTYPE_NUM 2    //0�����������֣�1�������ź���
#define MAX_OPTIMIZEOTHERTYPE_SCENE	15	//��󳡾����
struct TITSLicensePlateOptimizeParam
{
	int iEnableNoPlate;
	TITSLicensePlateOptimizeFirstHZInfo_V1  firstHZinfo[MAX_OPTIMIZEOTHERTYPE_SCENE];
	//modify by wd @20130626
	TITSLicesePlateOptimizeOther other[MAX_OPTIMIZEOTHERTYPE_NUM];
};

/************************************************************************/
///*  ���ô��������ƺͲ����ų���//�Ǳ��س��� 
/************************************************************************/
#define  MAX_MODIFYCHAR_COUNT    8
struct ITS_LicenseOptimizeOther
{
	int			iType;      							//0�����������֣�1�������ź���
	int			iCount;									//�׺�����Ŀ ���֧��8��
	char		cModifyChar[MAX_MODIFYCHAR_COUNT][5];	//Ӧ���ó���ʡ�����׺��ֻ򲻿��ܳ��ֵĳ����׺��֡�������4���ֽ�
	int			iOtherCharWeight[MAX_MODIFYCHAR_COUNT];	//�׺����Ż�������Ŷ�
} ;

/************************************************************************/
///*  ��ͨΥ�¼����  
/************************************************************************/
#define ITS_EXTRAINFO_CMD_SET_MIN								 0
//Υͣ��Ŀ
#define ITS_ILLEGALPARK_CMD_SET									(ITS_EXTRAINFO_CMD_SET_MIN + 0)	    //����Υ��ͣ������
#define ITS_ILLEGALPARKPARM_CMD_SET								(ITS_EXTRAINFO_CMD_SET_MIN + 1)		//����Υ��ͣ���궨�������
#define ITS_LICENSEOPTIMIZEOTHER_CMD_SET						(ITS_EXTRAINFO_CMD_SET_MIN + 2)		//���ô��������ƺͲ����ų���
#define ITS_OPTIM_CMD_SET										(ITS_EXTRAINFO_CMD_SET_MIN + 3)		//���ó����Ż��׺���
#define ITS_RECOPARAM_CMD_SET									(ITS_EXTRAINFO_CMD_SET_MIN + 4)		//����ʶ���㷨����
#define ITS_ROADWAYENABLE_CMD_SET								(ITS_EXTRAINFO_CMD_SET_MIN + 5)		//���ó���
#define ITS_CAMLOCATION_CMD_SET									(ITS_EXTRAINFO_CMD_SET_MIN + 6)		//�����豸��Ϣ
#define ITS_EXACTCONFIRM_CMD_SET								(ITS_EXTRAINFO_CMD_SET_MIN + 7)		//���þ�ȷ��λ��Ϣ
//�������
#define ITS_VIDEODECTECT_CMD_SET								(ITS_EXTRAINFO_CMD_SET_MIN + 8)		//ͼ�������
#define ITS_DETECTAREA_CMD_SET									(ITS_EXTRAINFO_CMD_SET_MIN + 10)	//�켣��������������
#define ITS_RECOTYPE_CMD_SET									(ITS_EXTRAINFO_CMD_SET_MIN + 11)	//�㷨���Ͳ���
//��ͨһ���
#define ITS_REDLIGHTDETECTAREA_CMD_SET							(ITS_EXTRAINFO_CMD_SET_MIN + 12)	//���ú�Ƽ���������
#define ITS_ENABLE_CMD_SET										(ITS_EXTRAINFO_CMD_SET_MIN + 13)	//���ý�ͨ��ع���ʹ��Э��
#define ITS_DAYNIGHT_CMD_SET									(ITS_EXTRAINFO_CMD_SET_MIN + 14)	//���ý�ͨ��ع���ʹ��Э��
#define ITS_WORKMODE_CMD_SET									(ITS_EXTRAINFO_CMD_SET_MIN + 15)	//���ù���ģʽ
#define  ITS_LIGHTINFO_CMD_SET                                  (ITS_EXTRAINFO_CMD_SET_MIN + 16)   //�����źŵƶ�Ӧ�Ĳ���
#define ITS_CROSSINFO_CMD_SET                                   (ITS_EXTRAINFO_CMD_SET_MIN + 17)	//����·����Ϣ����
#define ITS_AREAINFO_CMD_SET                                    (ITS_EXTRAINFO_CMD_SET_MIN + 18)    //���������������
#define ITS_ILLEGALPARKINFO_CMD_SET								(ITS_EXTRAINFO_CMD_SET_MIN + 19)	//Υͣ��
#define ITS_SECURITYCODE_CMD_SET								(ITS_EXTRAINFO_CMD_SET_MIN + 20)	//���÷�α��λ��
#define ITS_LIGHTSUPPLY_CMD_SET									(ITS_EXTRAINFO_CMD_SET_MIN + 21)	//�������Ȳ���
#define ITS_CAPTURECOUNT_CMD_SET								(ITS_EXTRAINFO_CMD_SET_MIN + 22)	//ץ������
#define ITS_LINEPRESSPERMILLAGE_CMD_SET							(ITS_EXTRAINFO_CMD_SET_MIN + 23)	//���ó����ڳ�������ѹ��ǧ�ֱȲ���
#define ITS_ITSWORDOVERLAY_CMD_SET								(ITS_EXTRAINFO_CMD_SET_MIN + 24)	//��ͨר���ַ�����
#define ITS_VIDEODECTECT_NEW_CMD_SET							(ITS_EXTRAINFO_CMD_SET_MIN + 25)	//����ͼ�������
#define ITS_RADERINFO_CMD_SET									(ITS_EXTRAINFO_CMD_SET_MIN + 26)	//Set roadway rader information
#define ITS_PICCUT_CMD_SET										(ITS_EXTRAINFO_CMD_SET_MIN + 27)	//Picture Cut Infomation
#define ITS_EXTRAINFO_CMD_SET_MAX								(ITS_EXTRAINFO_CMD_SET_MIN + 28)	//���(�Ѵﵽ��󣬲�������)

#define ITS_EXTRAINFO_CMD_GET_MIN								 0
//Υͣ��Ŀ
#define ITS_ILLEGALPARK_CMD_GET									(ITS_EXTRAINFO_CMD_GET_MIN + 0)	    //���Υ��ͣ������
#define ITS_ILLEGALPARKPARM_CMD_GET								(ITS_EXTRAINFO_CMD_GET_MIN + 1)		//���Υ��ͣ���궨�������
#define ITS_LICENSEOPTIMIZEOTHER_CMD_GET						(ITS_EXTRAINFO_CMD_GET_MIN + 2)		//��ô��������ƺͲ����ų���
#define ITS_OPTIM_CMD_GET										(ITS_EXTRAINFO_CMD_GET_MIN + 3)		//��ó����Ż��׺���
#define ITS_RECOPARAM_CMD_GET									(ITS_EXTRAINFO_CMD_GET_MIN + 4)		//���ʶ���㷨����
#define ITS_ROADWAYENABLE_CMD_GET								(ITS_EXTRAINFO_CMD_GET_MIN + 5)		//��ó���
#define ITS_CAMLOCATION_CMD_GET									(ITS_EXTRAINFO_CMD_GET_MIN + 6)		//����豸��Ϣ
//�������
#define ITS_VIDEODECTECT_CMD_GET								(ITS_EXTRAINFO_CMD_GET_MIN + 7)		//ͼ�������
#define ITS_DETECTAREA_CMD_GET									(ITS_EXTRAINFO_CMD_GET_MIN + 9)		//�켣��������������
#define ITS_RECOTYPE_CMD_GET									(ITS_EXTRAINFO_CMD_GET_MIN + 10)	//�㷨���Ͳ���
#define ITS_CAMRATYPE_CMD_GET									(ITS_EXTRAINFO_CMD_GET_MIN + 11)	//�������
//��ͨһ���
#define ITS_REDLIGHTDETECTAREA_CMD_GET							(ITS_EXTRAINFO_CMD_SET_MIN + 12)	//��ú�Ƽ���������
#define ITS_ENABLE_CMD_GET										(ITS_EXTRAINFO_CMD_SET_MIN + 13)	//��ý�ͨ��ع���ʹ��Э��
#define ITS_VERSIONPRO_CMD_GET									(ITS_EXTRAINFO_CMD_SET_MIN + 14)	//�������汾��Ϣ
#define ITS_DAYNIGHT_CMD_GET									(ITS_EXTRAINFO_CMD_SET_MIN + 15)	//���ý�ͨ��ع���ʹ��Э��
#define ITS_WORKMODE_CMD_GET									(ITS_EXTRAINFO_CMD_GET_MIN + 16)    //��ȡ����ģʽ
#define ITS_LIGHTINFO_CMD_GET                                   (ITS_EXTRAINFO_CMD_GET_MIN + 17)    //��ȡ�źŵƶ�Ӧ�Ĳ���
#define ITS_CROSSINFO_CMD_GET	                                (ITS_EXTRAINFO_CMD_GET_MIN + 18)	//��ȡ·����Ϣ����
#define ITS_AREAINFO_CMD_GET	                                (ITS_EXTRAINFO_CMD_GET_MIN + 19)    //��ȡ�����������
#define ITS_ILLEGALPARKINFO_CMD_GET								(ITS_EXTRAINFO_CMD_GET_MIN + 20)	//Υͣ��
#define ITS_SECURITYCODE_CMD_GET								(ITS_EXTRAINFO_CMD_GET_MIN + 21)	//���÷�α��λ��
#define ITS_LIGHTSUPPLY_CMD_GET									(ITS_EXTRAINFO_CMD_GET_MIN + 22)	//�������Ȳ���
#define ITS_CAPTURECOUNT_CMD_GET								(ITS_EXTRAINFO_CMD_GET_MIN + 23)	//ץ������
#define ITS_LINEPRESSPERMILLAGE_CMD_GET							(ITS_EXTRAINFO_CMD_GET_MIN + 24)	//���ó����ڳ�������ѹ��ǧ�ֱȲ���
#define ITS_ITSWORDOVERLAY_CMD_GET								(ITS_EXTRAINFO_CMD_GET_MIN + 25)	//��ͨר���ַ�����
#define ITS_VIDEODECTECT_NEW_CMD_GET							(ITS_EXTRAINFO_CMD_GET_MIN + 26)	//����ͼ�������
#define ITS_RADERINFO_CMD_GET									(ITS_EXTRAINFO_CMD_GET_MIN + 27)	//Get roadway rader information
#define ITS_PICCUT_CMD_GET										(ITS_EXTRAINFO_CMD_GET_MIN + 28)	//Picture Cut Infomation
#define ITS_EXTRAINFO_CMD_GET_MAX								(ITS_EXTRAINFO_CMD_GET_MIN + 29)	//���(�Ѵﵽ��󣬲�������)

#define ITS_EXTRAINFO_CMD_MIN									0
#define ITS_EXTRAINFO_COMMON_CMD_MIN							(ITS_EXTRAINFO_CMD_GET_MIN + 29)

//ITS 6M zyb add 14.12.05
#define ITS_EXTRAINFO_CMD_TRAFFIC_FLOW							(ITS_EXTRAINFO_COMMON_CMD_MIN + 0)  //ITS Lane Traffic Statistics
#define ITS_EXTRAINFO_CMD_TRAFFICFLOWREPORT						(ITS_EXTRAINFO_COMMON_CMD_MIN + 1)
#define ITS_EXTRAINFO_CMD_ILLEGAL								(ITS_EXTRAINFO_COMMON_CMD_MIN + 2) //ITS Illegal Type
#define ITS_EXTRAINFO_CMD_PICMERGEOVERLAY						(ITS_EXTRAINFO_COMMON_CMD_MIN + 3) //ITS Pictrue Merge Over Lay
#define ITS_EXTRAINFO_CMD_RECOPARAM								(ITS_EXTRAINFO_COMMON_CMD_MIN + 4)
#define ITS_EXTRAINFO_CMD_FTP_UPLOAD							(ITS_EXTRAINFO_COMMON_CMD_MIN + 5)
#define ITS_EXTRAINFO_CMD_CARLOGO_OPTIM							(ITS_EXTRAINFO_COMMON_CMD_MIN + 6)
#define ITS_EXTRAINFO_CMD_LOOP_MODE								(ITS_EXTRAINFO_COMMON_CMD_MIN + 7)
#define ITS_EXTRAINFO_CMD_IPDCAPMODEL							(ITS_EXTRAINFO_COMMON_CMD_MIN + 8)//ipd capture model
#define ITS_EXTRAINFO_CMD_IPDCARPOSITION						(ITS_EXTRAINFO_COMMON_CMD_MIN + 9)//ipd car     position
#define ITS_EXTRAINFO_CMD_MAX									(ITS_EXTRAINFO_COMMON_CMD_MIN + 10)


#define MAX_PRESET_COUNT			16					//Υ��ͣ�����Ԥ��λ��
#define MAX_AREA_COUNT				8					//Υ��ͣ�����������
#define MAX_PARAM_COUNT				4					//Υ��ͣ���ı������������궨λ����
#define MAX_ITS_RULE_NUM			1					//Υ��ͣ����������
/************************************************************************/
///*  ����Υ��ͣ������  
/************************************************************************/
//struct ITS_IllegalPark
//{
//	int			iPresetNo;      						//Ԥ��λ���
//	int			iAreaNo;								//������
//	int		  	iIllegalParkTime;						//Υ��ͣ�����ʱ��
//	int			iTimeRange[MAX_TIMESEGMENT];								//ʹ��ʱ�䷶Χ
//	POINT		arrPTArea[MAX_AREA_COUNT];				//�����ĸ�������  iX1,iY1:iX2,iY2:iX3,iY3:iX4,iY4
//} ;

struct ITS_IllegalPark                                  //��չΥͣ������iCheckParkTime&iSensitivity��@120905
{
	int			iPresetNo;      						//Ԥ��λ���   ���8����0-7
	int			iAreaNo;								//������     ���4����0-3
	int		  	iIllegalParkTime;						//Υ��ͣ�����ʱ��  ��Ϊ��λ
	int			iTimeRange[MAX_TIMESEGMENT];								//ʹ��ʱ�䷶Χ
	POINT		arrPTArea[MAX_AREA_COUNT];				//�����ĸ�������  iX1,iY1:iX2,iY2:iX3,iY3:iX4,iY4
	int         iCheckParkTime;                         //ͣ���ж�ʱ��
	int         iSensitivity;                           //�����ȵȼ�
	char		cAreaName[LEN_32];						//��������
	int			iEnable;								//����ʹ��	0δʹ�� 1ʹ��
	int			iRuleID;								//����ID:0~10������0~7��ͨ�ù���8~10:�������,�������������ʹ��--zyb add 20150530
	int			iValid;									//���¼�����Ƿ���Ч--zyb add 20150530
} ;

/************************************************************************/
//  ����Υ��ͣ���궨������� 
/************************************************************************/
struct ITS_IllegalParkParm
{
	int			iPresetNo;      					    //Ԥ��λ���
	int			iAreaNo;								//������
	POINT       arrPTParam[MAX_PARAM_COUNT][2];         //�궨λ������������ iAX1,iAY1:iAX2,iAY2;iBX1,iBY1:iBX2,BiY2;iCX1,iCY1:iCX2,iCY2;iDX1,iDY1:iDX2,iDY2
} ;

/************************************************************************/
//  ���ô��������ƺͲ����ų���//�Ǳ��س��� 
/************************************************************************/
//#define  MAX_MODIFYCHAR_COUNT    8
//struct ITS_LicenseOptimizeOther
//{
//	int			iType;      							//0�����������֣�1�������ź���
//	int			iCount;									//�׺�����Ŀ ���֧��8��
//	char		cModifyChar[MAX_MODIFYCHAR_COUNT][5];	//Ӧ���ó���ʡ�����׺��ֻ򲻿��ܳ��ֵĳ����׺��֡�������4���ֽ�
//	int			iOtherCharWeight[MAX_MODIFYCHAR_COUNT];	//�׺����Ż�������Ŷ�
//} ;


/************************************************************************/
//*  ���þ�ȷ��λ���� 
/************************************************************************/
struct ITS_ExactConfirm
{
	int			iPresetNo;      						//Ԥ��λ���
	int			iAreaNo;								//������
	int         iLocalPos1;								//�궨����
} ;

/************************************************************************/
//*							�����������				
/************************************************************************/

#define		MAX_ITS_DETECTAREA_NUM		16
#define		MAX_ITS_AREA_POINTNUM		16
//130T�㷨˽�в���
typedef struct __tagTITS_Params130T
{
	int			m_iZoomRate;
	int			m_iVehicleWidth;
	int			m_iConfidenceValve;
	int			m_iProportion;
}TITS_Params130T, *PTITS_Params130T;

//ITSͼ�������
typedef	struct __tagTITS_VDetect
{
	int			m_iEngineType;
	int			m_iEnable;
	int			m_iVedioType;
	int			m_iVedioWidth;
	int			m_iVedioHeight;
	int			m_iVehicleMinSize;
	int			m_iVehicleMaxSize;
	int			m_iModelType;
	int			m_iFrameRateCount;
	TITS_Params130T	m_stParmas130T;
}TITS_VDetect, *PTITS_VDetect;

typedef	struct __tagTITS_VDetect_Ex
{
	int				m_iSize;
	TITS_VDetect	m_tVDetect;
	int				m_iVDetectMotor;	//�ǻ��������ʹ��
	int iSceneID;// ����ID(0~15) 20140203��չ
}TITS_VDetect_Ex, *PTITS_VDetect_Ex;

//ITSͼ�����������
#define  MAX_ITS_DETECTAREAPOINT_NUM  15
typedef struct __tagTITS_DetectArea
{ 
	int			m_iRegionID;                      //������  0~11
	int			m_iEnabled;                       //ʹ�ܱ��  0�������ã�1������ 
	int			m_iPointCount;                    //������� 4~15
	vca_TPoint	m_arrPoint[MAX_ITS_AREA_POINTNUM];//�궨λ�����������꣨���15���㣩 iAX1:iAY1,iAX2:iAY2,iAX3:iAY3,iAX4:iAY4,��
	int iSceneID;// ����ID(0~15) 20140203��չ
}TITS_DetectArea, *LTITS_PDetectArea;

/************************************************************************/
//*  ��ͨ��ع���ʹ�ܲ���
/************************************************************************/

#define MAX_ITS_ENABLE_NUM	16
#define MAX_ITS_POWERINPHASEENABLE_NUM	0
#define MAX_ITS_ITSMANUALENABLE_NUM	1
struct ITS_EnalbeParam	
{
	int iType;	    // �������� 0--��Դͬ������  1--�����ֶ�ģʽ  2--CRCУ�鹦��  3--��Ƶ���ٹ���   4���������ʹ��   5��ftpʹ��
					// 6--ͼ��ϳɹ���  7--����ʶ����  8--����ʶ��ʹ�� 9--��ȫ��ʶ��ʹ�� 10--����ʻ����ʶ���� 11--����ʻ����ʶ����
					// 12--ͼƬ������� 13--����ͼƬ���ʹ�� 14--����ʻ����ͼƬ���ʹ�� 15--����ʻ����ͼƬ���ʹ��
	int iEnable;	// 0-��ʹ�ܣ�1-ʹ��
};

#define	MAX_ITS_REDLIGHT_DETECTAREA_NUM		8

struct ITS_RedLightDetectArea
{
	int iRegionID;	//��������� 0-3
	int iEnabled;	//ʹ�ܱ��	0�������ã�1������					
	RECT rcStandardArea;	//�궨����λ��
	RECT rcReviseArea;		
};

typedef struct __tagITS_RedLightDetectAreaV1
{
	int  iBufSize;                  //��������ṹ��ĳ���(���Զ���)
	int  iRegionID;	                //��Ʊ�� 8��
	int  iEnabled;	                //ʹ�ܱ��	0�������ã�������	
	RECT rcStandardArea;	        //�궨����λ��
	RECT rcReviseArea;		
	int  iLightType;                //�������� 0-���Ƶ��̣�1-���Ƶ��̣�2-��Ƶ��̺죬3-��Ƶ�����
	int  iChannelType;              //��������Ӧ�ĳ������� �������Ͱ�λ����,0-�����ã�1-���ã� bit0-ֱ�� bit1-��ת bit2-��ת bit3-��ͷ
} ITS_RedLightDetectAreaV1, *PITS_RedLightDetectAreaV1;

//add by wd @20130603 ·����Ϣ�ṹ��
struct ITS_CrossInfo
{
	int iBufSize;                //��������ṹ��ĳ���(���Զ���)
	char pcCrossingID[LEN_64];       //·�ڱ��  ���Ȳ�����63�ַ�
	char pcCrossingName[LEN_64];     //·������  ���Ȳ�����63�ַ�
	int iSceneID;// ����ID(0~15) 20140203��չ
	int iUseType;// Ӧ������(0-�����ã�1-Υ��ͣ���� 2-Υ����ͷ�� 3-Υ�����) 20140203��չ
};
//add end

//add by wd @20130620�����������
#define  MAX_AREAINFOPOINT_NUM 6
#define	 MAX_ITS_AREAINFOREGIONID_NUM		8     //������ 
#define  MAX_ITS_AREAINFO_TYPE				2		//��������
struct ITS_AreaInfo
{
	int iBufSize;                 //��������ṹ��ĳ���
	int iType;                    //��������  0���������
	int iRegionID;               //����������ID��0~7
	int iEnabled;                //ʹ�ܱ�� 0�������ã�1������
	int iCount;                  //������� 4~6
	vca_TPoint  stTPoint[MAX_AREAINFOPOINT_NUM];//�궨λ�����������꣨���6���㣩
};
//add end

/************************************************************************
*	�������ʶ���㷨����                                                                     
************************************************************************/
struct ITS_RecoParam 
{
	int iMaxPlateNO;					//�������
	int iMaxPlateWidth;					//����ƿ��
	int iMinPlateWidth;					//��С���ƿ��
};

/************************************************************************
*	�������λ��                                                                     
************************************************************************/
struct ITS_CamLocation
{
	char cDeviceVode[LEN_64];								//�豸���
	char cRoadName[LEN_64];									//·������
};

//	ITS(���ܽ�ͨ)���õı����ܼ�
struct ITS_VariableParam
{
	int m_iAgcBlock[MAX_ITS_TIMERANGE];              //ʱ��β������ʹ��
	int m_iFlashLamp[MAX_ITS_TIMERANGE];             //ʱ��������ʹ��
	int m_iDetectMode;								 //������ģʽ
	int m_iLoopMode;			  				     //��Ȧ����ģʽ,0������Ȧ��1��˫��Ȧ��2������Ȧ
	int m_iITSDeviceType;							 //�����������

	//�������
	int	m_iITSCamraType;
	//����汾��Ϣ
	char m_strITSVersionPro[1024];	
};


#define CAMERA_PARAM_JPEG_QUALITY			0
#define CAMERA_PARAM_LUT					1

struct TCameraParam
{
	int iParamType;		//�������� 0:ͼƬ���� 1:LUT  ����Ԥ��					
	int iParamValue;	//����ֵ iParamType=0ʱ��0-100����ֵԽ������Խ��; iParamType=1ʱ��0:������;1:����		
};

/************************************************************************
* �õ��豸��������Ϣ                                                                     
************************************************************************/
#define MAX_CONNECT_COUNT		128

struct TChannelConnectInfo
{
	int iChannelID;					//	ȡֵ��Χ���豸��ͨ����������������NVSSDK����ͬ��������ͨ����0��ʼ������ͨ������
	char cClientIP[16];				//	�ͻ���IP��ַ
	int iChannelType;				//ͨ�����ͣ�0��������ͨ����1��������ͨ��
	int iNetMode;					//1��TCP�� 2��UDP�� 3���ಥ�� 4������ģʽ
	int iConnectState;				//����״̬��0�����������ӣ�  1����Ͽ�����
	char cUserName[17];				//�û��������16���ַ�
};

struct TTotalConnectInfo
{
	TChannelConnectInfo	connInfo[MAX_CONNECT_COUNT];
};

/************************************************************************
*	ƽ̨��������б�����                                                                     
************************************************************************/
#define MAX_PLATFORM_COUNT			10
#define MAX_PALTFORM_NAME_LENGTH	33

#define PLATFORM_CMD_SET_MIN		0
#define PLATFORM_CMD_SET_RUN		(PLATFORM_CMD_SET_MIN+0)
#define PLATFORM_CMD_SET_MAX		(PLATFORM_CMD_SET_MIN+1)

//#define PLATFORM_CMD_GET_MIN		(PLATFORM_CMD_SET_MAX)			//	�����������չ������			
#define PLATFORM_CMD_GET_MIN		(1)
#define PLATFORM_CMD_GET_LIST		(PLATFORM_CMD_GET_MIN+0)
#define PLATFORM_CMD_GET_MAX		(PLATFORM_CMD_GET_MIN+1)


struct TPlatformVersion
{
	char cData[LEN_32+1];
};

struct TPlatformApp
{
	char cName[MAX_PLATFORM_COUNT][MAX_PALTFORM_NAME_LENGTH];			//	ƽ̨�����������ƣ����10�������������32�ֽ�
	int iState[MAX_PLATFORM_COUNT];										//	����ƽ̨��������״̬, 0-ֹͣ��1-����
};

//ATM��ز���
#define ATM_CONFIG_CMD_MIN							0
#define ATM_CONFIG_CMD_ATM_INFO				(ATM_CONFIG_CMD_MIN+0)
#define ATM_CONFIG_CMD_OSD_INFO				(ATM_CONFIG_CMD_MIN+1)
#define ATM_CONFIG_CMD_PROTOCOL_COUNT		(ATM_CONFIG_CMD_MIN+2)
#define ATM_CONFIG_CMD_PROTOCOL_NAME		(ATM_CONFIG_CMD_MIN+3)
#define ATM_CONFIG_CMD_MAX					(ATM_CONFIG_CMD_MIN+4)

#define ATM_PROTOCOL_NAME_LEN 31
#define ATM_PROTOCOL_NAME_COUNT 40
#define ATM_FIELD_COUNT	5
#define ATM_FIELD_LEN	64
#define ATM_CARD_ID_LEN 32

//ATM�ļ���ѯ��ز���
#define ATM_QUERY_CMD_MIN						0
#define ATM_QUERY_CMD_FIELD				(ATM_QUERY_CMD_MIN+0)
#define ATM_QUERY_CMD_CARD				(ATM_QUERY_CMD_MIN+1)
#define ATM_QUERY_CMD_MAX				(ATM_QUERY_CMD_MIN+2)

//ǰ��¼���ѯ
#define CMD_NETFILE_QUERY_MIN			0
#define CMD_NETFILE_QUERY_FILE			(CMD_NETFILE_QUERY_MIN+0)
#define CMD_NETFILE_QUERY_MAX			(CMD_NETFILE_QUERY_MIN+1)


typedef struct ATM_INFO
{
	int		iSize;									//Size of the structure,must be initialized before used
	int		iInformWay;								//ͨ�ŷ�ʽ 0:����,1:UDP
	char	cProtocolName[ATM_PROTOCOL_NAME_LEN+1];	//Э������ Э�鳧��
	char	cSrcIP[LENGTH_IPV4];					//ATM IP��ַ
	int		iSrcPort;								//ATM �˿ں�
	char	cDestIP[LENGTH_IPV4];					//�豸IP��ַ Ĭ��0
	int		iDestPort;								//�豸�˿ں�
}*PATM_INFO;

typedef struct ATM_OSD_INFO
{
	int				iSize;					//Size of the structure,must be initialized before used
	int 	        iX; 	    			//������
	int             iY; 	    			//������
	unsigned int	uEnable;				//ʹ�� ͨ����λȡ
}*PATM_OSD_INFO;

typedef struct ATM_PROTOCOL_NAME
{
	int		iSize;							//Size of the structure,must be initialized before used
	int		iIndex;							//Э������
	char	cName[ATM_PROTOCOL_NAME_LEN+1];	//Э������
}*PATM_PROTOCOL_NAME;

typedef struct ATM_FIELD_QUERY
{
	int				iSize;										//Size of the structure,must be initialized before used
	int          	iType; 										//¼������ 33:ATM
	int     	    iChannel; 									//ͨ���� ��0��ʼ,0xff��������
	NVS_FILE_TIME	struStartTime; 								//��ʼʱ��
	NVS_FILE_TIME	struStopTime; 								//����ʱ��
	int     	    iPageSize;									//ҳ��С
	int         	iPageNo;									//ҳ���
	int             iFiletype;									//�ļ����� 0:ȫ��,1:��Ƶ,2:ͼƬ
	int				iDevType;									//�豸���� 0:�����,1:������Ƶ������,2:���������,0xff:ȫ��
	char			cOtherQuery[65];							//�ַ�����
	int				iTriggerType;								//�������� 3:�˿ڱ���,4:�ƶ�����,5:��Ƶ��ʧ����,0x7FFFFFFF:��Ч
	int				iTrigger;									//�˿ڣ�ͨ������
	char			cField[ATM_FIELD_COUNT][ATM_FIELD_LEN+1];	//���ѯ
}*PATM_FIELD_QUERY;

typedef struct ATM_CARD_QUERY
{
	int				iSize;					//Size of the structure,must be initialized before used
	int          	iType; 					//¼������ 33:ATM
	int     	    iChannel; 				//ͨ���� ��0��ʼ,0xff��������
	NVS_FILE_TIME	struStartTime; 			//��ʼʱ��
	NVS_FILE_TIME	struStopTime; 			//����ʱ��
	int     	    iPageSize;				//ҳ��С
	int         	iPageNo;				//ҳ���
	int             iFiletype;				//�ļ����� 0:ȫ��,1:��Ƶ,2:ͼƬ
	int				iDevType;				//�豸���� 0:�����,1:������Ƶ������,2:���������,0xff:ȫ��
	char			cOtherQuery[65];		//�ַ�����
	int				iTriggerType;			//�������� 3:�˿ڱ���,4:�ƶ�����,5:��Ƶ��ʧ����,0x7FFFFFFF:��Ч
	int				iTrigger;				//�˿ڣ�ͨ������
	char			cID[ATM_CARD_ID_LEN+1];	//����
	int				iOperate;				//ҵ������ 0:�����ѯ,1:ȡ��,2:����,3:ת��,4:���,5:�޿���ѯ,6:�޿����,20:����
	int				iQuantity;				//���:��������Ŀ���֣�
	int				iAbnormity;				//�쳣��� 0:�̳�,1:�̿�
}*PATM_CARD_QUERY;

typedef struct ATM_FILE_DATA
{
	int				iSize;					//Size of the structure,must be initialized before used
	int             iType; 	        		//¼������ 33:ATM
	int 	        iChannel; 	    		//ͨ���� ��0��ʼ,0xff��������
	char 		    cFileName[250]; 		//�ļ���
	NVS_FILE_TIME	struStartTime; 			//��ʼʱ��
	NVS_FILE_TIME	struStoptime; 			//����ʱ��
	int             iFileSize; 	    		//�ļ���С
	int				iOperate;				//�������� 1:ȡ��,2:����,3:ת��,4:���,5:�޿���ѯ,6:�޿����,20:����,21:�̳�,22:�̿�
	NVS_FILE_TIME	struOperateTime;		//����ʱ��
}*PATM_FILE_DATA;

#define  QUERY_MSG_COUNT	5
#define  QUERY_MSG_LEN		68
typedef struct NETFILE_QUERY_V4
{
	int				iSize;										//Size of the structure,must be initialized before used
	int          	iType; 										//¼������ 33:ATM
	int     	    iChannel; 									//ͨ���� ��0��ʼ,0xff��������
	NVS_FILE_TIME	struStartTime; 								//��ʼʱ��
	NVS_FILE_TIME	struStopTime; 								//����ʱ��
	int     	    iPageSize;									//ҳ��С
	int         	iPageNo;									//ҳ���
	int             iFiletype;									//�ļ����� 0:ȫ��,1:��Ƶ,2:ͼƬ
	int				iDevType;									//�豸���� 0:�����,1:������Ƶ������,2:���������,0xff:ȫ��
	char			cOtherQuery[65];							//�ַ�����
	int				iTriggerType;								//�������� 3:�˿ڱ���,4:�ƶ�����,5:��Ƶ��ʧ����,0x7FFFFFFF:��Ч
	int				iTrigger;									//�˿ڣ�ͨ������
	int				iQueryType;									//��ѯ���� 0: ������ѯ 1:ATM��ѯ 2��ITS��ѯ					
	int				iQueryCondition;							//��ѯ���� 0�������ѯ  1�������Ų�ѯ��2����ͨ��ѯ������
	char			cField[QUERY_MSG_COUNT][QUERY_MSG_LEN];		//��ѯ��Ϣ
}*PNETFILE_QUERY_V4;


//��Ƶ��������ز���
#define AUDIO_SAMPLE_CMD_MIN						0			
#define AUDIO_SAMPLE_CMD_SAMPLE				(AUDIO_SAMPLE_CMD_MIN+0)	//���û��ȡĳͨ���Ĳ�����
#define AUDIO_SAMPLE_CMD_SAMPLE_COUNT		(AUDIO_SAMPLE_CMD_MIN+1)	//���ĳͨ��֧�ֵĲ����ʸ���
#define AUDIO_SAMPLE_CMD_SAMPLE_LIST		(AUDIO_SAMPLE_CMD_MIN+2)	//���ĳͨ��֧�ֵĵڼ��������ʣ���Ӧ�ṹ��ΪAUDIO_SAMPLE
#define AUDIO_SAMPLE_CMD_MAX				(AUDIO_SAMPLE_CMD_MIN+3)

#define MAX_AUDIO_SAMPLE_NUM 16

typedef struct AUDIO_SAMPLE
{
	int				iSize;					//Size of the structure,must be initialized before used
	int				iIndex;					//����������
	int				iSample;				//��Ƶ������
}*PAUDIO_SAMPLE;

/*-------------------------------------------------------*/
//ϵͳ���Ͳ���
#define	SYSTEMTYPE_WORKMODE		0		//	����ģʽ	
#define SYSTEMTYPE_DEVPROD		1		//	�豸������
typedef struct __tagSYSTEMTYPE
{
	int m_iSize;
	int m_iWorkMode;					//	���޸Ĵ˲���ʱ��0x7FFFFFFF
	int m_iDevProducter;
}SYSTEMTYPE, *LPSYSTEMTYPE;


//��ͨ���
//#define MAX_ROADWAY_COUNT           4          //��󳵵���
//#define MAX_LOOP_COUNT              4           //�����������Ȧ��
//#define DAY_OR_NIGHT                2          //0---���죬1---����
//
//#define LIGHT_COUNT							4					//�źŵ���
//#define LIGHT_COM_COUNT						4					//�źŵƴ�����
//#define CAPTURE_PLACE_COUNT					3					//������Ӧ��ץ��λ����Ŀ


struct TImgDisposal	//ͼƬЧ��
{
	int iChannelID;
	int iStartUp;		//ͼƬЧ�������Ƿ����ñ�ʶ, 0�������� 1������
	int iQuality;		//����,	��Χ��0��100
	int iSaturation;	//���Ͷ�,��Χ��0��255
	int iBrighten;		//����,��Χ��0��255
	int iContrast;		//�Աȶ�,��Χ��0��255
	int iGamma;			//٤��ֵ,��Χ��0��50	
	int iAcutance;		//���,��Χ��0��255
	int iImgEnhanceLevel;//ͼ����ǿ����,��Χ��0��3					
};

struct TECOPChannelParam	//������ز���
{
	TImgDisposal tImgDisposal;
	int iRecognizeEnable;
	int iWaterMarkEnable;
};

//�źŵƶ�Ӧ�Ĳ���
struct TITSLightInfo
{
	int iLightID;						//�źŵƱ�� 0-3	
	int iTimeInterval;					//�źŵ�ʱ���� ��λ��	
	int iTimeAcceptDiff;				//�źŵ�ʱ������ֵ ��λ��					
};
//��չ�źŵƲ���
//��չ�źŵƲ���
typedef	struct __tagTITSLightInfoEx
{
	int				iBufSize;
	TITSLightInfo	m_stLightInfo;			//���� �źŵƱ�� 0-7	
	int				iChannelType;			//�źŵƶ�Ӧ�ĳ�������  �������Ͱ�λ����,0-�����ã�1-���ã�bit0-ֱ�� bit1-��ת bit2-��ת bit3-��ͷ
	int				iPhase;					//�����λֵ   ��λֵ 0~360
	int				iDetectType;			//����ƻ����̵� 0����ơ�1���̵�
	int				m_iYellowLightTime;		//�ƵƳ���ʱ��, ��λms��0-100000ms----->add new
	int				m_iLightEnhanceLevel;	//�������ȼ�;���淶Χ��1~255��0-������
}TITSLightInfoEx,LPTITSLightInfoEx;

struct TECOPParam
{
	TECOPChannelParam tChannel[MAX_ROADWAY_COUNT];
	int iSystemType;	//ϵͳ����
	//0������
	//3������
	//4���羯	
	int iLockRet;		//��ȡע����
	//bit0��ʶ��
	//bit1�����ܽ�ͨ���ܹ�
	//0��δע�ᣬ1��ע��
	TITSLightInfo tLightInfo[LIGHT_COUNT];
};

typedef int				VIDEO_METHOD;
#define VIDEO_MODE_HAND			0x00
#define VIDEO_MODE_AUT			0x01

#define MAX_MONITOR_DESCLEN     512
typedef struct MONITOR_INFO
{
	char        cDriverDesc[MAX_MONITOR_DESCLEN];
	char        cDriverName[MAX_MONITOR_DESCLEN];	
	RECT		rect;
}*PMONITOR_INFO;

typedef struct
{
	int         m_iStructSize;        //�ṹ���С
	char        m_cVersion[32];       //�������汾��
	char		m_cUIVersion[32];	  //UI�������汾��
	char		m_cSlaveVersion[32];  //��Ƭ�汾��
	char        m_cPlugInVersion[32]; //��Ұ汾�� 
	char		m_cSCMVersion[32];	  //��Ƭ���汾��
	char 		m_cSCGuiVersion[32];  //UI�汾��
	char		m_cMediaVersion[32];  //T2 Camera Media Version

	char		m_cMethodVersion[32];  //Method Version
	char		m_cCpldVersion[32];  //Cpld Version
	char		m_cDomeDriveVersion[32];  //DomeDrive Version
	char		m_cDigitalMovementVersion[32];  //Digital Movement Version
}SERVER_VERSION;

/************************************************************************/
/*							���ڸ�ʽ����                                */
/************************************************************************/
typedef struct COMFORMAT
{
	int				iSize;				//Size of the structure,must be initialized before used
	int				iComNo;				//���ڱ��
	char			cDeviceName[32];	//Э������
	char			cComFormat[32];		//��ʽ�� 9600,n,8,1
	int				iWorkMode;			//����ģʽ 1:Э�����,2:͸��ͨ��,3:��ҵԤ��,4:7601B���б�������,5:485����
	int				iComType;			//�������� 0:485,1:232,2:422
} *PCOMFORMAT;

/*-------------------------------------------------------*/
//����߼�����
#define	MAX_DOME_PRESET_NUM			255	//	���Ԥ��λ��Ÿ���
#define	MAX_DOME_SCAN_NUM			255	//	���ɨ���Ÿ���
#define	MAX_DOME_CURISE_NUM			255	//	���Ѳ����Ÿ���
#define	MAX_DOME_PATTERN_NUM		255	//	���ģʽ·����Ÿ���
#define	MAX_DOME_ZONES_NUM			255	//	���������ʾ��Ÿ���

#define TITLETYPE_RESERVED			0	//	Ԥ��
#define TITLETYPE_PRESET			1	//	Ԥ��λ
#define TITLETYPE_SCAN				2	//	ɨ��
#define TITLETYPE_CRUISE			3	//	Ѳ��
#define TITLETYPE_PATTERN			4	//	ģʽ·��
#define TITLETYPE_ZONES				5	//	������ʾ

#define	MAX_TITLE_LEN				31

#define	CMD_DOME_ADV_SETTITLE		0	//	�����������
typedef struct __tagDOME_TITLE
{
	int		m_iStructSize;				//	�ṹ��size
	int		m_iType;					//	��������
	int		m_iNum;						//	������
	char	m_cTitle[MAX_TITLE_LEN+1];	//	������
}DOME_TITLE, *LPDOME_TITLE;

/*-------------------------------------------------------*/
//cb@1212068124: SET GET��CMDʹ��ͬһ�����֣�ʹ����ɶ��Ժ������Ը��ã�
#define IPC_CMD_MIN 0
#define IPC_CMD_SENSOR_VOLTAGE       (IPC_CMD_MIN + 0)
#define IPC_CMD_MAX                  (IPC_CMD_MIN + 1)

typedef struct  
{
	int		iType; 
	int		iMold;
	float   fValue;
	int     iReserved;
}HardWare_Param;

typedef struct
{
	unsigned int uiTextColor;
	unsigned char ucOSDText[LENGTH_CHANNEL_TITLE+1];
}OSD_TEXT;

//���ռƻ���ز���
#define HOLIDAY_PLAN_CMD_MIN						0			
#define HOLIDAY_PLAN_CMD_SCHEDULE			(HOLIDAY_PLAN_CMD_MIN+0)	//���û��ȡ���ռƻ�¼��ģ��
#define HOLIDAY_PLAN_CMD_MAX				(HOLIDAY_PLAN_CMD_MIN+1)
#define MAX_HOLIDAY_PLAN_SCHEDULE					10

typedef struct
{
	unsigned short iYear;
	unsigned short iMonth;
	unsigned short iDay;
	unsigned short iWeekOfMonth;
	unsigned short iDayOfWeek;
}HOLIDAY_TIME;

typedef struct
{
	int iPlanID;		//�ƻ�ģ��ID,[0,9]
	int iEnable;		//�Ƿ�ʹ��,1:ʹ�� 0:��ʹ��
	char cPlanName[33]; //�ƻ�ģ������
	int iMode;			//ʱ���ʽ,0:������(������) 1:������(���� ����) 2:���·�(����)
	HOLIDAY_TIME tStartTime;	//��ʼʱ��
	HOLIDAY_TIME tEndTime;		//����ʱ��
}HOLIDAYPLAN_SCHEDULE;

//M7.6֮���豸���صĴ���ID		EC:ERROR CODE
//����Ƶ
#define EC_AV_SWITCHOVER_CHANNEL			0x1000	//������ͨ�����л�ͨ������ʧ��
#define EC_AV_QUERY_FILE					0x1010	//���ϵ���������ѯ���������ļ�
//�ַ�����
#define EC_OSD_LOGO_FORMAT					0x2001	//���ַ����ӣ������logoͼƬ��ʽ����
#define EC_OSD_LOGO_DATA					0x2002	//���ַ����ӣ������logoͼƬ���ݴ���
//����PTZ
#define EC_SERIAL_PORT_PTZ					0x3000
//�쳣
#define EC_EXCEPTION						0x4000
//����
#define EC_ALARM							0x5000
//�洢
#define EC_STORAGE_LOCK_FILE				0x6000	//���ӽ����������ļ�ʧ��
#define EC_STORAGE_UNLOCK_FILE				0x6001	//���ӽ����������ļ�ʧ��
#define EC_STORAGE_REDUNDANCY				0x6010	//���洢���ԣ�û������������ԣ��޷���������¼��
#define EC_STORAGE_HOLIDAY_TIME				0x6020	//�����ռƻ�������ģ�����ʱ���ͻ
//�������
#define EC_NET_SERVICE						0x7000	
//�û�
#define EC_LOGON_USERNAME					0x8000	//����¼���û���������
#define EC_LOGON_PASSWORD					0x8001	//����¼���������
#define EC_LOGON_DECRYPT_PWD				0x8002	//����¼���������벻��
#define EC_LOGON_AUTHORITY					0x8010	//���������û�Ȩ�޲���
#define EC_LOGON_AUTHORITY_GROUP			0x8020	//���޸��û�Ȩ�ޣ��û������鲻�����������ø�Ȩ��
//PU����
#define EC_PU								0x9000
//���̹���
#define EC_DISK_RECORDING					0xA000	//(��;)����¼���޷��޸Ĵ���
#define EC_DISK_MEMORY						0xA001	//(��;)������¼����̲���
#define EC_DISK_TYPE						0xA002	//(��;)�������Ͳ�֧�ִ���;
#define EC_DISK_NO_DISK						0xA010	//(���飩������û��¼����
#define EC_DISK_NO_SETTING					0xA011	//�����飩������δ�����κδ���
#define EC_DISK_REPEAT_ADD					0xA012	//�����飩�ظ����ͬһ���̵���ͬ����
#define EC_DISK_NO_RECORD_CHANNEL			0xA013	//�����飩��������δ����κ�¼��ͨ��
#define EC_DISK_RECORD_QUOTA_BIG			0xA020	//����¼��������
#define EC_DISK_IMAGE_QUOTA_BIG				0xA021	//����ͼƬ������
#define EC_DISK_DEL_LOCK_FILE				0xA022	//�����޸�������ʱ���޷�ɾ�������ļ�
//ϵͳ����
#define EC_SYSTEM							0xB000	
//ͨ��
#define EC_COM								0xC000
//��������
#define EC_NET_CONNECTION					0xD000	
//���ܷ���
#define EC_VCA								0xE000	
#define EC_VCA_UNSUPPORT_SIZE				0xE001	//����ͨ�����ܷ�������Ŀǰ��֧�ֵķֱ���
#define EC_VCA_VDEC_VENC_FAILED				0xE002  //����ͨ���������ܷ��������ͨ��ʧ��
#define EC_VCA_VCPRIOR_FAILED				0xE003  //vcͨ���Ѿ��������޷���������ͨ���������ܷ���
#define EC_VCA_UNSUPPORT_FAILED				0xE004  //��֧�����ñ������ܷ���
#define EC_VCA_MAX2CHN_FAILED				0xE005  //�������ܷ���ֻ�ܿ���ǰ2·
#define EC_VCA_UNSUPPORT_H265_FAILED		0xE006  //�������ܷ�����֧��H265	
#define EC_VCA_PREVIEW_FAILED				0xE007  //Ԥ���������޲�֧�ֿ����������ܷ���
//ATM
#define EC_ATM								0xF000	
//��ͨ���
#define EC_ITS								0x10000	
//DZ���
#define EC_DZ								0x11000	
//ͨ��ʹ��
#define EC_COMMON_ENABLE					0x12000


//M7.6֮���豸����ͨ��ʹ��ID		CI:COMMON ID
#define CI_ALARM_MOTION_DETECT				0x5000
#define CI_ALARM_VIDEO_COVER				0x5001

#define CI_NET_SERVICE_SNMP					0x7000
#define CI_NET_SERVICE_SMTP					0x7001

#define CI_STORAGE_DISK_GROUP				0xA000
#define CI_STORAGE_DISK_QUOTA				0xA001
#define CI_COMMON_ID_RAID					0xA002	//RAID����ʹ��״̬

#define CI_VEHICLE_GPS_CALIBRATION			0x12001
#define CI_VEHICLE_ALARM_THRESHOLD			0x12002
#define CI_VEHICLE_SHUTDOWN_THRESHOLD		0x12003

#define CI_NET_CONNECT_INFO					0xD001  //�ײ�socket���ͷְ������Ԫ
#define CI_NET_PUBLIC_NETWORK				0xD002	//public network state

#define CI_COMMON_ID_EIS					0x1001	//���ӷ���
#define CI_COMMON_ID_SVC					0x1002	//svc
#define CI_COMMON_ID_VO_ENABLE				0x1003	//Video Output Control
#define CI_COMMON_ID_DENOISE				0x1004	//audio denoise	 |zyb add 20141223
#define CI_COMMON_ID_TRENDS_ROI				0x1005	//trends ROI	 |zyb add 20150304

#define CI_COMMON_ID_VIDEO_REVERSE			0x1006	//video reverse			
 
#define CI_COMMON_ID_ANR					0x12010	//ANRʹ��
#define CI_COMMON_ID_WORKDEV_BACKUP			0x12011	//�������ȱ�ʹ��
#define CI_COMMON_ID_FORBIDCHN				0x12012	//����ͨ����
#define CI_COMMON_ID_EVENT					0x12013	//�¼�ģ��ʹ��
#define CI_COMMON_ID_TELNET					0x12014	//telnetʹ��
#define CI_COMMON_ID_DISK					0x12015	//���̵��̺ͷ���״̬
#define CI_COMMON_ID_VCACARCHECK				0x12016 //�������ʹ��

#define CI_HASH_CHECK						0x6002	//��ϣУ��,0-��ʹ�ܣ�1-ʹ��
#define CI_COMMON_ID_MAKE_AUDIO_FILE		0x6003	//��Ƶ�ļ�����ʹ�ܣ���ͨ���й�
#define CI_COMMON_ID_BURN_AUDIO_FILE		0x6004	//��Ƶ�ļ���¼ʹ��
#define CI_COMMON_ID_BACK_AUDIO_FILE		0x6005	//��Ƶ�ļ�����ʹ��
#define CI_COMMON_ID_BURNFILE_FORMAT		0x6006	//Video Record File Format

#define CI_COMMON_ID_RTSP_CHECK				0x7002 	//RTSPУ��ʹ��

#define CI_COMMON_ID_FAN_CONTROL			0xB000	//����Զ�̿���
#define CI_COMMON_ID_FAN_TEMP_CONTROL		0xB001	//�����¿أ�ʵ����ֵ���¶�ȡֵ-100��100�����ȡֵ���1000֮�����10������0.1��
													//����-50.5 ���϶ȣ� -50.5��10 + 1000 = 495�������緢��495
#define CI_COMMON_ID_WHTITELIGHT_ENABLE		0xB002	//white light is enable
//OSD
#define CI_COMMON_ID_OSD_DOT_MATRIX			0x2001	
#define CI_COMMON_ID_OSD_VECTOR				0x2002	
//ITS 6M zyb add 14.12.05
#define CI_ITS_COMMON_ID					0x10000
#define CI_COMMON_ID_ITS_SIGNAL_LIGHT		0x10001
#define CI_COMMON_ID_ITS_BUSINESS			0x10002	//ITS Business Enable
#define CI_COMMON_ID_ITS_ILLEGAL_PARK		0x10003	//ITS Illegal parking

#define CI_COMMON_ID_NVR_LOCAL_VCA			0xE000	//NVR Local VCA 

//add by wd @20130621
//���ץ�ķֱ��ʽṹ�� 
struct JPEGSizeInfo
{
	int iBufSize;                 //��������ṹ��ĳ���
	int iChannelNo;              //ͨ���� 0~n-1
	int iWidth;                   //ץ��ͼƬ��  ������ֵ
	int iHeight;                 //ץ��ͼƬ��  �ߣ�����ֵ
};
//add end

//������ز���
#define DOWNLOAD_FLAG_MIN							0
#define DOWNLOAD_FLAG_FIRST_REQUEST		(DOWNLOAD_FLAG_MIN+0)
#define DOWNLOAD_FLAG_OPERATE_RECORD	(DOWNLOAD_FLAG_MIN+1)
#define DOWNLOAD_FLAG_BREAK_CONTINUE	(DOWNLOAD_FLAG_MIN+2)
#define DOWNLOAD_FLAG_MAX				(DOWNLOAD_FLAG_MIN+3)

#define DOWNLOAD_CMD_MIN							0
#define DOWNLOAD_CMD_FILE				(DOWNLOAD_CMD_MIN+0)
#define DOWNLOAD_CMD_TIMESPAN			(DOWNLOAD_CMD_MIN+1)
#define DOWNLOAD_CMD_CONTROL			(DOWNLOAD_CMD_MIN+2)
#define DOWNLOAD_CMD_FILE_CONTINUE		(DOWNLOAD_CMD_MIN+3)
#define DOWNLOAD_CMD_GET_FILE_COUNT		(DOWNLOAD_CMD_MIN+4)
#define DOWNLOAD_CMD_GET_FILE_INFO		(DOWNLOAD_CMD_MIN+5)
#define DOWNLOAD_CMD_MAX				(DOWNLOAD_CMD_MIN+6)

#define DOWNLOAD_FILE_FLAG_MULTI		0
#define DOWNLOAD_FILE_FLAG_SINGLE		1	

typedef struct
{
	int				m_iSize;				//�ṹ���С
	char            m_cRemoteFilename[255];	//ǰ��¼���ļ���
	char			m_cLocalFilename[255];	//����¼���ļ���
	int				m_iPosition;			//�ļ���λʱ,���ٷֱ�0��100;�ϵ�����ʱ��������ļ�ָ��ƫ����
	int				m_iSpeed;				//1��2��4��8�������ļ������ٶ�, 0-��ͣ
	int				m_iIFrame;				//ֻ��I֡ 1,ֻ����I֡;0,ȫ������					
	int				m_iReqMode;				//�������ݵ�ģʽ 1,֡ģʽ;0,��ģʽ					
	int				m_iRemoteFileLen;		//��������ļ�����Ϊ�գ��˲�����Ϊ��
	int				m_iVodTransEnable;		//ʹ��
	int				m_iVodTransVideoSize;	//��Ƶ�ֱ���
	int				m_iVodTransFrameRate;   //֡��
	int				m_iVodTransStreamRate;  //����
}DOWNLOAD_FILE;

typedef struct
{
	int				m_iSize;				//�ṹ���С
	char			m_cLocalFilename[255];	//����¼���ļ���
	int				m_iChannelNO;			//ͨ����
	NVS_FILE_TIME	m_tTimeBegin;			//��ʼʱ��
	NVS_FILE_TIME	m_tTimeEnd;				//����ʱ��
	int				m_iPosition;			//��ʱ��㶨λ��>100
	int				m_iSpeed;				//1��2��4��8�������ļ������ٶ�, 0-��ͣ
	int				m_iIFrame;				//ֻ��I֡ 1,ֻ����I֡;0,ȫ������
	int				m_iReqMode;				//�������ݵ�ģʽ 1,֡ģʽ;0,��ģʽ
	int				m_iVodTransEnable;		//ʹ��
	int				m_iVodTransVideoSize;	//��Ƶ�ֱ���
	int				m_iVodTransFrameRate;   //֡��
	int				m_iVodTransStreamRate;  //����
	int				m_iFileFlag;			//0:���سɶ���ļ� 1:���سɵ����ļ�
}DOWNLOAD_TIMESPAN;

typedef struct
{
	int				m_iSize;				//�ṹ���С
	int				m_iPosition;			//0��100����λ�ļ�����λ�ã�-1�������ж�λ
	int				m_iSpeed;				//1��2��4��8�������ļ������ٶ�, 0-��ͣ
	int				m_iIFrame;				//ֻ��I֡ 1,ֻ����I֡;0,ȫ������
	int				m_iReqMode;				//�������ݵ�ģʽ 1,֡ģʽ;0,��ģʽ
}DOWNLOAD_CONTROL;

typedef struct
{
	int				m_iBufSize;				//�ṹ���С
	int				m_iFileIndex;			//�ļ��������
	char			m_cFilenNme[255];		//����¼���ļ���
	long long		m_lFileSize;			//�ļ���С,��λ���ֽ�
	NVS_FILE_TIME	m_tTimeBeg;				//��ʼʱ��
	NVS_FILE_TIME	m_tTimeEnd;				//����ʱ��
}DOWNLOAD_FILEINFO;

//�Զ����������Э��
#define CMD_AUTOTEST_MIN                    0
#define	CMD_AUTOTEST_RESERVED 				(CMD_AUTOTEST_MIN+0)      //Ԥ��
#define	CMD_AUTOTEST_SETMACADDR				(CMD_AUTOTEST_MIN+1)	  //����MAC��ַ�� 
#define	CMD_AUTOTEST_LAN					(CMD_AUTOTEST_MIN+2)	  //��������
#define	CMD_AUTOTEST_AUDIOIN				(CMD_AUTOTEST_MIN+3)      //������Ƶ���룻
#define	CMD_AUTOTEST_VIDEOIN				(CMD_AUTOTEST_MIN+4)	  //������Ƶ���룻
#define	CMD_AUTOTEST_AUDIOOUT				(CMD_AUTOTEST_MIN+5)	  //������Ƶ�����
#define	CMD_AUTOTEST_VIDEOOUT				(CMD_AUTOTEST_MIN+6)	  //������Ƶ�����
#define	CMD_AUTOTEST_MICIN					(CMD_AUTOTEST_MIN+7)	  //����MIC���룻
#define	CMD_AUTOTEST_SPEAKER				(CMD_AUTOTEST_MIN+8)	  //����SPEAKER�����
#define	CMD_AUTOTEST_VIDEOADJUST			(CMD_AUTOTEST_MIN+9)	  //������Ƶ���ڣ�Э��Ϊ��AUTOTEST_ADADJUST
#define	CMD_AUTOTEST_USB					(CMD_AUTOTEST_MIN+10)     //����USB�ڣ�
#define	CMD_AUTOTEST_SATA					(CMD_AUTOTEST_MIN+11)	  //����SATA�ڣ�
#define	CMD_AUTOTEST_ALARMIN				(CMD_AUTOTEST_MIN+12)	  //���Ա�������˿ڣ�
#define	CMD_AUTOTEST_ALARMOUT				(CMD_AUTOTEST_MIN+13)	  //���Ա�������˿ڣ�
#define	CMD_AUTOTEST_SERIAL					(CMD_AUTOTEST_MIN+14)	  //���Դ��ڣ�
#define	CMD_AUTOTEST_RTC			   		(CMD_AUTOTEST_MIN+15)	  //����ʱ��оƬ��
#define	CMD_AUTOTEST_VGADETECTIVE   		(CMD_AUTOTEST_MIN+16)	  //����VGA�ź���⣻�豸��
#define	CMD_AUTOTEST_HDMIDETECTIVE			(CMD_AUTOTEST_MIN+17)	  //����HDMI�ź���⣻�豸��
#define	CMD_AUTOTEST_RESETDETECTIVE			(CMD_AUTOTEST_MIN+18)	  //���Ը�λ�ź���⣻�豸��
#define	CMD_AUTOTEST_FORMATDISK				(CMD_AUTOTEST_MIN+19)	  //��ʽ�����̣�
#define	CMD_AUTOTEST_BACKUPSYSTEM			(CMD_AUTOTEST_MIN+20)	  //����ϵͳ��
#define	CMD_AUTOTEST_ENABLEGUIDE			(CMD_AUTOTEST_MIN+21)	  //���ÿ����򵼣�

#define CMD_AUTOTEST_IRIS					(CMD_AUTOTEST_MIN+22)	  //��ȦУ����
#define CMD_AUTOTEST_BADPOINTS				(CMD_AUTOTEST_MIN+23)	  //����У����
#define CMD_AUTOTEST_IRCUT					(CMD_AUTOTEST_MIN+24)	  //IRCUTУ����
#define CMD_AUTOTEST_SDCARD					(CMD_AUTOTEST_MIN+25)	  //SD��У����
#define CMD_AUTOTEST_VERIFYTIME				(CMD_AUTOTEST_MIN+26)	  //ʱ��У����
#define CMD_AUTOTEST_CALIBRATEVIDEOBRIGHTNESS	(CMD_AUTOTEST_MIN+27) //���Ȳ���У����
#define CMD_AUTOTEST_POWER_SYNC				(CMD_AUTOTEST_MIN+28)	  //��Դͬ����⣻
#define AUTOTEST_AGGING_RESULT				(CMD_AUTOTEST_MIN+29)	  //�豸�ϻ������
#define AUTOTEST_CREATKEYFILE				(CMD_AUTOTEST_MIN+30)	  //���Key�ļ���
#define CMD_AUTOTEST_ANALOG					(CMD_AUTOTEST_MIN+31)	  //ģ�������ԣ�

#define AUTOTEST_FOCUSLIGHT					(CMD_AUTOTEST_MIN+32)	  //�۽�ָʾ�Ʋ��ԣ�
#define AUTOTEST_WHITELIGHT					(CMD_AUTOTEST_MIN+33)	  //�׹�Ƶ��ԣ�
#define AUTOTEST_LINEIN						(CMD_AUTOTEST_MIN+34)	  //����LineIn��
#define AUTOTEST_DC							(CMD_AUTOTEST_MIN+35)	  //����DC��Ȧ��
#define AUTOTEST_PIRIS						(CMD_AUTOTEST_MIN+36)	  //����P-iris��
#define AUTOTEST_CAMERARESET				(CMD_AUTOTEST_MIN+37)	  //���Ծ�ͷ��λ��
#define AUTOTEST_SWITCHLAN					(CMD_AUTOTEST_MIN+38)     //�л�UI���ԣ�
#define AUTOTEST_WIFI						(CMD_AUTOTEST_MIN+39)     //�������ߣ�
#define AUTOTEST_KEYPRESS					(CMD_AUTOTEST_MIN+40)     //�����������

#define AUTOTEST_VIDEOSIZE					(CMD_AUTOTEST_MIN+41)	  //���ֱ��ʵ��ԣ�
#define AUTOTEST_IICLINE					(CMD_AUTOTEST_MIN+42)	  //I2C��·���ԣ�
#define AUTOTEST_INSIDELIGHT				(CMD_AUTOTEST_MIN+43)	  //���õƲ��ԣ�
#define AUTOTEST_OUTSIDELIGHT				(CMD_AUTOTEST_MIN+44)	  //����Ʋ��ԣ�
#define AUTOTEST_ZIGBEE						(CMD_AUTOTEST_MIN+45)	  //Zigbee�����⣻
#define AUTOTEST_TEMPERATURE				(CMD_AUTOTEST_MIN+46)	  //�¶ȣ�
#define	CMD_AUTOTEST_END					(CMD_AUTOTEST_MIN+100)	  //���Խ�����
#define	CMD_AUTOTEST_MAX					(CMD_AUTOTEST_MIN+101)

#define MAX_AUTOTEST_ID     CMD_AUTOTEST_MAX

typedef struct
{
	int     iTestParam[5];
	char    cTestParam[65];
}strAutoTest_Para;

#define MAX_HD_TEMPLATE_NUMBER	8

//�û��Զ����������
#define STREAMDATA_CMD_MIN		                0
#define	STREAMDATA_CMD_USER_DEFINE				STREAMDATA_CMD_MIN + 1
#define STREAMDATA_CMD_MAX						STREAMDATA_CMD_MIN + 1
#define MAX_INSERT_STREAM_LEN					64

#define INSERTDATA_FLAG_MIN						0
#define INSERTDATA_MAIN_STRAM					INSERTDATA_FLAG_MIN + 1
#define INSERTDATA_SUB_STRAM					INSERTDATA_FLAG_MIN + 2
#define INSERTDATA_MAIN_SUB						INSERTDATA_FLAG_MIN + 3
#define INSERTDATA_FLAG_MAX						INSERTDATA_FLAG_MIN + 4
typedef struct __tagTStreamData
{
	int  iSize;				//�ṹ���С
	char cStreamData[MAX_INSERT_STREAM_LEN];
	int  iStreamLen;
} TStreamData, *PTStreamData;

#define DOME_PTZ_TYPE_MIN  1
#define DOME_PTZ_TYPE_PRESET_FREEZE_UP  		(DOME_PTZ_TYPE_MIN +0)
#define DOME_PTZ_TYPE_AUTO_FLIP 	 			(DOME_PTZ_TYPE_MIN +1)
#define DOME_PTZ_TYPE_PRESET_SPEED_LEVE			(DOME_PTZ_TYPE_MIN +2)
#define DOME_PTZ_TYPE_MANUL_SEPPD_LEVEL			(DOME_PTZ_TYPE_MIN +3)
#define DOME_PTZ_TYPE_WAIT_ACT             		(DOME_PTZ_TYPE_MIN +4)
#define DOME_PTZ_TYPE_INFRARED_MODE 			(DOME_PTZ_TYPE_MIN +5)
#define DOME_PTZ_TYPE_SCALE_ZOOM				(DOME_PTZ_TYPE_MIN +6)
#define DOME_PTZ_TYPE_LINK_DISJUNCTOR 			(DOME_PTZ_TYPE_MIN +7)
#define DOME_PTZ_TYPE_MAX			 			(DOME_PTZ_TYPE_MIN +8)
typedef struct
{
	int iSize;
	int iType;       		//����
	int iAutoEnable; 		//0--��ʹ�ܣ�1--ʹ��
	int iParam1;			//����
							//1--����Ԥ��λ���᣺δʹ��
							//2--�����Զ���ת��δʹ��
							//3--Ԥ��λ�ٶȵȼ���0--�ͣ�1--�У�2--��
							//4--�ֿ��ٶȵȼ���0--�ͣ�1--�У�2--��
							//5--���ô���������������ֵ��30��60��300��600��1800����λ���룩
							//6--�����ģʽ��0--�ֶ���1--�Զ�
	int iParam2;    		//����
							//1--����Ԥ��λ���᣺δʹ��
							//2--�����Զ���ת��δʹ��
							//3--Ԥ��λ�ٶȵȼ���δʹ��
							//4--�ֿ��ٶȵȼ���δʹ��
							//5--���ô���������0--Ԥ��λ��1--ɨ�裬2--Ѳ����3--ģʽ·��
							//6--�����ģʽ��"0--�ֶ�ʱ�������������ȣ�������ֵ��[0,10]��
								//1--�Զ�ʱ�����������\���ȣ�������ֵ��0-��ͣ�1-�ϵͣ�2-��׼��
								//3-�ϸߣ�4-��ߣ�"
	int iParam3;	//Ԥ��
	int iParam4;	//Ԥ��	
}DOMEPTZ;

/*****************************************************************************/
#define GET_USERDATA_INFO_MIN                            (0)                           
#define GET_USERDATA_INFO_TIME							 (GET_USERDATA_INFO_MIN )		//����û������е�ʱ����Ϣ
#define GET_USERDATA_INFO_OSD							 (GET_USERDATA_INFO_MIN +1)		//����û������е�OSD��Ϣ
#define GET_USERDATA_INFO_GPS                            (GET_USERDATA_INFO_MIN +2)		//����û������е�GPS��Ϣ
#define GET_USERDATA_INFO_VCA                            (GET_USERDATA_INFO_MIN +3)		//����û������е�VCA��Ϣ
#define GET_USERDATA_INFO_USER_DEFINE                    (GET_USERDATA_INFO_MIN +4)		//����û������е�VCA��Ϣ
#define GET_USERDATA_INFO_MAX                            (GET_USERDATA_INFO_MIN +4)	
/*****************************************************************************/

//�㲥��ϢID
#define MSG_VEHICLE_GPS_CALIBRATION				0x12001		//GPSУʱ iMsgValue(0:����,1:����)
#define MSG_VEHICLE_VOLTAGE						0x12002		//iMsgValue(��ʾ��ѹֵ)
#define MSG_VEHICLE_TEMPERATURE					0x12003		//iMsgValue(��ʾ�¶�ֵ)
#define MSG_VEHICLE_SATELLITE_NUM				0x12004		//iMsgValue(��ʾ���Ǹ���)
#define MSG_VEHICLE_SIGNAL_INTENSITY			0x12005		//iMsgValue(��ʾGPS�ź�ǿ��)
#define MSG_VEHICLE_GPS_MODULE_TYPE				0x12006		//GPSģ������ iMsgValue(0:��ģ��,1:GPS,2:����)
#define MSG_VEHICLE_ALARM_THRESHOLD				0x12007		//��ѹ������ֵ
#define MSG_VEHICLE_SHUTDOWN_THRESHOLD			0x12008		//��ѹ�ػ���ֵ

#define MSG_VALUE_MAX_LEN 64
typedef struct  
{
	int iMsgID;								//��ϢID
	int iChannelNo;							//ͨ����
	int iMsgValue;							//��Ϣ��������
	char cMsgValue[MSG_VALUE_MAX_LEN];		//��Ϣ��������
}BROADCAST_MSG,*PBROADCAST_MSG;

//ģ������ID
#define MODULE_CAP_MIN						(0) 
#define MODULE_CAP_VEHICLE			(MODULE_CAP_MIN+0)
#define MODULE_CAP_MAX				(MODULE_CAP_MIN+1)

//����ģ������
#define MODULE_CAP_VEHICLE_GPS_OVERLAY			0x1
#define MODULE_CAP_VEHICLE_POWER_DELAY			0x2
#define MODULE_CAP_VEHICLE_ALARM_THRESHOLD		0x4
#define MODULE_CAP_VEHICLE_SHUTDOWN_THRESHOLD	0x8

//ģ��485���̿���ָ��
#define KEYBOARD_CTRL_MIN			0
#define KEYBOARD_CURRENT_SCREEN		KEYBOARD_CTRL_MIN + 0 	//����UIѡ�д��ڣ�ValueΪUI���ں�
#define KEYBOARD_SINGLE_SCREEN 		KEYBOARD_CTRL_MIN + 1 	//������ʾUI���ڣ�ValueԤ��
#define KEYBOARD_PRESET_CALL		KEYBOARD_CTRL_MIN + 2 	//�ӵ�����ʾ�ָ���Valueδʹ��
#define KEYBOARD_SCREEN_RRSTORE 	KEYBOARD_CTRL_MIN + 3 	//����Ԥ��λ,ValueΪԤ��λ��
#define KEYBOARD_CTRL_MAX			KEYBOARD_CTRL_MIN + 4

#define TTEMPLATE_CMD_MIN				0
#define TTEMPLATE_CMD_SMART	(TTEMPLATE_CMD_MIN+0)   //��������¼��ģ��
#define TTEMPLATE_CMD_VIDEO	(TTEMPLATE_CMD_MIN+1)   //��������¼�����ģ��
#define TTEMPLATE_CMD_MAX	(TTEMPLATE_CMD_MIN+2)

#define SMART_TTEMPLATE_MAX_NUM	4
typedef struct  
{
	int iWeek;								//�����յ�������Ϊ����iWeekday=7��������ռƻ�
	int iIndex[SMART_TTEMPLATE_MAX_NUM];		//ģ����1-8��-�����ã�-8����ģ��-ģ��
}SMART_TEMPLATE,*PSMART_TEMPLATE;

typedef struct  
{
	int iIndex;			//ģ����
	int iType;			//��������
	int iValue;			//����ֵ
}VIDEO_TEMPLATE,*PVIDEO_TEMPLATE;

//*****VIDEO_TEMPLATE��Ӧ��type��value******/
// 1--���뷽ʽ	0��H.264����M-JPEG����MPEG4
// 2--����Profile 0:baseline,1:main,2:high
// 3--�ֱ���	����ֱ���ҳ
// 4--֡��	1��
// 5--ѹ��ģʽ	0��CBR����VBR
// 6--����	��λΪKBytes/s����kbps�����ʣ��˴�Ӧ��(����)
// 7--��������	4����ԽС����Խ��
// 8--I֡֡��	0-100
// 9--��������	3������Ƶ������������Ƶ��������Ƶ��
// 10--��Ƶ�����㷨	AUDIO_CODEC_FORMAT_G711A   = 1,   /*G.711 A��*/
//					AUDIO_CODEC_FORMAT_G711Mu  = 2,   /*G.711 Mu��*/
//					AUDIO_CODEC_FORMAT_ADPCM   = 3,   /*ADPCM */
//					AUDIO_CODEC_FORMAT_G726    = 4,   /*G.726 */
//					AUDIO_CODEC_FORMAT_AAC_LC  = 22---31,   /*AAC */Ԥ��һ�������AAC������չ
//					AUDIO_CODEC_FORMAT_BUTT

//add by 20131022
#define DEVICE_STATE_MIN            0
#define DEVICE_STATE_BRIGHT			(DEVICE_STATE_MIN)    //����
#define DEVICE_STATE_ACUTANCE		(DEVICE_STATE_MIN+1)  //���
#define DEVICE_STATE_SYSTEM			(DEVICE_STATE_MIN+2)  //ϵͳ
#define DEVICE_STATE_MAX			(DEVICE_STATE_MIN+2)  

#define NVS_TIME NVS_FILE_TIME
typedef struct __tagSystemState
{
	int iSize;
	int iCamera;				//�����״̬
	int iHLimit;				//ˮƽ��λ
	int iVLimit;				//��ֱ��λ
	int iInterface;				//�ӿ�ģ��
	int iTSensor;				//�¶ȴ�����
	int temperature;			//����¶�
	int itemperatureScale;		//�¶ȵ�λ
	NVS_TIME strPublishData;		//��������
}TSystemState, *pTSystemState;

#define NET_CLIENT_MIN						0	
#define NET_CLIENT_DOME_MENU				(NET_CLIENT_MIN + 0)	//����˵�����
#define NET_CLIENT_DOME_WORK_SCHEDULE		(NET_CLIENT_MIN + 1)	//�������ģ�����
#define NET_CLIENT_INTERESTED_AREA			(NET_CLIENT_MIN + 2)	//����Ȥ����
#define NET_CLIENT_APPEND_OSD				(NET_CLIENT_MIN + 3)	//�����ַ�����
#define NET_CLIENT_LOGONFAILED_REASON		(NET_CLIENT_MIN + 4)	//��½ʧ��
#define NET_CLIENT_AUTOREBOOT				(NET_CLIENT_MIN + 5)	//NVR�Զ���������ʱ��
#define NET_CLIENT_IP_FILTER				(NET_CLIENT_MIN + 6)	//IP��ַ���ˣ��ڰ�������
#define NET_CLIENT_DATE_FORMATE				(NET_CLIENT_MIN + 7)	//�����ַ�������ʾ��ʽ
#define NET_CLINET_COLOR2GRAY				(NET_CLIENT_MIN + 8)	//��Ƶ��ת�Ҽ�ʱ��Ч
#define NET_CLINET_LANPARAM					(NET_CLIENT_MIN + 9)	//�������ù���
#define NET_CLINET_DAYLIGHT_SAVING_TIME		(NET_CLIENT_MIN + 10)	//�����豸����ʱ
#define NET_CLINET_NETOFFLINE				(NET_CLIENT_MIN + 11)	//ǿ�ƶϿ��û�����
#define NET_CLINET_HTTPPORT					(NET_CLIENT_MIN + 12)	//HTTP�˿�
#define NET_CLINET_PICTURE_MERGE			(NET_CLIENT_MIN + 13)	//ͼ��ϳ�
#define NET_CLIENT_SNAP_SHOT_INFO			(NET_CLIENT_MIN + 14)	//ǰ��ץ�Ĳ���
#define NET_CLIENT_IO_LINK_INFO				(NET_CLIENT_MIN + 15)	//IO�����豸����
#define NET_CLIENT_DEV_COMMONNAME			(NET_CLIENT_MIN + 16)	//�Զ����豸ͨ�ýӿڵı���
#define NET_CLIENT_ITS_DEV_COMMONINFO		(NET_CLIENT_MIN + 17)	//ITS�豸ͨ����Ϣ
#define NET_CLIENT_ITS_COMPOUNDPARAM		(NET_CLIENT_MIN + 18)	//���û�ϴ�����ز���
#define NET_CLIENT_DEV_VOLUME_CTRL			(NET_CLIENT_MIN + 19)	//��Ƶ��������
#define NET_CLIENT_INTIMITY_COVER			(NET_CLIENT_MIN + 20)	//3D��˽�ڵ�
#define NET_CLIENT_ANYSCENE					(NET_CLIENT_MIN + 21)	//��������
#define NET_CLIENT_CALL_ANYSCENE			(NET_CLIENT_MIN + 22)	//�����������Ӧ��������
#define NET_CLIENT_SENCE_CRUISE_TYPE		(NET_CLIENT_MIN + 23)	//����Ӧ��Ѳ��ʹ������
#define NET_CLIENT_SENCE_CRUISE_TIMER		(NET_CLIENT_MIN + 24)	//����Ӧ�ö�ʱѲ��ģ��
#define NET_CLIENT_SENCE_CRUISE_TIMERANGE	(NET_CLIENT_MIN + 25)	//����Ӧ��ʱ���Ѳ��ģ��
#define NET_CLIENT_FACE_DETECT_ARITHMETIC	(NET_CLIENT_MIN + 26)	//��������㷨
#define NET_CLIENT_PERSON_STATISTIC_ARITHMETIC	(NET_CLIENT_MIN + 27)	//����ͳ���㷨
#define NET_CLIENT_TRACK_ARITHMETIC			(NET_CLIENT_MIN + 28)	//���ܸ����㷨
#define NET_CLIENT_TRACK_ASTRICT_LOCATION	(NET_CLIENT_MIN + 29)	//������λ
#define NET_CLIENT_TRACK_ZOOMX				(NET_CLIENT_MIN + 30)	//���ٱ���ϵ��--ʵʱ����
#define NET_CLIENT_TRACK_MANUAL_LOCKED		(NET_CLIENT_MIN + 31)	//�ֶ���������
#define NET_CLIENT_VCA_SUSPEND				(NET_CLIENT_MIN + 32)	//��ͣ���ܷ���
#define NET_CLIENT_COVER_ALARM_AREA			(NET_CLIENT_MIN + 33)	//��Ƶ�ڵ������������
#define NET_CLIENT_MANUAL_SNAPSHOT			(NET_CLIENT_MIN + 34)	//�ֶ�ץ��
#define NET_CLIENT_FILE_APPEND_INFO			(NET_CLIENT_MIN + 35)	//¼���ļ�������Ϣ
#define NET_CLIENT_CURRENT_SENCE			(NET_CLIENT_MIN + 36)	//��ȡ�����ǰʹ�÷�������
#define NET_CLIENT_BACKUP_SEEK_WORKDEV		(NET_CLIENT_MIN + 37)	//�鲥����������
#define NET_CLIENT_BACKUP_GET_SEEK_COUNT	(NET_CLIENT_MIN + 38)	//�鲥��������������
#define NET_CLIENT_BACKUP_GET_SEEK_WORKDEV	(NET_CLIENT_MIN + 39)	//��������б���ĳ������������Ϣ
#define NET_CLIENT_BACKUP_WORKMODE			(NET_CLIENT_MIN + 40)	//����ģʽ
#define NET_CLIENT_BACKUP_MODIFY			(NET_CLIENT_MIN + 41)	//�����������ɾ���ȱ���
#define NET_CLIENT_BACKUP_BACKUPDEV_STATE	(NET_CLIENT_MIN + 42)	//����ȱ���������Ϣ������״̬ 
#define NET_CLIENT_BACKUP_GET_WORKDEV_NUM	(NET_CLIENT_MIN + 43)	//������ȱ��������Ĺ���������
#define NET_CLIENT_BACKUP_WORKDEV_STATE		(NET_CLIENT_MIN + 44)	//��ù�����������Ϣ������״̬
#define NET_CLIENT_APP_SERVER_LIST			(NET_CLIENT_MIN + 45)	//��ȡ�豸��ǰƽ̨֧�ֵķ�������б�
#define NET_CLIENT_RTMP_URL_INFO			(NET_CLIENT_MIN + 46)	//RTMP�б���Ϣ
#define NET_CLIENT_FRAME_RATE_LIST			(NET_CLIENT_MIN + 47)	//��ȡĳͨ��֧��֡���б�
#define NET_CLIENT_RTSP_LIST_INFO			(NET_CLIENT_MIN + 48)	//����RTSP�б���Ϣ
#define NET_CLIENT_DISABLE_PROC_VCA			(NET_CLIENT_MIN + 49)	//���ö�VCA�����Ĵ�����ֹռ�ù����ڴ�
#define NET_CLIENT_ENABLE_PROC_VCA			(NET_CLIENT_MIN + 50)	//���ö�VCA�����Ĵ���
#define NET_CLIENT_COM_DEVICE               (NET_CLIENT_MIN + 51)	//�����豸����
#define NET_CLIENT_GAIN_LOG					(NET_CLIENT_MIN + 52)	//��ȡ�ѻ���־
#define NET_CLIENT_EXPORT_CONFIG            (NET_CLIENT_MIN + 53)	//��������
#define NET_CLIENT_ELECNET_METER            (NET_CLIENT_MIN + 54)	//electronic net meters param
#define NET_CLIENT_DEVINFO_AUTO_CONFIRM		(NET_CLIENT_MIN + 55)   //Dh DevInvo confirm
#define NET_CLIENT_COLOR_TYPE				(NET_CLIENT_MIN + 56)   //color input type
#define NET_CLIENT_CHANGE_VI_MODE			(NET_CLIENT_MIN + 57)   //change video input mode
#define NET_CLIENT_WATER_STEADY_PERIOD		(NET_CLIENT_MIN + 58)   
#define NET_CLIENT_WATER_SNAPINFO    		(NET_CLIENT_MIN + 59)   
#define NET_CLIENT_VIDEOENCODE_LIST			(NET_CLIENT_MIN + 60)   //Video Encod List
#define NET_CLIENT_BARCODE					(NET_CLIENT_MIN + 61)   //device bar code
#define NET_CLIENT_RESET_PASSWORD			(NET_CLIENT_MIN + 62)
#define NET_CLIENT_PSECH_STATE				(NET_CLIENT_MIN + 63)
#define NET_CLIENT_WHITELIGHT_MODE			(NET_CLIENT_MIN + 64)	//white light mode
#define NET_CLIENT_IRIS_TYPE				(NET_CLIENT_MIN + 65)	//aperture type
#define NET_CLIENT_IDENTI_CODE				(NET_CLIENT_MIN + 67)	//identity code
#define NET_CLIENT_EMAIL_TEST				(NET_CLIENT_MIN + 68)	//email test
#define NET_CLIENT_CREATEFREEV				(NET_CLIENT_MIN + 69)	//create freev0
#define NET_CLIENT_VCAFFINAL				(NET_CLIENT_MIN + 70)	//VCAFFINAL
#define NET_CLIENT_PREVIEW_MODE				(NET_CLIENT_MIN + 71)	//MODE
#define NET_CLIENT_DISK_SMART_ENABLE		(NET_CLIENT_MIN + 72)	//disk smart check enable
#define NET_CLIENT_SMART_INFO				(NET_CLIENT_MIN + 73)	//disk smart check info
#define NET_CLIENT_AUDIO_PONTICELLO			(NET_CLIENT_MIN + 74)	//AUDIO_PONTICELLO
#define NET_CLIENT_FACEMOSAIC				(NET_CLIENT_MIN + 75)	//FACEMOSAIC
#define NET_CLIENT_MAX_LANRATE				(NET_CLIENT_MIN + 76)	//max lan rate
#define NET_CLIENT_P2P_APP_URL				(NET_CLIENT_MIN + 77)	//P2p load app url
#define NET_CLIENT_CREATEFREEVO				(NET_CLIENT_MIN + 78)
#define NET_CLIENT_DEVAMPLITUDE				(NET_CLIENT_MIN + 79)
#define NET_CLIENT_PTZLIST					(NET_CLIENT_MIN + 80)
#define NET_CLIENT_COLORPARALIST			(NET_CLIENT_MIN + 81)
#define NET_CLIENT_EXCEPTION_LINKOUTPORT	(NET_CLIENT_MIN + 82)	//Exceptyion LinkOutPort
#define NET_CLIENT_WIRELESSMODULE			(NET_CLIENT_MIN + 83)	//WifiLessModule
#define NET_CLIENT_MODIFYAUTHORITY			(NET_CLIENT_MIN + 84)	//modify authority
#define NET_CLIENT_DISK_OPERATION           (NET_CLIENT_MIN + 85)	//disk operate
#define NET_CLIENT_SCENETIMEPOINT	        (NET_CLIENT_MIN + 86)	//scene time port
#define NET_CLIENT_H323_LOCALPARAS          (NET_CLIENT_MIN + 87)	//H.323 local param
#define NET_CLIENT_H323_GKPARAS				(NET_CLIENT_MIN + 88)	//H.323 GK param
#define NET_CLIENT_ENABLE_LOGON_LIGHT		(NET_CLIENT_MIN + 89)	//Enable Light Logon Mode
#define NET_CLIENT_CHN_CONNECT_STATE		(NET_CLIENT_MIN + 90)	//channel connect state
#define NET_CLIENT_GET_PROTOCOL_ENABLE		(NET_CLIENT_MIN + 91)	//device is send protocol
#define NET_CLIENT_GET_LIFEMONITOR_HBREAL	(NET_CLIENT_MIN + 92)	//Vital signs in realtime heart rate and blood oxygen
#define NET_CLIENT_GET_LIFEMONITOR_GRAMREAl	(NET_CLIENT_MIN + 93)	//Vital signs waveform data
#define NET_CLIENT_LIFEMONITOR_SET_CONFIG	(NET_CLIENT_MIN + 94)	//Set life monitor config
#define NET_CLIENT_PASSWD_ERR_TIMES			(NET_CLIENT_MIN + 95)	//Password Error times
#define NET_CLIENT_DEV_ABSTRACT				(NET_CLIENT_MIN + 96)	//dev abstract
#define NET_CLIENT_OSD_EXTRA_INFO			(NET_CLIENT_MIN + 97)	//OSD extra info
#define NET_CLIENT_SERVERINFO				(NET_CLIENT_MIN + 98)	//Server Info
#define NET_CLIENT_GET_FUNC_ABILITY			(NET_CLIENT_MIN + 99)	//get function ability
#define NET_CLIENT_NTP_INFO					(NET_CLIENT_MIN + 100)	//NTP info
#define NET_CLIENT_VODEV_LIST				(NET_CLIENT_MIN + 101)	//VO Device list
#define NET_CLIENT_STREAM_SMOOTH			(NET_CLIENT_MIN + 102)	//stream smooth
#define NET_CLIENT_MAX						(NET_CLIENT_MIN + 103)

//���ܶ���ӿ�
typedef struct _tagEncrypt_Info
{
	int iSize;
	int iPasswdErrTimes;			//����������
}TEncrypt_Info_Out, *PTEncrypt_Info_Out;

//pse ch zyb add
#define MAX_PSE_CHANNEL_NUM		16
typedef struct tagPseChState
{
	int				iBufSize;
	int				iPseCh;
	int				iState;			
	int				iPower;
}PseChState, *pPseChState;

typedef struct tagPseChProperty
{
	int				iBufSize;	
	int				iPseCh;			
	int				iWorkMode;
}PseChProperty, *pPseChProperty;

typedef struct tagPseChInfo
{
	int				iBufSize;	
	int				iPseChNum;		//PSE total channel number			
	int				iPsePower;		//PSE total power
}PseChInfo, *pPseChInfo;

//autotest use it to confirm test  item
typedef struct _tagDevAbstract 
{
	int		iBufSize;
	int		iSataPortNum;  //sata num
	int		iESataPortNum; //esata num
	int     iUsbPortNum;   //usb num
	int     iSDCardNum;	   //sd card num
}TDevAbstract, *pTDevAbstract;

typedef struct tagDiskSmartEnable
{
	int				iSize;	
	int				iDiskId;			
	int				iEnable;
}DiskSmartEnable, *pDiskSmartEnable;


typedef struct tagSmartInfo
{
	int				iSize;	
	char			cId[LEN_8];			
	char			cAttributeName[LEN_32];	
	int				iFlag;
	int				iValue;
	int				iWorst;
	int				iThresh;
	char			cStatue[LEN_16];
	char			cRawValue[LEN_64];
}SmartInfo, *pSmartInfo;

#define MAX_SMART_INFO_NUM	256
#define SMART_INFO_NUM		32
typedef struct tagDiskSmartMsg
{
	int				iSize;	
	int				iDiskId;			
	int				iTotalPackageNum;		//total message number
	int				iCurrentPackageMsgNo;			//current message No
	char			cHDDModel[LEN_32];//HDD model
	char			cSericalNum[LEN_16];
	int				iTemprt;			//temperature
	int				iUsedTime;
	int				iHealth;
	int				iAllHealth;
	int				iTotalSmartInfoNum;
	pSmartInfo		pstSmartInfo;
}DiskSmartMsg, *pDiskSmartMsg;

typedef struct tagP2PAppUrl
{
	int				iBufSize;
	int				iTypeId;	//1=android��2=IOS
	char			cUrl[256];	//include "http://"
}P2PAppUrl,*pP2PAppUrl;

//S3E
typedef struct tagWhiteLight
{
	int				iBufSize;
	int				iMode;		//work mode
}WhiteLight,*pWhiteLight;

typedef struct tagApertureType
{
	int				iBufSize;
	int				iType;			//0-retain 1-DC 2-Piris
}ApertureType, *pApertureType;

typedef struct tagIdentiCode
{
	int				iBufSize;
	char			cParam[LEN_32];
}IdentiCode, *pIdentiCode;

typedef struct tagMaxLanRate
{
	int				iBufSize;
	int				iLanNo;
	int				iRate;
}MaxLanRate, *pMaxLanRate;

#define MAX_PRESET_LIST_NUM	501
typedef struct tagPTZList
{
	int				iBufSize;
	int				iChanNo;
	int             iFocusMode[MAX_PRESET_LIST_NUM]; //PTZ focus mode  0:auto   1:fixed -1:unknown
	int				iPtzState[MAX_PRESET_LIST_NUM];  //PTZ state 0:set 1:delete 2:call -1:unknown
}PTZList, *pPTZList;

typedef struct tagColorParaList
{
	int iSize;
	int iChanNo;
	int iParam;
}tColorParaList, *ptColorParaList;

//ITS 6M 2014/12/05
typedef struct tagITSTrafficFlow
{
	int				iBufSize;		//Size of struct
	int				iLaneID;		//LaneID ([0, 3])
	int				iType;			//Statistical Type ([0, 1])
	int				iEnable;		//Traffic Statistics Enable ([0, 1])
	int				iTimeInterval;	//Statistics Time Interval ([1, 1440])
}ITSTrafficFlow, *pITSTrafficFlow;

typedef struct tagITSTrafficFlowReport
{
	int				iSize;				//Size of struct
	int				iLaneID;			//Lane ID ([0, 3])
	int				iFlow;				//Flow 
	int				iHoldRate;			//Lane Hold Rate
	int				iSpeed;				//Average Speed
	int				iDistance;			//  	
	NVS_FILE_TIME 	stStartTime;	
	NVS_FILE_TIME  	stStopTime;			//Time Range
}ITSTrafficFlowReport, *pITSTrafficFlowReport;

#define MAX_ITS_ILLEGALTYPE		12	
typedef struct tagITSIllegalType
{
	int				iBufSize;				//Size of struct
	int				iLaneID;				//Lane ID ([0, 3])
	int				iTypeID;				//Illegal Type ID ([1, 12 ])
	char			cMarkID[LEN_32];		//Type ID
	int				iPriority;				//Priority([0, 100] )
	char			pcName[LEN_64];			//Illegal Name
}ITSIllegalType, *pITSIllegalType;

typedef struct tagITSPicMergeOverLay
{
	int		iBufSize;				//Size of struct
	int		iLaneID;			//Lane No
	int		iOneOSD;
	int		iOsdType;				
	int		iEnable;				
	char	cOsdName[LEN_32];
	int		iPosX;				
	int		iPosY;
	int     iZoomScale;
}ITSPicMergeOverLay, *pITSPicMergeOverLay;


#define MAX_LIST_COUNT	32

typedef struct tagPicDirectory
{
	int				iDirectoryId;
	char			cUserDefine[LEN_32];
}PicDirectory,*pPicDirectory;

typedef struct tagPicName
{
	int				iDirectoryId;
	char			cUserDefine[LEN_32];
}PicName,*pPicName;

typedef struct tagFtpUpload
{
	int				iSize;
	int				iFtpNum;		//0��retain��1��ftp1��2��ftp2
	int				iFtpEnable;		//0-unable 1-enable
	char			cFtpAddr[LEN_32];
	char			cUserName[LEN_16];
	char			cPassWord[LEN_16];
	int				iPort;			//ftp port [0,65535]
	int				iFtpType;		//0��retain�� 1��bayonet�� 2��break rules��two ftp mutual exclusion��
	int				iListCount;		//0��root list�� 1~n��n-list
	int				iPicNameCount;	//0��none 1~n��n-list
	int				iPlateEnable;	//small picture of plate number 
	pPicDirectory   pstDirectory;	
	pPicName        pstPicName;	
}FtpUpload, *pFtpUpload;

typedef struct tagCarLogoOptim
{
	int				iSize;
	int				iOptimID;
	int				iEnable;				//0-Unable��1-Enable
	char			cFirstChar[LEN_32];
	char			cFirstLetter[LEN_16];
	char			cSecondLetter[LEN_32];
	int				iLogoType;				
}CarLogoOptim, *pCarLogoOptim;

typedef struct tagITS_LoopMode
{
	int		iBufSize;
	int		iLaneID;			//Lane No	Max:4  value��0-3	
	int		iLoopMode;			//0��One Loop��1��two Loop��2��three Loop											
}ITS_LoopMode, *pITS_LoopMode;

//add end

//HN zyb add 20150123
typedef struct tagDevAmplitude
{
	int				iSize;
	int				iChanNo;
	int				iMicNo;
	int				iValue;		
}DevAmplitude, *pDevAmplitude;
//add end

#define MAX_DOME_TYPE	25
#define MAX_DOME_PARA_GROUP_NUM		8
typedef struct __tagDomeParam
{
	int iType;
	int iParam1;
	int iParam2;
	int iParam3;
	int iParam4;
}TDomeParam, *pTDomeParam;

#define MAX_SCHEDULE 8
typedef struct __tagDomeWork
{
	int iBeginHour;
	int iBeginMinute;
	int iEndHour;
	int iEndMinute;
	int iWorkType;  //0���޶���1��Ԥ��λ2��ɨ�� 3��Ѳ�� 4��ģʽ  5���������
	int iWorkValue; //Ԥ��λ��ȡֵ��Χ1��8��ɨ���ȡֵ��Χ��1��4��Ѳ����ȡֵ��Χ��1��4��ģʽ·����ȡֵ��Χ��1��4 ,���������ȡֵ��Χ��bit0:���1��bit1:���2
	int iEnable;	//0-������,1-����
}TDomeWork, *pTDomeWork;


//��ת�����ͣ���SDK����
#define COLOR_AUTO_IN		0			//�Զ���ͬ��
#define COLOR2GRAY_GRAY		1			//�ڰ�
#define COLOR2GRAY_COLOR	2			//��ɫ
#define COLOR_AUTO_OUT		3			//�Զ���ͬ��
#define COLOR_AUTO_ALARM	4			//����ͬ��
#define COLOR2GRAY_TIMED	5			//��ʱ
#define COLOR2GRAY_AUTO		6           //auto

//��ת��
typedef struct __tagColor2GrayParam
{
	int  m_isize;         //�ṹ���С
	int  m_iColor2Gray;   //��ת������
	int  m_iDayRange;     //��������ֵ
	int  m_iNightRange;   //ҹ������ֵ
	int  m_iTimeRange;    //��ʱ��Χ�����쿪ʼʱ�䣬���Ͽ�ʼʱ�䡣
	int  m_iColorDelay;   //��ת����ʱ
	int  m_iGrayDelay;    //��ת����ʱ
	int	 m_iDevType;	  //0:Monitoring Products   1:ITS Products
}TColor2GrayParam,*pTColor2GrayParam;

//��������
typedef struct __tagLanParam
{
	int m_iSize;		  //�ṹ���С
	int m_iNo;			  //������ţ�lan1��0�� lan2:1
	int m_iMode;		  //����ģʽ��0��������1����˫����2��������3��˫����
	int m_iSpeed;		  //����
}TLanParam,*pTLanParam;

#define MAX_DEVCOMNAME_NO   3		//ͨ��������Ŀ
#define DEVCOMNAME_VIDEO    0		//��Ƶ
#define DEVCOMNAME_ALARMIN	1       //��������
#define DEVCOMNAME_ALARMOUT 2       //�������
//�Զ����豸ͨ�ýӿڵı���
typedef struct __tagDevCommonName
{
	int  m_iSize;				//�ṹ���С
	int	 m_iChannelType;		//ͨ������
	char m_cChanneleName[LEN_64+1];   //ͨ������
}TDevCommonName,*pTDevCommonName;

typedef struct __tagDomeWorkSchedule
{	
	int iWeekDay;
	TDomeWork tWork[MAX_SCHEDULE];
}TDomeWorkSchedule, *pTDomeWorkSchedule;

#define  MAX_APPEND_DSD_LAY_BUM	7
#define MAX_OSD_LENGTH 256
typedef struct __tagAppendOSD
{
	int iSize;
	int iLayNo;
	int iColor;
	int iDiaphaneity;
	int iFontSize;
	int iPosX;
	int iPosY;
	char pcText[MAX_OSD_LENGTH];
}TAppendOSD, *pTAppendOSD;

#define MAX_INTERESTED_AREA_NUM	8
typedef struct __tagInterestedArea
{
	int iSize;
	int iIndex;
	int iEnable;
	int iAbsQp;
	int iQp;
	int iLeft;
	int iTop;
	int iRight;
	int iBottom;
	int iDstFrameRate;	//destination frame rate��1-current framerate
}TInterestedArea, *pTInterestedArea;

#define MAX_NVR_AUTOREBOOT_SCHEDULE 9
typedef struct __tagAutoRebootSchedule
{
	int iSize;
	int iWeekDay;			//�����յ�������Ϊ0��6��7-ÿ�죬8-�Ӳ�
	int iHour;
	int iMinute;
}TAutoRebootSchedule, *pTAutoRebootSchedule;

#define MAX_IP_FILTER_NUM	16
typedef struct __tagIpFilter
{
	int iSize;
	char cIp[LENGTH_IPV4];
	char cSubMask[LENGTH_SUB_MASK];
	int iFilterType;								//��������:0,����IPȨ���趨;1,��ֹIP����;2,����IP����
	int iIpNum;										//����ip����,���֧��16����ͬ��IP��ַ
	char cFilterIp[MAX_IP_FILTER_NUM][LENGTH_IPV4];	//�����ַ�ַ������磺pcFilterIp[0]=��192.168.1.10��
}TIpFilter, *pTIpFilter;

typedef struct __tagDateFormat
{
	int iSize;
	int iFormatType;	//��ʽ����:case 0:/*yyyy/mm/dd*/; 1:/*mm/dd/yyyy*/; 2:/*dd/mm/yyyy*/; 3:/*M day yyyy*/; 4:/*day M yyyy*/
	char cSeparate;		//�ָ���,������Ϊ��/������-��
	char cReserved[3];	//�ֽڶ���
	int iFlagWeek;		//��ʾ�����Ƿ���ʾ: 0-����ʾ��1-��ʾ					
	int iTimeMode;		//��ʾʱ����ʽ: 0-24Сʱ�ƣ�1-12Сʱ��							
}TDateFormat, *pDateFormat;

//�����豸��ʱ��
typedef struct __tagTimeZone
{
	int iSize;
	int iTimeZone;			//��ǰ����ʹ�õ�ʱ��,��ΧӦ����-12��+13
	int iDSTEnable;			//����ʱʹ��,0-���ã�1-����
}TTimeZone, *pTTimeZone;

//����ʱ
typedef struct __tagDaylightSavingTime
{
	int m_iSize;
	int m_iMonth;				//�·�
	int m_iWeekOfMonth;			//���ڱ��(0��ĩ����1���׸���2���ڶ�����3����������4�����ĸ�)
	int m_iDayOfWeek;			//���ڣ������յ�������Ϊ0��6��
	int m_iHourOfDay;			//Сʱ��0��23��
}TDaylightSavingTime, *pTDaylightSavingTime;

typedef struct __tagDaylightSavingTimeSchedule
{
	int m_iSize;
	int m_iDSTEnable;					//����ʱʹ��,0-���ã�1-����
	int m_iOffsetTime;					//ƫ��ʱ��,0��120����λ���֣�
	TDaylightSavingTime m_tBeginDST;	//��ʼʱ��
	TDaylightSavingTime m_tEndDST;		//����ʱ��
}TDaylightSavingTimeSchedule, *pTDaylightSavingTimeSchedule;

//Զ��ǿ�ƶϿ��û�����
typedef struct __tagNetOffLine
{
	int iSize;
	char cIPAddr[LENGTH_IPV4];		//IP��ַ������
	int iOffTime;					//�Ͽ�ʱ��,�룻��С10��
}TNetOffLine, *pTNetOffLine;

//HTTP�˿�
typedef struct __tagHttpPort
{
	int iSize;
	int iPort;			//http�˿ں�
	int iHttpsport;		//https�˿ں�,Ĭ��443
	int	iRtspPort;		//rtsp�˿ں�,Ĭ��554
	int iSchedulePort;	//schedule port num,default 8005
}THttpPort, *pTHttpPort;

//NTP info
typedef struct tagNTPInfo
{
	int		iBufSize;
	char	cServerIP[LEN_32];	//NTP Server IP
	int		iServerPort;			//NTP Server Port
	int		iIntervalHour;
	int		iIntervalSec;		
}NTPInfo,*pNTPInfo;


//VoDev list
#define MAX_VODEV_LIST_NUM	50
typedef struct tagVoDevList
{
	int		iBufSize;
	int		iCount;
	int		iVoDevId[MAX_VODEV_LIST_NUM];	
}VoDevList,*pVoDevList;

//��¼ʧ�ܴ�����
#define UNKNOW_ERROR			0
#define CIPHER_USERNAME_ERROR	1
#define CIPHER_USERPASS_ERROR	2
#define NO_SUPPORT_ALGORITHM	3
#define PSWD_ERR_TIMES_OVERRUN	4
#define LOGON_NET_ERROR			0xFF01
#define LOGON_PORT_ERROR		0xFF02

//�������������ģ������
#define CAMERA_PARA_MIN								0
#define CAMERA_PARA_IRIS_ADJUSTMENT 				CAMERA_PARA_MIN + 0		//0--��Ȧ����
#define CAMERA_PARA_WDR								CAMERA_PARA_MIN + 1		//1--����̬
#define CAMERA_PARA_BLC								CAMERA_PARA_MIN + 2		//2--���ⲹ��
#define CAMERA_PARA_EXPOSURE_TIME					CAMERA_PARA_MIN + 3		//3--�ع�ʱ��
#define CAMERA_PARA_SHUTTER_ADJUSTMENT				CAMERA_PARA_MIN + 4		//4--���ŵ���
#define CAMERA_PARA_GAIN_ADJUSTMENT					CAMERA_PARA_MIN + 5		//5--�������
#define CAMERA_PARA_GAMMA_ADJUSTMENT				CAMERA_PARA_MIN + 6		//6--gamma����
#define CAMERA_PARA_SHARPNESS_ADJUSTMENT			CAMERA_PARA_MIN + 7		//7--��ȵ���
#define CAMERA_PARA_NOISE_REDUCTION_ADJUSTMENT		CAMERA_PARA_MIN + 8		//8--�������
#define CAMERA_PARA_EXPOSURE_REGION					CAMERA_PARA_MIN + 9		//9--�ع�����
#define CAMERA_PARA_BLC_REGION						CAMERA_PARA_MIN + 10	//10--���ⲹ������
#define CAMERA_PARA_AF_REGION_SET					CAMERA_PARA_MIN + 11	//11--AF��������
#define CAMERA_PARA_TARGET_BRIGHTNESS_ADJUSTMENT	CAMERA_PARA_MIN + 12	//12--Ŀ�����ȵ���
#define CAMERA_PARA_WHITE_BALANCE_ADJUSTMENT		CAMERA_PARA_MIN + 13	//13--��ƽ�����
#define CAMERA_PARA_JPEG_IMAGE_QUALITY				CAMERA_PARA_MIN + 14	//14--JPEGͼ������
#define CAMERA_PARA_LUT_ENABLE						CAMERA_PARA_MIN + 15	//15--LUTʹ��
#define CAMERA_PARA_AUTOMATIC_BRIGHTNESS_ADJUST		CAMERA_PARA_MIN + 16	//16--�Զ����ȵ���ʹ��
#define CAMERA_PARA_HSBLC							CAMERA_PARA_MIN + 17	//17--ǿ�����ƹ���
#define CAMERA_PARA_AUTO_EXPOSURE_MODE				CAMERA_PARA_MIN + 18	//18--�Զ��ع�ģʽ
#define CAMERA_PARA_SCENE_MODE						CAMERA_PARA_MIN + 19	//19--����ģʽ
#define CAMERA_PARA_FOCUS_MODE						CAMERA_PARA_MIN + 20	//20--�۽�ģʽ
#define CAMERA_PARA_MIN_FOCUSING_DISTANCE			CAMERA_PARA_MIN + 21	//21--��С�۽�����
#define CAMERA_PARA_DAY_AND_NIGHT					CAMERA_PARA_MIN + 22	//22--��ҹ�л�
#define CAMERA_PARA_RESTORE_DEFAULT					CAMERA_PARA_MIN + 23	//23--�ָ�Ĭ��
#define CAMERA_PARA_THROUGH_FOG						CAMERA_PARA_MIN + 24	//24--ȥ��
#define CAMERA_PARA_MAX								CAMERA_PARA_MIN + 25

//����˵�����ģ������
#define DOME_PARA_MIN										0
#define DOME_PARA_PRESET_TITLE_DISPLAY_TIME					DOME_PARA_MIN + 1		//1--Ԥ��λ������ʾʱ��
#define DOME_PARA_AUTOMATIC_FUNCTION_TITLE_DISPLAY_TIME		DOME_PARA_MIN + 2		//2--�Զ����ܱ�����ʾʱ��
#define DOME_PARA_REGION_TITLE_DISPLAY_TIME					DOME_PARA_MIN + 3		//3--���������ʾʱ��
#define DOME_PARA_COORDINATE_DIRECTION_DISPLAY_TIME			DOME_PARA_MIN + 4		//4--���귽����ʾʱ��
#define DOME_PARA_TRACEPOINTS_DISPLAY_TIME					DOME_PARA_MIN + 5		//5--���ٵ������ʾʱ��
#define DOME_PARA_TITLE_BACKGROUND							DOME_PARA_MIN + 6		//6--���ⱳ��
#define DOME_PARA_AUTOMATIC_STOP_TIME						DOME_PARA_MIN + 7		//7--�Զ�ֹͣʱ��
#define DOME_PARA_MENU_OFF_TIME								DOME_PARA_MIN + 8		//8--�˵��ر�ʱ��
#define DOME_PARA_VERTICAL_ANGLE_ADJUSTMENT					DOME_PARA_MIN + 9		//9--��ֱ�Ƕȵ���
#define DOME_PARA_MANIPULATION_SPEED_RATING					DOME_PARA_MIN + 10		//10--�ٿ��ٶȵȼ�
#define DOME_PARA_PRESET_SPEED_RATING						DOME_PARA_MIN + 11		//11--Ԥ��λ�ٶȵȼ�
#define DOME_PARA_TEMPERATURE_CONTROL_MODE					DOME_PARA_MIN + 12		//12--�¶ȿ���ģʽ
#define DOME_PARA_485_ADDRESS_SETTING						DOME_PARA_MIN + 13		//13--485��ַ����
#define DOME_PARA_ZERO_SETTING								DOME_PARA_MIN + 14		//14--�������
#define DOME_PARA_NORTH_SETTING								DOME_PARA_MIN + 15		//15--ָ������
#define DOME_PARA_CONTROL_MODE								DOME_PARA_MIN + 16		//16--���Ʒ�ʽ
#define DOME_PARA_SENSITIVE_THRESHOLD						DOME_PARA_MIN + 17		//17--�й���ֵ
#define DOME_PARA_DELAY_TIME								DOME_PARA_MIN + 18		//18--��ʱʱ��
#define DOME_PARA_ZOOM_MATCH								DOME_PARA_MIN + 19		//19--�䱶ƥ��
#define DOME_PARA_PRESET									DOME_PARA_MIN + 20		//20--Ԥ��λ
#define DOME_PARA_SCANNING									DOME_PARA_MIN + 21		//21--ɨ��
#define DOME_PARA_SCHEMA_PATH								DOME_PARA_MIN + 22		//22--ģʽ·��
#define DOME_PARA_SCHEMA_PATH_CURRENT_STATE					DOME_PARA_MIN + 23		//23--ģʽ·����ǰ״̬
#define DOME_PARA_REGIONAL_INDICATIVE						DOME_PARA_MIN + 24		//24--����ָʾ
#define DOME_PARA_ZOOM_SPEED								DOME_PARA_MIN + 25		//25--�䱶�ٶ�
#define DOME_PARA_DIGITAL_ZOOM								DOME_PARA_MIN + 26		//26--���ֱ䱶
#define DOME_PARA_PRESET_FROZEN								DOME_PARA_MIN + 27		//27--Ԥ��λ����
#define DOME_PARA_LASER_LIGHT								DOME_PARA_MIN + 28		//28--������������ֵ
#define DOME_PARA_LASER_COAXIAL								DOME_PARA_MIN + 29		//29--������ͬ������
#define DOME_PARA_KEYING_LIMIT								DOME_PARA_MIN + 31		//31--������λ����
#define DOME_PARA_OUTAGE_MEMORY								DOME_PARA_MIN + 32		//32--�������ģʽ
#define DOME_PARA_PTZ_PRIOR									DOME_PARA_MIN + 33		//33--��̨����
#define DOME_PARA_KEYING_USING								DOME_PARA_MIN + 34		//34--������λ����
#define DOME_PARA_MAX										DOME_PARA_MIN + 35		

//CMD
#define COMMAND_ID_MIN					0
#define COMMAND_ID_ITS_FOCUS			(COMMAND_ID_MIN + 1)		//���۽��������Э��
#define COMMAND_ID_MODIFY_CGF_FILE		(COMMAND_ID_MIN + 2)		//���������ļ������ֶε�ֵ
#define COMMAND_ID_AUTO_FOCUS			(COMMAND_ID_MIN + 3)		//auto focus
#define COMMAND_ID_SAVECFG				(COMMAND_ID_MIN + 4)		//save param
#define COMMAND_ID_DEFAULT_PARA			(COMMAND_ID_MIN + 5)		//default para
#define COMMAND_ID_DIGITAL_CHANNEL		(COMMAND_ID_MIN + 6)		//digital channel
#define COMMAND_ID_DELETE_FOG			(COMMAND_ID_MIN + 7)		//delete fog
#define COMMAND_ID_QUERY_REPORT			(COMMAND_ID_MIN + 8)		//query report form
#define COMMAND_ID_MAX					(COMMAND_ID_MIN + 9)

//default para
typedef struct _tagDefaultPara
{
	int iSize;
	int iType;
}TDefaultPara, *PTDefaultPara;

//digital channel
typedef struct _tagDigitalChnBatch
{
	int iSize;
	int iType;  //0-���� 1-����ͨ������ɾ�� 2-����ͨ����������
	int iOpt;	//iOpΪ 1 ��ʾ����ͨ������������;iOpΪ 0 ��ʾ����ͨ��������������
}TDigitalChnBatch, PTDigitalChnBatch;

//ͼ��ϳɷ�ʽ����
#define MAX_ITS_MERGE_TYPE	4
typedef struct __tagTITS_MergeInfo
{
	int		m_iSize;
	int		m_iMergeType;		//�ϳ���������:1-һ�źϳɷ�ʽ����,2-���źϳɷ�ʽ����,3-���źϳɷ�ʽ����
	int		m_iMergeMode;		//���źϳɷ�ʽ��0-����ԭͼ����д�ϳ�,1-����ԭͼ����д�ϳ�,
								//				2-������д��ԭͼ�ϳ�,3-������д��ԭͼ�ϳɡ�
								//���źϳɷ�ʽ���ͣ�0-��ֱ�ϳ�ģʽ,1-ˮƽ�ϳ�ģʽ
								//���źϳɷ�ʽ����:0-Ʒ����,��һ�¶�;1-Ʒ����,�϶���һ;2-�����һ;3-��һ�Ҷ�;
								//				4-��ֱ����;5-ˮƽ����;6-������7-��ֱ����3�ż���д��8-ˮƽ����3�ż���д��
								//				9-������-��д���£�10-������-��д���ϣ�11-������-��д����
	int		m_iResizePercent;	//ͼ�����ű���
}TITS_MergeInfo, *PTITS_MergeInfo;

#define IODEVICE_NUM 3		//IO�豸��
//ǰ��ץ�Ĳ���
#define MAX_LINK_CAMERA_ID		4
typedef struct __tagTITS_SnapShotInfo
{
	int		m_iSize;
	int		m_iRoadNo;				//�������
	int		m_iEnable;				//ʹ��
	char	m_cLinkIP[LENGTH_IPV4];	//�������IP
	int		m_iLinkRoadNo;			//�����������ID
	int		m_iSceneID;// ����ID(0~15) 20140203��չ
}TITS_SnapShotInfo, *pTITS_SnapShotInfo;

//IO�����豸��������
#define MAX_IO_DEVICE_NUM	10
typedef struct __tagTITS_IOLinkInfo
{
	int		m_iSize;
	int		m_iIONo;		//IO�ڱ��,0-10
	int		m_iLinkDevice;	//�豸��������,0-�����,1-Ƶ����,2-ƫ��
	int		m_iDevicePulse;	//���������������,0us-10000us
	int		m_iAheadTime;	//output ahead time
	int		m_iWorkState;	//work state
	int		m_iDefaultState;//default state
	int		m_iFrequency;
	int		m_iDutyCycle;	// Duty cycle
	int		m_iDayNightEnable;//default:0(Night Enable)
}TITS_IOLinkInfo, *pTITS_IOLinkInfo;

//ʱ��������ʹ��
typedef struct __tagTITS_FlashEnable
{
	int   m_iSize;
	int   m_iIndex;				//ʱ��α��,0-7��֧�����8��ʱ���
	int   m_iFlashLampEnable;	//�Ƿ����������,bit0��0-��������ƣ�1-��������ƣ�bit1: 0-����Ƶ���ƣ�1-����Ƶ���ƣ�bit2: 0-����ƫ�񾵣�1-����ƫ�񾵣�
	int   m_iVideoLinkEnbale;	//��Ƶ����ʹ��,��λbit0:0-��ʹ�ܣ�1-ʹ��; bit1:0-����Ʋ�ʹ�ܣ�1-�����ʹ�ܣ�bit2:0-Ƶ���Ʋ�ʹ�ܣ�1-Ƶ����ʹ�ܣ�bit3:0-ƫ�񾵲�ʹ�ܣ�1-ƫ��ʹ�ܣ�
}TITS_FlashEnable, *pTITS_FlashEnable;

#define REBUILDINDEX_SUCCESS			0
#define REBUILDINDEX_NO_DISK			1
#define REBUILDINDEX_EXCEPTION			2

#define MAX_ITS_DEV_COMMOMINFO_TYPE		7
typedef struct tagITS_DevCommonInfo
{
	int		iSize;
	int		iType;					//��Ϣ����,0-������1-���ش��룬2-�������ƣ�3-ץ��ͼƬ�ַ�����λ
	char	cCommonInfo[LEN_64];	//��Ϣ����,���Ȳ�����63�ַ�;iTypeΪ3ʱ��0-ͼƬ�ڵ��ӣ�1-ͼƬ�ºڿ��е���,2-ͼƬ�Ϻڿ��е���
}ITS_DevCommonInfo, *pITS_DevCommonInfo;

typedef struct tagITS_CamLocationEx
{
	int				iSize;
	ITS_CamLocation tITS_CamLocation;	//�������λ���豸��Ϣ�ṹ��
	int				iDeviceType;		//�豸����,��λ��bit0��������豸,bit1������֤�豸,bit2�����¼��,
										//bit3����������,bit4�����ڼ��,bit5���������֤�豸��
}ITS_CamLocationEx, *pITS_CamLocationEx;

//���û�ϴ�����ز���
#define MAX_ITS_ROADID_NUM	4
typedef struct tagITS_ComPoundParam
{
	int		iSize;
	int		iITSRoadID;					//�������,���֧��4��������0-3
	int		iRadarMatchTime;			//�״��ٶ�ƥ��ʱ��,��λ������
	int		iRadarMinSpeed;				//�״���С�����ٶ�ֵ,��λ����/Сʱ
	int		iRadarMaxSpeed;				//�״�����ٶ�ֵ;��λ����/Сʱ
	int		iSceneID;					//�������(0~15)
}ITS_ComPoundParam, *pITS_ComPoundParam;

#define VOLUMECTRL_TYPE_MIN					0
#define VOLUMECTRL_TYPE_INPUT	(VOLUMECTRL_TYPE_MIN+1)
#define VOLUMECTRL_TYPE_OUTPUT	(VOLUMECTRL_TYPE_MIN+2)
#define VOLUMECTRL_TYPE_LINEIN	(VOLUMECTRL_TYPE_MIN+3)
#define VOLUMECTRL_TYPE_MICIN	(VOLUMECTRL_TYPE_MIN+4)
#define VOLUMECTRL_TYPE_MAX		(VOLUMECTRL_TYPE_MIN+5)
typedef struct tagDevAudioVolume
{
	int				iBufSize;			//����������Ϣ�ṹ���С
	int				iCtrlType;			//�����������ͣ�0-������1-���룻 2-�����3-LineIn��4-MicIn		
	int				iVolume;			//������ֵ��0������ ��1-255��������ֵ
}DevAudioVolume, *pDevAudioVolume;

#define MAX_CRUISE_NUM_TDWY	16
#define SENCE_CRUISE_TYPE_TIMER      1
#define SENCE_CRUISE_TYPE_TIMERANGE 2

#define MAX_PRAVICY_COVER_AREA_NUM	  24
#define MAX_INTIMITY_COVER_AREA_COUNT 32
typedef struct tagIntimityCover
{
	int				iBufSize;			//3D��˽�ڵ��ṹ���С
	int				iAreaNum;			//������(1~24)
	int				iMinZoom;			//��С�䱶(1~50)
	int				iColor;				//��ɫ(1����ɫ 2����ɫ 3����ɫ 4����ɫ 5����ɫ 6����ɫ 7����ɫ 8����ɫ 9:������)
	int				iPointNum;			//��������(3~32)
	POINT			ptArea[MAX_INTIMITY_COVER_AREA_COUNT]; //����
}IntimityCover, *pIntimityCover;

typedef struct tagAnyScene
{
	int				iBufSize;			//����Ӧ�ö�ʱѲ��ģ��ṹ���С
	int				iSceneID;			//�������(0~15)
	char			cSceneName[LEN_32];	//��������
	int				iArithmetic;		//�����㷨����( bit0: 1-��Ϊ�����㷨���ã�0-������
										//bit1��1-�����㷨���ã�0-������
										//bit2��1-��������㷨���ã�0-������
										//bit3��1-����ͳ���㷨���ã�0-������)
										//bit4��1-��Ƶ����㷨���ã�0-������
	                                    //bit5��1-����ʶ���㷨���ã�0-������
	                                    //bit6��1-��Ƶ�쳣�㷨���ã�0-������ 
										//bit7��1-����㷨���ã�0-������      
										//bit8��1-��Ⱥ�ۼ��㷨���ã�0-������
}AnyScene, *pAnyScene;

typedef struct tagSceneTimerCruise
{
	int				iBufSize;			//����Ӧ�ö�ʱѲ��ģ��ṹ���С
	int				iCruiseID;			//Ѳ�������(0~15)
	int				iSceneID;			//�������(0~15)
	int				iEnable;			//ʹ��(0����ʹ�� 1��ʹ��)
	int				iTime;				//��ʱʱ��(30~3600s)
}SceneTimerCruise, *pSceneTimerCruise;

typedef struct tagSceneTimeRangeCruise
{
	int				iBufSize;			//����Ӧ��ʱ���Ѳ��ģ��ṹ���С
	int				iCruiseID;			//ʱ��α��(0~15) 
	int				iSceneID;			//�������(0~15)
	int				iEnable;			//ʹ��(0����ʹ�� 1��ʹ��)
	int				iBeginHour;			//��ʼСʱ
	int				iBeginMinute;		//��ʼ����
	int				iEndHour;			//����Сʱ
	int				iEndMinute;			//��������
	int				iWeekday;			//����(�����յ�������Ϊ0��6��
}SceneTimeRangeCruise, *pSceneTimeRangeCruise;

typedef struct tagTrackArithmetic
{
	int				iBufSize;			//�����㷨�ṹ���С
	int				iSceneID;			//�������(0~15)
	int				iZoomRate;			//���ٱ���(10~360,1~36������ȷ��0.1)
	int				iMaxFallowTime;		//�����ʱ��(0~300��)
	int				iHeight;			//����߶�
	int				iDesStopTime;		//Ŀ�꾲ֹ����ʱ��(2~600��,Ĭ��8��)
	int				iEnable;			//ʹ�ܱ�־(��λ��ʾ��1-ʹ�ܣ�0-��ʹ��
										//��0λ����⵽�������Ƿ񷵻�
										//��1λ���Ƿ�ʹ�ÿ��ڵ�����
										//��2λ���Ƿ�������λ
										//��3λ���Ƿ���ʾ���ٿ�)
	int				iMinSize;			//min size,range[0, 100],default 20
	int				iMaxSize;			//max size,range[0, 100] 
}TrackArithmetic, *pTrackArithmetic;

typedef struct tagTrackAstrictLocation
{
	int				iBufSize;			//������λ�ṹ���С
	int				iSceneID;			//�������(0~15)
	int				iType;				//��λ����(1:����λ��2:����λ 3:����λ 4:����λ)
}TrackAstrictLocation, *pTrackAstrictLocation;

#define MANUALCAP_TYPE_TRACK_LOCKED		 1  //�ֶ���������
#define MANUALCAP_TYPE_SNAPSHOT			 2	//�ֶ�ץ��

#define MAX_FACE_DETECT_AREA_COUNT 32
typedef struct tagFaceDetectArithmetic
{
	int				iBufSize;			//��������㷨�����С
	int				iSceneID;			//�������(0~15)
	int				iMaxSize;			//��������ߴ�(0~100 ͼ���Ȱٷֱ�,100��ʾ������Ļ�Ŀ��)
	int				iMinSize;			//��С�����ߴ�(0~100 ͼ���Ȱٷֱȣ�100��ʾ������Ļ�Ŀ��)
	int				iLevel;				//�㷨���м���(0~5)
	int				iSensitiv;			//������(0~5)
	int				iPicScale;			//ͼ�����ű���(1~10)
	int				iSnapEnable;		//����ץ��ʹ��(1-ʹ�ܣ�0-��ʹ��)
	int				iSnapSpace;			//ץ�ļ��(���֡��)
	int				iSnapTimes;			//ץ�Ĵ���(1~8)
	int				iPointNum;			//��������򶥵����(3��32)
	POINT			ptArea[MAX_FACE_DETECT_AREA_COUNT]; //��������򶥵�����
}FaceDetectArithmetic, *pFaceDetectArithmetic;

typedef struct tagPersonStatisticArithmetic
{
	int				iBufSize;			//����ͳ���㷨�����С
	int				iSceneID;			//�������(0~15)
	int				iMode;				//ģʽ(1-��ֱ����ͳ�� 2-ˮƽ����ͳ��)
	int				iTargetMaxSize;		//Ŀ�����ߴ�(0~100 ͼ���Ȱٷֱȣ�100��ʾ������Ļ�Ŀ��)
	int				iTargetMinSize;		//Ŀ����С�ߴ�(0~100 ͼ���Ȱٷֱȣ�100��ʾ������Ļ�Ŀ��)
	int				iSensitiv;			//������(0~5)
	int				iDetectType;		//�������(1-������� 2-�����)
	int				iPointNum;			//��������򶥵����(2��32)
	POINT			ptArea[MAX_FACE_DETECT_AREA_COUNT]; //��������򶥵�����
	vca_TPolygonEx	stRegion;			//Line1 Point Num and Points
}PersonStatisticArithmetic, *pPersonStatisticArithmetic;

typedef struct tagTrackZoomX
{
	int				iBufSize;			//���ٱ��ʽṹ���С
	int				iSceneID;			//�������(0~15)
	int				iRate;				//���ٱ���ϵ��(10~360[����0.1])
}TrackZoomX, *pTrackZoomX;

#define MAX_COVER_ALARM_BLOCK_COUNT 8
#define MAX_COVER_ALARM_AREA_COUNT 32
typedef struct tagCoverAlarmArea
{
	int				iBufSize;			//��Ƶ�ڵ���������ṹ���С
	int				iBlockNo;			//��ID(1~8)
	int				iAlarmTime;			//�������ʱ��
	int				iPointNum;			//��������򶥵����(3��32)
	POINT			ptArea[MAX_COVER_ALARM_AREA_COUNT]; //��������򶥵�����
}CoverAlarmArea, *pCoverAlarmArea;

//Record time parameter
typedef struct tagNVS_FILE_TIME_V1
{
	unsigned short iYear;   /* Year */
	unsigned short iMonth;  /* Month */
	unsigned short iDay;    /* Day */
	unsigned short iHour;   /* Hour */
	unsigned short iMinute; /* Minute */
	unsigned short iSecond; /* Second */
	unsigned short iMillisecond; /* Millisecond */
} NVS_FILE_TIME_V1,*PNVS_FILE_TIME_V1;

typedef struct tagITS_ILLEGALRECORD_V1
{
	int	 iBufSize;			//��ϴ����ṹ���С
	long lCarID;
	int  iRecordFlag;
	int  iIllegalType;
	int  iPreset;
	char cCrossingID[LEN_64];		 //·�ڱ�� 64���ַ�
	char cLaneID[LEN_64];			 //�������	64���ַ�
	NVS_FILE_TIME_V1 m_tSnapTime;//ץ�Ŀ�ʼʱ��(��һ��ͼƬץ��ʱ��)
}ITS_ILLEGALRECORD_V1,*pITS_ILLEGALRECORD_V1;

//���ܷ������Ӳ�������ɫ
typedef struct tagVCATargetParam
{
	int		iBufSize;						//���Ӳ�������ɫ�ṹ���С
	int		iSceneID;						//�������(0~15)
	int		iDisplayTarget;					//��Ƶ�Ƿ����Ŀ��(0�������� 1������)
	int		iDisplayTrace;					//��Ƶ�Ƿ���ӹ켣(0�������� 1������)
	int		iTargetColor;					//Ŀ��͹켣��ɫ(0:�ɳ����Զ��趨��ɫ��ÿ��Ŀ��һ����ɫ 
											//1����ɫ 2����ɫ 3����ɫ 4����ɫ 5����ɫ 6����ɫ 7����ɫ 8����ɫ)
	int		iTargetAlarmColor;				//����ʱĿ��͹켣����ɫ(1����ɫ 2����ɫ 3����ɫ 4����ɫ 5����ɫ 6����ɫ 7����ɫ 8����ɫ)
	int		iTraceLength;					//�켣����(0��40)
}VCATargetParam,*pVCATargetParam;	

// ���ܷ����߼�����
typedef struct tagVCAAdvanceParam
{
	int		iBufSize;					//�߼������ṹ���С
	int		iSceneID;					//�������(0~15)
	int		iTargetEnable; 				//�Ƿ����ø߼�����
	int		iTargetMinSize; 			//Ŀ����С������
	int		iTargetMaxSize; 			//Ŀ�����������
	int   	iTargetMinWidth;  			//�����
	int		iTargetMaxWidth;  			//��С���
	int		iTargetMinHeight;  			//���߶�
	int		iTargetMaxHeight;  			//��С�߶�
	int     iTargetMinSpeed;			//Ŀ����С�����ٶ�(-1Ϊ��������)
	int     iTargetMaxSpeed;			//Ŀ����������ٶ�(-1Ϊ��������)
	int 	iTargetMinWHRatio;			//Ŀ�����С����ȡ�100	����/����100
	int 	iTargetMaxWHRatio;			//Ŀ�����󳤿�ȡ�100	����/����100
	int		iSensitivity;				//Ŀ����������ȼ���(0: �������� 1���������� 2����������)
	int		iForegroundMinDiff;			//ǰ����С����(4~100)
	int		iForegroundMaxDiff;			//ǰ��������(4~100)
	int		iBackUpdateSpeed;			//���������ٶ�(1~10)
	int		iRealTargetTime;			//Ŀ��ȷ��֡��(10~30��Ĭ��Ϊ16)
	int		iBlendBackTime;				//���뱳��ʱ��(֡�� ��Χ)
	int		iTarMergeSensitiv;			//Ŀ��ϲ�������(0~100)
}VCAAdvanceParam,*pVCAAdvanceParam;	

//���ܷ�������ͨ�ò���
typedef struct tagVCARule
{
	int		iSceneID;							//�������(0~15)
	int		iRuleID;							//����ID(0~7)
	int		iValid;								//�����Ƿ���Ч(0: ����Ч 1:��Ч)
}VCARule,*pVCARule;

//���ܷ����������
typedef struct tagVCARuleParam
{
	int					iBufSize;							//���Ӳ�������ɫ�ṹ���С
	VCARule				stRule;								//����ͨ�ò���
	char				cRuleName[VCA_MAX_RULE_NAME_LEN];	//��������
	int					iEventID;							//��ǰ�¼�ID
}VCARuleParam,*pVCARuleParam;

//���ܷ�����������ͳ������
typedef struct tagVCAAlarmStatisticClear
{
	int		iBufSize;					//��������ͳ������ṹ���С
	int		iSceneID;					//�������(0~15)
	int		iRuleID;					//����ID(0~7)
	int		iEventType;					//�¼�����(0�������� 1��˫���� 2���ܽ��� 3���ǻ� 4��ͣ�� 5������ 6����������Ա�ܶ� 7�������� 
										//8��������9������ʶ�� 10����Ƶ���  11�����ܸ���  12������ͳ�� 13����Ⱥ�ۼ� 14����ڼ��)
}VCAAlarmStatisticClear,*pVCAAlarmStatisticClear;

//Set AUDIO PONTICELLO
typedef struct tagAudioPonticello
{
	int				iSize;				// the size of Struct
	int				iTouchType;  		    
	int				iTouchParam;
	int				iPitchLevel;	
}AudioPonticello,pAudioPonticello;

typedef struct tagCallParam
{
	int		iBufSize;					//����ͨ�ò����ṹ���С
	int		iType;						//0-���ó���ID��1-������˽�ڵ�λ��
	int		iValue;						//����ID��Χ��0~15�� ��˽�ڵ�λ�÷�Χ��0~23
}CallParam, *pCallParam;

// Anxiety test/scene change
typedef struct tagVCA_TRuleParam_VideoDiagnose
{
	int  iSize;   		  // struct size
	int  iChannelNo ;	  // channel No.
	int  iSceneId ;		  // scene ID
	int  iRuleID ;		  // rule ID
	int  iDisplayStat ;	  // display alarm number or not
	int  iType ;		  // arithmetic enable type
	int  iEnable ;		  // enable type
	int  iLevel ;		  // level
	int  iTime ; 		  // test time
}VCA_TRuleParam_VideoDiagnose, *pVCA_TRuleParam_VideoDiagnose;

//Audio Exception
typedef struct tagVCA_TRuleParam_AudioDiagnose
{
	int  iSize;   			// struct size
	int  iChannelNo ;		// channel No.
	int  iSceneId ;			// scene ID
	int  iRuleID ;		    // rule ID
	int  iDisplayStat ;	    // display alarm number or not
	int  iType ;		    // arithmetic enable type
	int  iEnable ;		    // enable type
	int  iLevel ;		    // level
}VCA_TRuleParam_AudioDiagnose, *pVCA_TRuleParam_AudioDiagnose;

//VCA Tripwire for 300W
typedef struct tagVCA_TRuleParam_Tripwire
{
	int					iBufSize;
	VCARule				stRule;
	int					iTargetTypeCheck;
	vca_TDisplayParam	stDisplayParam;			
	int					iTripwireType;			//0: unidirectional, 1: bidirectional
	int					iTripwireDirection;		//(0��359��)
	int					iMinSize;				//[0, 100] init 5
	int					iMaxSize;				//[0, 100] init 30
	vca_TPolygonEx		stRegion1;				//Line1 Point Num and Points
}VCA_TRuleParam_Tripwire,*pVCA_TRuleParam_Tripwire;
//add end

//���ܷ��������߲���
typedef struct tagVCATripwire
{
	int					iBufSize;				//���߽ṹ���С
	VCARule				stRule;					//����ͨ�ò���
	int					iTargetTypeCheck;		//Ŀ����������(0�������� 1�������� 2�����ֳ� 3�������˳� )
	int					iMinDistance;			//��С����(Ŀ���˶����������ڴ���ֵ)
	int					iMinTime;				//���ʱ��(Ŀ���˶����������ڴ���ֵ)
	int					iTripwireType;			//��Խ����(0: ����, 1: ˫��)
	int					iTripwireDirection;		//��ֹ��Խ����(0��359��)
	vca_TDisplayParam	stDisplayParam;			//��ʾ����
	vca_TLine			stLine;					//��������
}VCATripwire,*pVCATripwire;

//���ܷ���˫���߲���
typedef struct tagVCADbTripwire
{
	int					iBufSize;
	VCARule				stRule;
	int					iTargetTypeCheck;
	vca_TDisplayParam	stDisplayParam;			
	int					iTripwireType;			//0: unidirectional, 1: bidirectional
	int					iDirTripwire1;			//tripwire 1 direction(0��359��)
	int					iDirTripwire2;			//tripwire 2 direction(0��359��)
	int					iMinDBTime;
	int					iMaxDBTime;
	int					iMinSize;				//[0, 100] default 5
	int					iMaxSize;				//[0, 100] default 30
	vca_TPolygonEx		stRegion1;				//Line1 Point Num and Points
	vca_TPolygonEx		stRegion2;				//Line2 Point Num and Points
}VCADbTripwire,*pVCADbTripwire;

//���ܷ����ܽ����
typedef struct tagVCAPerimeter
{
	int					iBufSize;			//�ܽ�ṹ���С
	VCARule				stRule;				//����ͨ�ò���
	int					iTargetTypeCheck;	//����Ŀ������(0�������� 1�������� 2�����ֳ� 3�������˳�)
	int 				iMode;				//�ܽ���ģʽ(0:���� 1:���� 2:�뿪)
	int					iMinDistance;		//��С����
	int					iMinTime;			//���ʱ��
	int					iType;				//�Ƿ�����������
	int					iDirection;			//��ֹ����Ƕ�
	vca_TDisplayParam	stDisplayParam;		//��ʾ����
	vca_TPolygon		stRegion;			//����Χ	
	int  				iMiniSize;			//Min Size(range:0~100, default:5)
	int  				iMaxSize;			//Max Size(range:0~100, default:30)
}VCAPerimeter,*pVCAPerimeter;

//���ܷ����ǻ�����
typedef struct tagVCALinger
{
	int					iBufSize;			//�ǻ��ṹ���С
	VCARule				stRule;				//����ͨ�ò���
	int					iMinTime;			//�ǻ����ʱ��(5000~600000���룬Ĭ��ֵ��10000)
	int					iSensitivity;		//������(0~100)
	int					iMinRange;			//�ǻ���С����(0~100 ͼ���Ȱٷֱȣ�100��ʾ������Ļ�Ŀ��)
	vca_TDisplayParam	stDisplayParam;		//��ʾ����
	vca_TPolygon		stRegion;			//����Χ
	int					iMinBoundy;			//circumscribed polygon area(range:0~100, default:5)
}VCALinger,*pVCALinger;	

//���ܷ���ͣ������
typedef struct tagVCAParking
{
	int					iBufSize;			//ͣ���ṹ���С
	VCARule				stRule;				//����ͨ�ò���
	int					iMinTime;			//����ͣ�������ĳ���ʱ��(0~30000���룬Ĭ��ֵ��5000)
	vca_TDisplayParam	stDisplayParam;		//��ʾ����
	vca_TPolygon		stRegion;			//����Χ
	int					iMiniSize;			//Min Size(range:0~100, default:5)
	int					iMaxSize;			//Max Size(range:0~100, default:30)
	int					fThVelocity;		//threshold value(Accurate to one decimal places, *10 before to send the network protocol)
}VCAParking,*pVCAParking;	

//VCA Leave Detect   
typedef struct tagVCALeaveDetect
{
	int					iBufSize;
	VCARule				stRule;
	vca_TDisplayParam	stDisplayParam;			
	int					iLeaveAlarmTime;		
	int					iRuturnClearAlarmTime;	//Clear Alarm Time for People return
	int					iAreaNum;				//Area Num
	vca_TPolygon		stRegion1[MAX_AREA_NUM];//Area Point Num and Point	
	int					iDutyNum;				//[0, 5] init 2
	int					iMinSize;				//[0, 100] init 3
	int					iMaxSize;				//[0, 100] init 15
	int					iSensitivity;
}VCALeaveDetect,*pVCALeaveDetect;

//���ܷ������ܲ���
typedef struct tagVCARunning
{
	int					iBufSize;			//���ܽṹ���С
	VCARule				stRule;				//����ͨ�ò���
	int					iMaxDistance;		//�˱���������(0.01��100��1.00��100)
	int					iMinDistancePerSec;	//ÿ����С�ƶ�����(0~100 ͼ���Ȱٷֱȣ�100��ʾ������Ļ�Ŀ��)
	vca_TDisplayParam	stDisplayParam;		//��ʾ����
	vca_TPolygon		stRegion;			//����Χ
	int  				iMiniSize;			//Min Size(range:0~100, default:5)
	int  				iMaxSize;			//Max Size(range:0~100, default:30)
}VCARunning,*pVCARunning;	

//VCA Face Mosaic
typedef struct tagVCAFaceMosaic 
{
	int					iBufSize;
	VCARule				stRule;
	vca_TDisplayParam	stDisplayParam;
	int					iMinSize;				//[0, 100] 
	int					iMaxSize;				//[0, 100] 
	int					iLevel;					//[0, 5] 
	int					iSensitiv;				//[0, 5] 
	int					iPicScal;				//[1, 10] 
	vca_TPolygonEx		stRegion1;				//[3, 32]
}VCAFaceMosaic,pVCAFaceMosaic;

//���ܷ�����Ⱥ�ۼ�����
typedef struct tagVCACrowd
{
	int					iBufSize;			//��Ⱥ�ۼ��ṹ���С
	VCARule				stRule;				//����ͨ�ò���
	int					iSensitivity;		//������(0~100)
	int					iTimes;				//����ʱ��(��)
	vca_TDisplayParam	stDisplayParam;		//��ʾ����
	vca_TPolygon		stRegion;			//����Χ
}VCACrowd,*pVCACrowd;

//���ܷ���������/���������
typedef struct tagVCADerelict
{
	int					iBufSize;			//������/������ṹ���С
	VCARule				stRule;				//����ͨ�ò���
	int					iMinTime;			//��������������ڵ����ʱ��(0~30000���룬Ĭ��ֵ��5000)
	int					iMinSize;			//��С���سߴ�(0~100���أ�Ĭ��ֵ��10)
	int					iMaxSize;			//������سߴ�(100~40000���أ�Ĭ��ֵ��10000)
	int 				iSunRegionNum;		//�Ӷ���θ���
	vca_TDisplayParam	stDisplayParam;		//��ʾ����
	vca_TPolygon 		stMainRegion;		//�����������
	vca_TPolygon 		stSubRegion[VCA_MAX_OSC_REGION_NUM];	//�Ӷ��������
}VCADerelict,VCAStolen,*pVCADerelict,*pVCAStolen;

//���ܷ���-�ӵ�������
typedef struct tagVCARiverClean
{
	int					iBufSize;			
	VCARule				stRule;				//����ͨ�ò���
	vca_TDisplayParam	stDisplayParam;		//��ʾ����
	int					iType;				//ģʽ 0:���桢�������㴦Ư������ 1:��ǰ�ѻ����� 2:վǰ�����ѻ�����	
	int					iSensitivity;		//������ Ĭ��2	��Χ0-5
	int					iMinSize;			//����Ư���� ��С�ߴ�(�����ȵİٷֱ�) ��Ĭ��ֵ10	ȡֵ��Χ[8, 100]����λ����
	int					iMaxSize;			//����Ư���� ���ߴ�(�����ȵİٷֱ�) ��Ĭ��ֵ30	ȡֵ��Χ[8, 100]����λ����
	int 				iPercentage;		//����Ư���� ռ��(�����ȵİٷֱ�) Ĭ��ֵ 10	ȡֵ��Χ[8, 100]����λ����
	vca_TPolygonEx		stPoints;			//��������򶥵�����Լ�����
}VCARiverClean,*pVCARiverClean;

//���ܷ���-���ɵ�ж����
typedef struct tagVCADredge
{
	int					iBufSize;			
	VCARule				stRule;				//����ͨ�ò���
	vca_TDisplayParam	stDisplayParam;		//��ʾ����
	int					iSensitivity;		//������ Ĭ��2	��Χ0-5
	int					iMinSize;			//��С�ߴ�(�����ȵİٷֱ�) ��Ĭ��ֵ5	ȡֵ��Χ[0, 100]����λ����
	int					iMaxSize;			//���ߴ�(�����ȵİٷֱ�) ��Ĭ��ֵ30	ȡֵ��Χ[0, 100]����λ����
	int 				iTimeMin;			//����ʱ�� ��Ĭ��ֵ3	ȡֵ��Χ[0, 3600]����λ����
	vca_TPolygonEx		stPoints;			//��������򶥵�����Լ�����
}VCADredge,*pVCADredge;

//���ܷ���-�ӵ����߼�����
typedef struct tagVCARiverAdvance
{
	int					iBufSize;			
	int					iSceneID;			//�������(0~15)
	int					iFGSampleNum;		//��ģ����������Ĭ��ֵΪ20��
	int					iFGDistThresh;		//������ֵ��Ĭ��ֵΪ18��
	int					iFGCountThresh;		//������ֵ��Ĭ��ֵΪ15��
	int 				iFGLifeThresh;		//������ֵ��Ĭ��ֵΪ10��
}VCARiverAdvance,*pVCARiverAdvance;

typedef struct tagVCASceneRecovery
{
	int					iBufSize;
	int					iChanNo;			//channel no
	int					iSceneId;			//scene id (0~15)
	int					iRecoveryTime;		//0~3600 second��defult:10��
}VCASceneRecovery,*pVCASceneRecovery;


typedef struct tagVCAChannelEnable
{
	int iBufSize;					//�ṹ���С
	int iEnable;
}VCAChannelEnable, *pVCAChannelEnable;

typedef struct tagVCAFacerec
{
	int iBufSize;					//�ṹ���С
	VCARule	stRule;
	vca_TDisplayParam stDisplayParam;			
	vca_TPolygon stRegion;				//	����Χ
}VCAFaceRec, *pVCAFacerec;

typedef struct tagVCATrace
{
	int iBufSize;					//�ṹ���С
	VCARule	stRule;
	vca_TDisplayParam stDisplayParam;			
	int					m_iStartPreset;			//��ʼ���ٵ�
	vca_TPolygon		m_TrackRegion;			//��������
	int					m_iScene;				//������/��/С
	int					m_iWeekDay;				//����
	NVS_SCHEDTIME		m_timeSeg[MAX_DAYS][MAX_TIMESEGMENT];		//����ʱ���
}VCATrace, *pVCATrace;

typedef struct tagVideoDetection
{
	int iBufSize;					//�ṹ���С
	VCARule	stRule;
	vca_TDisplayParam stDisplayParam;			
	int	m_iCheckTime;
}VideoDetection, *pVideoDetection;

typedef struct tagVCAVideoSize
{
	int iBufSize;					//�ṹ���С
	int iVideoSize;
}VCAVideoSize, *pVCAVideoSize;

//¼���ļ�������Ϣ
#define MAX_FILE_APPEND_INFO_TYPE	2
typedef struct tagFileAppendInfo
{
	int		iBufSize;				//¼���ļ�������Ϣ�ṹ���С
	int		iType;					//������Ϣ����,0-���� 1-��ϣ
	char    cFileName[LEN_256];		//¼���ļ���
	char	cValue[LEN_256];		//iTypeȡֵ1-����¼���ļ��Ĺ�ϣֵ���ַ�����
}FileAppendInfo, *pFileAppendInfo;

typedef struct tagVCASuspendResult
{
	int		iBufSize;				//�ṹ���С
	int		iStatus;				//״̬(0:��ͣ���ܷ��� 1:�������ܷ���)
	int		iResult;				//���(1:�ɹ�[��ʾ�������] 2:ʧ��[��ʾ�����������ã��������])
}VCASuspendResult, *pVCASuspendResult;

typedef struct tagBackupDevModify
{
	int					iBufSize;			//�ṹ���С
	int					iDevType;			//�豸����(0:�����������ȱ���1:�ȱ�������������)
	int					iOptType;			//��������(0:ADD 1:DEL)	
	char				cIP[LEN_64];		//�ȱ���IP
	char				cUserName[LEN_64];	//��������¼�û���	
	char				cPassword[LEN_64];	//��������¼����	
}BackupDevModify,*pBackupDevModify;

typedef struct tagWorkDevInfo
{
	int					iBufSize;		//�ṹ���С
	int					iIndex;			//����
	char				cIP[LEN_64];	//������IP��ַ	
}WorkDevInfo,*pWorkDevInfo;

typedef struct tagVirtualDisk
{
	int					iBufSize;			//�ṹ���С
	int 				iIndex;			//���������ָ������ֵ
	int					iOptType;		//��������
	char				cVDName[LEN_64];	//�����������
	int					iVdSize;			//������̴�С(��λΪM)
	int					iInitType;	//��ʼ������(1:ǰ̨2:��̨3:����) [�½����޸���ɾ������ʱ��Ч]
	char				cRaidName[LEN_64];	//��������
}VirtualDisk,*pVirtualDisk;



#define MAX_RAID_DISK_NUM 16   
typedef struct tagRAIDWorkMode
{
	int		iBufSize;		             //�ṹ���С
	int		iDiskID;		             //���̱��(SATA1~SATA8��š�,SATA9~SATA16���~1015)
	char	cRaidName[LEN_64];	         //��������
	int		iWorkMode;		             //�ȱ�������(0:�����̣�1:�����ȱ��̣�2:ȫ���ȱ��̣�3��RAID�̣�4����ЧRAID�̣�5��������)
}RAIDDiskWorkMode, *pRAIDDiskWorkMode;
#define DISKWORKTYPE_RAIDHOTBACK	1

#define MAX_RAID_NUM 8	//ȫ�����֧��8������   
typedef struct tagRAIDState
{
	int					iBufSize;			//�ṹ���С
	int 				iIndex;				//���������ָ������ֵ
	char				cRaidName[LEN_64];	//��������
	int					iRaidAllSize;		//�ܴ�С(��λΪM)
	int					iRiadUseAbleSize;	//���ÿռ�(��λΪM)
	int					iRaidState;			//����״̬(1:����2:����3:����)
	int					iRaidTask;			//����(1:��2:���ڳ�ʼ��3:�����ؽ�)
	int					iRaidTaskPara1;		//����״̬����(����x.x%)
	int					iRaidTaskPara2;	    //����״̬����(Ԥ��ʣ��ʱ��x.xСʱ,x.x=0ʱ����ʾ����)
}RAIDState, *pRAIDState;              

#define MAX_VIRTUAL_DISK_NUM 16			//ȫ�����֧��16���������  
typedef struct tagVirtualDiskState
{
	int					iBufSize;			//�ṹ���С
	int 				iIndex;			    //���������ָ������ֵ
	char				cVDName[LEN_64];	//�����������
	int					iVdUseAbleSize;		//������̿��ÿռ�(��λΪM)
	int					iVdState;			//�������״̬(1:����2:����3:����)
	int					iVdTask;			//����(1:��2:���ڳ�ʼ��3:�����޸�)
	int					iVdTaskPara1;		//����״̬����(����x.x%)
	int					iVdTaskPara2;		//����״̬����(Ԥ��ʣ��ʱ��x.xСʱ,x.x=0ʱ����ʾ����)
}VirtualDiskState, *pVirtualDiskState;     

#define MAX_HDD_NUM 16   
#define WORKDEV_BROADCAST_BACKUPDEV		0   
#define BACKUPDEV_BROADCAST_WORKDEV		1   
#define MAX_WORK_DEV_COUNT		16			//�����������

typedef struct tagBackupDevState
{
	int					iBufSize;			//�ṹ���С
	int 				iIndex;				//���������ָ������ֵ
	char				cIP[LEN_64];		//�豸IP
	int					iState;			    //״̬([DevType=0 0:����1:����2:����ͬ��]
	                                        //[DevType=1 0:�ȱ�1:����2:ͬ��])
	int					iProgress;		    //ͬ���ٷֱ�(0~100)
}BackupDevState,WorkDevState, *pBackupDevState,*pWorkDevState;  

typedef struct tagRAIDDiskInfo
{
	int					iBufSize;			//�ṹ���С
	int					iDiskID;			//���̱��(SATA1~SATA8��š�,SATA9~SATA16���~1015)
	int					iDiskType;			//��������(0:������1:�����ȱ���2:ȫ���ȱ���3:RAID��4����ЧRAID�̣�5��������)
	int					iDiskState;			//����״̬(0:����1:�쳣2:����Ԥ��)
	int					iDiskSize;			//���̴�С(��λΪM)
	char				cRaidName[LEN_64];	//��������
	char				cDiskModel[LEN_64];	//�����ͺ�
}RAIDDiskInfo,*pRAIDDiskInfo;

typedef struct tagIPSANDiskInfo
{
	int					iBufSize;			//�ṹ���С
	int					iDevNo;				//���(0��)
	int					iEnable;			//�Ƿ�����(0:����1:����)
	char				cDeviceIP[LEN_64];	//IP��ַ
	int					iDevicePort;		//�˿ں�
	char				cUserName[LEN_32];	//�û���
	char				cPassword[LEN_32];	//����
	char				cPath[LEN_64];		//Ŀ¼
}IPSANDiskInfo,*pIPSANDiskInfo;

typedef struct tagIPSANDiscovery
{
	int					iBufSize;			//�ṹ���С
	int 				iDevType;			//����(0:����1:iSCSI)
	char				cDeviceIP[LEN_64];	//IP��ַ
	int					iDevicePort;		//�˿ں�
	char				cUserName[LEN_32];	//�û���
	char				cPassword[LEN_32];	//����
}IPSANDiscovery,*pIPSANDiscovery;


//���IPSANĿ¼��Ϣ
typedef struct tagIPSANDirectoryInfo
{
	int					iBufSize;			//�ṹ���С
	int 				iIndex;				//���������ָ������ֵ
	char				cDeviceIP[LEN_64];	//IP��ַ
	char				cPath[LEN_64];		//Ŀ¼
}IPSANDirectoryInfo,*pIPSANDirectoryInfo;


#define MAX_RAID_NUM_NUM				8
#define MAX_RAID_TYPE_NUM				4
#define MAX_IPSAN_INFO_NUM				8
#define MAX_IPSAN_DISCOVERY				16
//�豸�������������Ϣʹ��
#define NET_CLIENT_IPSAN_DISCOVERY				(NET_CLIENT_MIN + 0)	//IPSAN�豸����
#define NET_CLIENT_IPSAN_GET_DIR_COUNT			(NET_CLIENT_MIN + 1)	//IPSAN����Ŀ¼����
#define NET_CLIENT_IPSAN_GET_DIRECTORY			(NET_CLIENT_MIN + 2)	//ȡ��Ŀ¼��Ϣ
#define NET_CLIENT_IPSAN_DISK_INFO				(NET_CLIENT_MIN + 3)	//IPSAN�豸��Ϣ
#define NET_CLIENT_RAID_DISK_WORKMODE			(NET_CLIENT_MIN + 4)	//RAID���̹���ģʽ
#define NET_CLIENT_RAID_DISK_INFO				(NET_CLIENT_MIN + 5)	//RAID������Ϣ
#define NET_CLIENT_RAID_ARRAY_MANAGE			(NET_CLIENT_MIN + 6)	//�½�RAID����
#define NET_CLIENT_RAID_ARRAY_INFO				(NET_CLIENT_MIN + 7)	//���RAID������Ϣ
#define NET_CLIENT_RAID_STATE					(NET_CLIENT_MIN + 8)	//RAID״̬
#define NET_CLIENT_RAID_VIRTUAL_DISK_MANAGE		(NET_CLIENT_MIN + 9)	//�½�RAID�������
#define NET_CLIENT_RAID_VIRTUAL_DISK_INFO		(NET_CLIENT_MIN + 10)	//���RAID���������Ϣ
#define NET_CLIENT_RAID_VIRTUAL_DISK_STATE		(NET_CLIENT_MIN + 11)	//RAID�������״̬

//raid �������� 0=������1=�½���2=�ؽ��� 3=ɾ��
#define RAID_OPERATE_RESERVE		0
#define RAID_OPERATE_CREATE			1
#define RAID_OPERATE_REBUILD		2
#define RAID_OPERATE_DELETE			3

//RAID������Ϣ
typedef struct tagRAIDArrayInfo
{
	int					iBufSize;					//�ṹ���С
	int 				iIndex;						//���������ָ������ֵ
	char				cRaidName[LEN_64];			//��������
	int					iOptType;					//��������,0=������1=�½���2=�ؽ��� 3=ɾ��
	int					iRaidType;					//���м���(0:RAID0 1:RAID1 5:RAID5 10:RAID10 100:JBOD)
	int					iBackDiskID;				//	-1��ʾ���ȱ��̣�����0��ʾ�ȱ���ID	
	char				cDiskModel[LEN_64];			//�����ͺ�
	int					iDiskNum;					//���̸���
	int					iDisk[MAX_RAID_DISK_NUM];	//�����б�
}RAIDArrayInfo,*pRAIDArrayInfo;

typedef struct tagAlarmLink_V1
{
	int				iBuffSize;
	int				iSceneID;			//�������(0~15)
	int				iAlarmTypeParam1;	//�������Ͳ���
	int				iAlarmTypeParam2;	//�������Ͳ���
	int				iLinkType;			//��������
	int				iLinkTypeParam1;	//��������
	int				iLinkTypeParam2;	//��������
	int				iLinkTypeParam3;	//��������
	int				iLinkTypeParam[13];	//����������֧��512·��չ
}AlarmLink_V1, *PAlarmLink_V1;
/************************************************************************/
/*       ��������      |      �������Ͳ���1    |      �������Ͳ���2     */
/*----------------------------------------------------------------------*/
/*      0:��Ƶ��ʧ     |        δʹ��         |          δʹ��        */
/*----------------------------------------------------------------------*/
/*      1:�˿ڱ���     |        δʹ��         |          δʹ��        */
/*----------------------------------------------------------------------*/
/*      2:�ƶ����     |        δʹ��         |          δʹ��        */
/*----------------------------------------------------------------------*/
/*      3:��Ƶ�ڵ�     |        δʹ��         |          δʹ��        */
/*----------------------------------------------------------------------*/
/*      4:���ܷ���     |        ����ID         |         �¼�����       */
/*----------------------------------------------------------------------*/
/*      5:��Ƶ��ʧ     |        δʹ��         |          δʹ��        */
/*----------------------------------------------------------------------*/
/*      6:��ʪ�ȱ���   |        δʹ��         |          δʹ��        */
/*----------------------------------------------------------------------*/
/*      7:Υ�¼��     |        δʹ��         |          Υ������      */
/************************************************************************/
/*								�¼�����								*/
/*----------------------------------------------------------------------*/
/* 0�������� 1��˫���� 2���ܽ��� 3���ǻ� 4��ͣ�� 5������				*/
/* 6����������Ա�ܶ� 7�������� 8�������� 9������ʶ�� 10����Ƶ���		*/
/* 11�����ܸ���  12������ͳ�� 13����Ⱥ�ۼ� 14����ڼ�� 15��ˮλ���	*/
/*16����Ƶ��� 17�������ڵ�(������)										*/                                                           
/************************************************************************/
/*								Υ������								*/
/*----------------------------------------------------------------------*/
/* 1:���� 2������� 3������ 4������ 5����ֹ����ʻ 6����ֹ����ʻ 		*/
/* 7��ѹ���� 8���ǻ����� 9������������ʻ 10����ֱֹ�� 11��ռ�ù������� 	*/
/* 12��ѹ������ 13����ת���򴳺�� 14��Υ��ͣ�� 15��Υ��ͣ�� 16������	*/ 
/* 17����ͷ																*/ 
/************************************************************************/

/************************************************************************/
/*     ��������    |    ��������1    |    ��������2   |    ��������3    */
/*----------------------------------------------------------------------*/
/* 0:����������ʾ  |       ʹ��      |      δʹ��    |      δʹ��     */
/*----------------------------------------------------------------------*/
/* 1:������Ļ��ʾ  |       ʹ��      |      δʹ��    |      δʹ��     */
/*----------------------------------------------------------------------*/
/* 2:��������˿�  |     ��λʹ��    |      δʹ��    |      δʹ��     */
/*----------------------------------------------------------------------*/
/* 3:����¼��      |     ��λʹ��    |      δʹ��    |      δʹ��     */
/*----------------------------------------------------------------------*/
/* 4:����PTZ       |      ͨ����     |     0:������   |    δʹ��       */
/*                 |                 |     1:Ԥ��λ   |    Ԥ��λ��     */
/*                 |                 |     2:�켣     |    �켣��       */
/*                 |                 |     3:·��     |    ·����       */
/************************************************************************/
/*								   ʹ��	    							*/
/*----------------------------------------------------------------------*/
/* 0����ʹ�ܣ�1��ʹ��													*/
/************************************************************************/
/*							     ��λʹ��								*/
/*----------------------------------------------------------------------*/
/* �����λ�����λÿһλ��ʾһ������Ƶͨ��/����˿ڵ�ʹ��				*/
/* 64·ʹ����������2��ʾ��32·ʹ��										*/
/************************************************************************/

//���÷�α��λ��
typedef struct tagITS_SecurityCode
{
	int		iBufSize;	//�ṹ���С
	int		iPos;		//��ʼλ��,ȡֵΪ0-64
	int		iCount;		//��α��λ��,ȡֵΪ0-64
}ITS_SecurityCode, *pITS_SecurityCode;

//�������Ȳ��������ṹ��
typedef struct tagITS_LightSupply
{
	int		iBufSize;		//�ṹ���С
	int		iIndex;			//ʱ��α��	0-7��֧�����8��ʱ���
	int		iFrontLight;	//˳�ⲹ��ֵ	
	int		iBackLight;		//��ⲹ��ֵ	
	int		iEnable;		//0-unable 1-enable(tolerant)
	int		iSensitivity;	//������(1,100)
	int		iPolarlzerValue;//ƫ����ֵ(0,100��
}ITS_LightSupply, *pITS_LightSupply;

#define MAX_ITS_CAR_TYPE	3
//���ó����ڳ�������ѹ��ǧ�ֱȲ���
typedef struct tagITS_LinePressPermillage
{
	int  iBufSize;			//�ṹ���С
	int  iSceneId;			//����ID,0~15
	int  iRoadID;			//�������,���֧��4��������0-3
	int  iCarType;			//��������,0-������1-С����2-��
	int  iSpeed;			//��������ѹ��ǧ�ֱȡ�1-1000��
}ITS_LinePressPermillage,*pITS_LinePressPermillage;

//���ͽ�ͨר���ַ�����
#define  MAX_ITS_OSDTYPE_NUM	       36		//��ǩ��������
typedef struct tagITS_WordOverLay
{
	int  	iBufSize;							//�ṹ���С
	int  	iChannelNo;							//ͨ����
	int   	iOsdType;							//�����ַ��ı�ǩ����
	int   	iEnable;							//ʹ��״̬,����λ�������ͣ��ַ�������������.0: ����ʱ��ʹ�ܣ�,,,
	char  	pcOsdName[LEN_32+1];				//��ǩ����
	int   	iPosX;							    //������
	int     iPosY;							    //������
	int		iZoomScale;							//���Űٷֱ�	��Χ0-500��100�����ޱ仯					
}ITS_WordOverLay,*pITS_WordOverLay;


#define MAX_ITS_CAPTURE_NUM	16
//ץ�������ṹ��
typedef struct tagITS_CapCount
{
	int		iBufSize;		//�ṹ���С
	int		iCapType;		//ץ������	0-����ץ�� 1--����� 2--���� 3--���� 4--��ֹ����ʻ 5--��ֹ����ʻ 6--ѹ���� 7--�ǻ����� 
							//8--�����涨������ʻ(����������ʻ) 9--��ֱֹ�� 10--�������� 11--ѹ������ 12--��ת���򴳺�� 13--Υ��ͣ��
							//14--��ϴ�������Ƶץ�ĳ��ٹ���ʹ�� 15--���� 16--��ͷ
	int		iCapCount;		//ץ������	ȡֵΪ1��2,��С1��Ŀǰ���޿�����10����
}ITS_CapCount, *pITS_CapCount;

#define MAX_ILLEGAL_PARK_POINT_NUM		8
typedef struct tagITS_IVTMoveCTR
{
	int		iBufSize;		//�ṹ���С
	int		iXMove;			//���·����˶����꣬��ȷ��0.01 *100
	int		iYMove;			//���ҷ����˶����꣬��ȷ��0.01 *100
	int		iZAim;			//��������ȷ��0.01*100*
}ITS_IVTMoveCTR, *pITS_IVTMoveCTR;

typedef struct tagITS_IllegalParkInfo
{
	int				iBufSize;										//�ṹ���С
	int				iPreset;										//Ԥ��λ���(����ID),���16���� 0~15
	int				iArea;											//������,���8����0~7 
	int				iCalibrationStaus;								//0-δ�궨,1-�ֶ��궨,2-�Զ��궨
	ITS_IVTMoveCTR	tITS_IVTMoveCTR[MAX_PARAM_COUNT];				//�궨������꣬����ֵ����
	POINT			tPoint[MAX_ILLEGAL_PARK_POINT_NUM];				//λ������������		
}ITS_IllegalParkInfo, *pITS_IllegalParkInfo;

typedef struct _tagITS_PicCutInfo
{
	int iSize;
	int iChannelNo;
	int iCutType;	// Picture cut types 0-base plate; 1-base car flag;2-base face
	int iCutScale;  //characteristics pictures cut range, such as 4, represent cut range is 1/4 of the size of raw picture.
}ITS_PicCutInfo, *pITS_PicCutInfo;

typedef struct tagCfgFileInfo
{
	int		iBufSize;				//�ṹ���С
	char	cFileName[LEN_64+1];	//�ļ���,64�ֽ�
	char	cSection[LEN_64+1];		//������,64�ֽ�
	char	cKey[LEN_64+1];			//�ؼ���,64�ֽ�
	char	cValue[LEN_64+1];		//�ֶ�ȡֵ,������ֶ��Ƕ�����ֵ������ʹ�ã����ŷָ���硰9600,n,7,2 ��
}CfgFileInfo, *pCfgFileInfo;

#define MAX_PLATFORM_NUM				10
#define MAX_APP_SERVER_LIST_NUM			10
//Զ�̶�ȡ/�����¿���ֵ
typedef struct tagAPPServerList
{
	int		iBufSize;					//�ṹ���С
	char	cAppName[LEN_32+1];			//ƽ̨���ƣ�������32�ֽ�
	int		iServerListNum;				//�������������10��
	char	cServerListName[MAX_APP_SERVER_LIST_NUM][LEN_32+1];	//�������ƣ�������32�ֽ�
	int		iState[MAX_APP_SERVER_LIST_NUM];
}APPServerList, *pAPPServerList;

#define MAX_RTMP_NUM		16
//RTMP�б���Ϣ
typedef struct tagRtmpInfo
{
	int		iBufSize;			//�ṹ���С
	int		iRtmpNo;			//���,ȡֵ0~15
	int		iRtmpChnNo;			//��Ƶͨ����
	int		iRtmpEnable;		//ʹ�ܣ�ʹ�ܺ󣬿�����������ʹ�ܺ󣬽�ֹ����
	char	cRtmpUrl[LEN_256];	//Rtmp��URL�ִ�,�ַ�����Ϣ���磺"rtmp://10.30.31.21:1935/live"
}RtmpInfo, *pRtmpInfo;

//ĳͨ��֧��֡���б�
#define MAX_FRAME_RATE_NUM		20
typedef struct tagFrameRateList
{
	int		iBufSize;			//�ṹ���С
	int		iChannelNo;			//ͨ���ţ�ȡֵ��Χ�����豸ʵ��������������������������16ģ��+16���ֵĻ��DVRΪ����ȡֵ��ΧΪ0��63
								//������4n~5n-1
	int		iFrameRateNum;		//֡�ʸ������豸��ʾ��֡�ʸ������������ֶ���
	int		iFrameRate[MAX_FRAME_RATE_NUM];	//֡���б����Ӵ�С˳��洢��N�ƣ�60 30 25 20 15 10 5 1 ��P�ƣ�50 25 20 15 10 5 1
}FrameRateList, *pFrameRateList;

//RTSP�б���Ϣ
#define MAX_RTSP_LIST_NUM		64
typedef struct tagRtspListInfo
{
	int		iBufSize;			//�ṹ���С
	int		iRtspNo;			//rtsp��ţ���Χ0~63
	char	cRtspIp[LEN_32];	//rtsp����ip��Ĭ��0.0.0.0����0.0.0.0�����������κ�������
}RtspListInfo, *pRtspListInfo;

/////////////////////////////////��������////////////////////////////////
//��������ͨ������
#define ALARM_INTERFACE_TYPE_MIN       0
#define ALARM_INTERFACE_TYPE_SWITCH    1  //switch
#define ALARM_INTERFACE_TYPE_ANALOG    2  //analog
#define ALARM_INTERFACE_TYPE_COM       3  //Serial port
#define ALARM_INTERFACE_TYPE_MAX       4


//������������ NetClient_SetAlarmConfig/ NetClient_GetAlarmConfig
#define CMD_DH_ALARM_MIN					(100)//����������ӿ���Сֵ
#define CMD_ALARM_IN_CONFIG                 (CMD_DH_ALARM_MIN + 0)//������������
#define CMD_ALARM_IN_LINK                   (CMD_DH_ALARM_MIN + 1)//������������
#define CMD_ALARM_IN_SCHEDULE				(CMD_DH_ALARM_MIN + 2)//��������ģ������
#define CMD_ALARM_IN_SCHEDULE_ENABLE        (CMD_DH_ALARM_MIN + 3)//��������ʹ������
#define CMD_ALARM_IN_OSD                    (CMD_DH_ALARM_MIN + 4)//�����ַ���������
#define CMD_ALARM_IN_DEBUG                  (CMD_DH_ALARM_MIN + 5)//��������
#define CMD_ALARM_IN_OFFLINE_TIME_INTERVEL  (CMD_DH_ALARM_MIN + 6)//�ѻ����
#define CMD_DH_ALARM_HOST  					(CMD_DH_ALARM_MIN + 7)//������������������
#define CMD_DH_ADD_ALARM_HOST				(CMD_DH_ALARM_MIN + 8)//��ӱ�������
#define CMD_DH_DEVICE_ENABLE				(CMD_DH_ALARM_MIN + 9)//���ö����豸ʹ��
#define CMD_DH_ALARM_MAX					(CMD_DH_ALARM_MIN + 10)//MAX

#define MAX_NAME_LEN 64
#define MAX_IPADDRESS_LEN 64

typedef struct __tagAlarmInConfig
{	
	int iSize;
	char cName[MAX_NAME_LEN + 1];
	int iInterfaceType; //ALARM_IN_CHANNEL_TYPE_MIN~  ALARM_IN_CHANNEL_TYPE_MAX
	int iInterfaceNo ;
	int iScheduleNo;
	int iDelay;
	char cParam[MAX_DHINFO_PARAM_LEN];
}AlarmInConfig, *PAlarmInConfig;


typedef struct __tagAlarmInLink
{	
	int iSize;
	int  iLinkIndex;
	int iLinkEnable;
	char cName[MAX_NAME_LEN + 1];
	int iAlarmType; 
	int iLinkType;
	int  iDisappearType;		//��������
	int  iDisappearTime;        //����ʱ��
	char cParam[MAX_DHINFO_PARAM_LEN];
}AlarmInLink, *PAlarmInLink;

typedef struct __tagAlarmInSchedule
{	
	int iSize;
	int iScheduleNo;
	int iEnable;
	char cName[MAX_NAME_LEN + 1];
	int iWeekday;//0~6 ����~��һ   -1 ��������  
	NVS_SCHEDTIME	timeSeg[MAX_DAYS][MAX_DH_TIMESEGMENT];	
}AlarmInSchedule, *PAlarmInSchedule;

#define ALARM_IN_SCHEDULE_DISENABLE 0
#define ALARM_IN_SCHEDULE_ENABLE    1 
#define ALARM_IN_SCHEDULE_AUTO      2
#define MAX_ALARM_IN_SCHEDULE		3

typedef struct __tagAlarmInOSD
{	
	int iSize;
	int iOsdIndex;
	int iEnable;
	char cIP[LENGTH_IPV4];
	int iChannelNo;
	int iBlockNo;	
	int iColor;
}AlarmInOSD, *PAlarmInOSD;

typedef struct __tagAlarmInDebug
{	
	int iSize;
	char    cName[MAX_NAME_LEN + 1];
	int iDelayTime;
	char cParam[MAX_DHINFO_PARAM_LEN];
}AlarmInDebug, *PAlarmInDebug;

#define MAX_DH_ALARMTYPE_NUM		2
#define MAX_DH_ALARMHOST_NUM		8
typedef  struct _tagAlarmHost
{
	int iSize;				
	int iDevType;  				//������������0�����籨��������1�����ڱ�������
	int	iDevNo;					//�����������0-7
	int iEnable;				//ʹ�ܱ�־0�����ã�1������
	char cDevAddr[LEN_64];		//����������ַ��IP��ַ�����64�ֽڣ�
	int	iDevPort; 				//���������˿ڣ�0��65535��Ĭ��18803��
	int	iConnType;				//�������ͣ�1��TCP��2��UDP��Ĭ�ϣ�
	int	iPortInNum; 			//����˿���
	int	iPortOutNum;			//����˿���
}tAlarmHost, *ptAlarmHost;

//������������
#define ALARM_SERVER_NET               0
#define ALARM_SERVER_COM               1

//��������
#define ALARM_SERVER_CONNTYPE_TCP       1
#define ALARM_SERVER_CONNTYPE_UDP       2

typedef struct __tagLogInterval
{	
	int		iSize;
	int		iDelayTime;
}tLogInterval, *PtLogInterval;

typedef struct __tagComDevice
{
	int		iSize;				//Size of the structure,must be initialized before used
	int		iComNo;				//���ڱ��
	char	cComFormat[LEN_32];	//��ʽ��9600,n,8,1
	int     iDeviceType;        //0- 1- 2-
	char	cParam[MAX_DHINFO_PARAM_LEN];
}ComDevice,*PComDevice;

#define FILE_COUNT			16	//���֧�ָ���
typedef struct ExportConfig
{
	int		iSize;		    //Size of the structure,must be initialized before used
	int		iCount;		    //��������			      
	char   	cFileList[FILE_COUNT][MAX_NAME_LEN];	//�ļ��б�
	char    cFileOut[128];  //����ļ�
} ExportConfig  ,*PExportConfig;

#define MAX_DH_SER_NO		100000   //������ˮ�� 
//���������ϱ� WCM_ALARM_INFORMATION
typedef struct __tagAlarmInfoData
{	
	int iSize;
	int iSerNo;
	int iDhInterfaceType;			  //ͨ������
	int iStatus;
	int iChannelNo;
	int iAlarmType;
	int iAlarmState;
	int iDataType;
	unsigned long iTime;
	char cInfo[MAX_DHINFO_PARAM_LEN];	
}AlarmInfoData, *PAlarmInfoData;

//�ַ������ͽ��յĲ�������
#define MAX_RECV_AND_SPLIT_NUM		16   //��̬���ջ��Ͳ�ֵ��ַ�����Ŀ
#define MAX_PROTOCO_SPLIT_NUM		40   //��̬���ջ��Ͳ�ֵ�Э���ַ�����Ŀ

//���������豸����
#define	DH_COM_DEV_TYPE_TEM			0	  //��ʪ��	
#define	MAX_DH_DEV_TYPE				40    //���֧�ֵ��豸���͸���
#define MAX_DH_COM_PROTOCOL			40	  //���֧��Э����������(�����豸��ӦΪ֧�ֵ������豸����)

//DH��������
#define MAX_DH_ALARMLINK_NUM		16    //��������������Ŀ
#define MAX_DH_ALARMSCHE_NUM		16    //��������ģ����Ŀ

//DH������������
#define DH_ALARM_LINK_TYPE_MIN				0
#define DH_ALARM_LINK_TYPE_RESERVE			(DH_ALARM_LINK_TYPE_MIN+0)	//����
#define DH_ALARM_LINK_TYPE_OUT				(DH_ALARM_LINK_TYPE_MIN+1)	//�������
#define DH_ALARM_LINK_TYPE_OSD				(DH_ALARM_LINK_TYPE_MIN+2)	//OSD
#define DH_ALARM_LINK_TYPE_EMBATTLE			(DH_ALARM_LINK_TYPE_MIN+3)	//����
#define DH_ALARM_LINK_TYPE_DISMANTLE		(DH_ALARM_LINK_TYPE_MIN+4)	//����
#define DH_ALARM_LINK_TYPE_RECOVERY			(DH_ALARM_LINK_TYPE_MIN+5)	//�ָ��Կ�
#define DH_ALARM_LINK_TYPE_MAX				(DH_ALARM_LINK_TYPE_MIN+6)

//DH��������
#define DH_ALARM_DISAPPEAR_TYPE_MIN				0
#define DH_ALARM_DISAPPEAR_TYPE_NOT				(DH_ALARM_LINK_TYPE_MIN+0)	//���ָ�
#define DH_ALARM_DISAPPEAR_TYPE_DELAY			(DH_ALARM_LINK_TYPE_MIN+1)	//��ʱ�ָ�
#define DH_ALARM_DISAPPEAR_TYPE_DIS_RECOVERY	(DH_ALARM_LINK_TYPE_MIN+2)	//�����ָ�
#define DH_ALARM_DISAPPEAR_TYPE_DIS_DELAY		(DH_ALARM_LINK_TYPE_MIN+3)	//������ʱ�ָ�
#define DH_ALARM_DISAPPEAR_TYPE_MAX				(DH_ALARM_LINK_TYPE_MIN+4)

#define MAX_DH_ALARM_HOST_NUM		16
typedef struct tagAddAlarmHost
{
	int iSize;					//�ṹ���С
	int iHostIndex;				//����������[0,15]
	int iEnable;				//0--��ʹ�ܣ�1--ʹ��
	char cIP[LEN_32];			//��������IP
	int	iPort;					//���������˿�
}AddAlarmHost, *pAddAlarmHost;
//////////////////////////////////��������///////////////////////////////

/////////////////////////////////����////////////////////////////////
#define		Td_Alg_Meter_TaoGuanBiao_1					0/* �׹ܱ��׵ף� */
#define		Td_Alg_Meter_ZhuBianYouBiao					1/* �����ͱ� */
#define		Td_Alg_Meter_BiLeiQi						2/* ������ */
#define		Td_Alg_Meter_DangWeiXianShiQi_1				3/* ��λ��ʾ��1���뾶С��*/
#define		Td_Alg_Meter_WenDuBiao						4/* �¶ȱ� */
#define		Td_Alg_Meter_KaiGuanFenHeQi					5/* ���طֺ��� */
#define		Td_Alg_Meter_QiTiYaLiBiao					6/* ����ѹ���� */
#define		Td_Alg_Meter_YaLiBiao						7/* ѹ���� */
#define		Td_Alg_Meter_DaoZha							8/* ��բ */
#define		Td_Alg_Meter_BiLeiJianCeBiao				9/* ���׼��� */
#define		Td_Alg_Meter_WenDuBiaoQiTiBiao				10/* �¶ȱ������ */

#define		Td_Alg_Meter_DuoGongNengDianNengBiao		11/* �๦�ܵ��ܱ� */
#define		Td_Alg_Meter_XianLuBaoHuZhuangZhi			12/* ��·����װ�� */
#define		Td_Alg_Meter_HuaLiDuoGongNengDianNeng		13/* �����๦�ܵ��� */
#define		Td_Alg_Meter_WeiShengDuoGongNengDianNeng	14/* ��ʤ�๦�ܵ��� */
#define		Td_Alg_Meter_ZhiNengWenShiDuKongZhiQi		15/* ������ʪ�ȿ����� */
#define		Td_Alg_Meter_KaiGuanZhiNengCaoKongZhuangZhi	16/* �������ܲٿ�װ�� */
#define		Td_Alg_Meter_SanXiangDuoGongNengDianNeng	17/* ����๦�ܵ��� */
#define		Td_Alg_Meter_KaiGuan						18/* ���� */
#define		Td_Alg_Meter_KaiGuanFenHeXianShiQi			19/* ���طֺ���ʾ�� */
#define		Td_Alg_Meter_TaoGuanBiao_2					20/* �׹ܱ�2�����֣��뾶�� */
#define		Td_Alg_Meter_MiDuJiDianQi					21/* �ܶȼ̵��� */
#define		Td_Alg_Meter_YouWeiJiBiao					22/* ��λ�Ʊ� */
#define		Td_Alg_Meter_DangWeiXianShiQi_2				23/* ��λ��ʾ��2����ף� */

typedef struct tagEleNetMeter
{
	int iSize;					//�ṹ���С
	int iChanNo;				//ͨ����
	int iMeterType;				//�Ǳ�����
	char cParam[1024];			//�ַ���
}EleNetMeter, *pEleNetMeter;
/////////////////////////////////����/////////////////////////////////

//NvsType����0-T��1-S��2-T+
#define NVS_TYPE_T			0
#define NVS_TYPE_S			1
#define NVS_TYPE_T_OTHER	2

typedef struct tagLogonPara
{
	int		iSize;					//�ṹ���С
	char	cProxy[LEN_32];			//������Ƶ��������һ������IP��ַ��������32���ַ���������\0��
	char	cNvsIP[LEN_32];			//IP��ַ��������32���ַ���������\0��
	char	cNvsName[LEN_32];		//Nvs name ��������ʱʹ��
	char	cUserName[LEN_16];		//��¼Nvs�û�����������16���ַ���������\0��
	char	cUserPwd[LEN_16];		//��¼Nvs���룬������16���ַ���������\0��
	char	cProductID[LEN_32];		//��ƷID��������32���ַ���������\0��
	int		iNvsPort;				//��Nvs��������ʹ�õ�ͨѶ�˿ڣ�����ָ����ʹ��ϵͳĬ�ϵĶ˿�(3000)��
	char	cCharSet[LEN_32];		//�ַ���
	char	cAccontName[LEN_16];	//����Ŀ¼���������ʻ�
	char	cAccontPasswd[LEN_16];	//����Ŀ¼���������ʻ�����
}LogonPara, *pLogonPara;

//ITSͼ������������ṹ��
typedef	struct tagITS_VideoDetect
{
	int			iSize;					//�ṹ���С
	int			iEngineType;
	int			iEnable;
	int			iVedioType;
	int			iVedioWidth;
	int			iVedioHeight;
	int			iVehicleMinSize;
	int			iVehicleMaxSize;
	int			iModelType;
	int			iFrameRateCount;
	int			iVDetectMotor;			//�ǻ��������ʹ��
	int			iSceneID;				//����ID
	char		cParams130T[LEN_64];	//�㷨1��˽�в���
	char		cParams130G[LEN_64];	//�㷨2��˽�в���
	char		cParams110G[LEN_64];	//�㷨3��˽�в���
}ITS_VideoDetect, *PITS_VideoDetect;

//ITS Roadway rader area config info struct
typedef	struct tagITS_RaderCfgInfo
{
	int			iSize;					//Size of Struct
	int			iRoadwayID;				//Roadway ID,range[0,3]
	int			iRaderAreaID;			//Rader area ID,range[1,8]
	int			iRaderAreaLeftEdge;		//Rader area left edge value,range[-200-200]
	int			iRaderAreaRightEdge;	//Rader area right edge value,range[-200-200]
	int			iRaderAreaLine;			//Rader area line,range[1,100] Unit :( m)
}ITS_RaderCfgInfo, *PITS_RaderCfgInfo;


//download state
#define		VOD_DOWNLOAD_NORMAL		0
#define		VOD_DOWNLOAD_PAUSE		1


typedef struct __tagVideoEncodList
{
	int iSize;		   //struct size	
	int iChnNo;        //dev channel no
	int iVideoEncode;  //EncodType:By byte��bit0��H.264��bit1��M-JPEG��bit2��MPEG4��bit3��H.265��def:0
}tVideoEncodList,*ptVideoEncodList;


typedef struct
{
	int iSize;    //�ṹ���С
	int iIndex;   //������
	int iTime;    //ʱ���
	int iCount;   //ץ������
	int iInterVal;//ץ�ļ��
}WaterSampleSnapInfo,*pWaterSampleSnapInfo;

#define MAX_PRESET_NUM		32
typedef  struct WaterSamplePoint
{
	int iSize;
	VCARule	stRule;
	vca_TDisplayParam	stDisplayParam;		//��ʾ����
	int iSamplePointId;
	int iSnapInterval;
	int iBaseValue;       //base water level value
	int iCurrentPresetId; //current preset id
	int	iGaugeType;		//  ˮ�����ͣ�0-������1-��ߡ��³ߣ�2-խ�ߡ��ɳ�
	int iPresetCount;
	WaterPresetInfo stPresetInfo[MAX_PRESET_NUM];
}WaterSamplePoint, *LPWaterSamplePoint;

typedef struct _tagExceptionLinkOut
{
	int iSize;								//Ȩ�޺�
	int iExceptionType;						//�쳣����
	unsigned int iExceptionEnable[LEN_16];  //�쳣ʹ��
}tExceptionLinkOut, *ptExceptionLinkOut;

typedef struct _tagDiskOperation
{
	int iSize;
	int iAction;
	int iDiskNo;
}tDiskOperation, *pDiskOperation;
typedef struct tagScenetimePoint
{
	int					iBufSize;			
	int					iType;	
	int           		iIndex;
	int 				iEnable;			
	NVS_FILE_TIME		stStartTime;
	NVS_FILE_TIME		stEndTime;
}ScenetimePoint,*pScenetimePoint;

//reboot device by type
#define REBOOT_BY_DEVICE	0
#define REBOOT_BY_GUI		1
#define REBOOT_BY_APP		2

#define MAX_H323_GROUP_ID	1
/*****************************H.323 Local Param*****************************/
//ID code mode��0-UTF-8�� 1-Unicode
#define H323_IDCODE_UTF8		0
#define H323_IDCODE_UNICODE		1
//Responds by call��0-automatic,1-manual,2-not disturb
#define  H323_RESPONDS_AUTOMATIC		0
#define  H323_RESPONDS_MANUAL			1
#define  H323_RESPONDS_NOTDISTURB		2

typedef struct _tagH323LocalParam
{
	int iSize;
	int iH323GroupId;				//H323 number,default 0
	int iListenPort;				//Listen port
	char cLocalNo[LEN_64];			//Terminal number
	char cPassWord[LEN_64];			//Encrytion password
	int iIdCodeType;				//ID code mode��0-UTF-8�� 1-Unicode
	int iAnwTypeForCall;			//Responds by call��0-automatic,1-manual,2-not disturb
	int iMainSrcChnNo;				//Encode main stream channel number
	int	iSubSrcChnNo;				//Encode sub stream channel number
	int	iMainDecChnNo;				//Decode main stream channel number
	int	iSubDecChnNo;				//Decode sub stream channel number
}H323LocalParam, *pH323LocalParam;

/*****************************H.323 GK Param*****************************/
//GK Mode:0-disable,1-designation,2-auto search
#define  H323_GK_MODE_DISABLE		0
#define  H323_GK_MODE_DESIGNATION	1
#define  H323_GK_MODE_AUTOSEARCH	2
//GK Encrytion mode,0-shut down,1-automatic,2-zhongxing,3-xinshitong,4-sike
#define  H323_GK_ENCRYTION_SHUTDOWN			0	
#define  H323_GK_ENCRYTION_AUTOMATIC		1
#define  H323_GK_ENCRYTION_ZHONGXING		2
#define  H323_GK_ENCRYTION_XINGSHITONG		3
#define  H323_GK_ENCRYTION_SIKE				4

typedef struct _tagH323GKParam
{
	int iSize;
	int iH323GroupId;				//H323 number,default 0
	int iGKMode;					//GK Mode:0-disable,1-designation,2-auto search
	char cGKAddress[LEN_64];		//GK address,64 bytes
	int iGKPort;					//GK port
	char cRegisterName[LEN_64];		//Register Name,64 bytes
	int iGKEncrytionType;			//GK Encrytion mode,0-shut down,1-automatic,2-zhongxing,3-xinshitong,4-sike
}H323GKParam, *pH323GKParam;

#define VCA_ARITHMETIC_TYPE_MIN							0						//Arithmetic Type Min
#define VCA_ARITHMETIC_BEHAVIOR_ANALYSIS		(VCA_ARITHMETIC_TYPE_MIN+0)		//Behavior Analysis
#define VCA_ARITHMETIC_LICENSE_RECOGNITION		(VCA_ARITHMETIC_TYPE_MIN+1)		//License Plate Recognition
#define VCA_ARITHMETIC_FACE_DETECT				(VCA_ARITHMETIC_TYPE_MIN+9)		//face detect
#define	VCA_ARITHMETIC_VIDEO_DETECT				(VCA_ARITHMETIC_TYPE_MIN+10)	//Video Detect
#define	VCA_ARITHMETIC_TRACK					(VCA_ARITHMETIC_TYPE_MIN+11)	//track
#define	VCA_ARITHMETIC_FLUX_STATISTIC			(VCA_ARITHMETIC_TYPE_MIN+12)	//Flux Statistics
#define	VCA_ARITHMETIC_CROWDS					(VCA_ARITHMETIC_TYPE_MIN+13)	//Crowds
#define	VCA_ARITHMETIC_LEAVE_DETECT				(VCA_ARITHMETIC_TYPE_MIN+14)	//Leave Detect
#define	VCA_ARITHMETIC_AUDIO_DIAGNOSE			(VCA_ARITHMETIC_TYPE_MIN+16)	//Audio Diagnose
#define VCA_ARITHMETIC_ILLEAGEPARK				(VCA_ARITHMETIC_TYPE_MIN+20)	//Illeage Park
#define	VCA_ARITHMETIC_TYPE_MAX					(VCA_ARITHMETIC_TYPE_MIN+21)	//Arithmetic Type Max

#define VCA_BEHAVIOR_TYPE_TRIPWIRE				(1<<0)  						//Tripwire 0
#define VCA_BEHAVIOR_TYPE_DBTRIPWIRE			(1<<1)  						//Double Tripwire 1
#define VCA_BEHAVIOR_TYPE_PERIMETER				(1<<2)  						//Perimeter 2
#define VCA_BEHAVIOR_TYPE_LOITER				(1<<3)  						//Loiter 3
#define VCA_BEHAVIOR_TYPE_PARKING				(1<<4)  						//Parking 4
#define VCA_BEHAVIOR_TYPE_RUN					(1<<5)  						//Running 5
#define VCA_BEHAVIOR_TYPE_OBJSTOLEN				(1<<7)  						//Stolen Objects 7
#define VCA_BEHAVIOR_TYPE_ABANDUM				(1<<8)  						//Abandum Objects 8


//�豸����Э��ĺ꣬IE���������豸
//��ͨ���޹�
#define PROTOCOL_VCALIST					(1<<0)
#define PROTOCOL_DHDEVICESET				(1<<1)
#define PROTOCOL_DHALARMLINK				(1<<2)
#define PROTOCOL_DHOSDREALTIME				(1<<3)
#define PROTOCOL_DHDEVENABLE				(1<<4)
#define PROTOCOL_IPCPLUGANDPLAY				(1<<5)
//��ͨ���й�
#define PROTOCAL_VCA						(1<<0)
#define PROTOCAL_CAMERAPARA					(1<<1)
#define PROTOCAL_DOMEPARA					(1<<2)
#define PROTOCOL_ALARMLINK					(1<<3)
#define PROTOCOL_ALARMSCHEDULE				(1<<4)
#define PROTOCOL_VIDEOROI					(1<<5)
#define PROTOCOL_AUDIOCODERLIST				(1<<6)
#define PROTOCOL_DOMEPTZ					(1<<7)
#define PROTOCOL_MULTIWORDOVERLAY			(1<<8)
#define PROTOCOL_TIMESEGMENT				(1<<9)
#define PROTOCOL_ALARMLINK_EX				(1<<10)

#define PROXY_SEND_ITS						(1<<0)
#define PROXY_SEND_CAMERAPARA				(1<<1)
#define PROXY_SEND_ALARMLINK				(1<<2)
#define PROXY_SEND_ALARMLINK_EX				(1<<3)
#define PROXY_SEND_DOMEPARAM				(1<<4)
#define PROXY_SEND_LIFEMONITOR				(1<<5)
#define PROXY_SEND_DHINFO					(1<<6)
#define PROXY_SEND_PREVIEWREC				(1<<7)


typedef struct _tagVcaArithmeticList
{
	int iSize;						//size of struct
	int iChannelNo;					//channel number
	int iArithmeticType;			//arithmetic type
	int iEnableCount;				//enable count
	int* piEnableValue;				//enable value
} VcaArithmeticList, *pVcaArithmeticList;

typedef struct _tagDownloadByTimeSpan
{
	int				iSize;							//size of struct
	NVS_FILE_TIME	tStartTime;						//Start Time
	NVS_FILE_TIME	tStopTime;						//Stop Time
	char			cLocalFilename[LEN_256-1];		//Local File Name
	int				iFileType;						//sdv:0; avi:1
} DownloadByTimeSpan, *pDownloadByTimeSpan;

typedef struct _tagPotocolEnable
{
	int iSize;
	int iEnable;
}PotocolEnable, *pPotocolEnable;

#define LIFE_MONITOR_TYPE_MIN					1
#define LIFE_MONITOR_HEART_RATE			LIFE_MONITOR_TYPE_MIN + 0
#define LIFE_MONITOR_OXYGEN				LIFE_MONITOR_TYPE_MIN + 1
#define LIFE_MONITOR_BLOOD_PRESSURE		LIFE_MONITOR_TYPE_MIN + 2
#define LIFE_MONITOR_TYPE_MAX			LIFE_MONITOR_TYPE_MIN + 3	
// Vital signs in real time heart rate and blood oxygen
typedef struct _tagLifeMonitorHBReal
{
	int iSize;
	int iMonType;			//Monitor type	1:heart rate, 2:oxygen, 3:blood pressure
	int	iMonStatus;			//Monitor status	1:normal, 2:Super caps, 3:Ultra-limit, 4:Shedding, 5:Device Offline, 6:testing blood pressure
	int	iMonRealVal1;		//Real-time value1	
	int	iMonRealVal2;		//Real-time value2	
	int	iMonRealVal3;		//Real-time value3	
	int	iMonRealVal4;		//Real-time value4	
} LifeMonitorHBReal, *pLifeMonitorHBReal;

//Vital signs waveform data
#define MAX_WAVEFORM_GRAM_ID	2
#define MAX_GRAM_SEQ_NUMBER		3
typedef struct _tagLifeMonitorGramReal
{
	int iSize;
	int iOscilloGramID;		//Gram ID	1:Electrocardiographic 2:Oxygen wave
	int	iGramSeq;			//The current wave number starting with 1, the heart waves can be divided into three packs,
							//followed by 1,2,3 transfer, oxygen wave only one package, this value is 1
	int	iGramRate;			//The frequency of collection points
	int	iReserved;			//Reserved value,default 0
	int	iPointCount;		//The total count of points collected
	int* piPoints;
} LifeMonitorGramReal, *pLifeMonitorGramReal;

#define MAX_LIFE_MONITOR_CMD_ID	2
typedef struct _tagLifeMonitorConfig
{
	int iSize;
	int iMonType;			//Monitor type,	1:heart rate, 2:oxygen, 3:blood pressure, 0x7FFFFFFF:All(default value)
	int	iCmdID;				//Command ID,	1:Set whether to report IE, 2:Set whether OSD
	int	iCmdContent;		//Command content,	iCmdID=1:1--yes 0--no, iCmdID=2:1--yes,0--no
} LifeMonitorConfig, *pLifeMonitorConfig;

typedef struct _tagOSDExtraInfo
{
	int iSize;
	int iOsdType;		//1:channel name 2:time and date 3:logo color 4:Extra OSD 5:ITS use 6:ITS combine picture
	int	iBgColor;		//back color,"32 is taken low 24 indicates color rgb,representative digitally bgr:Other bits reserved 0x00BBGGRR high eight,do not use."					
	int	iDigitNum;		//Superimposed digits,it shows the median insufficient bit zeros				
} OSDExtraInfo, *pOSDExtraInfo;

//����Υͣ��Υ��ͣ��ץ����ʽ����
#define MAX_ITS_IPDCAPMODEL_NUM		8		//���Υͣץ������
#define MAX_ITS_CAPTYPE				2		//���ץ������
typedef  struct _tagIpdCapModel
{
	int	 iSize;
	int  iChannelNo;
	int  iPreset;
	int  iCapType;
	int  iCapNum;
	int  iCapStyle[MAX_ITS_IPDCAPMODEL_NUM];   
}tITS_IpdCapModel, *ptITS_IpdCapModel;

//����˹�ץ�ĳ���λ��
#define  MAX_ITS_IPDCARPOSITION_NUM   15		//���������
#define MAX_REGION_NUM			      16		//������
typedef  struct _tagIpdCarPosition
{
	int  iSize;
	int  iChannelNo;
	int  iSceneId;
	int  iRegionID;
	int  iEnabled;
	int  iCount;
	vca_TPoint	m_arrPoint[MAX_ITS_IPDCARPOSITION_NUM];  
}tITS_IpdCarPosition, *ptITS_IpdCarPosition;

#define SERVERINFO_APP_TYPE_MIN			0
#define SERVERINFO_APP_TYPE_RETAIN		(SERVERINFO_APP_TYPE_MIN + 0)
#define	SERVERINFO_APP_TYPE_MULTICAST	(SERVERINFO_APP_TYPE_MIN + 1)  
#define	SERVERINFO_APP_TYPE_MAX			(SERVERINFO_APP_TYPE_MIN + 2)  

typedef struct _tagServerInfo
{
	int		iSize;
	int		iAppID;					//apply type
	char	cAddress[LEN_16];		//server addr				
	int		iProt;					//server port			
}tServerInfo, *ptServerInfo;


#define MAX_MAIN_FUNC_TYPE				7
#define MAX_SUB_FUNC_TYPE				3
#define	MAIN_FUNC_TYPE_TRANSCODING		2		//video transcoding
#define	MAIN_FUNC_TYPE_DDNSSERVICE		3		//DDNS service
#define	MAIN_FUNC_TYPE_FORMREPORT		4		//form report
typedef struct _tagFuncAbilityLevel
{
	int		iSize;
	int		iMainFuncType;			//main function type
	int		iSubFuncType;			//sub function type
	char	cParam[LEN_256];		//Capability Description
} FuncAbilityLevel, *pFuncAbilityLevel;

#define REPORT_FORM_TYPE_DEFAULT		0
#define REPORT_FORM_TYPE_HOUR			1
#define REPORT_FORM_TYPE_DAY			2
#define REPORT_FORM_TYPE_MONTH			3
#define REPORT_FORM_TYPE_YEAR			4
typedef struct _tagQueryReportForm
{
	int					iSize;			
	int					iFormType;		//form type
	NVS_FILE_TIME		tBeginTime;		//begin time
	NVS_FILE_TIME		tEndTime;		//end time
} QueryReportForm, *pQueryReportForm;

#define MAX_ONCE_FORM_REPORT_NUM			80
typedef struct _tagRecordReport
{
	int				iChannelNo;				//channel number	
	int				iPushPersonNum;			//enter the number
	int				iPopPersonNum;			//leave the number
	NVS_FILE_TIME	tOccurTime;				//time of occurrence
} RecordReport, *pRecordReport;

typedef struct _tagReportFormResult
{
	int		iSize;			
	int		iFormType;				//form type
	int		iTotalNum;				//total report number
	int		iCurrentNum;			//current report number
	RecordReport tReport[MAX_ONCE_FORM_REPORT_NUM];
} ReportFormResult, *pReportFormResult;

//===========================HD Para Type==============================================
#define HD_PARA_APERTURE						0						//0-Image Adjustment
#define HD_PARA_WIDE_DYNAMIC					1						//1-Wide dynamic policy
#define HD_PARA_BACK_LIGHT						2						//2-BackLight Compensation
#define HD_PARA_EXPOSAL_TIME					3						//3-Exposal Time
#define HD_PARA_SHUTTER							4						//4-Shutter adjustment
#define HD_PARA_GAIN							5						//5-Gain adjustment
#define HD_PARA_GAMMA							6						//6-Gamma type
#define HD_PARA_SHARPNESS						7						//7-Sharpness adjustment
#define HD_PARA_DNR								8						//8-Noise Reduction adjustment
#define HD_PARA_EXPOSAL_AREA					9						//9-Exposure area
#define HD_PARA_BACK_LIGHT_AREA					10						//10-BLC area
#define HD_PARA_AF_AREA							11						//11-AF area set
#define	HD_PARA_BRIGHTNESS_HD					12						//12-Brightness
#define HD_PARA_WHITE_BALANCE					13						//13-White Balance
#define HD_PARA_JPEG_QUALITY					14						//14-JPEG Quality
#define HD_PARA_LUT_ENABLE						15						//15-LUT enable
#define HD_PARA_AUTO_BRIGHTNESS					16						//16-Enabling automatic brightness adjustment
#define HD_PARA_STRONG_LIGHT					17						//17-Light Suppression
#define HD_PARA_EXPO_MODE						18						//18-Exposure mode
#define HD_PARA_INOUT_DOOR_MODE					19						//19-Indoor and outdoor mode
#define HD_PARA_FOCUS_MODE						20						//20-Focus Mode
#define HD_PARA_MIN_FOCUS_DISTANCE				21						//21-Minimum focusing distance
#define HD_PARA_DAY_NIGHT						22						//22-Day & Night Mode
#define HD_PARA_RESTORE_DEFAULT					23						//23-reset
#define HD_PARA_THROUGH_MIST					24						//24-Through fog
#define HD_PARA_AE_SPEED						25						//25-Automatic exposure adjustment
#define HD_PARA_SMARTIR							26						//26-smartIR
#define HD_PARA_OFFSET_INCREASE					27						//27-Exposure Compensation
#define HD_PARA_SENSETIVE						28						//28-High Sensitivity
#define	HD_PARA_AUTO_SLOW_EXPO					29						//29-Auto slow exposure
#define HD_PARA_AUTO_CONTRAST					30						//30-Auto Contrast
#define HD_PARA_VIDEO_FROST						31						//31-Image freeze
#define HD_PARA_INFRARED						32						//32-Infrared Calibration
#define HD_PARA_COLOR_INCREASE					33						//33-Color gain
#define HD_PARA_COLOR_PHASE						34						//34-Color phase
#define HD_PARA_LIGHTLESS						35						//35-Low light color suppression
#define HD_PARA_FOCUS_TYPE						36						//36-Focus Type
#define HD_PARA_NIGHT_STRENTH					37						//37-Night strengthen
#define HD_PARA_VIGNETTE						38						//38-Vignette compensation
#define HD_PARA_BRIGHTNESS_EX					39						//39-Brightness
#define HD_PARA_CONTRAST_HD						40						//40-Contrast
#define HD_PARA_SATURATION_HD					41						//41-saturation
#define HD_PARA_HUE_HD							42						//42-Chroma

typedef struct _tagStreamSmooth
{
	int		iSize;			
	int		iValue;			//smooth value,range[0,100]
} StreamSmooth, *pStreamSmooth;


#endif
