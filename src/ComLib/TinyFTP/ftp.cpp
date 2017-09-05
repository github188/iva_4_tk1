#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include "include/ftp.h"
#include "oal_log.h"
#include "oal_file.h"
#ifndef WIN32
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#else
#include <io.h>
#include <winsock2.h>
#pragma comment(lib, "WS2_32")
typedef int ssize_t;
typedef int socklen_t;
#endif
#include <string>
#include <fcntl.h>
using namespace std;


#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

typedef struct _FtpConnection
{
	char user[1024];
	char pass[1024];
	struct sockaddr_in s_in;
	FILE *stream;
}FtpConnection;

static int ftpcmd(const char *s1, const char *s2, FILE *stream, char *buf)
{
	if (stream == NULL || buf == NULL)
	{
		return -1;
	}

	if(s1) 
	{
		if (s2) 
		{
			fprintf(stream, "%s%s\r\n", s1, s2);
		} 
		else 
		{
			fprintf(stream, "%s\r\n", s1);
		}
	}

	do 
	{
		if (fgets(buf, 510, stream) == NULL) 
		{
			LOG_ERROR_FMT("fgets() failed");
			return -1;   
		}

		char *buf_ptr = strstr(buf, "\r\n");
		if (buf_ptr) 
		{
			*buf_ptr = '\0';
		}
	} while (! isdigit(buf[0]) || buf[3] != ' ');

	return atoi(buf);
}

int bb_xopen(const char *pathname, int flags)
{
	if (pathname == NULL)
	{
		return -1;
	}

	int ret;

#ifdef WIN32
	ret = _open(pathname, flags, 0777);
#else
	ret = open(pathname, flags, 0777);
#endif // WIN32

	if (ret < 0) 
	{
		LOG_INFOS_FMT("open %s failed", pathname);
	}

	return ret;
}

int safe_strtoul(char *arg, unsigned long* value)
{
	char *endptr;
	int errno_save = errno;

	errno = 0;
	*value = strtoul(arg, &endptr, 0);
	if (errno != 0 || *endptr!='\0' || endptr==arg)
	{
		return 1;
	}

	errno = errno_save;
	return 0;
}

ssize_t safe_read(int fd, void *buf, size_t count)
{
	ssize_t n;

	do 
	{
#ifdef WIN32
		n = _read(fd, buf, count);
#else
		n = read(fd, buf, count);
#endif // WIN32
	} while (n < 0 && errno == EINTR);

	return n;
}

ssize_t safe_write(int fd, const void *buf, size_t count)
{
	ssize_t n;

	do 
	{
#ifdef WIN32
		n = _write(fd, buf, count);
#else
		n = write(fd, buf, count);
#endif // WIN32

	} while (n < 0 && errno == EINTR);

	return n;
}

ssize_t bb_full_write(int fd, const void *buf, size_t len)
{
	ssize_t cc;
	ssize_t total;

	total = 0;

	while (len > 0) 
	{
		cc = safe_write(fd, buf, len);

		if (cc < 0)
			return cc;		/* write() returns -1 on failure. */

		total += cc;
		buf = ((const char *)buf) + cc;
		len -= cc;
	}

	return total;
}

static ssize_t bb_full_fd_action(int src_fd, int dst_fd, size_t size)
{
	int status = -1;
	size_t total = 0;
	char buffer[BUFSIZ] = {0};

	if (src_fd < 0) goto out;
	while (!size || total < size)
	{
		ssize_t wrote, xread;

		xread = safe_read(src_fd, buffer,
			(!size || size - total > BUFSIZ) ? BUFSIZ : size - total);

		if (xread > 0) 
		{
			/* A -1 dst_fd means we need to fake it... */
			wrote = (dst_fd < 0) ? xread : bb_full_write(dst_fd, buffer, xread);
			if (wrote < xread) 
			{
				LOG_ERROR_FMT("Write Error");
				break;
			}
			total += wrote;
			if (total == size) status = 0;
		} 
		else if (xread < 0) 
		{
			LOG_ERROR_FMT("Read Error");
			break;
		} 
		else if (xread == 0) 
		{
			/* All done. */
			status = 0;
			break;
		}
	}

out:
	return status ? status : (ssize_t)total;
}


