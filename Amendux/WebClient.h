#pragma once

#include "SimpleSocksNS.h"
#include "TCPSocket.h"

namespace Amendux {

	class WebClient
	{
	public:
		WebClient(const std::string& address);
		~WebClient();
	};

}
