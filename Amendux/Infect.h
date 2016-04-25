#pragma once

namespace Amendux {

	class Infect
	{
		static Infect *s_Infect;
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
