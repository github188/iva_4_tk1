#ifndef __UPLOAD_MQTT_H__
#define __UPLOAD_MQTT_H__

#include "upload_base_protocol.h"
#include "oal_typedef.h"
#include "mosquitto.h"

class UploadByMQTT : public UploadBaseProtocol
{
private:
	UploadByMQTT(void);
public:
	virtual ~UploadByMQTT(void);
	static ProtocolHandle m_pInstance;
	static ProtocolHandle GetInstance();

public:
	virtual int InitSever();
	virtual int UploadData(const AnalyDbRecord *pData);
	virtual int UploadAlarm(const AlarmDbRecord *pData);

	virtual bool CanUploadData();
	virtual bool CanUploadAlarm();
	virtual bool CanRemoveFile();

public:
	bool m_bExitThread;

	bool IsConnected();
	int CurlPost(std::string &szUrl, std::string &szContent, std::string &szResp);
	int DoUploadResp( std::string szResp );
	void Connect();
	void CloseConnect();
	void UnexpectedDisconnect();

	int PublishChannels();
	int PublishVersion();
	int PublishStatus();

	int DoSubscibe(const struct mosquitto_message *msg);
	int GetUpgradePack(const char * pszUrl);
	void ReportRightNow();
	void CheckRightNow();

	int PackeUploadDataJson(const AnalyDbRecord *pData, std::string &szContent);

	int UploadFile(char * filename);

private:
	mosquitto *m_mqtt;
	char m_szSubTopic[128];
	char m_szPubTopic[128];
	time_t m_tLastHeart;
	bool m_bRightNow;
	pthread_t m_heart_thread;
	pthread_mutex_t m_tMutex;
	bool m_bNeedUploadAlarm;
	DataUploadOption m_tUpOption;
	std::string m_uploadaddr;

};

#endif // __UPLOAD_MQTT_H__

