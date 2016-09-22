//  Copyright (C) 2016 Amendux
//  All Rights Reserved.
//
//  UNIT: FTPClient.cpp 
//
//  PURPOSE: FTP core client
//

#include "stdafx.h"
#include "Log.h"
#include "Config.h"
#include "FTPClient.h"

#include <winsock.h>

#include <errno.h>
#include <ctype.h>
#include <sys/types.h>

using namespace Amendux;

/* socket values */
#define DEFAULT_BUFSIZ 1024

/* IO types */
#define CONN_CONTROL 0
#define CONN_READ 1
#define CONN_WRITE 2

/*
 * Constructor
 */
FTPClient::FTPClient()
{
	mp_ftphandle = new FTPHandle;
	if (!mp_ftphandle) {
		Log::Error(L"FTPClient", L"Allocation error");
	}

	::memset(mp_ftphandle, '\0', sizeof(FTPHandle));

	mp_ftphandle->buf = new char[DEFAULT_BUFSIZ];
	if (!mp_ftphandle->buf) {
		Log::Error(L"FTPClient", L"Allocation error");
		delete mp_ftphandle;
	}

	ClearHandle();
}

/*
 * Destructor
 */
FTPClient::~FTPClient()
{
	Quit();
	delete[] mp_ftphandle->buf;
	delete mp_ftphandle;
}

/*
* read a line of text
*
* return -1 on error or bytecount
*/
int FTPClient::readline(char *buf, int max, FTPHandle *ctl)
{
	int retval = 0;

	if ((ctl->dir != CONN_CONTROL) && (ctl->dir != CONN_READ)) {
		return -1;
	}
	
	if (max == 0) {
		return 0;
	}

	ctl->sock->setBlocking(false);

read_retry:
	int got = 0;
	while (got < max) {
		retval = ctl->sock->recv(buf + got, max - got);
		if (retval == 0) {
			break;
		} else if (retval < 1) {
			if (WSAGetLastError() == WSAEWOULDBLOCK) {
				if (got == 0) {
					Sleep(100);
					goto read_retry;
				}
				break;
			}

			Log::Error(L"FTPClient", L"Socket error");
			return 0;
		}
		got += retval;
	}

	return retval;
}

/*
* read a response from the server
*
* return 0 if first char doesn't match
* return 1 if first char matches
*/
int FTPClient::readresp(char c, FTPHandle *nControl)
{
	if (readline(nControl->response, 512, nControl) == -1) {
		perror("Control socket read failed");
		return 0;
	}

	if (nControl->response[0] == c)
		return 1;

	return 0;
}

/*
* FtpLastResponse - return a pointer to the last response received
*/
char *FTPClient::LastResponse()
{
	if ((mp_ftphandle) && (mp_ftphandle->dir == CONN_CONTROL)) {
		return mp_ftphandle->response;
	}

	return nullptr;
}

/*
* ftplib::Connect - connect to remote server
*
* return 1 if connected, 0 if not
*/
int FTPClient::Connect(const char *host) // std:string
{
	if (SimpleSocks::initSimpleSocks()) {
		Log::Error(L"FTPClient", L"Failed to initialize Simple Socks");
		return 0;
	}

	mp_ftphandle->sock = new SimpleSocks::TCPSocket;
	if (mp_ftphandle->sock->connect(host, 21)) {
		Log::Error(L"FTPClient", L"Failed to connect to server");
		mp_ftphandle->sock = nullptr;
	}

	mp_ftphandle->dir = CONN_CONTROL;
	mp_ftphandle->ctrl = NULL;
	mp_ftphandle->xfered = 0;
	mp_ftphandle->xfered1 = 0;
	mp_ftphandle->offset = 0;

	if (readresp('2', mp_ftphandle) == 0) {
		return 0;
	}

	return 1;
}

