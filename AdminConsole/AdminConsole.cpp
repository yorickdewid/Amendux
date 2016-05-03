// AdminConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <curl\curl.h>

bool connectEndpoint(const std::string& endpoint);

int main()
{
	std::string endpoint;
	std::cout << "********************************************************************************" << std::endl;
	std::cout << "*                                [ ADMIN CONSOLE ]                             *" << std::endl;
	std::cout << "********************************************************************************" << std::endl;

	std::cout << "connect> ";

	std::cin >> endpoint;
	connectEndpoint(endpoint);

	while (true) {
		std::string command;

		std::cout << "(admin) console> ";
		std::getline(std::cin, command);
		if (command.empty()) {
			std::cout << std::endl;
			continue;
		}

		if (!command.compare("exit") || !command.compare("quit")) {
			break;
		}

		std::cout << "Unknown command '" << command << "'" << std::endl;
	}

    return 0;
}

static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

bool connectEndpoint(const std::string& endpoint)
{
	std::cout << "Conneting to " + endpoint + "..." << std::endl;

	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);

		/* Check for errors */
		if (res != CURLE_OK) {
			std::cerr << "Connection failed" << std::endl;
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
	}

	return true;
}
