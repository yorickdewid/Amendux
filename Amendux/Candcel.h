#pragma once

#include "Thread.h"

namespace Amendux {

	class Candcel
	{
		bool serverSolicitAck = false;

		DWORD CheckIn();

	public:
		Candcel() {}
		~Candcel() {}

		void isAlive();
		void Solicit();
		void Update();

		static void SpawnInterval(Candcel *cc) {
			Thread<Candcel> *thread = new Thread<Candcel>(cc, &Candcel::CheckIn);
			if (!thread->Start()) {
				Log::Instance()->error(L"Candcel", L"Cannot spawn checkin process");
			}
		}
	};

}
