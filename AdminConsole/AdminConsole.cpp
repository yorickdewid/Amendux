// AdminConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <curl\curl.h>
#include "Json.h"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <iomanip>


void theW00t();
JSONValue *RestCall(const std::string& endpoint, const std::string& auth, const std::string& data);

std::string name;
std::string endpoint;
std::string userpass;

BOOL WINAPI consoleHandler(DWORD signal) {

	if (signal == CTRL_C_EVENT) {
		printf("Ctrl-C handled\n"); // do cleanup
	}

	return TRUE;
}

int main(int argc, char *argv[], char *envp[])
{
	std::cout << "********************************************************************************" << std::endl;
	std::cout << "*                                [ ADMIN CONSOLE ]                             *" << std::endl;
	std::cout << "********************************************************************************" << std::endl;

	if (argc == 3) {
		endpoint = argv[1];
		userpass = argv[2];
	}

	//if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
	//	std::cerr << "Could not set control handler" << std::endl;
	//	return 1;
	//}
	JSONValue *response;

	do {
		if (endpoint.empty() || userpass.empty()) {
			std::cout << "endpoint> ";
			std::cin >> endpoint;

			std::cout << "auth> ";
			std::cin >> userpass;
		}
	} while (!(response = RestCall(endpoint, userpass, "data={\"code\":700,\"success\":true,\"data\":null}")));

	std::wstring wname;
	wname = response->Child(L"name")->AsString();
	name = std::string(wname.begin(), wname.end());

	std::cout << "Instances:\t" << response->Child(L"instances")->AsNumber() << std::endl;
	std::cout << "Checkins:\t" << response->Child(L"checkins")->AsNumber() << std::endl;

	std::cin.ignore(10000, '\n');

	while (true) {
		std::string command;

		std::cout << "(" << name << ")> ";
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
			std::cout << "  password <passwd>\tChange master password" << std::endl;
			std::cout << "  update <file> <build>\tDispatch new version to server" << std::endl;

			continue;
		}

		if (!command.compare("w00t!")) {
			theW00t();
			continue;
		}

		if (!command.compare("instances")) {
			response = RestCall(endpoint, userpass, "data={\"code\":701,\"success\":true,\"data\":null}");
			if (!response) {
				continue; // discon?
			}

			if (!response->IsArray()) {
				continue;
			}

			std::cout << "GUID                                   | IP           | OS" << std::endl;
			std::cout << "----------------------------------------+-------------+-----------" << std::endl;

			for (auto const& inst : response->AsArray()) {
				if (!inst->IsObject()) {
					continue;
				}
			
				std::wcout << inst->Child(L"guid")->AsString() << " | " << inst->Child(L"remote")->AsString() << " | " << inst->Child(L"os_version")->AsString() << std::endl;
			}

			continue;
		}

		if (!command.compare("settings")) {
			response = RestCall(endpoint, userpass, "data={\"code\":703,\"success\":true,\"data\":null}");
			if (!response) {
				continue; // discon?
			}

			if (!response->IsObject()) {
				continue;
			}

			std::cout << std::left << std::setw(20) << std::setfill(' ') << "Key";
			std::cout << "| " << std::left << std::setw(50) << std::setfill(' ') << "Value" << std::endl;
			
			std::cout << std::right << std::setw(21) << std::setfill('-') << '+' << std::setw(50) << std::setfill('-') << ' ' << std::endl;

			JSONObject obj = response->AsObject();
			JSONObject::iterator it_obj;
			for (it_obj = obj.begin(); it_obj != obj.end(); it_obj++) {
				std::wcout << std::left << std::setw(20) << std::setfill(L' ') << it_obj->first;
				std::wcout << L"| " << std::left << it_obj->second->AsString() << std::endl;
			}

			continue;
		}

		if (!command.substr(0, 4).compare("show")) {
			std::string quid = command.substr(5);
			if (quid.length() != 38) {
				std::cerr << "GUID malformed" << std::endl;
				continue;
			}

			response = RestCall(endpoint, userpass, "data={\"code\":701,\"success\":true,\"data\":null}");
			if (!response) {
				continue;
			}

			if (!response->IsArray()) {
				continue;
			}

			for (auto const& inst : response->AsArray()) {
				if (!inst->IsObject()) {
					continue;
				}

				std::wstring wquid = inst->Child(L"guid")->AsString();
				std::string _quid(wquid.begin(), wquid.end());
				if (!quid.compare(_quid)) {

					JSONObject obj = inst->AsObject();
					JSONObject::iterator it_obj;
					for (it_obj = obj.begin(); it_obj != obj.end(); it_obj++) {
						std::wcout << it_obj->first << "\t\t| ";
						if (it_obj->second->IsNull()) {
							std::wcout << "" << std::endl;
						} else {
							std::wcout << it_obj->second->AsString() << std::endl;
						}
					}

				}

			}

			continue;
		}

		if (!command.substr(0, 3).compare("set")) {
			std::istringstream iss(command);

			std::vector<std::string> tokens;
			std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

			if (tokens.size() != 3) {
				std::cerr << "Too few arguments" << std::endl;
				continue;
			}

			std::string data = "data={\"code\":703,\"success\":true,\"data\":{\"" + tokens.at(1) + "\":\"" + tokens.at(2) + "\"}}";
			RestCall(endpoint, userpass, data);

			continue;
		}

		if (!command.substr(0, 8).compare("password")) {
			if (command.size() < 10) {
				std::cerr << "Too few arguments" << std::endl;
				continue;
			}

			std::string password = command.substr(9);
			if (password.length() < 22) {
				std::cerr << "Password must have 22 characters at least" << std::endl;
				continue;
			}

			std::string data = "data={\"code\":704,\"success\":true,\"data\":{\"password\":\"" + password + "\"}}";
			RestCall(endpoint, userpass, data);

			userpass = "admin:" + password;

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

JSONValue *RestCall(const std::string& endpoint, const std::string& auth, const std::string& data)
{
	CURL *curl;
	CURLcode res;
	std::string readBuffer;
	bool bSuccess = false;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Amendux Admin Console/0.3");
		curl_easy_setopt(curl, CURLOPT_USERPWD, auth.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
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
			bSuccess = true;
		} else if (http_code == 401) {
			std::cerr << "Authentication failed" << std::endl;
		} else {
			std::cerr << "Connection to admin interface failed" << std::endl;
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
	}

	if (!bSuccess) {
		return nullptr;
	}

	JSONValue *obj = JSON::Parse(readBuffer.c_str());
	if (!obj->IsObject()) {
		std::cerr << "Server returned no object" << std::endl;
	}

	if (!obj->Child(L"success")->AsBool()) {
		std::cerr << "Server returned error status" << std::endl;
	}

	if (!obj->Child(L"code")->IsNumber()) {
		std::cerr << "Server returned no statuscode" << std::endl;
	}

	return obj->Child(L"data");
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
