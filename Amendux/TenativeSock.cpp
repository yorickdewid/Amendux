#include "stdafx.h"
#include "TenativeSock.h"
#include <winsock2.h>
#include <Ws2tcpip.h>

using namespace SimpleSocksPrivateNS;

TenativeSock::TenativeSock() : sock(INVALID_SOCKET), valid(false)
{
	//NOP
}

TenativeSock::~TenativeSock() {
	if (valid == false) {
		closesocket(sock);
	}
}

int TenativeSock::generate(int type, int protocol) {
	if (sock != INVALID_SOCKET) {
		return -1;
	}

	sock = socket(AF_INET, type, protocol);
	if (sock == INVALID_SOCKET) {
		return -1;
	}

	return 0;
}

SOCKET TenativeSock::get() const {
	return sock;
}

SOCKET TenativeSock::validate() {
	valid = true;
	return sock;
}
