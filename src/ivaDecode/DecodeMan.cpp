#include "DecodeMan.h"
#include <assert.h>
#include <stdio.h>
#include "oal_log.h"
#include "oal_time.h"

static void * DecodeCountThread(void * p)
{
	time_t tBegin = time(NULL);
	while(DecodeMan::GetInstance()->m_bExitThread == false)
	{
		sleep(1);

		if (time(NULL) - tBegin >= 60)
		{
			for (int i = 0; i < MAX_CHANNEL_NUM; i++)
			{
				DecodeMan::GetInstance()->PrintfCountInfo(i);
			}
			tBegin = time(NULL);
		}
	}

	pthread_exit(NULL);
	return NULL;
}

#ifdef DECODE_OPENMAX

OMX_VERSIONTYPE vOMX = {1,1,0,0};

int WaitForState(OMX_HANDLETYPE hComponent,
				 OMX_STATETYPE eTestState,
				 OMX_STATETYPE eTestState2)
{
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_STATETYPE eState;

	eError = OMX_GetState(hComponent, &eState);

	while(eState != eTestState && eState != eTestState2)
	{
		sleep(1);

		eError = OMX_GetState(hComponent, &eState);
	}

	return eError;
}

// set state function
int SetOmxState(OMX_HANDLETYPE handle, OMX_STATETYPE state, bool wait)
{
	int ret = -1;

	if(OMX_ErrorNone != OMX_SendCommand(handle,  OMX_CommandStateSet, state, 0))
	{
		printf("SendCommand with state=%d failed\n", state);
	}

	if(wait)
	{
		WaitForState(handle,  state, OMX_StateInvalid);
	}

	ret = 0;

	return ret;
}