/*
* FtpSendCmd - send a command and wait for expected response
*
* return 1 if proper response received, 0 otherwise
*/
int FTPClient::FtpSendCmd(const char *cmd, char expresp, FTPHandle *nControl)
{
	char buf[256];

	if (nControl->dir != CONN_CONTROL) {
		return 0;
	}

	strcpy_s(buf, 256, cmd);
	strcat_s(buf, 256, "\r\n");

	int retval = nControl->sock->send(buf, (int)strlen(buf));
	if (retval < 1) {
		Log::Error(L"FTPClient", L"Connection was lost");
		return 0;
	}

	return readresp(expresp, nControl);
}

/*
* FtpLogin - log in to remote server
*
* return 1 if logged in, 0 otherwise
*/
int FTPClient::Login(const char *user, const char *pass)
{
	char tempbuf[64];

	if (((strlen(user) + 7) > sizeof(tempbuf)) || ((strlen(pass) + 7) > sizeof(tempbuf))) {
		return 0;
	}

	strcpy_s(tempbuf, 64, "USER ");
	strcat_s(tempbuf, 64, user);
	if (!FtpSendCmd(tempbuf, '3', mp_ftphandle)) {
		return 1;
	}

	strcpy_s(tempbuf, 64, "PASS ");
	strcat_s(tempbuf, 64, pass);
	if (!FtpSendCmd(tempbuf, '2', mp_ftphandle)) {
		return 1;
	}

	isAuth = true;

	return 0;
}

/*
* FtpAccess - return a handle for a data stream
*
* return 1 if successful, 0 otherwise
*/
int FTPClient::FtpDataOpen(const char *path, accesstype type, transfermode mode, FTPHandle *nControl, FTPHandle **nData)
{
	char *buf = new char[256];
	int dir;

	::memset(buf, '\0', 256);

	if ((!path) && ((type == FTPClient::filewrite) || (type == FTPClient::fileread) || (type == FTPClient::filereadappend) || (type == FTPClient::filewriteappend))) {
		sprintf_s(nControl->response, 256, "Missing path argument for file transfer\n");
		return 0;
	}

	sprintf_s(buf, 256, "TYPE %c", mode);
	if (!FtpSendCmd(buf, '2', nControl)) {
		return 0;
	}

	switch (type) {
		case FTPClient::dir:
			strcpy_s(buf, 256, "NLST");
			dir = CONN_READ;
			break;
		case FTPClient::dirverbose:
			strcpy_s(buf, 256, "LIST -aL");
			dir = CONN_READ;
			break;
		case FTPClient::filereadappend:
		case FTPClient::fileread:
			strcpy_s(buf, 256, "RETR");
			dir = CONN_READ;
			break;
		case FTPClient::filewriteappend:
		case FTPClient::filewrite:
			strcpy_s(buf, 256, "STOR");
			dir = CONN_WRITE;
			break;
		default:
			sprintf_s(nControl->response,256, "Invalid open type %d\n", type);
			return 0;
	}

	if (path) {
		size_t i = strlen(buf);
		if ((strlen(path) + i) >= 256) {
			return 0;
		}
		strcat_s(buf, 256, " ");
		strcat_s(buf, 256, path);
	}

	if (nControl->cmode == FTPClient::pasv) {
		if (FtpOpenPasv(nControl, nData, mode, dir, buf) == -1) {
			return 0;
		}
	}

	return 1;
}

