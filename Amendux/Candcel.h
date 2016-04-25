#pragma once

#include "CoreInterface.h"
#include "Thread.h"

namespace Amendux {

	class Candcel : CoreInterface
	{
		static Candcel *s_Candcel;
		bool serverSolicitAck = false;

		DWORD CheckIn();
		void GetUpdate(unsigned int buildNumber, const std::wstring& wurl);
		void IsAlive();
		void Solicit();
		void CheckForUpdate();
		void InitClass();

	public:
		Candcel();
		~Candcel();

		static void SpawnInterval(Candcel *cc) {
			if (!Config::Current()->CanConnect()) {
				return;
			}

			Thread<Candcel> *thread = new Thread<Candcel>(cc, &Candcel::CheckIn);
			if (!thread->Start()) {
				Log::Error(L"Candcel", L"Cannot spawn checkin process");
			}
		}

		static Candcel *Current() {
			if (!s_Candcel) {
				s_Candcel = new Candcel;
			}

			return s_Candcel;
		}

		static void Init() {
			if (!s_Candcel) {
				s_Candcel = new Candcel;
			}

			s_Candcel->InitClass();
		}

		static void Terminate() {
			if (s_Candcel) {
				delete s_Candcel;
				s_Candcel = nullptr;
			}
		}

	};

}