OMX_ERRORTYPE EventHandler(OMX_IN OMX_HANDLETYPE hComponent,
						   OMX_IN OMX_PTR pAppData,
						   OMX_IN OMX_EVENTTYPE eEvent,
						   OMX_IN OMX_U32 nData1,
						   OMX_IN OMX_U32 nData2,
						   OMX_IN OMX_PTR pEventData)
{
	OMX_ERRORTYPE  omxRet = OMX_ErrorNone;
	OMXDecoder *ptDecoder = (OMXDecoder*)pAppData;

	switch(eEvent)
	{
	case OMX_EventBufferFlag:
		{
			LOG_DEBUG_FMT("[component has detected an EOS]");
			break;
		}
	case OMX_EventPortSettingsChanged:
		{
			LOG_DEBUG_FMT("[component is reported a port settings change]");

			// allocate output buffer
			if(OMX_IndexParamPortDefinition == (OMX_INDEXTYPE)nData2)
			{
				INIT_PARAM(ptDecoder->tOutputPortDef);
				ptDecoder->tOutputPortDef.nPortIndex = nData1;

				omxRet = OMX_GetParameter(ptDecoder->pHandle, 
					OMX_IndexParamPortDefinition,
					&(ptDecoder->tOutputPortDef));
				if(omxRet != OMX_ErrorNone)
				{
					LOG_ERROR_FMT("OMX_GetParameter OMX_IndexParamPortDefinition FAILED\n");
				}

				DecodeVideoInfo tVideoInfo = {0};
				DecodeMan::GetInstance()->GetChnVideoInfo(ptDecoder->iChnID,tVideoInfo);
				if( (tVideoInfo.iWidth  != ptDecoder->tOutputPortDef.format.video.nFrameWidth)||
					(tVideoInfo.iHeight != ptDecoder->tOutputPortDef.format.video.nFrameHeight))
				{
					// Dynamic resolution changes occured.
					LOG_WARNS_FMT("Dynamic resolution changes triggered");
				}
			}
			break;
		}
	case OMX_EventCmdComplete:
		{
			LOG_DEBUG_FMT("component has sucessfully completed a command");
			break;
		}
	case OMX_EventError:
		{
			LOG_DEBUG_FMT("component has detected an error condition");
			break;
		}
	case OMX_EventMark:
		{
			LOG_DEBUG_FMT("component has detected a buffer mark");
			break;
		}
	case OMX_EventResourcesAcquired:
		{
			LOG_DEBUG_FMT("component has been granted resources and is"
				"automatically starting the state change from"
				"OMX_StateWaitForResources to OMX_StateIdle");
			break;
		}
	case OMX_EventComponentResumed:
		{
			LOG_DEBUG_FMT("Component resumed due to reacquisition of resources");
			break;
		}
 	case OMX_EventDynamicResourcesAvailable:
		{
			LOG_DEBUG_FMT("Component has acquired previously unavailable dynamic resources");
			break;
		}
	case OMX_EventPortFormatDetected:
		{
			LOG_DEBUG_FMT("Component has detected a supported format.");
			break;
		}
	default:
		{
			LOG_DEBUG_FMT("[%d]",eEvent);
			break;
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE H264_Get_Frame(OMX_OUT OMX_HANDLETYPE hComponent,
							 OMX_OUT OMX_PTR pAppData,
							 OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMXDecoder *ptDecoder = (OMXDecoder*)pAppData;
	if (ptDecoder == NULL)
	{
		return OMX_ErrorNone;
	}
	int iChnID = ptDecoder->iChnID;

	while(1)
	{
		H264_Queue_Node tH264Node;
		int iRet = DecodeMan::GetInstance()->GetNewFrameData(iChnID, &tH264Node);
		if(iRet == 0)
		{
			if(pBuffer->pBuffer)
			{
				memcpy(pBuffer->pBuffer, tH264Node.frame_data, tH264Node.h264_info.data_size);
				pBuffer->nFilledLen = tH264Node.h264_info.data_size;
				//printf("h264_info.data_size = %d\n",tH264Node.h264_info.data_size);
				DecodeMan::GetInstance()->CountInputInfo(&tH264Node.h264_info);
				pBuffer->nOffset = 0;
			}
			break;
		}
	}

	OMX_EmptyThisBuffer(hComponent, pBuffer);

	return OMX_ErrorNone;

}

OMX_ERRORTYPE Yuv_Put_Frame(OMX_OUT OMX_HANDLETYPE hComponent,
							OMX_OUT OMX_PTR pAppData,
							OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer)
{
	static unsigned int	s_frame_id[MAX_CHANNEL_NUM] = {0};
	OMXDecoder *ptDecoder = (OMXDecoder*)pAppData;
	if (ptDecoder == NULL)
	{
		return OMX_ErrorNone;
	}

	int iChnID = ptDecoder->iChnID;

	Yuv_Queue_Node *pYuvNode = DecodeMan::GetInstance()->GetEmptyYuvNode(iChnID);
	if(pYuvNode != NULL)
	{
		s_frame_id[iChnID]++;

		DecodeMan::GetInstance()->CountOutputInfo(iChnID);

		DecodeVideoInfo tVideoInfo = {0};
		DecodeMan::GetInstance()->GetChnVideoInfo(iChnID,tVideoInfo);
		struct timeval _tv;
		gettimeofday(&_tv, NULL);
		pYuvNode->yuv_info.timestamp  = _tv.tv_sec*1000 + _tv.tv_usec/1000;
		pYuvNode->yuv_info.frame_id   = s_frame_id[iChnID];
		pYuvNode->yuv_info.channel    = iChnID;
		pYuvNode->yuv_info.frame_rate = tVideoInfo.iFrameRate;
		pYuvNode->yuv_info.width	  = tVideoInfo.iWidth;
		pYuvNode->yuv_info.height     = tVideoInfo.iHeight;
		pYuvNode->yuv_info.data_size  = (tVideoInfo.iWidth * tVideoInfo.iHeight) + (tVideoInfo.iWidth *tVideoInfo.iHeight)/2;
		//printf("PutFullYuvNode frame_id:%d timestamp:%llu\n", pYuvNode->yuv_info.frame_id, pYuvNode->yuv_info.timestamp);

		memcpy(pYuvNode->yuv_data, pBuffer->pBuffer, pYuvNode->yuv_info.data_size);

#ifdef SAVE_YUV
		static unsigned int	count = 0;
		if(count < 50)
		{
			fwrite(pYuvNode->yuv_data, 1, pYuvNode->yuv_info.data_size, ptDecoder->fp_yuv);
			count++;
		}
		else
		{
			if(ptDecoder->fp_yuv){
				fclose(ptDecoder->fp_yuv);
				ptDecoder->fp_yuv = NULL;
				printf("\n===SAVE_YUV DONE===\n");
			}
		}	
#endif
		DecodeMan::GetInstance()->PutFullYuvNode(iChnID);
	}
	else
	{
		//LOG_ERROR_FMT("No Empty Yuv Node");
		DecodeMan::GetInstance()->CountOutputInfoMiss(iChnID);
	}

	OMX_FillThisBuffer(hComponent, pBuffer);

	return OMX_ErrorNone;
}
#else
int g_CHNID[MAX_CHANNEL_NUM] = {0};

void *FfmpegDecodeThread(void *arg)
{
	unsigned char * addr = NULL;
	unsigned int num = 0;
	int frameFinished;

	int iChnID = *((int*)arg);
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return NULL;
	}

	H264_Queue_Node * ptDataNode = (H264_Queue_Node*)malloc(sizeof(H264_Queue_Node));
	assert(ptDataNode);

	while(DecodeMan::GetInstance()->m_bExitThread == false)
	{
		memset(ptDataNode, 0, sizeof(H264_Queue_Node));
		int iRet = DecodeMan::GetInstance()->GetNewFrameData(iChnID, ptDataNode);
		if(iRet != 0)
		{
			continue;
		}

		DecodeMan::GetInstance()->CountInputInfo(&ptDataNode->h264_info);

		DecodeMan::GetInstance()->Decode_H264(iChnID, ptDataNode);
	}

	free(ptDataNode);
	ptDataNode = NULL;

	return NULL;
}
#endif


DecodeMan* DecodeMan::m_pInstance = NULL;

DecodeMan* DecodeMan::GetInstance()
{
	return m_pInstance;
}

int DecodeMan::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new DecodeMan();
		assert(m_pInstance);
		m_pInstance->Run();

	}
	return (m_pInstance == NULL ? -1 : 0);
}

void DecodeMan::UnInitialize()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

DecodeMan::DecodeMan()
{
#ifdef DECODE_OPENMAX
	OMX_ERRORTYPE omxRet = OMX_Init();
	printf("\n");
	if(omxRet != OMX_ErrorNone)
	{
		LOG_ERROR_FMT("OMX_Init failed");
	}
	else
	{
		LOG_DEBUG_FMT("OMX_Init OK");
	}
#endif

	pthread_mutex_init(&m_tMutex, NULL);

	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		m_tVideoInfo[i].iFrameRate = 0;
		m_tVideoInfo[i].iWidth  = 0;
		m_tVideoInfo[i].iHeight = 0;

#ifdef DECODE_OPENMAX
		memset(m_tDecoder+i, 0, sizeof(OMXDecoder));
#else
		memset(m_tDecoder+i, 0, sizeof(FfmpegDecoder));
#endif
		m_tDecoder[i].iChnID = i;

#ifdef SAVE_YUV
		char yuvfile[128] = {0};
		sprintf(yuvfile, "output%d.yuv",i);
		printf("\n===SAVE_YUV Begin %s===\n", yuvfile);
		m_tDecoder[i].fp_yuv = fopen(yuvfile,"wb+"); 
#endif
	}
	m_bExitThread = false;
}

