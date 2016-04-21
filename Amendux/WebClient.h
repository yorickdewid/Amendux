#pragma once

#include "SimpleSocksNS.h"
#include "TCPSocket.h"

#include <map>

namespace Amendux {

	const std::string userAgent[] = {
		"Mozilla/5.0 (Windows NT 6.1; WOW64; rv:13.0) Gecko/20100101 Firefox/13.0.1",
		"Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.95 Safari/537.36",
		"Mozilla/5.0 (Macintosh; Intel Mac OS X 10.10; rv:34.0) Gecko/20100101 Firefox/34.0",
		"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; FSL 7.0.6.01001)",
		"Mozilla/5.0 (Windows NT 6.1; WOW64; rv:12.0) Gecko/20100101 Firefox/12.0",
		"Mozilla/5.0 (X11; U; Linux x86_64; de; rv:1.9.2.8) Gecko/20100723 Ubuntu/10.04 (lucid) Firefox/3.6.8",
	};

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
