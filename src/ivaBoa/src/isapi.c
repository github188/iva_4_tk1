#include "isapi.h"
#ifdef USE_ISAPI

#include "mq_master.h"
#include "isapi_formdata.h"

const char * GetISAPIMethod(int method)
{
	if(method == ISAPI_GET)
	{
		return "GET";
	}
	else if(method == ISAPI_PUT)
	{
		return "PUT";
	}
	else if(method == ISAPI_POST)
	{
		return "POST";
	}
	else if(method == ISAPI_DELETE)
	{
		return "DELETE";
	}
	else
	{
		return "UNKNOWN";
	}
}

static int ISAPI_DO(request *req)
{	
	ISAPI_REQ tReq;
	char req_json[8*1024] = {0};
	tReq.method = req->method;
	strcpy(tReq.szURI, req->request_uri);
	strcpy(tReq.szAuth, req->Authorization?req->Authorization:"");
	strcpy(tReq.szRemoteIP, req->remote_ip_addr);
	strcpy(tReq.szQuery, req->query_string?req->query_string:"");

	if (req->req_json)
	{
		int content_length = boa_atoi(req->content_length);
		strcpy(req_json, req->req_json);
		req_json[content_length] = '\0';
	}

	fprintf(stderr, "%s %d method:%s\n",__FILE__, __LINE__,GetISAPIMethod(tReq.method));
	fprintf(stderr, "%s %d request_uri:%s\n",__FILE__, __LINE__,tReq.szURI);
	fprintf(stderr, "%s %d Authorization:%s\n",__FILE__, __LINE__,tReq.szAuth);
	fprintf(stderr, "%s %d remote_ip_addr:%s\n",__FILE__, __LINE__,tReq.szRemoteIP);
	fprintf(stderr, "%s %d req_json:%s\n",__FILE__, __LINE__,req_json);

	ISAPI_RESP tResp = {0};
	char resp_json[8*1024] = {0};

	int ret = MQ_Master_ISAPI_Request(&tReq, req_json, &tResp, resp_json);
	if(ret != 0)
	{
		fprintf(stderr, "%s %d Ret = %d, send_r_error\n",__FILE__, __LINE__, ret);
		send_r_error(req);
	}
	else
	{
		fprintf(stderr, "%s %d code = %d\n",__FILE__, __LINE__, tResp.code);
		if(tResp.code == 200)
		{
			send_r_isapi_json(req,resp_json);
		}
		else if(tResp.code == 400)
		{
			send_r_bad_request(req);
		}
		else if(tResp.code == 401)
		{
			send_r_unauthorized(req, "ISAPI");
		}
		else if(tResp.code == 403)
		{
			send_r_forbidden(req);
		}
		else
		{
			send_r_error(req);
		}
	}

	return 1;
}

int isapi_read_body(request * req)
{
	int bytes_read, bytes_to_read, bytes_free;

	bytes_free = BUFFER_SIZE - (req->header_end - req->header_line);
	bytes_to_read = req->filesize - req->filepos;

	if (bytes_to_read > bytes_free)
		bytes_to_read = bytes_free;

	if (bytes_to_read <= 0) {
		req->status = BODY_WRITE; /* go write it */
		return 1;
	}

	bytes_read = read(req->fd, req->header_end, bytes_to_read);
	//fprintf(stderr, "bytes_to_read %d  to read %d bytes!\n",bytes_to_read,bytes_read);

	if (bytes_read == -1) {
		//fprintf(stderr, "%s %d errno:%d\n",__FILE__, __LINE__,errno);

		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			req->status = BODY_WRITE;
			return 1;
			// return -1;
		} else {
			boa_perror(req, "read body");
			return 0;
		}
	} else if (bytes_read == 0) {
		/* this is an error.  premature end of body! */
		log_error_time();
		fprintf(stderr, "%s:%d - Premature end of body!!\n", __FILE__, __LINE__);
		return 0;
	}

	req->status = BODY_WRITE;

	req->header_end += bytes_read;

	return 1;
}

