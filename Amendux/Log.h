#pragma once

#include "CoreInterface.h"

#if DEBUG
#define LogInfo(m,e) Log::Info(m,e)
#define LogWarn(m,e) Log::Warn(m,e)
#define LogError(m,e) Log::Error(m,e)
#else
#define LogInfo(m,e)
#define LogWarn(m,e)
#define LogError(m,e)
#endif

// LOGDIR
//#define LOGDIR			L"."

namespace Amendux {

#ifdef DEBUG
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
			localtime_s(&timeinfo, &rawtime);

			wcsftime(buff, sizeof(buff), L"%Y-%m-%d %H:%M:%S", &timeinfo);

			return buff;
		}

		void _write(const std::wstring& mod, const std::wstring& ws, LogType type = _INFO) {
			switch (type) {
				case Amendux::Log::_WARN:
					_log << getTimestamp() << " [" << mod << "] " << L"Warning: " << ws << std::endl;
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
			DWORD pid = Util::currentProcessId();
#ifdef LOGDIR
			_log.open((LOGDIR L"\\amendux_" + std::to_wstring(pid) + L".log").c_str());
#else
			std::wstring loc = Util::getDirectory(Util::Directory::USER_DOCUMENTS);
			_log.open((loc + L"\\amendux_" + std::to_wstring(pid) + L".log").c_str());
#endif
			_log << getTimestamp() << " [Log] Initialize logger class" << std::endl;
		}

		~Log() {
			_log << getTimestamp() << " [Log] Terminate logger class" << std::endl;
			_log.close();
		}

		static void Init() {
			if (!s_Log) {
				s_Log = new Log;
			}

			s_Log->InitClass();
		}

		static void Info(const std::wstring& mod, const std::wstring& s) {
			if (!s_Log) {
				return;
			}

			return s_Log->_write(mod, s);
		}

		static void Info(const std::wstring& mod, const std::string& s) {
			if (!s_Log) {
				return;
			}

			return s_Log->_write(mod, s);
		}

		static void Warn(const std::wstring& mod, const std::wstring& s) {
			if (!s_Log) {
				return;
			}

			return s_Log->_write(mod, s, LogType::_WARN);
		}

		static void Warn(const std::wstring& mod, const std::string& s) {
			if (!s_Log) {
				return;
			}

			return s_Log->_write(mod, s, LogType::_WARN);
		}

		static void Error(const std::wstring& mod, const std::wstring& s) {
			if (!s_Log) {
				return;
			}

			return s_Log->_write(mod, s, LogType::_ERROR);
		}

		static void Error(const std::wstring& mod, const std::string& s) {
			if (!s_Log) {
				return;
			}

			return s_Log->_write(mod, s, LogType::_ERROR);
		}

		static std::wstring Readback() {
			DWORD pid = Util::currentProcessId();
#ifdef LOGDIR
			std::wifstream file((LOGDIR L"\\amendux_" + std::to_wstring(pid) + L".log").c_str());
#else
			std::wstring loc = Util::getDirectory(Util::Directory::USER_DOCUMENTS);
			std::wifstream file((loc + L"\\amendux_" + std::to_wstring(pid) + L".log").c_str());
#endif
			std::wstring line, text;
			while (std::getline(file, line)) {
				line += L"\r\n";
				text += line;
			}

			return text;
		}

		static void Terminate() {
			if (s_Log) {
				delete s_Log;
				s_Log = nullptr;
			}
		}

	};
#else
namespace Log
{
	static void Init() {}
	static void Terminate() {}
};
#endif

}
