#pragma once

namespace Amendux {

	class Config
	{
		static std::wstring tempGUID;

		static void ShowEnvironment();
		static void InitDataDir();

	public:
		static void Init(Encrypt& encrypt);
		static void Terminate();
	};

}