/*
* FtpOpenPasv - Establishes a PASV connection for data transfer
*
* return 1 if successful, -1 otherwise
*/
int FTPClient::FtpOpenPasv(FTPHandle *nControl, FTPHandle **nData, transfermode mode, int dir, char *cmd)
{
	if (nControl->dir != CONN_CONTROL) {
		return -1;
	}

	if ((dir != CONN_READ) && (dir != CONN_WRITE)) {
		sprintf_s(nControl->response, 256, "Invalid direction %d\n", dir);
		return -1;
	}

	if ((mode != FTPClient::TEXT) && (mode != FTPClient::BIN)) {
		sprintf_s(nControl->response, 256, "Invalid mode %c\n", mode);
		return -1;
	}

	if (!FtpSendCmd("PASV", '2', nControl)) {
		return -1;
	}
	char *cp = strchr(nControl->response, '(');
	if (!cp) {
		return -1;
	}
	cp++;

	unsigned char v[6];
	unsigned int v_i[6];
	sscanf_s(cp, "%u,%u,%u,%u,%u,%u", &v_i[2], &v_i[3], &v_i[4], &v_i[5], &v_i[0], &v_i[1]);
	for (int i = 0; i < 6; i++) {
		v[i] = (unsigned char)v_i[i];
	}

	unsigned short port = (v[0] * 256) + v[1];

	FTPHandle *ctrl = new FTPHandle;
	if (!ctrl) {
		return -1;
	}

	::memset(ctrl, '\0', sizeof(FTPHandle));

	char address[32];
	::memset(address, '\0', 32);
	sprintf_s(address, 32, "%u.%u.%u.%u", v[2], v[3], v[4], v[5]);

	ctrl->sock = new SimpleSocks::TCPSocket;
	if (ctrl->sock->connect(address, port)) {
		Log::Error(L"FTPClient", L"Failed to connect to server");
		ctrl->sock = nullptr;
	}

	if (mp_ftphandle->offset != 0) {
		char buf[256];
		sprintf_s(buf, 256, "REST %lld", mp_ftphandle->offset);
		if (!FtpSendCmd(buf, '3', nControl)) {
			return 0;
		}
	}

	strcat_s(cmd, 256, "\r\n");

	int retval = nControl->sock->send(cmd, (int)strlen(cmd));
	if (retval < 1) {
		Log::Error(L"FTPClient", L"Connection was lost");
		return 0;
	}

	if (!readresp('1', nControl)) {
		return -1;
	}

	if ((mode == 'A') && ((ctrl->buf = new char[DEFAULT_BUFSIZ]) == NULL)) {
		Log::Error(L"FTPClient", L"Allocation error");
		delete ctrl;
		return -1;
	}

	ctrl->dir = dir;
	ctrl->ctrl = (nControl->cmode == FTPClient::pasv) ? nControl : NULL;
	ctrl->xfered = 0;
	ctrl->xfered1 = 0;
	ctrl->cbbytes = nControl->cbbytes;

	*nData = ctrl;
	return 1;
}

/*
* FtpClose - close a data connection
*/
int FTPClient::FtpDataClose(FTPHandle *nData)
{
	FTPHandle *ctrl;

	if (nData->dir == CONN_WRITE) {
		if (nData->buf) {
			int retval = nData->sock->send(nData->buf, (int)strlen(nData->buf));
			if (retval < 1) {
				Log::Error(L"FTPClient", L"Connection was lost");
				return 0;
			}
		}
	} else if (nData->dir != CONN_READ) {
		return 0;
	}

	if (nData->buf) {
		delete nData->buf;
	}

	nData->sock->close();
	delete nData->sock;
	nData->sock = nullptr;

	ctrl = nData->ctrl;
	delete nData;

	return 1;
}

/*
* FtpRead - read from a data connection
*/
int FTPClient::FtpDataRead(void *buf, int max, FTPHandle *nData)
{
	if (nData->dir != CONN_READ) {
		return 0;
	}

	int retval = readline(static_cast<char*>(buf), max, nData);
	if (retval < 1) {
		Log::Error(L"FTPClient", L"Connection was lost");
		return 0;
	}

	nData->xfered += retval;
	return retval;
}

/*
* FtpWrite - write to a data connection
*/
int FTPClient::FtpDataWrite(void *buf, int len, FTPHandle *nData)
{
	if (nData->dir != CONN_WRITE) {
		return 0;
	}

	int retval = nData->sock->send((char *)buf, len);
	if (retval < 1) {
		Log::Error(L"FTPClient", L"Connection was lost");
		return 0;
	}

	nData->xfered += retval;
	return retval;
}

/*
* FtpSite - send a SITE command
*
* return 1 if command successful, 0 otherwise
*/
int FTPClient::Site(const char *cmd)
{
	char buf[256];

	if ((strlen(cmd) + 7) > sizeof(buf)) {
		return 0;
	}

	sprintf_s(buf, 256, "SITE %s", cmd);
	if (!FtpSendCmd(buf, '2', mp_ftphandle)) {
		return 0;
	}

	return 1;
}

