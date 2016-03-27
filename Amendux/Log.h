#pragma once

namespace Amendux {

	class Log
	{
		std::wofstream _log;
		static Log *s_Log;

		Log() {
			_log.open("Amendux.log");
			_log << "[Log] Initialize logger module" << std::endl;
		}

		~Log() {
			_log << "[Log] Terminate logger module" << std::endl;
			_log.close();
		}

	public:
		void write(const std::wstring& mod, const std::wstring& s) {
			_log << "[" << mod << "] " << s << std::endl;
		}

		void write(const wchar_t *mod, const wchar_t *s) {
			_log << "[" << mod << "] " << s << std::endl;
		}

		static void Init() {
			if (!s_Log)
				s_Log = new Log;
		}

		static Log *Instance() {
			if (!s_Log)
				s_Log = new Log;

			return s_Log;
		}

		static void Terminate() {
			if (s_Log)
				s_Log = nullptr;
		}

	};

}