int bb_copyfd_size(int fd1, int fd2, size_t size)
{
	if (size) 
	{
		return(bb_full_fd_action(fd1, fd2, size));
	}
	return(0);
}


int bb_copyfd_eof(int fd1, int fd2)
{
	return(bb_full_fd_action(fd1, fd2, 0));
}

int bb_lookup_host(struct sockaddr_in * s_in, const char *host)
{
	struct hostent *he;

	memset(s_in, 0, sizeof(struct sockaddr_in));
	s_in->sin_family = AF_INET;
	he = gethostbyname(host);
	if (he == NULL)
		return -1;
	memcpy(&(s_in->sin_addr), he->h_addr_list[0], he->h_length);

	return 0;
}

static int connectWithTimeout(int sockfd, const struct sockaddr *serv_addr, int addrlen)
{
	int ret;
	int dwTimeOut = 10;
	fd_set ConnectSet;
	struct timeval timeVal;
	int nErrCode;
	socklen_t errorlen;

	if(sockfd < 0)
	{
		return -1;
	}

#ifdef WIN32
	u_long mode = 1;
	ret = ioctlsocket(sockfd, FIONBIO, &mode);
#else
	int mode = 1;
	ret = ioctl(sockfd, FIONBIO, (int)&mode);
#endif // WIN32

	if(ret < 0)
	{
		//close(sockfd);
		LOG_ERROR_FMT("set socket no block failed");
		return ret;
	}

	//
	ret = connect(sockfd,serv_addr,addrlen);
	if(ret == -1)
	{
		FD_ZERO(&ConnectSet);
		timeVal.tv_sec  = dwTimeOut;
		timeVal.tv_usec = 0;
		FD_SET(sockfd, &ConnectSet);
		ret = select(sockfd+1, NULL, &ConnectSet, NULL, &timeVal);
		if(ret > 0 && FD_ISSET(sockfd, &ConnectSet))
		{
			nErrCode = 1;
			errorlen = sizeof(nErrCode);
			ret = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char *)&nErrCode, &errorlen);
			if(ret == -1 || nErrCode != 0)
			{
				//close(sockfd);
				LOG_ERROR_FMT("connect error 1");
				return -1;
			}
		}
		else
		{
			//close(sockfd);
			if(ret == 0)
			{
				LOG_ERROR_FMT("connect time out");
				return -1;
			}
			else
			{
				LOG_ERROR_FMT("connect error 2");
				return -1;
			}
		}
	}

#ifdef WIN32
	mode = 0;
	ret = ioctlsocket(sockfd, FIONBIO, &mode);
#else
	mode = 0;
	ret = ioctl(sockfd, FIONBIO, (int)&mode);
#endif // WIN32

	if(ret == -1)
	{
		//close(sockfd);
		LOG_ERROR_FMT("set socket block failed");
		return ret;
	}

	return 0;
}

int xconnect(struct sockaddr_in * pAddrIn)
{
	int s = socket(AF_INET, SOCK_STREAM, 0);
	if(s < 0)
	{
		return -1;
	}

	// 设置当前socket在进行写操作时不产生SIGPIPE
#ifndef WIN32
	int set = 1;
	setsockopt(s, SOL_SOCKET, MSG_NOSIGNAL, (void*)&set, sizeof(int));
#endif // !WIN32

	//if (connect(s, (struct sockaddr *)s_addr, sizeof(struct sockaddr_in)) < 0)
	if (connectWithTimeout(s, (struct sockaddr *)pAddrIn, sizeof(struct sockaddr_in)) < 0)
	{
#ifdef WIN32
		_close(s);
#else
		close(s);
#endif
		LOG_ERROR_FMT("Unable to connect to remote host (%s,%u)",
			inet_ntoa(pAddrIn->sin_addr),ntohs(pAddrIn->sin_port));
		return -1;
	}
	return s;
}