DecodeMan::~DecodeMan()
{
	m_bExitThread = true;
	pthread_join(m_count_thread,NULL);
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
#ifndef DECODE_OPENMAX
		pthread_join(m_decode_thread[i],NULL);
#endif

		Decode_Close(i);
		H264_QueueClose(&m_tH264SQ[i]);
		Yuv_QueueRelease(&m_tYuvSQ[i]);
	}

	pthread_mutex_destroy(&m_tMutex);

#ifdef DECODE_OPENMAX
	OMX_Deinit();
#endif
}

int DecodeMan::Run()
{
	m_bExitThread = false;
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		// 打开共享队列
		m_tH264SQ[i].iChnID = i;
		m_tH264SQ[i].service_type = H264_QUEUE_DECODE_SERVICE;
		int iRet = H264_QueueOpen(&m_tH264SQ[i]);
		if (iRet != 0)
		{
			LOG_ERROR_FMT("Channel%d H264_QueueOpen failed", i);
		}
		else
		{
			LOG_DEBUG_FMT("Channel%d H264_QueueOpen OK", i);
		}

		m_tYuvSQ[i].iChnID = i;
		iRet = Yuv_QueueCreate(&m_tYuvSQ[i]);
		if (iRet != 0)
		{
			LOG_ERROR_FMT("Channel%d Yuv_QueueCreate failed", i);
		}
		else
		{
			LOG_DEBUG_FMT("Channel%d Yuv_QueueCreate OK", i);
		}

#ifndef DECODE_OPENMAX
		g_CHNID[i] = i;
		pthread_create(&m_decode_thread[i], NULL, FfmpegDecodeThread, &g_CHNID[i]);
#endif
	}
	
	pthread_create(&m_count_thread, NULL, DecodeCountThread, this);

	return 0;
}

int DecodeMan::SetChnVideoInfo( int iChnID, DecodeVideoInfo * ptInfo )
{
	if(ptInfo == NULL || iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}
	bool bChanged = false;
	pthread_mutex_lock(&m_tMutex);
	if (ptInfo->iWidth  != m_tVideoInfo[iChnID].iWidth||
		ptInfo->iHeight != m_tVideoInfo[iChnID].iHeight)
	{
		bChanged = true;
	}
	memcpy(m_tVideoInfo+iChnID, ptInfo, sizeof(DecodeVideoInfo));
	pthread_mutex_unlock(&m_tMutex);

	if(bChanged)
	{
		Decode_Close(iChnID);
		sleep(1);
		Decode_Init(iChnID);
	}

	return 0;
}

int DecodeMan::GetChnVideoInfo( int iChnID, DecodeVideoInfo & tInfo )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&tInfo, m_tVideoInfo+iChnID, sizeof(DecodeVideoInfo));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int DecodeMan::GetNewFrameData( int iChnID, H264_Queue_Node * ptNode )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM || ptNode == NULL)
	{
		return -1;
	}

	return H264_QueueGetFull(m_tH264SQ+iChnID, &ptNode->h264_info, ptNode->frame_data);
}

Yuv_Queue_Node * DecodeMan::GetEmptyYuvNode( int iChnID )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return NULL;
	}

	return Yuv_QueueGetEmpty(m_tYuvSQ+iChnID);
}

int DecodeMan::PutFullYuvNode( int iChnID )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	return Yuv_QueuePutFull(m_tYuvSQ+iChnID);
}

#ifdef DECODE_OPENMAX
int DecodeMan::Decode_Init(int iChnID)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if (m_tVideoInfo[iChnID].iWidth < 1 || m_tVideoInfo[iChnID].iHeight < 1)
	{
		LOG_ERROR_FMT("resolution error %d*%d",m_tVideoInfo[iChnID].iWidth,m_tVideoInfo[iChnID].iHeight);
		return -1;
	}

	OMXDecoder *ptDecoder = &m_tDecoder[iChnID];

	vOMX.s.nVersionMajor = 1;
	vOMX.s.nVersionMinor = 1;
	vOMX.s.nRevision = 0;
	vOMX.s.nStep = 0;

	char ComponentName[]= "OMX.Nvidia.h264.decode";
	
	OMX_ERRORTYPE omxRet = OMX_ErrorNone;