/*
* FtpRaw - send a raw string string
*
* return 1 if command successful, 0 otherwise
*/

int FTPClient::Raw(const char *cmd)
{
	char buf[256];
	strncpy_s(buf, 256, cmd, 256);
	if (!FtpSendCmd(buf, '2', mp_ftphandle)) return 0;
	return 1;
}

/*
* FtpSysType - send a SYST command
*
* Fills in the user buffer with the remote system type.  If more
* information from the response is required, the user can parse
* it out of the response buffer returned by FtpLastResponse().
*
* return 1 if command successful, 0 otherwise
*/
int FTPClient::SysType(char *buf, int max)
{
	int l = max;
	char *b = buf;
	char *s;
	if (!FtpSendCmd("SYST", '2', mp_ftphandle)) return 0;
	s = &mp_ftphandle->response[4];
	while ((--l) && (*s != ' ')) *b++ = *s++;
	*b++ = '\0';
	return 1;
}

/*
* FtpMkdir - create a directory at server
*
* return 1 if successful, 0 otherwise
*/
int FTPClient::Mkdir(const char *path)
{
	char buf[256];

	if ((strlen(path) + 6) > sizeof(buf)) return 0;
	sprintf_s(buf, 256, "MKD %s", path);
	if (!FtpSendCmd(buf, '2', mp_ftphandle)) return 0;
	return 1;
}

/*
* FtpChdir - change path at remote
*
* return 1 if successful, 0 otherwise
*/
int FTPClient::Chdir(const char *path)
{
	char buf[256];

	if ((strlen(path) + 6) > sizeof(buf)) return 0;
	sprintf_s(buf, 256, "CWD %s", path);
	if (!FtpSendCmd(buf, '2', mp_ftphandle)) return 0;
	return 1;
}

/*
* FtpCDUp - move to parent directory at remote
*
* return 1 if successful, 0 otherwise
*/
int FTPClient::Cdup()
{
	if (!FtpSendCmd("CDUP", '2', mp_ftphandle)) return 0;
	return 1;
}

/*
* FtpRmdir - remove directory at remote
*
* return 1 if successful, 0 otherwise
*/
int FTPClient::Rmdir(const char *path)
{
	char buf[256];

	if ((strlen(path) + 6) > sizeof(buf)) return 0;
	sprintf_s(buf, 256, "RMD %s", path);
	if (!FtpSendCmd(buf, '2', mp_ftphandle)) return 0;
	return 1;
}

/*
* FtpPwd - get working directory at remote
*
* return 1 if successful, 0 otherwise
*/
int FTPClient::Pwd(char *path, int max)
{
	int l = max;
	char *b = path;
	char *s;

	if (!FtpSendCmd("PWD", '2', mp_ftphandle)) {
		return 0;
	}
	s = strchr(mp_ftphandle->response, '"');
	if (!s) {
		return 0;
	}
	s++;
	while ((--l) && (*s) && (*s != '"')) {
		*b++ = *s++;
	}
	*b++ = '\0';
	return 1;
}

/*
* FtpNoop - No operation
*
* return 1 if successful, 0 otherwise
*/
int FTPClient::Noop()
{
	if (!FtpSendCmd("NOOP", '2', mp_ftphandle)) return 0;
	return 1;
}

