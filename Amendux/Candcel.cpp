#include "stdafx.h"
#include "Log.h"
#include "WebClient.h"
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

	WebClient("0x17.nl", "avc_endpoint.php");
}