// 	omxRet = OMX_Init();
// 	printf("\n");
// 	if(omxRet != OMX_ErrorNone)
// 	{
// 		LOG_ERROR_FMT("OMX_Init failed");
// 	}
// 	else
// 	{
// 		LOG_DEBUG_FMT("OMX_Init OK");
// 	}

	// Set callback functions
	static OMX_CALLBACKTYPE omxCallbacks;
	omxCallbacks.EventHandler    = EventHandler;
	omxCallbacks.EmptyBufferDone = H264_Get_Frame;
	omxCallbacks.FillBufferDone  = Yuv_Put_Frame;


	// get h264 handle
	omxRet = OMX_GetHandle(&ptDecoder->pHandle,ComponentName,(OMX_PTR)ptDecoder,&omxCallbacks);
	if(omxRet != OMX_ErrorNone)
	{
		LOG_ERROR_FMT("Channel%d OMX_GetHandle failed", iChnID);
		return -1;
	}
	LOG_DEBUG_FMT("Channel%d OMX_GetHandle OK", iChnID);

	OMX_STATETYPE eState;
	omxRet = OMX_GetState(ptDecoder->pHandle, &eState);
	if(omxRet != OMX_ErrorNone)
	{
		LOG_ERROR_FMT("Channel%d OMX_GetState failed", iChnID);
	}
	LOG_ERROR_FMT("Channel%d OMX_GetState = %d (OMX_StateLoaded = %d)",iChnID, eState,OMX_StateLoaded);

	INIT_PARAM(ptDecoder->tInputPortDef);
	ptDecoder->tInputPortDef.nPortIndex = 0;
	omxRet = OMX_GetParameter(ptDecoder->pHandle, OMX_IndexParamPortDefinition, &ptDecoder->tInputPortDef);
	if(omxRet != OMX_ErrorNone)
	{
		LOG_ERROR_FMT("Channel%d OMX_GetParameter OMX_IndexParamPortDefinition failed", iChnID);
		//return -1;
	}

// 	printf("0 [Input Port]\n");
// 	printf("PortIndex:%lu\n",ptDecoder->tInputPortDef.nPortIndex);
// 	printf("Dir:%d\n",ptDecoder->tInputPortDef.eDir);
// 	printf("BufferCountActual:%lu\n",ptDecoder->tInputPortDef.nBufferCountActual);
// 	printf("BufferCountMin:%lu\n",ptDecoder->tInputPortDef.nBufferCountMin);
// 	printf("BufferSize:%lu\n",ptDecoder->tInputPortDef.nBufferSize);
// 	printf("Enabled:%d\n",ptDecoder->tInputPortDef.bEnabled);
// 	printf("Populated:%d\n",ptDecoder->tInputPortDef.bPopulated);
// 	printf("Domain:%d (Audio=0,Video=1,Image=2)\n",ptDecoder->tInputPortDef.eDomain);
// 	printf("video.MIME:%s\n",ptDecoder->tInputPortDef.format.video.cMIMEType);
// 	printf("video.nFrameWidth:%lu\n",ptDecoder->tInputPortDef.format.video.nFrameWidth);
// 	printf("video.nFrameHeight:%lu\n",ptDecoder->tInputPortDef.format.video.nFrameHeight);
// 	printf("video.nStride:%ld\n",ptDecoder->tInputPortDef.format.video.nStride);
// 	printf("video.nSliceHeight:%lu\n",ptDecoder->tInputPortDef.format.video.nSliceHeight);
// 	printf("video.nBitrate:%lu\n",ptDecoder->tInputPortDef.format.video.nBitrate);
// 	printf("video.xFramerate:%lu\n",ptDecoder->tInputPortDef.format.video.xFramerate);
// 	printf("video.CompressionFormat:%d (h264=7)\n",ptDecoder->tInputPortDef.format.video.eCompressionFormat);
// 	printf("--------------------------\n\n");

	//ptDecoder->tInputPortDef.nBufferSize = (m_tVideoInfo[iChnID].iWidth*m_tVideoInfo[iChnID].iHeight)*3/2;
	ptDecoder->tInputPortDef.nBufferCountMin = MAX_INPUT_BUFFERS;
	//ptDecoder->tInputPortDef.format.video.nFrameWidth  = m_tVideoInfo[iChnID].iWidth;
	//ptDecoder->tInputPortDef.format.video.nFrameHeight = m_tVideoInfo[iChnID].iHeight;
	omxRet = OMX_SetParameter(ptDecoder->pHandle, OMX_IndexParamPortDefinition, &ptDecoder->tInputPortDef);
	if(omxRet != OMX_ErrorNone)
	{
		LOG_ERROR_FMT("Channel%d OMX_SetParameter OMX_IndexParamPortDefinition failed", iChnID);
		//return -1;
	}

	omxRet = OMX_GetParameter(ptDecoder->pHandle,OMX_IndexParamPortDefinition,&ptDecoder->tInputPortDef);
	if(omxRet != OMX_ErrorNone)
	{
		LOG_ERROR_FMT("Channel%d OMX_GetParameter OMX_IndexParamPortDefinition failed", iChnID);
		//return -1;
	}

