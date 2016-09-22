#include "stdafx.h"
#include "TCPSocket.h"
#include "TenativeSock.h"
#include "AddrLookup.h"
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <memory>
#include <new>

using namespace SimpleSocks;
using namespace SimpleSocksPrivateNS;

TCPSocket::TCPSocket() : pimpl(NULL)
{
	//NOP
}

TCPSocket::~TCPSocket()
{
	close();
}

int TCPSocket::connect(const char *host, unsigned short port)
{
	if (isConnected()) {
		return -1;
	}

	std::auto_ptr<impl> timpl(NULL);
	try {
		timpl.reset(new impl);
	} catch (std::bad_alloc) {
		return -5;
	}

	timpl->noblock = 0;
	timpl->sock = INVALID_SOCKET;

	AddrLookup addr;
	if (addr.lookup(host, port, IPPROTO_TCP)) {
		return -1;
	}

	TenativeSock tsock;
	if (tsock.generate(SOCK_STREAM, IPPROTO_TCP)) {
		return -1;
	}

	sockaddr *node;
	//Stop loop when out of addresses to try.
	while (node = addr.getNext()) {
		if (::connect(tsock.get(), node, sizeof(sockaddr))) {
			//If ::connect() failed then try next address.
			continue;
		}
		//Otherwise break the loop.
		break;
	}

	//If node != NULL then the loop was broken manually
	//and the sock is valid.
	if (node) {
		timpl->sock = tsock.validate();
		pimpl.reset(timpl.release());
		return 0;
	}

	//Otherwise we ran out of addresses to try.
	return -1;
}

void TCPSocket::close()
{
	if (isConnected()) {
		if (pimpl->sock != INVALID_SOCKET) {
			closesocket(pimpl->sock);
		}
		pimpl.reset(NULL);
	}
}

int TCPSocket::send(const char* buffer, int length)
{
	if (!isConnected()) {
		return -1;
	}

	return ::send(pimpl->sock, buffer, length, 0);
}

int TCPSocket::recv(char* buffer, int length)
{
	if (!isConnected()) {
		return -1;
	}

	return pimpl->coreRecv(buffer, length, 0);
}

int TCPSocket::peek(char* buffer, int length)
{
	if (isConnected() == false) {
		return -1;
	}

	//In my experience people usually don't use MSG_PEEK.
	//Instead they just over-read the data and then do
	//buffer ops like memcpy, etc to make up for it.
	//As far as I'm concerned, that's letting the
	//terrorists win. Are you not a patriot, sir?
	//SIR!?!?!
	return pimpl->coreRecv(buffer, length, MSG_PEEK);
}

bool TCPSocket::getBlocking() const
{
	if (isConnected() == false) {
		return false;
	}

	//If noblock is false then block is true.
	return (pimpl->noblock == 0);
}

int TCPSocket::setBlocking(bool block)
{
	if (isConnected() == false) {
		return -1;
	}

	//Avoid gratuitous ioctl.
	if (block == getBlocking()) {
		return 0;
	}

	unsigned long noblock = block ? 0 : 1;

	int result;
	//Hehehehe...
	if (result = ioctlsocket(pimpl->sock, FIONBIO, &noblock)) {
		return -1;
	}

	pimpl->noblock = noblock;

	return 0;
}

bool TCPSocket::isConnected() const
{
	return (pimpl.get() != NULL);
}

int TCPSocket::impl::coreRecv(char *buffer, int length, int flags)
{
	int sult = ::recv(sock, buffer, length, flags);
	//If we're a blocking sock then an error is an error.
	if (noblock == 0) {
		return sult;
	}

	//Otherwise there's a special case...
	if (sult == SOCKET_ERROR) {
		//WSAGetLastError() is thread-safe here. (see MSDN)
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			return -2;
		}
	}

	return sult;
}
