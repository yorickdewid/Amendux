#pragma once

#include "WebClient.h"

namespace Amendux {

	class TransferClient : public WebClient
	{
	public:
		TransferClient(const std::string& host, const std::string& uri = "") : WebClient(host, uri) {}
		~TransferClient() {}
	};

}