/*
* FtpXfer - issue a command and transfer data
*
* return 1 if successful, 0 otherwise
*/
int FTPClient::FtpXfer(const char *localfile, const char *path, FTPHandle *nControl, accesstype type, transfermode mode)
{
	int c;
	size_t l;
	FILE *local = NULL;
	FTPHandle *nData;
	int rv = 1;

	if (localfile) {
		char ac[3] = {'\0', '\0', '\0'};
		if ((type == FTPClient::dir) || (type == FTPClient::dirverbose)) {
			ac[0] = 'w';
			ac[1] = '\0';
		}
		if (type == FTPClient::fileread) {
			ac[0] = 'w';
			ac[1] = '\0';
		}
		if (type == FTPClient::filewriteappend) {
			ac[0] = 'r';
			ac[1] = '\0';
		}
		if (type == FTPClient::filereadappend) {
			ac[0] = 'a';
			ac[1] = '\0';
		}
		if (type == FTPClient::filewrite) {
			ac[0] = 'r';
			ac[1] = '\0';
		}
		if (mode == FTPClient::BIN) {
			ac[1] = 'b';
		}

		if (fopen_s(&local, localfile, ac) > 0) {
			strerror_s(nControl->response, 256, errno);
			return 0;
		}
		if (type == FTPClient::filewriteappend) {
			_fseeki64(local, mp_ftphandle->offset, SEEK_SET);
		}
	}
	
	if (!local) {
		local = ((type == FTPClient::filewrite) || (type == FTPClient::filewriteappend)) ? stdin : stdout;
	}

	if (!FtpDataOpen(path, type, mode, nControl, &nData)) {
		return 0;
	}

	char *dbuf = new char[DEFAULT_BUFSIZ];
	if ((type == FTPClient::filewrite) || (type == FTPClient::filewriteappend)) {
		while ((l = fread(dbuf, 1, DEFAULT_BUFSIZ, local)) > 0) {
			if ((c = FtpDataWrite(dbuf, (int)l, nData)) < l) {
				printf("short write: passed %zu, wrote %d\n", l, c);
				rv = 0;
				break;
			}
		}
	} else {
		while ((l = FtpDataRead(dbuf, DEFAULT_BUFSIZ, nData)) > 0) {
			if (fwrite(dbuf, 1, l, local) <= 0) {
				perror("localfile write");
				break;
			}
		}
	}
	delete[] dbuf;

	::fflush(local);
	if (localfile) {
		fclose(local);
	}

	return FtpDataClose(nData);
}

/*
* FtpNlst - issue an NLST command and write response to output
*
* return 1 if successful, 0 otherwise
*/
int FTPClient::Nlst(const char *outputfile, const char *path)
{
	mp_ftphandle->offset = 0;
	return FtpXfer(outputfile, path, mp_ftphandle, FTPClient::dir, FTPClient::TEXT);
}

/*
* FtpDir - issue a LIST command and write response to output
*
* return 1 if successful, 0 otherwise
*/
int FTPClient::Dir(const char *outputfile, const char *path)
{
	mp_ftphandle->offset = 0;
	return FtpXfer(outputfile, path, mp_ftphandle, FTPClient::dirverbose, FTPClient::TEXT);
}

/*
* FtpSize - determine the size of a remote file
*
* return 1 if successful, 0 otherwise
*/
int FTPClient::Size(const char *path, int *size, transfermode mode)
{
	char cmd[256];
	int resp, sz, rv = 1;

	if ((strlen(path) + 7) > sizeof(cmd)) {
		return 0;
	}

	sprintf_s(cmd, 256, "TYPE %c", mode);
	if (!FtpSendCmd(cmd, '2', mp_ftphandle)) {
		return 0;
	}

	sprintf_s(cmd, 256, "SIZE %s", path);
	if (!FtpSendCmd(cmd, '2', mp_ftphandle)) {
		rv = 0;
	} else {
		if (sscanf_s(mp_ftphandle->response, "%d %d", &resp, &sz) == 2) {
			*size = sz;
		} else {
			rv = 0;
		}
	}

	return rv;
}

/*
* FtpModDate - determine the modification date of a remote file
*
* return 1 if successful, 0 otherwise
*/
int FTPClient::ModDate(const char *path, char *dt, int max)
{
	char buf[256];
	int rv = 1;

	if ((strlen(path) + 7) > sizeof(buf)) {
		return 0;
	}

	sprintf_s(buf, 256, "MDTM %s", path);
	if (!FtpSendCmd(buf, '2', mp_ftphandle)) {
		rv = 0;
	} else {
		strncpy_s(dt, max, &mp_ftphandle->response[4], max);
	}

	return rv;
}

