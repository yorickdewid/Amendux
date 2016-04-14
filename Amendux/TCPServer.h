#pragma once

#include "TCPSocket.h"
#include <memory>

//Documented in public header.

namespace SimpleSocks {
	class TCPServer {
		friend class SelectSet;
	public:
		TCPServer();
		~TCPServer();
		int  start(unsigned short port, bool forceBind = false, const char *localHostAddr = NULL);
		void stop();
		int  accept(TCPSocket& sock);
		int  accept(TCPSocket* sock);
		bool isRunning() const;
		bool getBlocking() const;
		int  setBlocking(bool block);
	private:
		TCPServer(TCPServer&);
		// void operator=(TCPServer&);
		struct impl {
			SOCKET serv;
			unsigned long noblock;
		};
		std::auto_ptr<impl> pimpl;
	};
};
