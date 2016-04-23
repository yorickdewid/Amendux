#pragma once

#include "WebClient.h"

namespace Amendux {

	class TransferClient : public WebClient
	{
		bool callSuccess = false;

	public:
		TransferClient(const std::string& host, const std::string& uri) : WebClient(host, uri) {}
		TransferClient(const std::string& url) : WebClient(url) {}
		~TransferClient() {}

		bool ParseResponse(char *data);

		bool Call() {
			return ParseResponse(Perform(""));
		}
		
		bool Download() {
			return ParseResponse(Perform(""));
		}

	};

}
