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

		std::wstring ParseResponse(char *data);

		std::wstring Call() {
			return ParseResponse(Perform(""));
		}
		
		std::wstring Download() {
			return ParseResponse(Perform(""));
		}

	};

}
