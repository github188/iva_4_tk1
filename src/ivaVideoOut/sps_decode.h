#ifndef __SPS_DECODE_H__
#define __SPS_DECODE_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

/**
 * 解码SPS,获取视频图像宽、高信息 
 *
 * @param buf SPS数据内容
 * @param nLen SPS数据的长度
 * @param width 图像宽度
 * @param height 图像高度

 * @成功则返回1 , 失败则返回0
 */ 
bool h264_decode_sps(unsigned char * buf,unsigned int nLen,int &width,int &height,int &fps);

#endif//__SPS_DECODE_H__