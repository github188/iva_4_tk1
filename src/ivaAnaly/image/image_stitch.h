#ifndef __IMAGE_STITCH_H__
#define __IMAGE_STITCH_H__
#include "alg_osd.h"
#include "../AnalyEvidenceMan.h"

#ifdef __cplusplus
extern "C" {
#endif

int SaveImageWithLimit(IplImage *img, const char *file_name, int img_size, int img_quality, int max_times);

int Image_Stitch_None(AnalyData * ptData, OsdContent *pOsd);
int Image_Stitch_Horizontal(AnalyData * ptData, OsdContent *pOsd);
int Image_Stitch_Vertical(AnalyData * ptData, OsdContent *pOsd);
int Image_Stitch_Matts(AnalyData * ptData, OsdContent *pOsd);

#ifdef __cplusplus
};
#endif

#endif



