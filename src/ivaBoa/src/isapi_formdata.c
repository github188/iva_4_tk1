#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "isapi_formdata.h"

static int StrBeginsNc(char *s1, char *s2)
{
	while(1)
	{
		if (!(*s2)) {
			return 1;
		} else if (!(*s1)) {
			return 0;
		}
		if (isalpha(*s1)) {
			if (tolower(*s1) != tolower(*s2)) {
				return 0;
			}
		} else if ((*s1) != (*s2)) {
			return 0;
		}
		s1++;
		s2++;
	}
}


// Content-Type: multipart/form-data; boundary=---------------------------35242632614132
static void get_boundary(char * content_type, char * boundary)
{
	char szContentType[512] = {0};

	if (content_type == NULL || boundary == NULL)
	{
		return;
	}

	strcpy(szContentType, content_type);
	if (strchr(szContentType, ';'))
	{
		char *sat = strchr(szContentType, ';');
		while (sat)
		{
			*sat = '\0';
			sat++;
			while (isspace(*sat))
			{
				sat++;
			}

			if (StrBeginsNc(sat, "boundary="))
			{
				char *s;
				char *pBoundary = sat + strlen("boundary=");
				s = pBoundary;
				while ((*s) && (!isspace(*s)))
				{
					s++;
				}
				*s = '\0';
				strcpy(boundary, pBoundary);
				break;
			}
			else
			{
				sat = strchr(sat, ';');
			} 	
		}
	}
}

int isapi_post_is_form_data(request *req)
{
	// 处理form-data
	int bFormData = 0;
	if (req->content_type)
	{
		if (strstr(req->content_type, "multipart/form-data;"))
		{
			bFormData = 1;
		}
	}
	return bFormData;
}

int read_file_byte(int fd, char *buffer, int bytes_to_read)
{
	int ireaded = 0;
	int ineed = bytes_to_read;
	while (ineed > 0)
	{
		int bytes_read = read(fd, buffer+ireaded, ineed);
		if (bytes_read == -1)
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN)
			{
				continue;
			}
			else
			{
				perror("read file");
				return 0;
			}
		}
		else if (bytes_read == 0)
		{
			perror("read file return 0");
			return 0;
		}
		else
		{
			ineed -= bytes_read;
			ireaded+=bytes_read;
		}
	}

	return ireaded;
}

