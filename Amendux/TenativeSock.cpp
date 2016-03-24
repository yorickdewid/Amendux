#include "stdafx.h"
#include "TenativeSock.h"
#include <Ws2tcpip.h>

using namespace SimpleSocksPrivateNS;

TenativeSock::TenativeSock() :
	sock((__w64 unsigned int)INVALID_SOCKET),
	valid(false)
{
	//NOP
}

TenativeSock::~TenativeSock() {
	if (valid == false) {
		closesocket(sock);
	}
}

int TenativeSock::generate(int type, int protocol) {
	if (sock != INVALID_SOCKET)
		return -1;

	sock = (__w64 unsigned int)socket(AF_INET, type, protocol);
	if (sock == INVALID_SOCKET)
		return -1;

	return 0;
}

__w64 unsigned int TenativeSock::get() const {
	return sock;
}

__w64 unsigned int TenativeSock::validate() {
	valid = true;
	return sock;
}