static FILE *ftp_login(FtpConnection *server)
{
	FILE *control_stream = NULL;
	char buf[512] = {0};

	/* Connect to the command socket */
	//printf("%s,ftp connect start\n",__func__);//wangcong 613

	int s = xconnect(&server->s_in);
	if(s < 0)
	{
		LOG_ERROR_FMT("Couldnt open control socket");
		return NULL;
	}

#ifdef WIN32
	control_stream = _fdopen(s, "r+");
#else
	control_stream = fdopen(s, "r+");
#endif

	if (control_stream == NULL) 
	{
		LOG_ERROR_FMT("Couldnt open control stream");

#ifdef WIN32
		_close(s);
#else
		close(s);
#endif

		return NULL;
	}

	if (ftpcmd(NULL, NULL, control_stream, buf) != 220) 
	{
		LOG_ERROR_FMT("***%s", buf + 4);
#ifdef WIN32
		_close(_fileno(control_stream));
#else
		close(fileno(control_stream));
#endif
		fclose(control_stream);
		return NULL;
	}

	//printf("%s,ftp login start\n",__func__);
	/*  Login to the server */
	switch (ftpcmd("USER ", server->user, control_stream, buf)) 
	{
	case 230:
		{
			break;
		}
	case 331:
		{
			if (ftpcmd("PASS ", server->pass, control_stream, buf) != 230) 
			{
				LOG_ERROR_FMT("PASS error: %s", buf + 4);
#ifdef WIN32
				_close(_fileno(control_stream));
#else
				close(fileno(control_stream));
#endif
				fclose(control_stream);
				return NULL;
			}
			break;
		}
	default:
		{
			LOG_ERROR_FMT("USER error: %s", buf + 4);
#ifdef WIN32
			_close(_fileno(control_stream));
#else
			close(fileno(control_stream));
#endif
			fclose(control_stream);
			return NULL;
		}
	}

	ftpcmd("TYPE ", "I", control_stream, buf);
	return(control_stream);
}


static int xconnect_ftpdata(FtpConnection *server, char *buf)
{
	char *buf_ptr;
	unsigned short port_num;

	buf_ptr = strrchr((char *)buf, ',');
	*buf_ptr = '\0';
	port_num = atoi(buf_ptr + 1);

	buf_ptr = strrchr((char *)buf, ',');
	*buf_ptr = '\0';
	port_num += atoi(buf_ptr + 1) * 256;

	server->s_in.sin_port = htons(port_num);
	return(xconnect(&server->s_in));
}


