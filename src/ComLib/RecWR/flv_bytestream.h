/*****************************************************************************
 * flv_bytestream.h: flv muxer utilities
 *****************************************************************************
 * Copyright (C) 2009-2011 x264 project
 *
 * Authors: Kieran Kunhya <kieran@kunhya.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 * This program is also available under a commercial proprietary license.
 * For more information, contact us at licensing@x264.com.
 *****************************************************************************/

#ifndef X264_FLV_BYTESTREAM_H
#define X264_FLV_BYTESTREAM_H

#include <stdio.h>
/* OSDEF */

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef long long int  int64_t;
typedef unsigned long long int uint64_t;


// ARM compiliers don't reliably align stack variables
// - EABI requires only 8 byte stack alignment to be maintained
// - gcc can't align stack variables to more even if the stack were to be correctly aligned outside the function
// - armcc can't either, but is nice enough to actually tell you so
// - Apple gcc only maintains 4 byte alignment
// - llvm can align the stack, but only in svn and (unrelated) it exposes bugs in all released GNU binutils...

#if ARCH_ARM && SYS_MACOSX
#define ALIGNED_ARRAY_8( type, name, sub1, ... )\
uint8_t name##_u [sizeof(type sub1 __VA_ARGS__) + 7]; \
type (*name) __VA_ARGS__ = (void*)((intptr_t)(name##_u+7) & ~7)
#else
#define ALIGNED_ARRAY_8( type, name, sub1, ... )\
ALIGNED_8( type name sub1 __VA_ARGS__ )
#endif

#if ARCH_ARM
#define ALIGNED_ARRAY_16( type, name, sub1, ... )\
uint8_t name##_u [sizeof(type sub1 __VA_ARGS__) + 15];\
type (*name) __VA_ARGS__ = (void*)((intptr_t)(name##_u+15) & ~15)
#else
#define ALIGNED_ARRAY_16( type, name, sub1, ... )\
ALIGNED_16( type name sub1 __VA_ARGS__ )
#endif

#define UNINIT(x) x=x

#if defined(__GNUC__) && (__GNUC__ > 3 || __GNUC__ == 3 && __GNUC_MINOR__ > 0)
#define UNUSED __attribute__((unused))
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#define NOINLINE __attribute__((noinline))
#define MAY_ALIAS __attribute__((may_alias))
#define x264_constant_p(x) __builtin_constant_p(x)
#define x264_nonconstant_p(x) (!__builtin_constant_p(x))
#else
#ifdef __ICL
#define ALWAYS_INLINE __forceinline
#define NOINLINE __declspec(noinline)
#else
#define ALWAYS_INLINE inline
#define NOINLINE
#endif
#define UNUSED
#define MAY_ALIAS
#define x264_constant_p(x) 0
#define x264_nonconstant_p(x) 0
#endif

#define WORD_SIZE sizeof(void*)

#define asm __asm__

#if WORDS_BIGENDIAN
#define endian_fix(x) (x)
#define endian_fix64(x) (x)
#define endian_fix32(x) (x)
#define endian_fix16(x) (x)
#else
#if HAVE_X86_INLINE_ASM && HAVE_MMX
static ALWAYS_INLINE uint32_t endian_fix32( uint32_t x )
{
    asm("bswap %0":"+r"(x));
    return x;
}
#elif defined(__GNUC__) && HAVE_ARMV6
static ALWAYS_INLINE uint32_t endian_fix32( uint32_t x )
{
    asm("rev %0, %0":"+r"(x));
    return x;
}
#else
static ALWAYS_INLINE uint32_t endian_fix32( uint32_t x )
{
    return (x<<24) + ((x<<8)&0xff0000) + ((x>>8)&0xff00) + (x>>24);
}
#endif
#if HAVE_X86_INLINE_ASM && ARCH_X86_64
static ALWAYS_INLINE uint64_t endian_fix64( uint64_t x )
{
    asm("bswap %0":"+r"(x));
    return x;
}
#else
static ALWAYS_INLINE uint64_t endian_fix64( uint64_t x )
{
    return endian_fix32(x>>32) + ((uint64_t)endian_fix32(x)<<32);
}
#endif
/*
static ALWAYS_INLINE intptr_t endian_fix( intptr_t x )
{
    return WORD_SIZE == 8 ? endian_fix64(x) : endian_fix32(x);
}
*/
static ALWAYS_INLINE uint16_t endian_fix16( uint16_t x )
{
    return (x<<8)|(x>>8);
}
#endif
/*OSDEF*/

/* offsets for packed values */
#define FLV_AUDIO_SAMPLESSIZE_OFFSET 1
#define FLV_AUDIO_SAMPLERATE_OFFSET  2
#define FLV_AUDIO_CODECID_OFFSET     4

#define FLV_VIDEO_FRAMETYPE_OFFSET   4

/* bitmasks to isolate specific values */
#define FLV_AUDIO_CHANNEL_MASK    0x01
#define FLV_AUDIO_SAMPLESIZE_MASK 0x02
#define FLV_AUDIO_SAMPLERATE_MASK 0x0c
#define FLV_AUDIO_CODECID_MASK    0xf0

#define FLV_VIDEO_CODECID_MASK    0x0f
#define FLV_VIDEO_FRAMETYPE_MASK  0xf0

#define AMF_END_OF_OBJECT         0x09

enum
{
    FLV_HEADER_FLAG_HASVIDEO = 1,
    FLV_HEADER_FLAG_HASAUDIO = 4,
};

enum
{
    FLV_TAG_TYPE_AUDIO = 0x08,
    FLV_TAG_TYPE_VIDEO = 0x09,
    FLV_TAG_TYPE_META  = 0x12,
};

enum
{
    FLV_MONO   = 0,
    FLV_STEREO = 1,
};

enum
{
    FLV_SAMPLESSIZE_8BIT  = 0,
    FLV_SAMPLESSIZE_16BIT = 1 << FLV_AUDIO_SAMPLESSIZE_OFFSET,
};

enum
{
    FLV_SAMPLERATE_SPECIAL = 0, /**< signifies 5512Hz and 8000Hz in the case of NELLYMOSER */
    FLV_SAMPLERATE_11025HZ = 1 << FLV_AUDIO_SAMPLERATE_OFFSET,
    FLV_SAMPLERATE_22050HZ = 2 << FLV_AUDIO_SAMPLERATE_OFFSET,
    FLV_SAMPLERATE_44100HZ = 3 << FLV_AUDIO_SAMPLERATE_OFFSET,
};

enum
{
    FLV_CODECID_MP3 = 2 << FLV_AUDIO_CODECID_OFFSET,
    FLV_CODECID_AAC = 10<< FLV_AUDIO_CODECID_OFFSET,
};

enum
{
    FLV_CODECID_H264 = 7,//AVC
};

enum
{
    FLV_FRAME_KEY   = 1 << FLV_VIDEO_FRAMETYPE_OFFSET,
    FLV_FRAME_INTER = 2 << FLV_VIDEO_FRAMETYPE_OFFSET,
};

typedef enum
{
    AMF_DATA_TYPE_NUMBER      = 0x00,
    AMF_DATA_TYPE_BOOL        = 0x01,
    AMF_DATA_TYPE_STRING      = 0x02,
    AMF_DATA_TYPE_OBJECT      = 0x03,
    AMF_DATA_TYPE_NULL        = 0x05,
    AMF_DATA_TYPE_UNDEFINED   = 0x06,
    AMF_DATA_TYPE_REFERENCE   = 0x07,
    AMF_DATA_TYPE_MIXEDARRAY  = 0x08,
    AMF_DATA_TYPE_OBJECT_END  = 0x09,
    AMF_DATA_TYPE_ARRAY       = 0x0a,
    AMF_DATA_TYPE_DATE        = 0x0b,
    AMF_DATA_TYPE_LONG_STRING = 0x0c,
    AMF_DATA_TYPE_UNSUPPORTED = 0x0d,
} AMFDataType;


#define FLV_BUFFER_DEF_SIZE 2048
typedef struct flv_buffer
{
    uint8_t *data;
    unsigned d_cur;
    unsigned d_max;
    FILE *fp;
    uint64_t d_total;
} flv_buffer;


flv_buffer *flv_create_writer( const char *filename );
void flv_destroy_writer( flv_buffer *c );

int flv_append_data( flv_buffer *c, uint8_t *data, unsigned size );
int flv_write_byte( flv_buffer *c, uint8_t *byte );
int flv_flush_data( flv_buffer *c );
void rewrite_amf_be24( flv_buffer *c, unsigned length, unsigned start );

uint64_t dbl2int( double value );
uint64_t get_amf_double( double value );
void x264_put_byte( flv_buffer *c, uint8_t b );
void x264_put_be32( flv_buffer *c, uint32_t val );
void x264_put_be64( flv_buffer *c, uint64_t val );
void x264_put_be16( flv_buffer *c, uint16_t val );
void x264_put_be24( flv_buffer *c, uint32_t val );
void x264_put_tag( flv_buffer *c, const char *tag );
void x264_put_amf_string( flv_buffer *c, const char *str );
void x264_put_amf_double( flv_buffer *c, double d );

#endif
