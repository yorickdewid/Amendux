#pragma once

#include "WebClient.h"

namespace Amendux {

	class RestClient : public WebClient
	{
	public:
		RestClient(const std::string& host, const std::string& uri = "") : WebClient(host, uri) {}
		~RestClient() {}

		void *ParseResponse();
	};

}