// 	printf("1 [Input Port]\n");
// 	printf("PortIndex:%lu\n",ptDecoder->tInputPortDef.nPortIndex);
// 	printf("Dir:%d\n",ptDecoder->tInputPortDef.eDir);
// 	printf("BufferCountActual:%lu\n",ptDecoder->tInputPortDef.nBufferCountActual);
// 	printf("BufferCountMin:%lu\n",ptDecoder->tInputPortDef.nBufferCountMin);
// 	printf("BufferSize:%lu\n",ptDecoder->tInputPortDef.nBufferSize);
// 	printf("Enabled:%d\n",ptDecoder->tInputPortDef.bEnabled);
// 	printf("Populated:%d\n",ptDecoder->tInputPortDef.bPopulated);
// 	printf("Domain:%d (Audio=0,Video=1,Image=2)\n",ptDecoder->tInputPortDef.eDomain);
// 	printf("video.MIME:%s\n",ptDecoder->tInputPortDef.format.video.cMIMEType);
// 	printf("video.nFrameWidth:%lu\n",ptDecoder->tInputPortDef.format.video.nFrameWidth);
// 	printf("video.nFrameHeight:%lu\n",ptDecoder->tInputPortDef.format.video.nFrameHeight);
// 	printf("video.nStride:%ld\n",ptDecoder->tInputPortDef.format.video.nStride);
// 	printf("video.nSliceHeight:%lu\n",ptDecoder->tInputPortDef.format.video.nSliceHeight);
// 	printf("video.nBitrate:%lu\n",ptDecoder->tInputPortDef.format.video.nBitrate);
// 	printf("video.xFramerate:%lu\n",ptDecoder->tInputPortDef.format.video.xFramerate);
// 	printf("video.CompressionFormat:%d (h264=7)\n",ptDecoder->tInputPortDef.format.video.eCompressionFormat);
// 	printf("--------------------------\n");

	ptDecoder->tInputPortDef.nBufferCountActual = ptDecoder->tInputPortDef.nBufferCountMin;
	for(OMX_U32 i = 0; i < ptDecoder->tInputPortDef.nBufferCountActual; i++)
	{
		omxRet = OMX_AllocateBuffer(ptDecoder->pHandle,&ptDecoder->pInputBufPtr[i],
			ptDecoder->tInputPortDef.nPortIndex, NULL, 
			ptDecoder->tInputPortDef.nBufferSize);

		if(omxRet != OMX_ErrorNone)
		{
			LOG_ERROR_FMT("Channel%d OMX_AllocateBuffer failed", iChnID);
			//return -1;
		}
	}

	// allocate output buffer
	INIT_PARAM(ptDecoder->tOutputPortDef);
	ptDecoder->tOutputPortDef.nPortIndex = 1;
	omxRet = OMX_GetParameter(ptDecoder->pHandle,OMX_IndexParamPortDefinition,&ptDecoder->tOutputPortDef);
	if(omxRet != OMX_ErrorNone)
	{
		LOG_ERROR_FMT("Channel%d OMX_GetParameter OMX_IndexParamPortDefinition failed", iChnID);
		//return -1;
	}
// 	printf("0 [Output Port]\n");
// 	printf("PortIndex:%lu\n",ptDecoder->tOutputPortDef.nPortIndex);
// 	printf("Dir:%d\n",ptDecoder->tOutputPortDef.eDir);
// 	printf("BufferCountActual:%lu\n",ptDecoder->tOutputPortDef.nBufferCountActual);
// 	printf("BufferCountMin:%lu\n",ptDecoder->tOutputPortDef.nBufferCountMin);
// 	printf("BufferSize:%lu\n",ptDecoder->tOutputPortDef.nBufferSize);
// 	printf("Enabled:%d\n",ptDecoder->tOutputPortDef.bEnabled);
// 	printf("Populated:%d\n",ptDecoder->tOutputPortDef.bPopulated);
// 	printf("Domain:%d (Audio=0,Video=1,Image=2)\n",ptDecoder->tOutputPortDef.eDomain);
// 	printf("video.MIME:%s\n",ptDecoder->tOutputPortDef.format.video.cMIMEType);
// 	printf("video.nFrameWidth:%lu\n",ptDecoder->tOutputPortDef.format.video.nFrameWidth);
// 	printf("video.nFrameHeight:%lu\n",ptDecoder->tOutputPortDef.format.video.nFrameHeight);
// 	printf("video.nStride:%ld\n",ptDecoder->tOutputPortDef.format.video.nStride);
// 	printf("video.nSliceHeight:%lu\n",ptDecoder->tOutputPortDef.format.video.nSliceHeight);
// 	printf("video.nBitrate:%lu\n",ptDecoder->tOutputPortDef.format.video.nBitrate);
// 	printf("video.xFramerate:%lu\n",ptDecoder->tOutputPortDef.format.video.xFramerate);
// 	printf("video.bFlagErrorConcealment:%d\n",ptDecoder->tOutputPortDef.format.video.bFlagErrorConcealment);
// 	printf("video.CompressionFormat:%d (h264=7)\n",ptDecoder->tOutputPortDef.format.video.eCompressionFormat);
// 	printf("video.eColorFormat:%d\n",ptDecoder->tOutputPortDef.format.video.eColorFormat);
// 	printf("BuffersContiguous:%d\n",ptDecoder->tOutputPortDef.bBuffersContiguous);
// 	printf("BufferAlignment:%lu\n",ptDecoder->tOutputPortDef.nBufferAlignment);
// 	printf("--------------------------\n");

	ptDecoder->tOutputPortDef.format.video.nFrameWidth  = (OMX_U32)(m_tVideoInfo[iChnID].iWidth*1.1);
	ptDecoder->tOutputPortDef.format.video.nFrameHeight = (OMX_U32)(m_tVideoInfo[iChnID].iHeight*1.1);
	//ptDecoder->tOutputPortDef.nBufferSize = (m_tVideoInfo[iChnID].iWidth*m_tVideoInfo[iChnID].iHeight)*3/2;
	ptDecoder->tOutputPortDef.nBufferCountActual = MAX_OUTPUT_BUFFERS;
	omxRet = OMX_SetParameter(ptDecoder->pHandle,OMX_IndexParamPortDefinition,&ptDecoder->tOutputPortDef);
	if(omxRet != OMX_ErrorNone)
	{
		LOG_ERROR_FMT("Channel%d OMX_SetParameter OMX_IndexParamPortDefinition failed", iChnID);
		//return -1;
	}

	omxRet = OMX_GetParameter(ptDecoder->pHandle,OMX_IndexParamPortDefinition, &ptDecoder->tOutputPortDef);
	if(omxRet != OMX_ErrorNone)
	{
		LOG_ERROR_FMT("Channel%d OMX_GetParameter OMX_IndexParamPortDefinition failed", iChnID);
		//return -1;
	}