static int ftp_send_t(FtpConnection *server,  FILE *control_stream
					 , const char *server_path, const char *local_path)
{
	char buf[512];
	int fd_data;
	int fd_local;
	int response;

	/*服务器上文件是否已经存在*/
	long size = 0;
	char temp_path[512] = {0};
	sprintf(temp_path, "%s", server_path);
	if(ftpcmd("SIZE ", temp_path, control_stream, buf) == 213)
	{
		size = atol(buf + 4);
	}

	// PASV 被动模式 获取数据端口
	if (ftpcmd("PASV", NULL, control_stream, buf) != 227)
	{
		LOG_ERROR_FMT("PASV error: %s", buf + 4);
		return FTP_ERROR_SERVER;
	}

	/*  Connect to the data socket */
	fd_data = xconnect_ftpdata(server, buf);
	if(fd_data < 0)
	{
		return FTP_ERROR_SERVER;
	}

	/* get the local file */
	if ((local_path[0] == '-') && (local_path[1] == '\0'))
	{
		fd_local = STDIN_FILENO;
	}
	else
	{
		fd_local = bb_xopen(local_path, O_RDONLY);
		if(fd_local < 0)
		{
			LOG_ERROR_FMT("ftp send error:file is not exist\n");
#ifdef WIN32
			_close(fd_local);
			_close(fd_data);
#else
			close(fd_local);
			close(fd_data);
#endif

			return FTP_ERROR_FILE;
		}
		
		long lfilesize =  OAL_FileSize(local_path);
		sprintf(buf, "%ld",lfilesize);
		response = ftpcmd("ALLO ", buf, control_stream, buf);
		switch (response)
		{
		case 200:
		case 202:
			break;
		default:
			LOG_ERROR_FMT("ALLO error: %s", buf + 4);
			break;
		}
	}

#ifdef WIN32
	_lseek(fd_local, size, SEEK_SET);
#else
	lseek(fd_local, size, SEEK_SET);
#endif

	if(size == 0)
	{
		response = ftpcmd("STOR ", temp_path, control_stream, buf);
		switch (response)
		{
		case 125:
		case 150:
			break;
		default:
#ifdef WIN32
			_close(fd_local);
			_close(fd_data);
#else
			close(fd_local);
			close(fd_data);
#endif
			LOG_ERROR_FMT("STOR error: %s", buf + 4);
			return FTP_ERROR_SERVER;
		}
	}
	else
	{
		response = ftpcmd("APPE ", temp_path, control_stream, buf);
		switch (response)
		{
		case 125:
		case 150:
			break;
		default:
#ifdef WIN32
			_close(fd_local);
			_close(fd_data);
#else
			close(fd_local);
			close(fd_data);
#endif
			LOG_ERROR_FMT("STOR error: %s", buf + 4);
			return FTP_ERROR_SERVER;
		}
	}

	/* transfer the file  */
	if (bb_copyfd_eof(fd_local, fd_data) == -1)
	{
#ifdef WIN32
		_close(fd_local);
		_close(fd_data);
#else
		close(fd_local);
		close(fd_data);
#endif
		return FTP_ERROR_SERVER;
	}

	/* close it all down */
#ifdef WIN32
	_close(fd_local);
	_close(fd_data);
#else
	close(fd_local);
	close(fd_data);
#endif

	if (ftpcmd(NULL, NULL, control_stream, buf) != 226)
	{
		LOG_ERROR_FMT("error: %s", buf + 4);
		return FTP_ERROR_SERVER;
	}

	return FTP_SUCCESS;
}


int ftp_dir(FILE *control_stream,const char* dir)
{

	char buf[512];

	if (ftpcmd("CWD ", dir, control_stream, buf) != 250)
	{
		if(ftpcmd("MKD ",dir, control_stream, buf) != 257)
		{
#ifdef WIN32
			_close(_fileno(control_stream));
#else
			close(fileno(control_stream));
#endif
			fclose(control_stream);
			return FTP_ERROR_SERVER;
		}
		else
		{
			//再次进入该目录
			if (ftpcmd("CWD ", dir, control_stream, buf) != 250)
			{
#ifdef WIN32
				_close(_fileno(control_stream));
#else
				close(fileno(control_stream));
#endif
				fclose(control_stream);
				return FTP_ERROR_SERVER;
			}
		}
	}
	return FTP_SUCCESS;

}

int ftp_mkdir_by_path(FILE *control_stream,const char *ps8Path)
{
	if(ps8Path == NULL || control_stream == NULL )
	{
		return -1;
	}
	//printf("ftp_mkdir_by_path: [%s]\n", ps8Path);

	string strPath = ps8Path;
	string::size_type pos = strPath.find("/");
	if(pos == string::npos)
	{
		string thisPath = strPath;
		//printf("ftp_dir: [%s]\n", thisPath.c_str());
		if(ftp_dir(control_stream,thisPath.c_str()) != FTP_SUCCESS)
		{
			return FTP_ERROR_SERVER;
		}
	}
	else
	{
		string thisPath = strPath.substr(0, pos);
		//printf("ftp_dir: [%s]\n", thisPath.c_str());
		if(ftp_dir(control_stream,thisPath.c_str()) != FTP_SUCCESS)
		{
			return FTP_ERROR_SERVER;
		}

		string pubPath = strPath.substr(pos+1);
		int ret = ftp_mkdir_by_path(control_stream,pubPath.c_str());
		if(ret != FTP_SUCCESS)
		{
			return ret;
		}
	}

	return FTP_SUCCESS;
}

