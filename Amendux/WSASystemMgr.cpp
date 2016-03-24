#include "stdafx.h"
#include "WSASystemMgr.h"
#include <Ws2tcpip.h>

using namespace SimpleSocksPrivateNS;

WSASystemMgr::WSASystemMgr() :
	running(false)
{
	//NOP
}

int WSASystemMgr::start() {
	WSAData wsadata;
	int retval = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (retval) { return -1; }
	running = true;
	return retval;
}

void WSASystemMgr::stop() {
	//Only stop ws if we're the one that started it.
	if (running) {
		WSACleanup();
		running = false;
	}
}

WSASystemMgr::~WSASystemMgr() {
	stop();
}
