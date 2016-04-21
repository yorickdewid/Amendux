#pragma once

#include "Thread.h"

namespace Amendux {

	class Candcel
	{
		bool serverSolicitAck = false;

	public:
		Candcel() {}
		~Candcel() {}

		void isAlive();
		void Solicit();
		void Update();
		DWORD CheckIn();

		static void SpawnInterval(Candcel *cc) {
			Thread<Candcel> *thread = new Thread<Candcel>(cc, &Candcel::CheckIn);
			if (thread->Start()) {
				// std::cout << "Thread start()" << std::endl;
			}
			// thread.join(); // wait for thread
		}
	};

}