int isapi_write_body(request * req)
{
	int bytes_written, bytes_to_write = req->header_end - req->header_line;
	if (req->filepos + bytes_to_write > req->filesize)
		bytes_to_write = req->filesize - req->filepos;

	if (bytes_to_write == 0)
	{
		req->header_line = req->header_end = req->buffer;
		if (req->filepos >= req->filesize)
		{
			//fprintf(stderr, "req->filepos = %lu req->filesize = %lu!\n",req->filepos,req->filesize);
			return 0;
		}
		/* if here, we can safely assume that there is more to read */
		req->status = BODY_READ;
		return 1;
	}

	bytes_written = write(req->post_data_fd,req->header_line, bytes_to_write);
	//fprintf(stderr, "bytes_to_write %d  to write %d bytes!\n",bytes_to_write,bytes_written);

	if (bytes_written == -1) {
		//fprintf(stderr, "%s %d errno:%d\n",__FILE__, __LINE__,errno);

		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return -1;          /* request blocked at the pipe level, but keep going */
		else if (errno == EINTR)
			return 1;
		else if (errno == ENOSPC) {
			boa_perror(req, "write body"); /* OK to disable if your logs get too big */
			return 0;
		} else {
			boa_perror(req, "write body"); /* OK to disable if your logs get too big */
			return 0;
		}
	}

	req->filepos += bytes_written;
	req->header_line += bytes_written;

	return 1;                   /* more to do */
}

int process_isapi_post_file(request *req)
{
	if (req->content_length == NULL)
	{
		log_error_time();
		fprintf(stderr, "Unknown Content-Length POST!\n");
		send_r_bad_request(req);
		return 0;
	}

	int content_length = boa_atoi(req->content_length);
	if (content_length <= 0)
	{
		log_error_time();
		fprintf(stderr, "Invalid Content-Length [%s] on POST!\n",req->content_length);
		send_r_bad_request(req);
		return 0;
	}

	// 读取文件到临时文件
	char temp_file_name[1024] = {0};
	req->post_data_fd = create_temporary_file(0, temp_file_name, sizeof(temp_file_name));
	if (req->post_data_fd == 0)
	{
		log_error_time();
		fprintf(stderr, "create_temporary_file failed!\n");
		send_r_error(req);
		return(0);
	}

	req->header_line = req->client_stream + req->parse_pos;
	req->header_end = req->client_stream + req->client_stream_pos;
	req->status = BODY_WRITE;
	//fprintf(stderr, "%s %d content_length:%s\n",__FILE__, __LINE__,req->content_length?req->content_length:"");
	//fprintf(stderr, "%s %d content_type:%s\n",__FILE__, __LINE__,req->content_type?req->content_type:"");

	if (single_post_limit && content_length > single_post_limit)
	{
		log_error_time();
		fprintf(stderr, "Content-Length [%d] > SinglePostLimit [%d] on POST!\n",
			content_length, single_post_limit);
		send_r_bad_request(req);
		unlink(temp_file_name);
		return 0;
	}

	req->filesize = content_length;
	req->filepos  = 0;
	if (req->header_end - req->header_line > req->filesize)
	{
		req->header_end = req->header_line + req->filesize;
	}

	int iret = 1;
	int iRate = 0;
	fprintf(stderr, "%s %d read post file: %lu bytes, %%%02d",__FILE__, __LINE__,
		req->filesize, iRate);
	while((req->status == BODY_WRITE || req->status == BODY_READ) && iret ==1)
	{
		if (req->status == BODY_WRITE)
		{
			//fprintf(stderr, "BODY_WRITE: req->filesize:%lu, req->filepos:%lu\n\n",req->filesize, req->filepos);
			iret = isapi_write_body(req);
			int iiiR = (int)(((float)(req->filepos)*1.0/(float)(req->filesize))*100);
			if (iiiR != iRate)
			{
				iRate = iiiR;
				fprintf(stderr, "\b\b%02d",iRate);
			}
		}
		else
		{
			//fprintf(stderr, "BODY_READ: req->filesize:%lu, req->filepos:%lu\n\n",req->filesize, req->filepos);
			iret = isapi_read_body(req);
		}
		//fprintf(stderr, "iret:%d\n\n",iret);
	}
	fprintf(stderr, "\n");

	close(req->post_data_fd);
	req->post_data_fd = 0;

	if(iret != 0)
	{
		fprintf(stderr, "%s %d save_file = %d, send_r_error\n",__FILE__, __LINE__, iret);
		send_r_error(req);
		unlink(temp_file_name);
		return 0;
	}

	if (isapi_post_is_form_data(req))
	{
		char temp_file_name2[1024] = {0};
		iret = save_isapi_post_form_data(req, temp_file_name, temp_file_name2);
		if (iret == 0)
		{
			unlink(temp_file_name);
			strcpy(temp_file_name,temp_file_name2);
		}
		else
		{
			fprintf(stderr, "%s %d save_isapi_post_form_data = %d, send_r_error\n",__FILE__, __LINE__, iret);
			send_r_error(req);
			unlink(temp_file_name);
			return 0;
		}
	}

	// 验证合法性
	ISAPI_REQ tReq;
	char req_json[1024] = {0};
	sprintf(req_json, "{\"filepath\":\"%s\"}",temp_file_name);
	tReq.method = req->method;
	strcpy(tReq.szURI, req->request_uri);
	strcpy(tReq.szAuth, req->Authorization?req->Authorization:"");
	strcpy(tReq.szRemoteIP, req->remote_ip_addr);
	strcpy(tReq.szQuery, req->query_string?req->query_string:"");

	fprintf(stderr, "%s %d method:%d\n",__FILE__, __LINE__,tReq.method);
	fprintf(stderr, "%s %d request_uri:%s\n",__FILE__, __LINE__,tReq.szURI);
	fprintf(stderr, "%s %d Authorization:%s\n",__FILE__, __LINE__,tReq.szAuth);
	fprintf(stderr, "%s %d remote_ip_addr:%s\n",__FILE__, __LINE__,tReq.szRemoteIP);
	fprintf(stderr, "%s %d req_json:%s\n",__FILE__, __LINE__,req_json);

	ISAPI_RESP tResp = {0};
	char resp_json[8*1024] = {0};
	iret = MQ_Master_ISAPI_Request(&tReq, req_json, &tResp, resp_json);

	unlink(temp_file_name);

	if(iret != 0)
	{
		fprintf(stderr, "%s %d Ret = %d, send_r_error\n",__FILE__, __LINE__, iret);
		send_r_error(req);
		return 0;
	}
	else
	{
		fprintf(stderr, "%s %d code = %d\n",__FILE__, __LINE__, tResp.code);
		if(tResp.code != 200)
		{
			if(tResp.code == 400)
			{
				send_r_bad_request(req);
			}
			else if(tResp.code == 401)
			{
				send_r_unauthorized(req, "ISAPI");
			}
			else if(tResp.code == 403)
			{
				send_r_forbidden(req);
			}
			else
			{
				send_r_error(req);
			}
			return 0;
		}
	}

	send_r_isapi_json(req,resp_json);
	return 0;
}