int save_isapi_post_form_data(request *req, char * srcfile, char *dstfile)
{
	char szBoundary[512] = {0};
	char szBeginBoundary[1024];
	char szEndBoundary[1024];
	int iBeginBoundaryLength;
	int iEndBoundaryLength;
	char temp_file_name[1024] = {0};
	int boffset;
	int got;
	char d[3];	
	char szTitle[1024]  = {0};
	char szCRLF[3] = {"\r\n"};
	int  bEnd = 0;
	int fdDst = -1,fdSrc = -1;
	int content_length = boa_atoi(req->content_length);
	int iReaded = 0;
	char tmpbuf[4096]  = {0};
	int iFileSize = 0;

	get_boundary(req->content_type, szBoundary);
	fprintf(stderr, "%s %d boundary:%s\n",__FILE__, __LINE__,szBoundary);

	// 读取Boundary
	sprintf(szBeginBoundary, "--%s", szBoundary);
	sprintf(szEndBoundary, "--%s--", szBoundary);
	iBeginBoundaryLength = strlen(szBeginBoundary);
	iEndBoundaryLength = strlen(szEndBoundary);

	fdDst = create_temporary_file(0, temp_file_name, sizeof(temp_file_name));
	if (fdDst == 0)
	{
		fprintf(stderr, "create_temporary_file2 failed!\n");
		goto error_out;
	}

	fprintf(stderr, "%s %d srcfile:%s\n",__FILE__, __LINE__, srcfile);
	fprintf(stderr, "%s %d dstfile:%s\n",__FILE__, __LINE__, temp_file_name);

	fdSrc = open(srcfile, O_RDONLY);
	if (fdSrc == 0)
	{
		fprintf(stderr, "open %s failed!\n",srcfile);
		goto error_out;
	}

	// 读取起始boundary
	boffset = 0;
	while (1)
	{
		got = read_file_byte(fdSrc, d, 1);
		if (got != 1)
		{
			fprintf(stderr, "%s %d ReqRead 1 got %d\n",__FILE__, __LINE__,got);
			goto error_out;
		}
		iReaded+=got;

		if (d[0] == szBeginBoundary[boffset])
		{
			boffset++;
			if (boffset == iBeginBoundaryLength)
			{
				break;
			} 
		}
		else if (boffset > 0)
		{
			boffset = 0;
		}
		else
		{		
			fprintf(stderr, "%s %d ReqRead 1 got %c\n",__FILE__, __LINE__,d[0]);
		}	
	}
	fprintf(stderr, "%s %d BeginBoundar:%s \n",__FILE__, __LINE__,szBeginBoundary);

	// 读取CLRF
	got = read_file_byte(fdSrc, d, 2);
	if (got != 2)
	{
		fprintf(stderr, "%s %d ReqRead 2 got %d\n",__FILE__, __LINE__,got);
		goto error_out;
	}	
	iReaded+=got;

	if ((d[0] == '\r') && (d[1] == '\n'))
	{
		fprintf(stderr, "%s %d OK, EOL\n",__FILE__, __LINE__);
	}
	else
	{
		fprintf(stderr, "%s %d Read CLRF= %c%c\n",__FILE__, __LINE__,d[0], d[1]);
		goto error_out;
	}

	//Content-Disposition: form-data; name="file"; filename="20170406164721_001.tar.gz"
	boffset = 0;
	while (1)
	{
		got = read_file_byte(fdSrc, d, 1);
		if (got != 1)
		{
			fprintf(stderr, "%s %d ReqRead 1 got %d\n",__FILE__, __LINE__,got);
			goto error_out;
		}
		iReaded+=got;

		if (d[0] == szCRLF[0])
		{
			got = read_file_byte(fdSrc, d, 1);
			if (got != 1)
			{
				fprintf(stderr, "%s %d ReqRead 1 got %d\n",__FILE__, __LINE__,got);
				goto error_out;
			}
			iReaded+=got;

			if (d[0] == szCRLF[1])
			{
				break;
			}
		}

		szTitle[boffset] = d[0];
		boffset++;
	}

	fprintf(stderr, "%s %d szTitle:%s\n",__FILE__, __LINE__, szTitle);

	//Content-Type: application/gzip 
	memset(szTitle, 0, sizeof(szTitle));
	boffset = 0;
	while (1)
	{
		got = read_file_byte(fdSrc, d, 1);
		if (got != 1)
		{
			fprintf(stderr, "%s %d ReqRead 1 got %d\n",__FILE__, __LINE__,got);
			goto error_out;
		}
		iReaded+=got;

		if (d[0] == szCRLF[0])
		{
			got = read_file_byte(fdSrc, d, 1);
			if (got != 1)
			{
				fprintf(stderr, "%s %d ReqRead 1 got %d\n",__FILE__, __LINE__,got);
				goto error_out;
			}
			iReaded+=got;

			if (d[0] == szCRLF[1])
			{
				break;
			}
		}

		szTitle[boffset] = d[0];
		boffset++;
	}

	fprintf(stderr, "%s %d szTitle:%s\n",__FILE__, __LINE__, szTitle);

	// 读取CLRF
	got = read_file_byte(fdSrc, d, 2);
	if (got != 2)
	{
		fprintf(stderr, "%s %d ReqRead 2 got %d\n",__FILE__, __LINE__,got);
		goto error_out;
	}	
	iReaded+=got;

	if ((d[0] == '\r') && (d[1] == '\n'))
	{
		fprintf(stderr, "%s %d OK, EOL\n",__FILE__, __LINE__);
	}
	else
	{
		//fprintf(stderr, "%s %d Read CLRF= %c%c\n",__FILE__, __LINE__,d[0], d[1]);
		goto error_out;
	}

	// 拷贝文件
	iFileSize = content_length - iReaded-iEndBoundaryLength;
	fprintf(stderr, "%s %d content_length= %d iReaded=%d iEndBoundaryLength=%d iFileSize=%d\n",__FILE__, __LINE__,content_length,iReaded, iEndBoundaryLength, iFileSize);
	while (iFileSize>sizeof(tmpbuf))
	{
		memset(tmpbuf, 0, sizeof(tmpbuf));
		got = read_file_byte(fdSrc, tmpbuf, sizeof(tmpbuf)-1);
		if (got != sizeof(tmpbuf)-1)
		{
			fprintf(stderr, "%s %d read_file_byte %d got %d\n",__FILE__, __LINE__,sizeof(tmpbuf)-1,got);
			goto error_out;
		}
		write(fdDst, tmpbuf, got);
		iFileSize -= got;
	}

	while (bEnd == 0)
	{
		got = read_file_byte(fdSrc, d, 1);
		if (got != 1)
		{
			fprintf(stderr, "%s %d ReqRead 1 got %d\n",__FILE__, __LINE__,got);
			goto error_out;
		}

		if (d[0] == szCRLF[0])
		{
			got = read_file_byte(fdSrc, d, 1);
			if (got != 1)
			{
				fprintf(stderr, "%s %d ReqRead 1 got %d\n",__FILE__, __LINE__,got);
				goto error_out;
			}

			if (d[0] == szCRLF[1])
			{
				// 读取起始boundary
				memset(szTitle, 0, sizeof(szTitle));
				boffset = 0;
				while (1)
				{
					got = read_file_byte(fdSrc, d, 1);
					if (got != 1)
					{
						//fprintf(stderr, "%s %d ReqRead 1 got %d\n",__FILE__, __LINE__,got);
						goto error_out;
					}

					if (d[0] == szEndBoundary[boffset])
					{
						boffset++;
						if (boffset == iEndBoundaryLength)
						{
							bEnd = 1;
							break;
						} 
					}
					else
					{		
						//fprintf(stderr, "%s %d ReqRead 1 got %c\n",__FILE__, __LINE__,d[0]);
						write(fdDst, szCRLF, 2);
						write(fdDst, szEndBoundary, boffset);
						write(fdDst, d, 1);
						break;
					}
				}
				if (bEnd)
				{
					fprintf(stderr, "%s %d Find szEndBoundary\n",__FILE__, __LINE__);
				}
			}
			else
			{
				write(fdDst, szCRLF, 1);
				write(fdDst, d, 1);
			}
		}
		else
		{
			write(fdDst, d, 1);
		}
	}

error_out:
	if (fdDst > 0)
	{
		close(fdDst);
		fdDst = -1;
	}
	
	if (fdSrc > 0)
	{
		close(fdSrc);
		fdSrc = -1;
	}

	strcpy(dstfile, temp_file_name);
	return bEnd?0:-1;
}
