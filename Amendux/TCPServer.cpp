#include "stdafx.h"
#include "TCPServer.h"
#include <Ws2tcpip.h>
#include "TenativeSock.h"
#include <memory>
#include <new>

using namespace SimpleSocks;
using namespace SimpleSocksPrivateNS;

TCPServer::TCPServer() :
	pimpl(NULL)
{
	//NOP
}

TCPServer::~TCPServer() {
	stop();
}

int TCPServer::start(unsigned short port, bool forceBind, const char* localHostAddr) {
	if (isRunning()) 
		return -1;

	//t(enative)impl - get it?
	std::auto_ptr<impl> timpl(NULL);

	try {
		timpl.reset(new impl);
	} catch (std::bad_alloc) {
		return -5;
	}

	timpl->serv = INVALID_SOCKET;
	timpl->noblock = 0;

	sockaddr_in sain;
	ZeroMemory(&sain, sizeof(sain));
	sain.sin_family = AF_INET;
	sain.sin_port = htons(port);
	sain.sin_addr.s_addr = INADDR_ANY;
	if (localHostAddr) {
		inet_pton(AF_INET, localHostAddr, &(sain.sin_addr));
		if (sain.sin_addr.s_addr == INADDR_NONE)
			return -1;
	}

	TenativeSock tsock;
	if (tsock.generate(SOCK_STREAM, IPPROTO_TCP))
		return -1;

	if (forceBind) {
		//This sockopt call will force the bind to occur even if the system says
		//that the socket in question is already bound. It can take a little while
		//for a port to be 'released' after the binding socket closes because the
		//port is supposed to stay that way temporarily in order to handle delayed
		//packets coming over the interbutts. (Sometimes packets can be delayed for
		//a long time. These days it's pretty rare though.)
		//The only reason forceBind should really be needed is if you had an error
		//on your server socket and you're doing a s.stop(); s.start(); on the same
		//port. Be aware that by using forceBind you could possibly bind a socket
		//that's already in use by another process, which could potentially cause
		//you some serious trouble. Be careful.
		BOOL optval = TRUE;
		if (setsockopt(tsock.get(), SOL_SOCKET, SO_REUSEADDR, (char*)(&optval), sizeof(optval))) {
			return -1;
		}
	}
	
	if (bind(tsock.get(), (sockaddr*)(&sain), sizeof(sain)))
		return -1;

	if (listen(tsock.get(), SOMAXCONN))
		return -1;

	timpl->serv = tsock.validate();
	pimpl.reset(timpl.release());

	return 0;
}

void TCPServer::stop() {
	if (isRunning()) {
		if (pimpl->serv != INVALID_SOCKET) {
			closesocket(pimpl->serv);
		}
		pimpl.reset(NULL);
	}
}

int TCPServer::accept(TCPSocket& sock) {
	//Calling the overload below.
	return accept(&sock);
}

int TCPServer::accept(TCPSocket* sock) {
	if (isRunning() == false)
		return -1;

	if (sock->isConnected())
		return -1;

	std::auto_ptr<TCPSocket::impl> timpl(NULL);

	try {
		timpl.reset(new TCPSocket::impl);
	} catch (std::bad_alloc) {
		return -5;
	}

	timpl->noblock = 0;

	//This may not look thread-safe, but WSAGetLastError()
	//works on a per-thread basis (using evil sorcery) so this
	//is actually okay.
	timpl->sock = ::accept(pimpl->serv, NULL, NULL);
	if (timpl->sock == INVALID_SOCKET) {
		if (getBlocking() && (WSAGetLastError() == WSAEWOULDBLOCK)) {
			return -2;
		}
		return -1;
	}
	timpl->noblock = pimpl->noblock;

	sock->pimpl.reset(timpl.release());

	return 0;
}

bool TCPServer::isRunning() const {
	return (pimpl.get() != NULL);
}

bool TCPServer::getBlocking() const {
	if (isRunning() == false) { return 0; }
	//If noblock is false then block is true.
	return (pimpl->noblock == false);
}

int TCPServer::setBlocking(bool block) {
	if (isRunning() == false) {
		return -1;
	}

	// Avoid gratuitous ioctl
	if (block == getBlocking()) {
		return 0;
	}

	bool noblock = block ? 0 : 1;

	int result;
	if (result = ioctlsocket(pimpl->serv, FIONBIO, (u_long *)&noblock)) {
		return -1;
	}

	pimpl->noblock = noblock;

	return 0;
}
