#pragma once

#include <winsock2.h>
#include <time.h>
#include "SimpleSocksNS.h"
#include "TCPSocket.h"

namespace Amendux {

	struct FTPHandle {
		SimpleSocks::TCPSocket *sock = nullptr;
		char *buf;
		int dir;
		FTPHandle *ctrl;
		int cmode;
		__int64 xfered;
		__int64 cbbytes;
		__int64 xfered1;
		char response[256];
		__int64 offset;
	};

	class FTPClient
	{
		FTPHandle *mp_ftphandle = nullptr;
		bool isAuth = false;

		int readline(char *buf, int max, FTPHandle *ctl);
		int writeline(char *buf, int len, FTPHandle *nData);
		int readresp(char c, FTPHandle *nControl);

		void ClearHandle();
	
	public:
		enum accesstype {
			dir = 1,
			dirverbose,
			fileread,
			filewrite,
			filereadappend,
			filewriteappend
		};

		enum transfermode {
			TEXT = 'A',
			BIN = 'I'
		};

		enum connmode {
			pasv = 1,
			port
		};

		FTPClient();
		~FTPClient();
		char *LastResponse();
		int Connect(const char *host);
		int Login(const char *user, const char *pass);
		int Site(const char *cmd);
		int Raw(const char *cmd);
		int SysType(char *buf, int max);
		int Mkdir(const char *path);
		int Chdir(const char *path);
		int Cdup();
		int Noop();
		int Rmdir(const char *path);
		int Pwd(char *path, int max);
		int Nlst(const char *outputfile, const char *path);
		int Dir(const char *outputfile, const char *path);
		int Size(const char *path, int *size, transfermode mode);
		int ModDate(const char *path, char *dt, int max);
		int Get(const char *outputfile, const char *path, transfermode mode, __int64 offset = 0);
		int Put(const char *inputfile, const char *path, transfermode mode, __int64 offset = 0);
		int Rename(const char *src, const char *dst);
		int Delete(const char *path);
		int Quit();
		void SetConnmode(connmode mode);

		FTPHandle *RawOpen(const char *path, accesstype type, transfermode mode);
		int RawClose(FTPHandle *handle);
		int RawWrite(void* buf, int len, FTPHandle*handle);
		int RawRead(void* buf, int max, FTPHandle *handle);

		inline bool isConnected() {
			return mp_ftphandle->sock != nullptr;
		}

	private:
		int FtpXfer(const char *localfile, const char *path, FTPHandle *nControl, accesstype type, transfermode mode);
		int FtpOpenPasv(FTPHandle *nControl, FTPHandle **nData, transfermode mode, int dir, char *cmd);
		int FtpSendCmd(const char *cmd, char expresp, FTPHandle *nControl);
		int FtpDataRead(void *buf, int max, FTPHandle *nData);
		int FtpDataWrite(void *buf, int len, FTPHandle *nData);
		int FtpDataOpen(const char *path, accesstype type, transfermode mode, FTPHandle *nControl, FTPHandle **nData);
		int FtpDataClose(FTPHandle *nData);
	};

}
