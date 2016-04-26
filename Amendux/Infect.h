#pragma once

#include "CoreInterface.h"

namespace Amendux {

	class Infect : CoreInterface
	{
		static Infect *s_Infect;
		void SetupHomeDirectory();
		void SetupRegisterHook();
		void SetupStartupFolder();
		void InitClass();

	public:
		Infect();
		~Infect();

		static Infect *Current() {
			if (!s_Infect) {
				s_Infect = new Infect;
			}

			return s_Infect;
		}

		static void Init() {
			if (!s_Infect) {
				s_Infect = new Infect;
			}

			s_Infect->InitClass();
		}

		static void Terminate() {
			if (s_Infect) {
				delete s_Infect;
				s_Infect = nullptr;
			}
		}
	};

}
