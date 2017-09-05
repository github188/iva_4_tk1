/***************************************************************
模块 ： 全局数据类型定义                        
文件 ： oal_typedef.h
作者 ： Ripple Lee(libozjw@163.com)
版本 ： V1.0
日期 ： 2017-01-18
*****************************************************************/
#ifndef __OAL_TYPEDEF_H__
#define __OAL_TYPEDEF_H__

// 通用包含文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* 系统基本数据类型定义 */
typedef char				s8;			/* 有符号的8bit整数   */
typedef unsigned char		u8;			/* 无符号的8bit整数   */
typedef short				s16;		/* 有符号的16bit整数  */
typedef unsigned short 		u16;		/* 无符号的16bit整数  */
typedef int					s32;		/* 有符号的32bit整数  */
typedef unsigned int		u32;		/* 无符号的32bit整数  */
typedef long long			s64;		/* 有符号的64bit整数  */
typedef unsigned long long	u64;		/* 无符号的64bit整数; */
typedef float				f32;		/* 32bit的浮点数      */
typedef double				f64;		/* 64bit的浮点数      */

#if !defined( __cplusplus )
	#ifndef bool
		#if SIZEOF_BOOL == 8
			typedef uint64_t bool;
		#elif SIZEOF_BOOL == 4
			typedef uint32_t bool;
		#elif SIZEOF_BOOL == 2
			typedef uint16_t bool;
		#else
			typedef unsigned char bool;
		#endif
	#endif
#endif

/* 错误类型 */
#define FAILURE              -1   /* 错误 */
#define SUCCESS               0   /* 成功 */

#ifndef OAL_API
	#ifndef WIN32
		#ifdef __cplusplus
			#define OAL_API extern "C"
		#else
			#define OAL_API
		#endif
	#else
		#ifdef OAL_EXPORTS
			#define OAL_API extern  "C"   __declspec(dllexport)
		#else
			#define OAL_API extern  "C"   __declspec(dllimport)
		#endif
	#endif
#endif


#define IPV4_LEN	16	// IPV4字符长度
#define MAC_LEN		18	// MC地址字符长度
#define NAME_LEN	30	// 用户名字符长度
#define PWD_LEN		30	// 密码字符长度
#define URI_LEN		128	// URI字符长度
#define PLATE_LEN	16	// 车牌号码长度

#ifdef __cplusplus
}
#endif

#endif //__OAL_TYPEDEF_H__