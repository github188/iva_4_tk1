#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oal_log.h"
#include "Protocol_Factory.h"
#include "upload_ftp_com.h"
#include "upload_http_com.h"
#include "upload_mqtt_com.h"

ProtocolHandle UploadProtocolFactory::Product(int protocol_type)
{
	ProtocolHandle handle = NULL;

	switch(protocol_type)
	{
	case PROTOCOL_HTTP_NEW:
		handle = UploadByHttpCommon::GetInstance();
		break;
	case PROTOCOL_FTP_NEW:
		handle = UploadByFtpCommon::GetInstance();
		break;
	case PROTOCOL_MQTT:
		handle = UploadByMQTT::GetInstance();
		break;
	default:
		LOG_WARNS_FMT("Unknown Upload Protocol...%d",protocol_type);
		break;
	};

	return handle;
}

