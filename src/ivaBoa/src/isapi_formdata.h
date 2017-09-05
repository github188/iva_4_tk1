#ifndef __ISAPI_FORMDATA_H__
#define __ISAPI_FORMDATA_H__

#include "boa.h"

int isapi_post_is_form_data(request *req);
int save_isapi_post_form_data(request *req, char * srcfile, char *dstfile);

#endif /* __ISAPI_FORMDATA_H__ */

