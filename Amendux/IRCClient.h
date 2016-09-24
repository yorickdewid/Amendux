#pragma once

#include "SimpleSocksNS.h"
#include "TCPSocket.h"

//#include "Module.h"

namespace Amendux {

	class IRCClient //: Module
	{
		SimpleSocks::TCPSocket *sock = nullptr;
		std::multimap<std::string, std::pair<std::string, std::string>> map;
		const char *serverIdent = nullptr;
		const char *nickname = nullptr;
		const char *authname = nullptr;

	public:
		//IRCClient(ModuleLoader *loader) : Module(L"ircclient", loader) {}
		IRCClient() {}

		inline void SetNickName(const char *name) {
			nickname = name;
		}

		DWORD Run();
		int LeaveChannel(const char *channel);
		void GenerateNname(char *name);
		int Connect(const char *host);
		int Quit();
		const char *GetIdentName(std::string& name);
		int Read();
		int SendUser();
		int SendNick();
		int SendPing();
		int JoinChannel(const char *channel);
		int SendChatMessage(const char *channel, const char *message);
		int SendPong();
		void ChatLoop();
	};

}
