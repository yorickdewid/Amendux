#pragma once

/* <WSASystemMgr>
 * RAII object for winsock.
 * Instance is declared in "SimpleSocksNS.cpp"
 */

namespace SimpleSocksPrivateNS {
	class WSASystemMgr {
	public:
		WSASystemMgr();
		int  start();
		void stop();
		~WSASystemMgr();
	private:
		bool running;
	};
};