#ifndef __PROTOCOL_FACTORY_H__
#define __PROTOCOL_FACTORY_H__

#include "upload_base_protocol.h"

// 平台类型
#define PROTOCOL_HTTP_NEW	0
#define PROTOCOL_FTP_NEW	1
#define PROTOCOL_MQTT   	2

class UploadProtocolFactory
{
public:
    static ProtocolHandle Product(int protocol_type);
};
#endif //__PROTOCOL_FACTORY_H__
