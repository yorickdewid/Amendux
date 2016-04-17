#pragma once

#include "SimpleSocksNS.h"
#include "TCPSocket.h"

#include <map>

namespace Amendux {

	enum class HttpType {
		GET,
		POST,
	};

	class WebClient
	{
		SimpleSocks::TCPSocket *sock = nullptr;
		std::string httpHeader;
		std::string host;
		std::string uri;
		HttpType type;

		void WebClient::buildHeader();

	protected:
		std::map<std::string, std::string> responseHeader;
		std::string responseData;

	public:
		WebClient(const std::string& host, const std::string& uri = "");
		~WebClient();

		inline bool isConnected() {
			return sock != nullptr;
		}

		inline void setType(HttpType t) {
			type = t;
		}

		inline std::string getResponseHeader(const std::string& name) const {
			if (responseHeader.empty())
				return "";

			if (responseHeader.find(name) != responseHeader.end()) {
				return responseHeader.at(name);
			}

			return "";
		}

		inline std::string getResponseData() const {
			return responseData;
		}

		inline void addHeader(std::string header) {
			if (httpHeader.empty()) {
				return;
			}

			httpHeader += header + "\r\n";
		}

		void Perform(const std::wstring& postData = L"");
		void Perform(const std::string& postData = "");

		virtual void *ParseResponse() {
			return nullptr;
		}
	};

}
