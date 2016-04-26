#pragma once

#include <sstream>
#include "CoreInterface.h"

namespace Amendux {

	constexpr unsigned int clientVersion = 124;

	enum class OperationMode {
		BASE = 1,
		UPDATE,
		ELIMINATE,
		GUARD,
	};

	class Config : CoreInterface
	{
		static Config *s_Config;
		std::wstring instanceGUID;
		OperationMode currentMode = OperationMode::BASE;
		bool bSuccess = true;

		void LogEnvironment();
		void SetupDataDir();
		void SetupPersistentConfig();
		void CheckConfig();
		void InitClass();

	public:
		Config();
		~Config();

		inline std::wstring Guid() const {
			return instanceGUID;
		}

		inline std::wstring DataDirectory() const {
			std::wstring dataDir = std::wstring(Util::getDirectory(Util::Directory::USER_APPDATA));
			return dataDir + L"\\Amendux";
		}

		inline void SetMode(OperationMode om) {
			currentMode = om;
		}

		inline bool CanUpdate() const {
			switch (currentMode) {
				case OperationMode::UPDATE:
				case OperationMode::ELIMINATE:
				case OperationMode::GUARD:
					return false;
				default:
					break;
			}

			return true;
		}

		inline bool CanConnect() const {
			switch (currentMode) {
				case OperationMode::UPDATE:
				case OperationMode::GUARD:
					return false;
				default:
					break;
			}

			return true;
		}

		inline bool CanRunModules() const {
			switch (currentMode) {
			case OperationMode::UPDATE:
			case OperationMode::GUARD:
				return false;
			default:
				break;
			}

			return true;
		}

		inline bool CanInfect() const {
			switch (currentMode) {
			case OperationMode::UPDATE:
			case OperationMode::GUARD:
				return false;
			default:
				break;
			}

			return true;
		}

		static std::wstring getVersion() {
			unsigned int major = static_cast<unsigned int>(floor(clientVersion / 100));
			unsigned int minor = static_cast<unsigned int>(floor((clientVersion - (major * 100)) / 10));
			unsigned int patch = clientVersion % 10;

			return std::to_wstring(major) + L"." + std::to_wstring(minor) + L"." + std::to_wstring(patch);
		}

		static bool Success() {
			return s_Config->bSuccess;
		}

		static Config *Current() {
			if (!s_Config) {
				s_Config = new Config;
			}

			return s_Config;
		}

		static void Init() {
			if (!s_Config) {
				s_Config = new Config;
			}

			s_Config->InitClass();
		}

		static void Terminate() {
			if (s_Config) {
				delete s_Config;
				s_Config = nullptr;
			}
		}
	};

}