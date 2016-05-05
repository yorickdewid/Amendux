// AdminConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <curl\curl.h>

void theW00t();
bool connectEndpoint(const std::string& endpoint, const std::string& auth);
static bool bIsConnected = false;

BOOL WINAPI consoleHandler(DWORD signal) {

	if (signal == CTRL_C_EVENT) {
		printf("Ctrl-C handled\n"); // do cleanup
	}

	return TRUE;
}

int main()
{
	std::string endpoint;
	std::string userpass;
	std::cout << "********************************************************************************" << std::endl;
	std::cout << "*                                [ ADMIN CONSOLE ]                             *" << std::endl;
	std::cout << "********************************************************************************" << std::endl;

	//if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
	//	std::cerr << "Could not set control handler" << std::endl;
	//	return 1;
	//}

	do {
		std::cout << "(endpoint) console> ";
		std::cin >> endpoint;

		std::cout << "(auth) console> ";
		std::cin >> userpass;

		connectEndpoint(endpoint + "?admin=true", userpass);

	} while (!bIsConnected);

	std::cin.ignore(10000, '\n');

	while (true) {
		std::string command;

		std::cout << "(admin) console> ";
		std::getline(std::cin, command);
		if (command.empty()) {
			continue;
		}

		if (!command.compare("help") || !command.compare("?")) {
			std::cout << "  help\t\t\tShow help" << std::endl;
			std::cout << "  instances\t\tList all instances" << std::endl;
			std::cout << "  settings\t\tList current settings" << std::endl;
			std::cout << "  set <key> <value>\tSet key to value" << std::endl;
			std::cout << "  show <instance>\tShow info about instance" << std::endl;

			continue;
		}

		if (!command.compare("w00t!")) {
			theW00t();
			continue;
		}

		if (!command.compare("exit") || !command.compare("quit")) {
			break;
		}

		std::cerr << "Unknown command '" << command << "'" << std::endl;
	}

    return 0;
}

static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

bool connectEndpoint(const std::string& endpoint, const std::string& auth)
{
	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Amendux Admin Console/0.1");
		curl_easy_setopt(curl, CURLOPT_USERPWD, auth.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);

		/* Check for errors */
		if (res != CURLE_OK) {
			std::cerr << "Connection to admin interface failed" << std::endl;
		}

		long http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		if (http_code == 200) {
			bIsConnected = true;
			//Succeeded
		} else if (http_code == 401) {
			std::cerr << "Authentication failed" << std::endl;
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
	}

	return true;
}

void theW00t()
{
	std::cout << "                   ,%%%," << std::endl;
	std::cout << "                 ,%%%` %==--" << std::endl;
	std::cout << "                ,%%`( '|" << std::endl;
	std::cout << "               ,%%@ /\\_/" << std::endl;
	std::cout << "     ,%.-\"\"\"--%%% \"@@__  " << std::endl;
	std::cout << "    %%/             |__`\\       " << std::endl;
	std::cout << "   .%'\\     |   \\   /  //" << std::endl;
	std::cout << "   ,%' >   .'----\\ |  [/" << std::endl;
	std::cout << "      < <<`       ||      " << std::endl;
	std::cout << "      ||          ||        " << std::endl;
	std::cout << "      )\\\\         )\\" << std::endl;
	std::cout << "Unicorn inside..." << std::endl;
}
