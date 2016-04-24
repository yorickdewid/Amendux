#include "stdafx.h"
#include "TransferClient.h"

using namespace Amendux;

std::wstring TransferClient::ParseResponse(char *data)
{
	std::string dataType = getResponseHeader("content-type");
	if (dataType.empty()) {
		return L"";
	}

	std::wstring tempFile = Util::tempFile();

	std::ofstream outfile(tempFile, std::ofstream::binary);
	outfile.write(data, getResponseSize());
	outfile.close();

	callSuccess = true;

	return tempFile;
}