/*
* FtpGet - issue a GET command and write received data to output
*
* return 1 if successful, 0 otherwise
*/
int FTPClient::Get(const char *outputfile, const char *path, transfermode mode, __int64 offset)
{
	mp_ftphandle->offset = offset;
	return FtpXfer(outputfile, path, mp_ftphandle, offset == 0 ? FTPClient::fileread : FTPClient::filereadappend, mode);
}

/*
* FtpPut - issue a PUT command and send data from input
*
* return 1 if successful, 0 otherwise
*/
int FTPClient::Put(const char *inputfile, const char *path, transfermode mode, __int64 offset)
{
	mp_ftphandle->offset = offset;
	return FtpXfer(inputfile, path, mp_ftphandle, offset == 0 ? FTPClient::filewrite : FTPClient::filewriteappend, mode);
}

int FTPClient::Rename(const char *src, const char *dst)
{
	char cmd[256];

	if (((strlen(src) + 7) > sizeof(cmd)) || ((strlen(dst) + 7) > sizeof(cmd))) {
		return 0;
	}

	sprintf_s(cmd, 256, "RNFR %s", src);
	if (!FtpSendCmd(cmd, '3', mp_ftphandle)) {
		return 0;
	}

	sprintf_s(cmd, 256, "RNTO %s", dst);
	if (!FtpSendCmd(cmd, '2', mp_ftphandle)) {
		return 0;
	}

	return 1;
}

int FTPClient::Delete(const char *path)
{
	char cmd[256];

	if ((strlen(path) + 7) > sizeof(cmd)) {
		return 0;
	}

	sprintf_s(cmd, 256, "DELE %s", path);
	if (!FtpSendCmd(cmd, '2', mp_ftphandle)) {
		return 0;
	}

	return 1;
}

/*
* FtpQuit - disconnect from remote
*
* return 1 if successful, 0 otherwise
*/
int FTPClient::Quit()
{
	if (mp_ftphandle->dir != CONN_CONTROL) {
		return 0;
	}

	if (!isConnected()) {
		return 0;
	}

	int retval = 0;
	if (!FtpSendCmd("QUIT", '2', mp_ftphandle)) {
		retval = 1;
	}

	mp_ftphandle->sock->close();
	delete mp_ftphandle->sock;
	mp_ftphandle->sock = nullptr;

	return retval;
}

void FTPClient::SetConnmode(connmode mode)
{
	mp_ftphandle->cmode = mode;
}

void FTPClient::ClearHandle()
{
	mp_ftphandle->dir = CONN_CONTROL;
	mp_ftphandle->ctrl = NULL;
	mp_ftphandle->cmode = FTPClient::pasv;
	mp_ftphandle->xfered = 0;
	mp_ftphandle->xfered1 = 0;
	mp_ftphandle->cbbytes = 0;
	mp_ftphandle->offset = 0;
}

FTPHandle *FTPClient::RawOpen(const char *path, accesstype type, transfermode mode)
{
	FTPHandle *datahandle;
	int retval = FtpDataOpen(path, type, mode, mp_ftphandle, &datahandle);
	if (retval) {
		return datahandle;
	}
	
	return NULL;
}

int FTPClient::RawClose(FTPHandle *handle)
{
	return FtpDataClose(handle);
}

int FTPClient::RawWrite(void* buf, int len, FTPHandle *handle)
{
	return FtpDataWrite(buf, len, handle);
}

int FTPClient::RawRead(void* buf, int max, FTPHandle *handle)
{
	return FtpDataRead(buf, max, handle);
}

#if 0
void ftp_test()
{
	FTPClient fc;
	fc.Connect("server20.000webhost.com");
	fc.Login("a6289090", "qabQrvqUeTv1se4GmcxG");

	char path[256];
	fc.Pwd(path, 256);
	//fc.Dir(NULL, "/var");
	fc.Noop();
	fc.Get("avc_endpoint.php", "/public_html/avc_endpoint.php", FTPClient::TEXT);
	//fc.Mkdir("/var/test");
	//fc.Put("Util.obj", "/var/Util.obj", FTPClient::image);
}
#endif 