// 	printf("1 [Output Port]\n");
// 	printf("PortIndex:%lu\n",ptDecoder->tOutputPortDef.nPortIndex);
// 	printf("Dir:%d\n",ptDecoder->tOutputPortDef.eDir);
// 	printf("BufferCountActual:%lu\n",ptDecoder->tOutputPortDef.nBufferCountActual);
// 	printf("BufferCountMin:%lu\n",ptDecoder->tOutputPortDef.nBufferCountMin);
// 	printf("BufferSize:%lu\n",ptDecoder->tOutputPortDef.nBufferSize);
// 	printf("Enabled:%d\n",ptDecoder->tOutputPortDef.bEnabled);
// 	printf("Populated:%d\n",ptDecoder->tOutputPortDef.bPopulated);
// 	printf("Domain:%d (Audio=0,Video=1,Image=2)\n",ptDecoder->tOutputPortDef.eDomain);
// 	printf("video.MIME:%s\n",ptDecoder->tOutputPortDef.format.video.cMIMEType);
// 	printf("video.nFrameWidth:%lu\n",ptDecoder->tOutputPortDef.format.video.nFrameWidth);
// 	printf("video.nFrameHeight:%lu\n",ptDecoder->tOutputPortDef.format.video.nFrameHeight);
// 	printf("video.nStride:%ld\n",ptDecoder->tOutputPortDef.format.video.nStride);
// 	printf("video.nSliceHeight:%lu\n",ptDecoder->tOutputPortDef.format.video.nSliceHeight);
// 	printf("video.nBitrate:%lu\n",ptDecoder->tOutputPortDef.format.video.nBitrate);
// 	printf("video.xFramerate:%lu\n",ptDecoder->tOutputPortDef.format.video.xFramerate);
// 	printf("video.bFlagErrorConcealment:%d\n",ptDecoder->tOutputPortDef.format.video.bFlagErrorConcealment);
// 	printf("video.CompressionFormat:%d (h264=7)\n",ptDecoder->tOutputPortDef.format.video.eCompressionFormat);
// 	printf("video.eColorFormat:%d\n",ptDecoder->tOutputPortDef.format.video.eColorFormat);
// 	printf("BuffersContiguous:%d\n",ptDecoder->tOutputPortDef.bBuffersContiguous);
// 	printf("BufferAlignment:%lu\n",ptDecoder->tOutputPortDef.nBufferAlignment);
// 	printf("--------------------------\n");

	for(OMX_U32 i = 0; i < ptDecoder->tOutputPortDef.nBufferCountActual; i++)
	{
		omxRet = OMX_AllocateBuffer(ptDecoder->pHandle,	&ptDecoder->pIOutputBufPtr[i], 
			ptDecoder->tOutputPortDef.nPortIndex,NULL,
			ptDecoder->tOutputPortDef.nBufferSize);
		if(omxRet != OMX_ErrorNone)
		{
			LOG_ERROR_FMT("Channel%d OMX_AllocateBuffer failed", iChnID);
			//return -1;
		}
	}

	// set state to idle
	SetOmxState(ptDecoder->pHandle, OMX_StateIdle, 1);

	//set state to execute
	SetOmxState(ptDecoder->pHandle, OMX_StateExecuting, 1);

	ptDecoder->pInputBufPtr[0]->nFilledLen = 0;
	ptDecoder->pInputBufPtr[0]->nOffset = 0;
	OMX_EmptyThisBuffer(ptDecoder->pHandle, ptDecoder->pInputBufPtr[0]);

	for(OMX_U32 i = 0; i < ptDecoder->tOutputPortDef.nBufferCountActual; i++)
	{
		ptDecoder->pIOutputBufPtr[i]->nFlags = 40;
		OMX_FillThisBuffer(ptDecoder->pHandle, ptDecoder->pIOutputBufPtr[i]);
	}

	return 0;
}

