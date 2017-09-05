#include "librtspsource.h"
#include "RTSPSource.h"
#include "SPSParser.h"
#include "oal_base64.h"
#include <vector>
#include <string>
using namespace std;
using namespace avstream;

extern "C" {

	void* rtsp_source_new(StreamCallbackFunc callback, DisConnectNotifyFunc disconnectcallback, void* context)
	{
		RTSPSource *source = new RTSPSource();
		if(source == NULL)
		{
			printf("new RTSPSource NULL\n");
			return NULL;
		}

		source->setStreamCallback(callback, context);
		source->setDisconnectCallback(disconnectcallback);
		return source;
	}

	int rtsp_source_open(void* rtsp, const char* url, int timeout)
	{
		if(rtsp == NULL || url == NULL)
		{
			printf("Input is NULL\n");
			return -1;
		}

		RTSPSource *source = (RTSPSource *)rtsp;
		return	source->open(url, timeout) ? 0 : -1;
	}

	int rtsp_source_is_opened(void* rtsp)
	{
		if(rtsp == NULL)
		{
			printf("Input is NULL\n");
			return -1;
		}

		RTSPSource *source = (RTSPSource *)rtsp;
		return	source->isOpened()?1:0;
	}

	int rtsp_source_is_play(void* rtsp)
	{
		if(rtsp == NULL)
		{
			printf("Input is NULL\n");
			return -1;
		}

		RTSPSource *source = (RTSPSource *)rtsp;
		return	source->isPlay()?1:0;
	}

	static int rtsp_source_get_tracks(void* rtsp, TrackInfoPtr *tracks)
	{
		if(rtsp == NULL)
		{
			printf("Input is NULL\n");
			return -1;
		}

		RTSPSource *source = (RTSPSource *)rtsp;
		const vector<live_track_t*>& tks = source->getTracks();
		int trackNum = tks.size();

		if (tracks == NULL)
		{
			return trackNum;
		}

		if (trackNum > 0)
		{
			TrackInfoPtr temp = (TrackInfoPtr)calloc(trackNum, sizeof(TrackInfo));
			if (temp == NULL)
			{
				return -1;
			}

			for (int i = 0; i < trackNum; i++)
			{
				MediaSubsession *sub = tks[i]->sub;
				assert(sub != NULL);

#ifdef WIN32
				temp[i].codecName     = _strdup(sub->codecName()); 
				temp[i].spsparam      = _strdup(sub->fmtp_spropparametersets());
				temp[i].framerate     = sub->videoFPS();//_strdup(sub->attrVal_str("framerate"));
#else
				temp[i].codecName     = strdup(sub->codecName()); 
				temp[i].spsparam      = strdup(sub->fmtp_spropparametersets());
				temp[i].framerate     = sub->videoFPS();//strdup(sub->attrVal_str("framerate"));
#endif // WIN32
				temp[i].timeFreq      = sub->rtpTimestampFrequency();
				temp[i].payloadFormat = sub->rtpPayloadFormat();
				temp[i].channels      = sub->numChannels();
				temp[i].streamType    = tks[i]->streamType;

				if (i + 1 < trackNum) {
					temp[i].next = temp + i + 1;
				} else {
					temp[i].next = NULL;
				}

				char *sdp = source->getSDP();
				if (sdp == NULL)
					continue;

				*tracks = temp;
			}
		}
		else
		{
			*tracks = NULL;
		}

		return trackNum;
	}

	static void rtsp_source_free_tracks(TrackInfoPtr tracks)
	{
		if (tracks)
		{
			TrackInfoPtr node = tracks;
			while(node)
			{
				if(node->codecName)
				{
					free(node->codecName);
					node->codecName = NULL;
				}

				if(node->spsparam)
				{
					free(node->spsparam);
					node->spsparam = NULL;
				}

				node = node->next;
			}
			free(tracks);
		}
	}

	int rtsp_source_play(void* rtsp)
	{
		if(rtsp == NULL)
		{
			printf("Input is NULL\n");
			return -1;
		}

		RTSPSource *source = (RTSPSource *)rtsp;
		return source->play()?0:-1;
	}

	void rtsp_source_close(void* rtsp)
	{
		if(rtsp == NULL)
		{
			printf("Input is NULL\n");
			return;
		}

		RTSPSource *source = (RTSPSource *)rtsp;
		source->close();
	}

	void rtsp_source_free(void* rtsp)
	{
		if(rtsp == NULL)
		{
			printf("Input is NULL\n");
			return;
		}

		RTSPSource *source = (RTSPSource *)rtsp;
		delete source;
		source = NULL;
	}

	int rtsp_source_get_resolution(void* rtsp,int &width,int &height, int &fps)
	{
		if(rtsp == NULL)
		{
			printf("Input is NULL\n");
			return -1;
		}

		RTSPSource *source = (RTSPSource *)rtsp;

		int num = 0;
		string strsps = "";

		TrackInfoPtr ptr = NULL;
		num = rtsp_source_get_tracks(source,&ptr);

		for(int i = 0; i< num ;i++)
		{
			if(ptr[i].streamType == 2)
			{
				strsps = ptr[i].spsparam;
				fps = (int)ptr[i].framerate;
			}
		}

		rtsp_source_free_tracks(ptr);

		if (fps < 1 || fps > 60)
		{
			fps = 25;
		}
		

		string::size_type pos = strsps.find_first_of(",");
		if(pos != strsps.npos)
		{
			strsps = strsps.substr(0,pos);
		}

		int decodeLen = Base64decode_len(strsps.c_str());
		char* temp = (char *)malloc(decodeLen + 1);
		if(temp == NULL)
		{
			width = 1920;
			height = 1088;
			return -1;
		}

		Base64decode(temp, strsps.c_str()); 

		SPSParser spsreader;
		bs_t s;
		s.p		  = (uint8_t *)temp;//在这里,_Resolution是经过BASE64解码后的SPS数据的指针
		s.p_start = (uint8_t *)temp;
		s.p_end	  = (uint8_t *)(temp + strsps.length());//size是SPS数据的长度
		s.i_left  = 8;//这个是固定的.是指SPS的对齐宽度

		int iret = spsreader.Do_Read_SPS(&s,&width,&height);

		free(temp);
		temp = NULL;

		if(iret !=0)
		{
			if((width == 0)||(height == 0))
			{
				width = 1920;
				height = 1088;
			}

			return -1;
		}

		return 0;
	}
}
