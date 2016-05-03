#pragma once

#include "CoreInterface.h"

namespace Amendux {

	class Log : CoreInterface
	{
	private:
		std::wofstream _log;
		static Log *s_Log;

		enum LogType {
			_INFO,
			_WARN,
			_ERROR,
		};

		inline std::wstring getTimestamp() {
			wchar_t buff[20];

			time_t rawtime = time(NULL);
			struct tm timeinfo;
			gmtime_s(&timeinfo, &rawtime);

			wcsftime(buff, sizeof(buff), L"%Y-%m-%d %H:%M:%S", &timeinfo);

			return buff;
		}

		void _write(const std::wstring& mod, const std::wstring& ws, LogType type = _INFO) {
			switch (type) {
				case Amendux::Log::_WARN:
					_log << getTimestamp() << " [" << mod << "] " << L"Warn: " << ws << std::endl;
					break;
				case Amendux::Log::_ERROR:
					_log << getTimestamp() << " [" << mod << "] " << L"Error: " << ws << std::endl;
					break;
				case Amendux::Log::_INFO:
				default:
					_log << getTimestamp() << " [" << mod << "] " << ws << std::endl;
					break;
			}
		}

		void _write(const std::wstring& mod, const std::string& s, LogType type = _INFO) {
			std::wstring ws(s.begin(), s.end());
			_write(mod, ws, type);
		}

		void InitClass() {}

	public:
		Log() {
			std::wstring loc = Util::getDirectory(Util::Directory::USER_DOCUMENTS);
			_log.open((loc + L"\\Amendux.log").c_str());
			_log << getTimestamp() << " [Log] Initialize logger class" << std::endl;
		}

		~Log() {
			_log << getTimestamp() << " [Log] Terminate logger class" << std::endl;
			_log.close();
		}

		static void Init() {
#ifdef DEBUG
			if (!s_Log) {
				s_Log = new Log;
			}

			s_Log->InitClass();
#endif
		}

		static void Info(const std::wstring& mod, const std::wstring& s) {
#ifdef DEBUG
			if (!s_Log) {
				return;
			}

			return s_Log->_write(mod, s);
#endif
		}

		static void Info(const std::wstring& mod, const std::string& s) {
#ifdef DEBUG
			if (!s_Log) {
				return;
			}

			return s_Log->_write(mod, s);
#endif
		}

		static void Warn(const std::wstring& mod, const std::wstring& s) {
#ifdef DEBUG
			if (!s_Log) {
				return;
			}

			return s_Log->_write(mod, s, LogType::_WARN);
#endif
		}

		static void Warn(const std::wstring& mod, const std::string& s) {
#ifdef DEBUG
			if (!s_Log) {
				return;
			}

			return s_Log->_write(mod, s, LogType::_WARN);
#endif
		}

		static void Error(const std::wstring& mod, const std::wstring& s) {
#ifdef DEBUG
			if (!s_Log) {
				return;
			}

			return s_Log->_write(mod, s, LogType::_ERROR);
#endif
		}

		static void Error(const std::wstring& mod, const std::string& s) {
#ifdef DEBUG
			if (!s_Log) {
				return;
			}

			return s_Log->_write(mod, s, LogType::_ERROR);
#endif
		}

		static std::wstring Readback() {
#ifdef DEBUG
			std::wstring loc = Util::getDirectory(Util::Directory::USER_DOCUMENTS);
			std::wstring line;
			std::wstring text;
			std::wifstream file((loc + L"\\Amendux.log").c_str());
			while (std::getline(file, line)) {
				line += L"\r\n";
				text += line;
			}

			return text;
#endif
		}

		static void Terminate() {
#ifdef DEBUG
			if (s_Log) {
				delete s_Log;
				s_Log = nullptr;
			}
#endif
		}

	};

}
