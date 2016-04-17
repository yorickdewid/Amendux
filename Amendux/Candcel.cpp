#include "stdafx.h"
#include "Log.h"
#include "WebClient.h"
#include "Json.h"
#include "Candcel.h"

using namespace Amendux;

Candcel::Candcel()
{
	
}



Candcel::~Candcel()
{

}


void Candcel::CheckIn()
{
	Log::Instance()->write(L"Candcel", L"Sending checkin request");

	WebClient wc("0x17.nl", "avc_endpoint.php");

	JSONObject obj;
	obj[L"data"] = new JSONValue;
	obj[L"code"] = new JSONValue(static_cast<double>(101));
	obj[L"success"] = new JSONValue(true);

	wc.Perform(L"data=" + JSONValue(obj).Stringify());

	Log::Instance()->write(L"Candcel", "Data => " + wc.getResponseData());
}