int DecodeMan::Decode_Close(int iChnID)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	OMXDecoder *ptDecoder = &m_tDecoder[iChnID];

	if (ptDecoder->pHandle == NULL)
	{
		return 0;
	}

	// set state to idle
	SetOmxState(ptDecoder->pHandle, OMX_StateIdle, 1);

	// release output buffers
	for(OMX_U32 i = 0; i < ptDecoder->tOutputPortDef.nBufferCountActual; i++)
	{
		OMX_FreeBuffer(ptDecoder->pHandle,
			ptDecoder->tOutputPortDef.nPortIndex, 
			ptDecoder->pIOutputBufPtr[i]);
		ptDecoder->pIOutputBufPtr[i] = NULL;
	}

	// release input buffers
	for(OMX_U32 i = 0; i < ptDecoder->tInputPortDef.nBufferCountActual; i++)
	{
		OMX_FreeBuffer(ptDecoder->pHandle, 
			ptDecoder->tInputPortDef.nPortIndex,
			ptDecoder->pInputBufPtr[i]);
		ptDecoder->pInputBufPtr[i] = NULL;	
	}

	// set state to loaded
	SetOmxState(ptDecoder->pHandle, OMX_StateLoaded, 1);

	// free component handle
	OMX_FreeHandle(ptDecoder->pHandle);
	ptDecoder->pHandle = NULL;

	//OMX_Deinit();

	return 0;
}
#else
int DecodeMan::Decode_Init(int iChnID)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if (m_tVideoInfo[iChnID].iWidth < 1 || m_tVideoInfo[iChnID].iHeight < 1)
	{
		LOG_ERROR_FMT("resolution error %d*%d",m_tVideoInfo[iChnID].iWidth,m_tVideoInfo[iChnID].iHeight);
		return -1;
	}

	FfmpegDecoder *ptDecoder = &m_tDecoder[iChnID];
	ptDecoder->findpps = 0;
	ptDecoder->findsps = 0;

	av_register_all();
	av_log_set_level(AV_LOG_INFO);
	
	av_init_packet(&(ptDecoder->avpkt));

	/* find the H264 video decoder */
	ptDecoder->ptAvCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (!ptDecoder->ptAvCodec)
	{
		LOG_ERROR_FMT("AV_CODEC_ID_H264 Codec not found");
		return -1;
	}

	ptDecoder->ptAvCodecContext = avcodec_alloc_context3(ptDecoder->ptAvCodec);
	if (!ptDecoder->ptAvCodecContext)
	{
		LOG_ERROR_FMT("Could not allocate video codec context");
		return -1;
	}

	if(ptDecoder->ptAvCodec->capabilities & CODEC_CAP_TRUNCATED)
	{
		ptDecoder->ptAvCodecContext->flags |= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */
	}

	/* open it */
	if (avcodec_open2(ptDecoder->ptAvCodecContext, ptDecoder->ptAvCodec, NULL) < 0)
	{
		LOG_ERROR_FMT("Could not open codec");
		avcodec_free_context(&ptDecoder->ptAvCodecContext);
		ptDecoder->ptAvCodecContext = NULL;
		ptDecoder->ptAvCodec = NULL;
		return -1;
	}

	ptDecoder->ptAvFrame = av_frame_alloc();
	if (!ptDecoder->ptAvFrame)
	{
		LOG_ERROR_FMT("Could not allocate video frame");
		return -1;
	}

	return 0;
}

int DecodeMan::Decode_Close(int iChnID)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	FfmpegDecoder *ptDecoder = &m_tDecoder[iChnID];

	if(ptDecoder)
	{
		av_packet_unref(&(ptDecoder->avpkt));
		
		av_frame_free(&ptDecoder->ptAvFrame);
		ptDecoder->ptAvFrame = NULL;

		if(ptDecoder->ptAvCodecContext)
		{
			avcodec_close(ptDecoder->ptAvCodecContext);
			av_free(ptDecoder->ptAvCodecContext);
			ptDecoder->ptAvCodecContext = NULL;
		}
		
		ptDecoder->ptAvCodec = NULL;
		ptDecoder->findpps = 0;
		ptDecoder->findsps = 0;
	}

	return 0;
}


int DecodeMan::Decode_H264(int iChnID, H264_Queue_Node * ptH264Data)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	FfmpegDecoder *ptDecoder = &m_tDecoder[iChnID];
	if(ptDecoder->ptAvCodecContext == NULL || ptDecoder->ptAvFrame == NULL)
	{
		return -1;
	}

	static unsigned int	s_frame_id[MAX_CHANNEL_NUM] = {0};

	ptDecoder->avpkt.size = ptH264Data->h264_info.data_size;
	ptDecoder->avpkt.data = ptH264Data->frame_data;

	while (ptDecoder->avpkt.size > 0)
	{
		int frameFinished = 0;
		int bytesDecoded = avcodec_decode_video2(ptDecoder->ptAvCodecContext, ptDecoder->ptAvFrame, &frameFinished, &(ptDecoder->avpkt));
		if (bytesDecoded < 0)
		{
			DecodeMan::GetInstance()->CountOutputInfoMiss(iChnID);
			break;
		}

		ptDecoder->avpkt.size -= bytesDecoded;
		ptDecoder->avpkt.data += bytesDecoded;

		if(frameFinished)
		{
// 			printf("ptDecoder->ptAvFrame:%d*%d format:%d keyframe=%d linesize[0]=%d linesize[1]=%d linesize[2]=%d\n",
// 				ptDecoder->ptAvFrame->width,ptDecoder->ptAvFrame->height,ptDecoder->ptAvFrame->format,
// 				ptDecoder->ptAvFrame->key_frame,ptDecoder->ptAvFrame->linesize[0],ptDecoder->ptAvFrame->linesize[1],ptDecoder->ptAvFrame->linesize[2]);
			
			Yuv_Queue_Node *pYuvNode = DecodeMan::GetInstance()->GetEmptyYuvNode(iChnID);
			if(pYuvNode != NULL)
			{
				s_frame_id[iChnID]++;
		
				DecodeMan::GetInstance()->CountOutputInfo(iChnID);
		
				DecodeVideoInfo tVideoInfo = {0};
				DecodeMan::GetInstance()->GetChnVideoInfo(iChnID,tVideoInfo);
				
				struct timeval _tv;
				gettimeofday(&_tv, NULL);
				
				pYuvNode->yuv_info.timestamp  = _tv.tv_sec*1000 + _tv.tv_usec/1000;
				pYuvNode->yuv_info.frame_id   = s_frame_id[iChnID];
				pYuvNode->yuv_info.channel	  = iChnID;
				pYuvNode->yuv_info.frame_rate = tVideoInfo.iFrameRate;
				pYuvNode->yuv_info.width	  = ptDecoder->ptAvFrame->width;
				pYuvNode->yuv_info.height	  = ptDecoder->ptAvFrame->height;
				pYuvNode->yuv_info.data_size  = (ptDecoder->ptAvFrame->width * ptDecoder->ptAvFrame->height) + (ptDecoder->ptAvFrame->width * ptDecoder->ptAvFrame->height)/2;
				//printf("PutFullYuvNode frame_id:%d timestamp:%llu\n", pYuvNode->yuv_info.frame_id, pYuvNode->yuv_info.timestamp);

				int y_size = ptDecoder->ptAvFrame->width * ptDecoder->ptAvFrame->height;
				unsigned char* y = ptDecoder->ptAvFrame->data[0];  
				unsigned char* u = ptDecoder->ptAvFrame->data[1];  
				unsigned char* v = ptDecoder->ptAvFrame->data[2];  

				unsigned char* y_tmp  = pYuvNode->yuv_data;  
				memcpy(y_tmp, y, y_size);  

#ifdef YUV420
				unsigned char* v_tmp = pYuvNode->yuv_data + y_size;  
				unsigned char* v_tmp = pYuvNode->yuv_data + y_size + y_size/4;  
				memcpy(v_tmp, u, y_size/4);  
				memcpy(v_tmp, v, y_size/4);  

#else// NV12
				int i = 0, j = 0;
				unsigned char* uv_tmp = pYuvNode->yuv_data + y_size;  
			    for (j = 0, i = 0; j < y_size/2; j+=2, i++)  
			    {  
			    	//NV12
			        uv_tmp[j] = u[i];  
			        uv_tmp[j+1] = v[i];
					//NV21
			       	//uv_tmp[j] = v[i];  
			        //uv_tmp[j+1] = u[i];  
			    }  
#endif

#ifdef SAVE_YUV
				static unsigned int count = 0;
				if(count < 50)
				{
					fwrite(pYuvNode->yuv_data, 1, pYuvNode->yuv_info.data_size,ptDecoder->fp_yuv);
					count++;
				}
				else
				{
					if(ptDecoder->fp_yuv)
					{
						fclose(ptDecoder->fp_yuv);
						ptDecoder->fp_yuv = NULL;
						printf("\n===SAVE_YUV DONE===\n");
					}
				}	
#endif	

				DecodeMan::GetInstance()->PutFullYuvNode(iChnID);
			}
		}
	}

	av_packet_unref(&(ptDecoder->avpkt));

	return 0;
}


