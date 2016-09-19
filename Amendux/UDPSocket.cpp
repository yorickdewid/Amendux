#include "stdafx.h"
#include "UDPSocket.h"
#include "TenativeSock.h"
#include "AddrLookup.h"
#include <Ws2tcpip.h>
#include <memory>
#include <new>

using namespace SimpleSocks;
using namespace SimpleSocksPrivateNS;

UDPSocket::UDPSocket() : pimpl(NULL)
{
	//NOP
}

UDPSocket::~UDPSocket() {
	reset();
}

void UDPSocket::reset() {
	if (pimpl.get() != NULL) {
		if (pimpl->sock != INVALID_SOCKET) {
			closesocket(pimpl->sock);
		}
		pimpl.reset(NULL);
	}
}

int UDPSocket::bind(unsigned short port, bool forceBind) {
	if (getMode() != UDP_INACTIVE) { return -2; }

	sockaddr_in sain;
	sain.sin_family = AF_INET;
	sain.sin_port = htons(port);
	sain.sin_addr.s_addr = ADDR_ANY;

	TenativeSock tsock;
	if (tsock.generate(SOCK_DGRAM, IPPROTO_UDP)) { return -1; }

	if (forceBind) {
		BOOL optval = TRUE;
		//See comment in TCPServer::start() [TCPServer.cpp]
		if (setsockopt(tsock.get(), SOL_SOCKET, SO_REUSEADDR, (char*)(&optval), sizeof(optval))) {
			return -1;
		}
	}

	if (::bind(tsock.get(), (sockaddr*)&sain, sizeof(sain))) { return -1; }

	try { pimpl.reset(new impl); }
	catch (std::bad_alloc) { return -5; }
	pimpl->sock = tsock.validate();
	pimpl->mode = UDP_BOUND;

	return 0;
}

int UDPSocket::sendto(const char* host, unsigned short port, const char* buffer, int length, IPv4Addr* out_remoteAddr) {
	if (getMode() != UDP_BOUND) { return -2; }

	AddrLookup lookup;
	if (lookup.lookup(host, port, IPPROTO_UDP)) { return -1; }
	//Just use the first result, since UDP doesn't actually verify the remote host.
	sockaddr* saddr = lookup.getNext();
	if (saddr == NULL) { return -1; }

	int result = 0;
	//You can use a length of zero if you just want to populate out_remoteAddr.
	if (length) {
		//I don't call the IPv4Addr overload here because it would replicate work.
		result = ::sendto(pimpl->sock, buffer, length, 0, saddr, sizeof(sockaddr));
		if (result == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAEWOULDBLOCK) {
				return -3;
			}
			return -1;
		}
	}

	//We set 'out_remoteAddr' only after everything has succeeded.
	if (out_remoteAddr) {
		sockaddr_in* sain = (sockaddr_in*)saddr;
		out_remoteAddr->addr = sain->sin_addr.s_addr;
		out_remoteAddr->port = sain->sin_port;
	}

	return result;
}

int UDPSocket::sendto(const IPv4Addr* ipaddr, const char* buffer, int length) {
	if (getMode() != UDP_BOUND) { return -2; }

	sockaddr_in sain;
	memset(&sain, 0, sizeof(sain));
	sain.sin_family = AF_INET;
	sain.sin_addr.s_addr = ipaddr->addr;
	sain.sin_port = ipaddr->port;

	int result = ::sendto(pimpl->sock, buffer, length, 0, (sockaddr*)&sain, sizeof(sain));
	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			return -3;
		}
	}
	return result;
}

int UDPSocket::recvfrom(IPv4Addr* ipaddr, char* buffer, int length) {
	if (getMode() != UDP_BOUND) { return -2; }

	sockaddr_in sain;
	int sainlen = sizeof(sain);
	int result = ::recvfrom(pimpl->sock, buffer, length, 0, (sockaddr*)&sain, &sainlen);

	if (result == -1) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			return -3;
		}
	}
	else if (ipaddr) {
		ipaddr->addr = sain.sin_addr.s_addr;
		ipaddr->port = sain.sin_port;
	}

	return result;
}

