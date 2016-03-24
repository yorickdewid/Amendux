#pragma once

#include <Ws2tcpip.h>
#include <memory>
//Documented in public header

namespace SimpleSocks {
	struct IPv4Addr {
		unsigned long addr;
		unsigned short port;
	};

	class UDPSocket {
		friend class SelectSet;
	public:
		UDPSocket();
		~UDPSocket();

		void reset();

		int bind(unsigned short port, bool forceBind = false);
		int sendto(const char* host, unsigned short port, const char* buffer, int length, IPv4Addr* out_remoteAddr = NULL);
		int sendto(const IPv4Addr* ipaddr, const char* buffer, int length);
		int recvfrom(IPv4Addr* ipaddr, char* buffer, int length);

		int connect(const char* host, unsigned short port);
		int connect(IPv4Addr* ipaddr);
		int disconnect();
		int send(const char* buffer, int length);
		int recv(char* buffer, int length);

		bool getBlocking() const;
		int setBlocking(bool block);

		enum UDPMode {
			UDP_INACTIVE,
			UDP_CONNECTED,
			UDP_BOUND
		};

		UDPMode getMode() const;
		unsigned short getBoundPort() const;

	private:
		UDPSocket(UDPSocket&);
		void operator=(UDPSocket&);
		struct impl {
			int coreConnect(sockaddr* saddr);
			UDPMode mode;
			__w64 unsigned int sock;
			unsigned long noblock;
		};
		std::auto_ptr<impl> pimpl;
	};
};
