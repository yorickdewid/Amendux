#include "stdafx.h"
#include "Log.h"
#include "BAse64.h"
#include "RestClient.h"

using namespace Amendux;

JSONValue *RestClient::ParseResponse(char *data)
{
	if (!data) {
		return nullptr;
	}

	std::string dataType = getResponseHeader("content-type");
	if (dataType.empty()) {
		return nullptr;
	}

	if (static_cast<HttpCode>(getResponseCode()) != HttpCode::HTTP_OK) {
		Log::Warn(L"RestClient", L"Call failed, server returned HTTP " + std::to_wstring(getResponseCode()));
		return nullptr;
	}

	if (dataType != "application/json") {
		return nullptr;
	}

	std::string wdata = base64_decode((char *)data);

	JSONValue *obj = JSON::Parse(wdata.c_str());

	if (!obj->IsObject()) {
		Log::Error(L"RestClient", L"Server returned no object");
		return nullptr;
	}

	if (!obj->Child(L"success")->AsBool()) {
		Log::Error(L"RestClient", L"Server returned error status");
		return nullptr;
	}

	if (!obj->Child(L"code")->IsNumber()) {
		Log::Error(L"RestClient", L"Server returned no statuscode");
		return nullptr;
	}

	serverCode = static_cast<RestServerCommand>(static_cast<unsigned int>(obj->Child(L"code")->AsNumber()));
	if (serverCode == RestServerCommand::CM_SERVER_INVALID) {
		Log::Error(L"RestClient", L"Server returned invalid command");
		return nullptr;
	}

	callSuccess = true;

	return obj->Child(L"data");
}
