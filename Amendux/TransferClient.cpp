#include "stdafx.h"
#include "log.h"
#include "TransferClient.h"

using namespace Amendux;

std::wstring TransferClient::ParseResponse(char *data)
{
	std::string dataType = getResponseHeader("content-type");
	if (dataType.empty()) {
		return L"";
	}

	if (static_cast<HttpCode>(getResponseCode()) != HttpCode::HTTP_OK) {
		LogWarn(L"TransferClient", L"Cannot get file, server returned HTTP " + std::to_wstring(getResponseCode()));
		return L"";
	}

	std::wstring tempFile = Util::tempFile();

	std::ofstream outfile(tempFile, std::ofstream::binary);
	outfile.write(data, getResponseSize());
	outfile.close();

	callSuccess = true;

	return tempFile;
}