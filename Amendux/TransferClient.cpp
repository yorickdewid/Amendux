#include "stdafx.h"
#include "TransferClient.h"

using namespace Amendux;

bool TransferClient::ParseResponse(char *data)
{
	std::string dataType = getResponseHeader("content-type");
	if (dataType.empty()) {
		return false;
	}

	//if (dataType != "application/json") {
	//	return false;
	//}

	std::ofstream outfile("putty.exe", std::ofstream::binary);
	outfile.write(data, getResponseSize());
	outfile.close();

	callSuccess = true;

	return true;
}