#pragma once

#include <sstream>

namespace Amendux {

	constexpr unsigned int clientVersion = 103;

	class Config
	{
		static std::wstring instanceGUID;

		static void ShowEnvironment();
		static void InitDataDir();

	public:
		static void Init(Encrypt& encrypt); // TODO should remove this
		static void Terminate();

		static inline std::wstring Guid() {
			return instanceGUID;
		}

		static std::wstring getVersion() {
			unsigned int major = static_cast<unsigned int>(floor(clientVersion / 100));
			unsigned int minor = static_cast<unsigned int>(floor((clientVersion - (major * 100)) / 10));
			unsigned int patch = clientVersion % 10;

			return std::to_wstring(major) + L"." + std::to_wstring(minor) + L"." + std::to_wstring(patch);
		}
	};

}