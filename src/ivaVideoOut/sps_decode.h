#ifndef __SPS_DECODE_H__
#define __SPS_DECODE_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

/**
 * ����SPS,��ȡ��Ƶͼ�������Ϣ 
 *
 * @param buf SPS��������
 * @param nLen SPS���ݵĳ���
 * @param width ͼ����
 * @param height ͼ��߶�

 * @�ɹ��򷵻�1 , ʧ���򷵻�0
 */ 
bool h264_decode_sps(unsigned char * buf,unsigned int nLen,int &width,int &height,int &fps);

#endif//__SPS_DECODE_H__