int init_isapi(request * req)
{
	//fprintf(stderr, "%s %d content_length:%s\n",__FILE__, __LINE__,req->content_length?req->content_length:"NULL");
	//fprintf(stderr, "%s %d client_stream:%s\n",__FILE__, __LINE__,req->client_stream?req->client_stream:"NULL");

	if (req->method == M_POST && (
		strcmp("/ISAPI/System/Maintain/Profile", req->request_uri) == 0||
		strcmp("/ISAPI/System/Maintain/Upgrade", req->request_uri) == 0))
	{
		process_isapi_post_file(req);
		req->parse_pos = req->client_stream_pos;// 全部解析
		return 0;
	}

	// JSON操作
	int content_length = 0;
	if (req->content_length) {
		content_length = boa_atoi(req->content_length);
	}

	if(content_length > 0)
	{
		req->req_json = req->client_stream + req->parse_pos;
		//fprintf(stderr, "req_json:%s\n",req->req_json);

		// 已经接收的字节数
		int body_read_bytes = req->client_stream_pos - req->parse_pos;
		if(body_read_bytes >= content_length)
		{
			// OK 接收完了
			fprintf(stderr, "req_json:OK\n");
		}
		else
		{
			int bytes_free = CLIENT_STREAM_SIZE - req->client_stream_pos;
			int bytes_to_read = content_length - body_read_bytes;

			if (bytes_to_read > bytes_free)
				bytes_to_read = bytes_free;

			// 读取剩余body
			while(body_read_bytes < content_length)
			{
				char *buffer = req->client_stream + req->parse_pos+ body_read_bytes;
				int bytes_read	= read(req->fd, buffer, bytes_to_read);
				if (bytes_read == -1) {
					if (errno == EWOULDBLOCK || errno == EAGAIN) {
						usleep(1000);
						continue;
					} else {
						boa_perror(req, "read body");
						return 0;
					}
				} else if (bytes_read == 0) {
					send_r_bad_request(req);
					return 0;
				}

				if(bytes_read > 0)
				{
					body_read_bytes += bytes_read;
					req->client_stream_pos+=bytes_read;
				}
			}				
		}

		//fprintf(stderr, "req_json:%s\n",req->req_json);
		req->parse_pos = req->client_stream_pos;// 全部解析
	}
	else
	{
		//fprintf(stderr, "req_json:=NULL\n");
		req->req_json = NULL;
	}

	ISAPI_DO(req);

	return 0;
	//return(1); /* success */
}
#endif


