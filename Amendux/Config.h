#pragma once

namespace Amendux {

	class Config
	{
		static void Environment();

	public:
		static void Init(Encrypt& encrypt);
		static void Terminate();
	};

}