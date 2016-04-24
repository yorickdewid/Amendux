#pragma once

#include "Thread.h"

namespace Amendux {

	class Candcel
	{
		bool serverSolicitAck = false;

		DWORD CheckIn();
		void GetUpdate(unsigned int buildNumber, const std::wstring& wurl);

	public:
		Candcel() {}
		~Candcel() {}

		void IsAlive();
		void Solicit();
		void CheckForUpdate();

		static void SpawnInterval(Candcel *cc) {
			Thread<Candcel> *thread = new Thread<Candcel>(cc, &Candcel::CheckIn);
			if (!thread->Start()) {
				Log::Error(L"Candcel", L"Cannot spawn checkin process");
			}
		}
	};

}
