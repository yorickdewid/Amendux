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
		unsigned int reponseCode;
		unsigned int reponseSize;

		void WebClient::buildHeader();

	protected:
		std::map<std::string, std::string> responseHeader;

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

		inline unsigned int getResponseSize() const {
			return reponseSize;
		}

		inline unsigned int getResponseCode() const {
			return reponseCode;
		}

		inline void addHeader(std::wstring header) {
			addHeader(std::string(header.begin(), header.end()));
		}

		inline void addHeader(std::string header) {
			if (httpHeader.empty()) {
				return;
			}

			httpHeader += header + "\r\n";
		}

		char *Perform(const std::wstring& postData = L"");
		char *Perform(const std::string& postData = "");

		virtual void *Call(const std::wstring& postData = L"") {
			return Perform(postData);
		}

		virtual void *ParseResponse() {
			return nullptr;
		}
	};

}
