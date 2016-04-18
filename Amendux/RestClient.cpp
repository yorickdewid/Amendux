#include "stdafx.h"
#include "Log.h"
#include "RestClient.h"

using namespace Amendux;

void *RestClient::ParseResponse(void *data)
{
	std::string dataType = getResponseHeader("content-type");
	if (dataType.empty()) {
		return nullptr;
	}

	if (dataType != "application/json") {
		return nullptr;
	}

	wchar_t *wdata = Util::chartowchar((char *)data);
	JSONValue *obj = JSON::Parse(wdata);
	delete wdata;

	if (!obj->IsObject()) {
		Log::Instance()->error(L"RestClient", L"Server returned no object");
		return nullptr;
	}

	if (!obj->Child(L"success")->AsBool()) {
		Log::Instance()->error(L"RestClient", L"Server returned error status");
		return nullptr;
	}

	if (!obj->Child(L"code")->IsNumber()) {
		Log::Instance()->error(L"RestClient", L"Server returned no statuscode");
		return nullptr;
	}

	serverCode = static_cast<RestServerCommand>(static_cast<unsigned int>(obj->Child(L"code")->AsNumber()));
	if (serverCode == RestServerCommand::CM_CLIENT_INVALID) {
		Log::Instance()->error(L"RestClient", L"Server returned invalid command");
		return nullptr;
	}

	callSuccess = true;

	return obj->Child(L"data");
}
