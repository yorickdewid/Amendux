#include "stdafx.h"
#include "Log.h"
#include "RestClient.h"
#include "Json.h"
#include "Candcel.h"

using namespace Amendux;

void Candcel::CheckIn()
{
	Log::Instance()->write(L"Candcel", L"Sending checkin request");

	RestClient rc("0x17.nl", "avc_endpoint.php");

	JSONObject obj;
	obj[L"data"] = new JSONValue;
	obj[L"code"] = new JSONValue(static_cast<double>(101));
	obj[L"success"] = new JSONValue(true);

	rc.Perform(L"data=" + JSONValue(obj).Stringify());

	Log::Instance()->write(L"Candcel", "Data => " + rc.getResponseData());
}
