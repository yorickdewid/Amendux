#pragma once

namespace Amendux {

	class Config
	{
		static std::wstring instanceGUID;

		static void ShowEnvironment();
		static void InitDataDir();

	public:
		static void Init(Encrypt& encrypt);
		static void Terminate();

		static inline std::wstring Guid() {
			return instanceGUID;
		}
	};

}