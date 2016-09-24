#pragma once

#include <winsock2.h>

namespace SimpleSocksPrivateNS {
	class TenativeSock {
	public:
		TenativeSock();
		~TenativeSock();

		//Creates a socket in 'sock'
		//returns -1 on fail or 0 on success
		//'sock' will be closed by dtor unless
		//'validate()' is called.
		//The args are passed directly to socket().
		//If 'sock' is already a socket the
		//call will fail and return -1.
		//For other failures, the function may
		//be re-called to retry creation.
		int generate(int type, int protocol);

		//Retrieves 'sock'
		SOCKET get() const;

		//Prevents dtor from closing 'sock'
		//and returns 'sock'.
		SOCKET validate();
	private:

		SOCKET sock;
		bool valid;
	};
};