#endif

int DecodeMan::PrintfCountInfo( int iChnID )
{
	gettimeofday(&m_tPrintInfo[iChnID].tEndTime,NULL);

	long timeuse = (1000*m_tPrintInfo[iChnID].tEndTime.tv_sec+ m_tPrintInfo[iChnID].tEndTime.tv_usec/1000)
		- (1000*m_tPrintInfo[iChnID].tStartTime.tv_sec+ m_tPrintInfo[iChnID].tStartTime.tv_usec/1000);
	if (timeuse == 0)
	{
		return -1;
	}

	LOG_DEBUG_FMT("==========Channel %d Decode Info============", iChnID);
	LOG_DEBUG_FMT("Input Frame Count:%u | IFrame Count:%u | FPS:%u",
		m_tPrintInfo[iChnID].uInputFrameCnt, m_tPrintInfo[iChnID].uInputIFrameCnt,
		m_tPrintInfo[iChnID].uInputFrameCnt/(timeuse/1000));
	LOG_DEBUG_FMT("Output Frame Count:%u | Miss Count:%u | FPS:%u",
		m_tPrintInfo[iChnID].uOutputFrameCnt, m_tPrintInfo[iChnID].uOutputFrameMissCnt,
		m_tPrintInfo[iChnID].uOutputFrameCnt/(timeuse/1000));
	LOG_DEBUG_FMT("Decode TotalTime:%ld(s) | AvgTime:%ld(ms)",
		timeuse/1000, m_tPrintInfo[iChnID].uOutputFrameCnt==0?0:timeuse/m_tPrintInfo[iChnID].uOutputFrameCnt);

	m_tPrintInfo[iChnID].uInputFrameCnt = 0;
	m_tPrintInfo[iChnID].uInputIFrameCnt = 0;
	m_tPrintInfo[iChnID].uOutputFrameCnt = 0;
	m_tPrintInfo[iChnID].uOutputFrameMissCnt = 0;
	return 0;
}

int DecodeMan::CountInputInfo( const H264_Data_Header *ptHeader )
{
	if (ptHeader == NULL || ptHeader->channel <0 || ptHeader->channel>MAX_CHANNEL_NUM-1)
	{
		return -1;
	}
	int iChnID = ptHeader->channel;

	if(m_tPrintInfo[iChnID].uInputFrameCnt == 0)
	{
		gettimeofday(&m_tPrintInfo[iChnID].tStartTime,NULL);
	}

	m_tPrintInfo[iChnID].uInputFrameCnt++;
	if (ptHeader->frame_type == I_FRAME)
	{
		m_tPrintInfo[iChnID].uInputIFrameCnt++;
	}

	return 0;
}

int DecodeMan::CountOutputInfo(int iChnID)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return -1;
	}

	m_tPrintInfo[iChnID].uOutputFrameCnt++;
	return 0;
}

int DecodeMan::CountOutputInfoMiss(int iChnID)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return -1;
	}

	m_tPrintInfo[iChnID].uOutputFrameMissCnt++;
	return 0;
}
