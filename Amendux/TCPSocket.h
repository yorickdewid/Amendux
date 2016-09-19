#pragma once

#include <memory>
#include <winsock2.h>

namespace SimpleSocks {

	class TCPSocket
	{
		friend class TCPServer;
		friend class SelectSet;

	public:
		TCPSocket();
		~TCPSocket();
		int  connect(const char* host, unsigned short port);
		void close();
		int  send(const char* buffer, int length);
		int  recv(char* buffer, int length);
		int  peek(char* buffer, int length);
		bool getBlocking() const;
		int  setBlocking(bool block);
		bool isConnected() const;

	private:
		TCPSocket(TCPSocket&);
		// void operator=(TCPSocket&);
		struct impl {
			int coreRecv(char* buffer, int length, int flags);
			SOCKET sock;
			unsigned long noblock;
		};
		std::auto_ptr<impl> pimpl;
	};

};