int UDPSocket::connect(const char *host, unsigned short port) {
	if (getMode() == UDP_CONNECTED) { return -2; }

	AddrLookup lookup;
	if (lookup.lookup(host, port, IPPROTO_UDP)) { return -1; }
	//Using the first address since UDP does not confirm connection.
	sockaddr* saddr = lookup.getNext();
	if (saddr == NULL) { return -1; }

	int retval = 0;

	if (getMode() == UDP_INACTIVE) {
		std::auto_ptr<impl> timpl(NULL);

		try {
			timpl.reset(new impl);
		} catch (std::bad_alloc) {
			return -5;
		}

		timpl->mode = UDP_INACTIVE;
		timpl->noblock = 0;
		timpl->sock = INVALID_SOCKET;
		retval = timpl->coreConnect(saddr);
		if (retval == 0) {
			pimpl.reset(timpl.release());
		}
	}
	else { //mode is UDP_BOUND
		retval = pimpl->coreConnect(saddr);
	}

	return retval;
}

int UDPSocket::connect(IPv4Addr* ipaddr) {
	if (getMode() == UDP_CONNECTED) { return -2; }

	sockaddr_in sain;
	memset(&sain, 0, sizeof(sain));
	sain.sin_family = AF_INET;
	sain.sin_addr.s_addr = ipaddr->addr;
	sain.sin_port = ipaddr->port;

	int retval = 0;

	if (getMode() == UDP_INACTIVE) {
		std::auto_ptr<impl> timpl(NULL);
		try { timpl.reset(new impl); }
		catch (std::bad_alloc) { return -5; }
		timpl->mode = UDP_INACTIVE;
		timpl->noblock = 0;
		timpl->sock = INVALID_SOCKET;
		retval = timpl->coreConnect((sockaddr*)&sain);
		if (retval == 0) {
			pimpl.reset(timpl.release());
		}
	}
	else { //mode is UDP_BOUND
		retval = pimpl->coreConnect((sockaddr*)&sain);
	}

	return retval;
}

int UDPSocket::disconnect() {
	if (getMode() != UDP_CONNECTED) { return -2; }

	//Calling ::connect() on a connected UDP sock with
	//an address of zero relinquishes the connection state
	//but does not unbind the socket.
	sockaddr saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sa_family = AF_INET;

	if (::connect(pimpl->sock, &saddr, sizeof(saddr))) { return -1; }

	pimpl->mode = UDP_BOUND;

	return 0;
}

int UDPSocket::send(const char* buffer, int length) {
	if (getMode() != UDP_CONNECTED) { return -2; }

	int result = ::send(pimpl->sock, buffer, length, 0);
	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			return -3;
		}
	}
	return result;
}

int UDPSocket::recv(char* buffer, int length) {
	if (getMode() != UDP_CONNECTED) { return -2; }

	int result = ::recv(pimpl->sock, buffer, length, 0);
	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			return -3;
		}
	}
	return result;
}

bool UDPSocket::getBlocking() const {
	if (getMode() == UDP_INACTIVE) { return false; }
	//If noblock is false then block is true.
	return (pimpl->noblock == 0);
}

int UDPSocket::setBlocking(bool block) {
	if (getMode() == UDP_INACTIVE) { return -2; }
	//Avoid gratuitous ioctl.
	if (block == getBlocking()) { return 0; }
	//(Ugh...)
	unsigned long noblock = block ? 0 : 1;

	int result;
	//Hahahahaha!
	if (result = ioctlsocket(pimpl->sock, FIONBIO, &noblock)) { return -1; }

	pimpl->noblock = noblock;

	return 0;
}

UDPSocket::UDPMode UDPSocket::getMode() const {
	if (pimpl.get() == NULL) { return UDP_INACTIVE; }
	return pimpl->mode;
}

unsigned short UDPSocket::getBoundPort() const {
	if (getMode() == UDP_INACTIVE) { return 0; }

	//Can't remember why I did it this way instead of just
	//keeping a record of the port num in the pimpl...
	//Ah, whatever. It was probably a good reason.
	sockaddr_in sain;
	int sainlen = sizeof(sain);
	if (getsockname(pimpl->sock, (sockaddr*)&sain, &sainlen)) { return 0; }
	return ntohs(sain.sin_port);
}

int UDPSocket::impl::coreConnect(sockaddr* saddr) {
	if (mode == UDP_INACTIVE) {
		TenativeSock tsock;
		if (tsock.generate(SOCK_DGRAM, IPPROTO_UDP)) { return -1; }
		if (::connect(tsock.get(), saddr, sizeof(sockaddr))) { return -1; }
		sock = tsock.validate();
	}
	else {
		if (::connect(sock, saddr, sizeof(sockaddr))) { return -1; }
	}

	mode = UDP_CONNECTED;

	return 0;
}
