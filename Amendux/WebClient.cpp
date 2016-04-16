#include "stdafx.h"
#include "Log.h"
#include "WebClient.h"

#include <iostream>

using namespace Amendux;

WebClient::WebClient(const std::string& host, const std::string& endpoint)
{
	if (SimpleSocks::initSimpleSocks()) {
		Log::Instance()->error(L"WebClient", L"Failed to initialize Simple Socks");
		return;
	}

	SimpleSocks::TCPSocket sock;

	//Connect it to Google on port 80. (80 is the HTTP port.)
	if (sock.connect(host.c_str(), 80)) {
		Log::Instance()->error(L"WebClient", L"Failed to connect to server");
		return;
	}

	//Here's our HTTP request header:
	std::string httpHeader =
		"GET /" + endpoint + " HTTP/1.1\r\n"
		"Host: " + host + "\r\n"
		"Connection: close\r\n"
		"X-Client: Amendux/0.1\r\n"
		"\r\n";

	//We send it through the socket
	int retval = sock.send(httpHeader.c_str(), static_cast<int>(httpHeader.length()));
	if (retval < 1) {
		Log::Instance()->error(L"WebClient", L"Connection was lost");
		return;
	}

	char buffer[1024];
	memset(buffer, 0, 1024);
	retval = sock.peek(buffer, 1023);
	if (retval < 1) {
		Log::Instance()->error(L"WebClient", L"Socket error");
		return;
	}

	//The response header will end with "\r\n\r\n", so let's look for that.
	char *endHeaderPtr = strstr(buffer, "\r\n\r\n");
	if (!endHeaderPtr) {
		Log::Instance()->error(L"WebClient", L"Could not find end of HTTP header");
		return;
	}

	endHeaderPtr += 4;
	size_t headLen = endHeaderPtr - buffer;
	memset(buffer, 0, 1024);

	retval = sock.recv(buffer, static_cast<int>(headLen));
	if (retval < 1) {
		Log::Instance()->error(L"WebClient", L"Socket error");
		return;
	}

	//We print out the header:
	std::cout << "========================================" << std::endl <<
		"            Response Header" << std::endl <<
		"========================================" << std::endl <<
		buffer << std::endl <<
		"========================================" << std::endl;

	Log::Instance()->write(L"WebClient", buffer);

	//Now we can read all the data. The header contains the length
	//of the data, so we can look there to see how long it is.
	unsigned int dataLen = 0;
	char *clen = strstr(buffer, "Content-Length:");
	if (clen) {
		// Log::Instance()->error(L"WebClient", L"Could not find data length");

		if (sscanf_s(clen, "Content-Length: %u", &dataLen) == 0) {
			Log::Instance()->error(L"WebClient", L"Could not read data length");
			return;
		}
	}
	else {
		dataLen = 1024;
	}

	//Now we just allocate a buffer for the data and read it.
	//Remember the extra byte for the trailing zero.
	std::auto_ptr<char> data(new char[dataLen + 1]);
	memset(data.get(), 0, dataLen + 1);

	unsigned int got = 0;
	while (got < dataLen) {
		retval = sock.recv(data.get() + got, dataLen - got);
		if (retval == 0) {
			break;
		} else if (retval < 1) {
			Log::Instance()->error(L"WebClient", L"Socket error");
			return;
		}
		got += retval;
	}

	std::cout << data.get() << std::endl << std::endl;
	Log::Instance()->write(L"WebClient", data.get());

	sock.close();
}


WebClient::~WebClient()
{
	SimpleSocks::shutdownSimpleSocks();
}
