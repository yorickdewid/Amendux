#pragma once

namespace Amendux {

	class Log
	{
		std::wofstream _log;
		static Log *s_Log;

		Log() {
#ifdef DEBUG
			_log.open("Amendux.log");
			_log << "[Log] Initialize logger module" << std::endl;
#endif
		}

		~Log() {
#ifdef DEBUG
			_log << "[Log] Terminate logger module" << std::endl;
			_log.close();
#endif
		}

	public:
		void write(const std::wstring& mod, const std::wstring& s) {
#ifdef DEBUG
			_log << "[" << mod << "] " << s << std::endl;
#endif
		}

		void write(const std::wstring& mod, const std::string& s) {
#ifdef DEBUG
			std::wstring ws(s.begin(), s.end());
			_log << "[" << mod << "] " << ws << std::endl;
#endif
		}

		void write(const wchar_t *mod, const wchar_t *s) {
#ifdef DEBUG
			_log << "[" << mod << "] " << s << std::endl;
#endif
		}

		void error(const std::wstring& mod, const std::wstring& s) {
#ifdef DEBUG
			_log << "[" << mod << "] " << L"Error: " << s << std::endl;
#endif
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
