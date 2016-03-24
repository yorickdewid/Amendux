#pragma once

#include <Ws2tcpip.h>

/* <AddrLookup>
 * Internal class for looking up addresses.
 * Basically an RAII wrapper for getaddrinfo().
 */

namespace SimpleSocksPrivateNS {
	class AddrLookup {
	public:
		AddrLookup();
		~AddrLookup();

		//Perform DNS query or convert IP address string.
		//Call to populate the internal list.
		//Returns 0 on success.
		//Calls 'clear()' internally before running, so
		//an instance can be re-used for multiple lookups.
		int lookup(const char* host, unsigned short port, int protocol);

		//Return the next addrinfo in the internal list
		//and moves the list node forward.
		//Return value of NULL indicates no more addresses.
		sockaddr* getNext();

		//Clear the internal list if one exists.
		void clear();
	private:
		addrinfo* list;
		addrinfo* node;
	};
};
