#pragma once

#include "TCPSocket.h"
#include "TCPServer.h"
#include "UDPSocket.h"
#include <map>
#include <memory>

/* <SelectSet>
 * Documented in public interface.
 */

namespace SimpleSocks {
	enum {
		SELECT_FOREVER = ~0
	};
	
	class SelectSet {
		friend int select(SelectSet* set, unsigned int timeout);

	public:
		SelectSet();
		~SelectSet();
		int copyFrom(SelectSet& src);
		int copyFrom(SelectSet* src);
		int getCount() const;
		void clear();
		int validate();
		int pushObject(TCPSocket* sock);
		int pushObject(TCPServer* serv);
		int pushObject(UDPSocket* sock);
		void removeObject(TCPSocket* sock);
		void removeObject(TCPServer* serv);
		void removeObject(UDPSocket* sock);
		TCPSocket* popTCPSocket();
		TCPServer* popTCPServer();
		UDPSocket* popUDPSocket();
	private:
		SelectSet(SelectSet&);
		// SelectSet& operator=(SelectSet&);
		struct impl {
			impl();
			impl(impl* src);
			std::map<SOCKET, TCPSocket*> tcps;
			std::map<SOCKET, TCPServer*> srvs;
			std::map<SOCKET, UDPSocket*> udps;
		};
		std::auto_ptr<impl> pimpl;
	};

	int select(SelectSet *set, unsigned int timeout);
};