static int ftpSendInner(FtpConnection *server,  FILE *control_stream
						,const char *server_path, const char *local_path)
{
	//struct stat sbuf;
	char buf[512];
	int fd_data;
	int fd_local;
	int response;
	long size = 0;
	char temp_path[512] = {0};

	sprintf(temp_path, "%s", server_path);

	if(ftpcmd("SIZE ", temp_path, control_stream, buf) == 213)
	{
		size = atol(buf+4);
	}

	/*  Connect to the data socket */
	if (ftpcmd("PASV", NULL, control_stream, buf) != 227)
	{
		LOG_ERROR_FMT("PASV error: %s", buf + 4);
		return FTP_ERROR_SERVER;
	}

	fd_data = xconnect_ftpdata(server, buf);
	if(fd_data < 0)
	{
		return FTP_ERROR_SERVER;
	}

	/* get the local file */
	if ((local_path[0] == '-') && (local_path[1] == '\0'))
	{
		fd_local = STDIN_FILENO;
	}
	else
	{
		fd_local = bb_xopen(local_path, O_RDONLY);
		if(fd_local < 0)
		{
			printf("ftp send error:file is not exist\n");
#ifdef WIN32
			_close(fd_local);
			_close(fd_data);
#else
			close(fd_local);
			close(fd_data);
#endif

			return FTP_ERROR_FILE;
		}

		long lfilesize =  OAL_FileSize(local_path);
		sprintf(buf, "%ld",lfilesize);
		response = ftpcmd("ALLO ", buf, control_stream, buf);
		switch (response)
		{
		case 200:
		case 202:
			break;
		default:
			LOG_ERROR_FMT("ALLO error: %s", buf + 4);
			break;
		}
	}
#ifdef WIN32
	_lseek(fd_local,size,SEEK_SET);
#else
	lseek(fd_local,size,SEEK_SET);
#endif

	if(size == 0)
	{
		response = ftpcmd("STOR ", temp_path, control_stream, buf);
		switch (response)
		{
		case 125:
		case 150:
			break;
		default:
#ifdef WIN32
			_close(fd_local);
			_close(fd_data);
#else
			close(fd_local);
			close(fd_data);
#endif
			LOG_ERROR_FMT("STOR error: %s", buf + 4);
			return FTP_ERROR_SERVER;
		}
	}
	else
	{
		response = ftpcmd("APPE ", temp_path, control_stream, buf);
		switch (response)
		{
		case 125:
		case 150:
			break;
		default:
#ifdef WIN32
			_close(fd_local);
			_close(fd_data);
#else
			close(fd_local);
			close(fd_data);
#endif
			LOG_ERROR_FMT("STOR error: %s", buf + 4);
			return FTP_ERROR_SERVER;
		}
	}

	/* transfer the file  */
	if (bb_copyfd_eof(fd_local, fd_data) == -1)
	{
#ifdef WIN32
		_close(fd_local);
		_close(fd_data);
#else
		close(fd_local);
		close(fd_data);
#endif
		return FTP_ERROR_FILE;
	}

	/* close it all down */
#ifdef WIN32
	_close(fd_local);
	_close(fd_data);
#else
	close(fd_local);
	close(fd_data);
#endif

	if (ftpcmd(NULL, NULL, control_stream, buf) != 226)
	{
		LOG_ERROR_FMT("error: %s", buf + 4);
		return FTP_ERROR_SERVER;
	}

	return FTP_SUCCESS;
}


