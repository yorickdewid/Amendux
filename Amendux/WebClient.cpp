#include "stdafx.h"
#include "Log.h"
#include "Encrypt.h"
#include "Config.h"
#include "WebClient.h"

#include <iostream>

using namespace Amendux;

void WebClient::Init(const std::string& host, const std::string& uri) {
	if (SimpleSocks::initSimpleSocks()) {
		Log::Error(L"WebClient", L"Failed to initialize Simple Socks");
		return;
	}

	sock = new SimpleSocks::TCPSocket;

	//Connect it to Google on port 80. (80 is the HTTP port.)
	if (sock->connect(host.c_str(), 80)) {
		Log::Error(L"WebClient", L"Failed to connect to server");
		sock = nullptr;
	}

	this->host = host;
	this->uri = uri;
}


WebClient::WebClient(const std::string& url)
{
	HttpUrl urlPart = HttpUrl(url);
	if (urlPart.protocol != "http") {
		return;
	}

	Init(urlPart.host, urlPart.path);
}


void WebClient::buildHeader()
{
	switch (type)
	{
	default:
	case Amendux::HttpType::GET:
		httpHeader =
			"GET /" + uri + " HTTP/1.1\r\n"
			"Host: " + host + "\r\n";
		break;
	case Amendux::HttpType::POST:
		httpHeader =
			"POST /" + uri + " HTTP/1.1\r\n"
			"Host: " + host + "\r\n";
		break;
	}
}


char *WebClient::Perform(const std::wstring& postData)
{
	return Perform(std::string(postData.begin(), postData.end()));
}


char *WebClient::Perform(const std::string& postData)
{
	if (!isConnected()) {
		return nullptr;
	}

	if (!postData.empty()) {
		type = HttpType::POST;
	}

	buildHeader();

	addHeader("Connection: close");

#ifdef DEBUG
	addHeader(L"User-Agent: Amendux/" + Config::getVersion());
	addHeader(L"X-Client: Amendux/" + Config::getVersion());
#else
	int randAgent = rand() % 6;
	addHeader("User-Agent: " + userAgent[randAgent]);
#endif

	if (!postData.empty()) {
		addHeader("Content-Type: application/x-www-form-urlencoded");
		addHeader("Content-Length: " + std::to_string(postData.length()));
		httpHeader += "\r\n";

		httpHeader += postData;
	}
	else {
		httpHeader += "\r\n";
	}

	// We send it through the socket
	int retval = sock->send(httpHeader.c_str(), static_cast<int>(httpHeader.length()));
	if (retval < 1) {
		Log::Error(L"WebClient", L"Connection was lost");
		sock = nullptr;
		return nullptr;
	}

	char buffer[1024];
	memset(buffer, 0, 1024);
	retval = sock->peek(buffer, 1023);
	if (retval < 1) {
		Log::Error(L"WebClient", L"Socket error");
		return nullptr;
	}

	// The response header will end with "\r\n\r\n", so let's look for that.
	char *endHeaderPtr = strstr(buffer, "\r\n\r\n");
	if (!endHeaderPtr) {
		Log::Error(L"WebClient", L"Could not find end of HTTP header");
		return nullptr;
	}

	endHeaderPtr += 4;
	size_t headLen = endHeaderPtr - buffer;
	memset(buffer, 0, 1024);

	retval = sock->recv(buffer, static_cast<int>(headLen));
	if (retval < 1) {
		Log::Error(L"WebClient", L"Socket error");
		return nullptr;
	}

	std::vector<std::string> resp;
	Util::split<std::string>(std::string(buffer), "\r\n", resp);

	std::string httpProtocol = resp.at(0);
	if (!(httpProtocol.substr(0, 8) == "HTTP/1.1" || httpProtocol.substr(0, 8) == "HTTP/1.0")) {
		return nullptr;
	}

	std::string httpCode = resp.at(0).substr(9, 3);
	reponseCode = atoi(httpCode.c_str());
	if (reponseCode < 100 || reponseCode > 600) {
		return nullptr;
	}

	resp.erase(resp.begin(), resp.begin() + 1);

	for (auto const& value : resp) {
		size_t pos = value.find_first_of(':');

		responseHeader[Util::tolower(value.substr(0, pos))] = Util::trim<std::string>(Util::tolower(value.substr(pos + 1)));
	}

	//Now we can read all the data. The header contains the length
	//of the data, so we can look there to see how long it is.
	unsigned int dataLen = 0;
	if (responseHeader.find("content-length") != responseHeader.end()) {
		dataLen = atoi(responseHeader["content-length"].c_str());
	}
	else {
		dataLen = 1024;
	}

	//Now we just allocate a buffer for the data and read it.
	//Remember the extra byte for the trailing zero.
	char *data = new char[dataLen + 1];
	memset(data, 0, dataLen + 1);

	unsigned int got = 0;
	while (got < dataLen) {
		retval = sock->recv(data + got, dataLen - got);
		if (retval == 0) {
			break;
		} else if (retval < 1) {
			Log::Error(L"WebClient", L"Socket error");
			return nullptr;
		}
		got += retval;
	}

	reponseSize = dataLen;

	return data;
}


WebClient::~WebClient()
{
	sock->close();
	delete sock;
	sock = nullptr;
	SimpleSocks::shutdownSimpleSocks();
}
