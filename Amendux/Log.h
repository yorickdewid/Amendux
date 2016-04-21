#pragma once

namespace Amendux {

	class Log
	{
		std::wofstream _log;
		static Log *s_Log;

		inline std::wstring getTimestamp() {
			wchar_t buff[20];

			time_t rawtime = time(NULL);
			struct tm timeinfo;
			gmtime_s(&timeinfo, &rawtime);

			wcsftime(buff, sizeof(buff), L"%Y-%m-%d %H:%M:%S", &timeinfo);

			return buff;
		}

	public:
		Log() {
#ifdef DEBUG
			_log.open("Amendux.log");
			_log << getTimestamp() << " [Log] Initialize logger module" << std::endl;
#endif
		}

		~Log() {
#ifdef DEBUG
			_log << getTimestamp() << " [Log] Terminate logger module" << std::endl;
			_log.close();
#endif
		}

	public:
		inline void write(const std::wstring& mod, const std::wstring& s) {
#ifdef DEBUG
			_log << getTimestamp() << " [" << mod << "] " << s << std::endl;
#endif
		}

		void write(const std::wstring& mod, const std::string& s) {
#ifdef DEBUG
			std::wstring ws(s.begin(), s.end());
			_log << getTimestamp() << " [" << mod << "] " << ws << std::endl;
#endif
		}

		void write(const wchar_t *mod, const wchar_t *s) {
#ifdef DEBUG
			_log << getTimestamp() << " [" << mod << "] " << s << std::endl;
#endif
		}

		void error(const std::wstring& mod, const std::wstring& s) {
#ifdef DEBUG
			_log << getTimestamp() << " [" << mod << "] " << L"Error: " << s << std::endl;
#endif
		}

		void warn(const std::wstring& mod, const std::wstring& s) {
#ifdef DEBUG
			_log << getTimestamp() << " [" << mod << "] " << L"Warn: " << s << std::endl;
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