FTP_API int FTP_UploadFileEntire( const char *addr, int port, const char *user, const char *pass, const char *dst_dir, const char *dst_file, const char *src_file )
{
	char buf[512];

	/* socket to ftp server */
	FILE *control_stream;

	/* continue a prev transfer (-c) */
	FtpConnection host_info;

	/* Set default values */
	strcpy(host_info.user,user);
	strcpy(host_info.pass,pass);

	/* We want to do exactly _one_ DNS lookup, since some
	* sites (i.e. ftp.us.debian.org) use round-robin DNS
	* and we want to connect to only one IP... */
	bb_lookup_host(&host_info.s_in, addr);
	host_info.s_in.sin_port = htons(port);

	printf("Connecting to %s[%s]:%d\n", addr
		,inet_ntoa(host_info.s_in.sin_addr), ntohs(host_info.s_in.sin_port));


	/*  Connect/Setup/Configure the FTP session */
	control_stream = ftp_login(&host_info);
	if(control_stream == NULL)
	{
		return FTP_ERROR_SERVER;
	}

	if(dst_dir && strlen(dst_dir) > 0)
	{
		if(ftp_mkdir_by_path(control_stream,dst_dir) != FTP_SUCCESS)
			return FTP_ERROR_SERVER;
	}

	int ret;
	ret = ftp_send_t(&host_info, control_stream, dst_file, src_file);
	if(ret == FTP_SUCCESS)
	{

	}
	else if(ret == FTP_ERROR_FILE)
	{

	}
	else
	{

	}

	ftpcmd("QUIT", NULL, control_stream, buf);

#ifdef WIN32
	_close(_fileno(control_stream));
#else
	close(fileno(control_stream));
#endif
	fclose(control_stream);
	return ret;
}

FTP_API FtpHanlde FTP_Open( const char *addr, int port, const char *user, const char *pass )
{
	if (addr == NULL || user == NULL || pass == NULL)
	{
		return NULL;
	}

	LOG_DEBUG_FMT("ftp open [%s:%d] user [%s]...", addr, port, user);
	FtpConnection *ftpInfo = (FtpConnection *)malloc(sizeof(FtpConnection));
	strcpy(ftpInfo->user,user);
	strcpy(ftpInfo->pass,pass);


	bb_lookup_host(&ftpInfo->s_in, addr);
	ftpInfo->s_in.sin_port = htons(port);

	ftpInfo->stream = ftp_login(ftpInfo);
	if (NULL == ftpInfo->stream)
	{
		printf("failed.\n");
		free(ftpInfo);
		return NULL;
	}
	printf("ok.\n");
	return (FtpHanlde)ftpInfo;
}

FTP_API int FTP_UploadFile( FtpHanlde tHandle, const char *dst_dir, const char *dst_file, const char *src_file )
{
	FtpConnection *ftpInfo = (FtpConnection*)tHandle;

	if(dst_dir && strlen(dst_dir) > 0)
	{
		if(ftp_mkdir_by_path(ftpInfo->stream,dst_dir) != FTP_SUCCESS)
			return FTP_ERROR_SERVER;
	}

	int ret = ftpSendInner(ftpInfo, ftpInfo->stream, dst_file, src_file);
	printf("ftp send [%s] to [%s], ret %d.\n", src_file, dst_file, ret);
	return ret;
}

FTP_API int FTP_SendCmd( FtpHanlde tHandle, const char *cmd, const char *params )
{
	FtpConnection *ftpInfo = (FtpConnection*)tHandle;
	char buf[512];
	int ret = ftpcmd(cmd, params, ftpInfo->stream, buf);
	printf("[%s%s] return [%s].\n", cmd, params, buf);
	return ret;
}

FTP_API int Ftp_Close( FtpHanlde tHandle )
{
	FtpConnection *ftpInfo = (FtpConnection*)tHandle;
#ifdef WIN32
	_close(_fileno(ftpInfo->stream));
#else
	close(fileno(ftpInfo->stream));
#endif
	fclose(ftpInfo->stream);
	free(ftpInfo);
	return 1;
}

