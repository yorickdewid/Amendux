#pragma once

#include <map>
#include <vector>
#include <algorithm>

namespace Amendux {

	namespace Util {
		enum class Directory {
			USER_DOCUMENTS,
			USER_CONTACT,
			USER_DESKTOP,
			USER_DOWNLOAD,
			USER_FAVORITE,
			USER_APPDATA,
			USER_PICTURES,
			USER_MUSIC,
			USER_VIDEOS,
			USER_STARTUP,
		};

		PWCHAR getDirectory(Directory folderId);
		std::wstring generateUUID();
		std::wstring user();
		std::wstring machine();
		std::wstring winver();
		DWORD cpuCores();
		DWORD maxmem();
		std::wstring tempFile();
		HRESULT CreateHardLink(LPCWSTR lpszPathObj, LPCWSTR lpszPathLink, LPCWSTR lpszDesc);
		bool CreateSoftLink(LPTSTR lpSymlinkFileName, LPTSTR lpTargetFileName);
		bool CopyFile(LPTSTR lpExistingFileName, LPTSTR lpNewFileName);
		std::wstring CurrentDirectory();
		std::map<std::wstring, std::wstring> *EnvVariables();

		static std::wstring windowResolution() {
			std::wstring res;

			int x = ::GetSystemMetrics(SM_CXSCREEN);
			int y = ::GetSystemMetrics(SM_CYSCREEN);

			res.append(std::to_wstring(x));
			res.append(L"x");
			res.append(std::to_wstring(x));

			return res;
		}

		static std::wstring timezoneName() {
			TIME_ZONE_INFORMATION tzi = {0};
			
			if (GetTimeZoneInformation(&tzi) == TIME_ZONE_ID_INVALID) {
				return L"";
			}

			return tzi.StandardName;
		}

		static std::wstring currentModule() {
			WCHAR szFileName[MAX_PATH];

			::GetModuleFileName(NULL, szFileName, MAX_PATH);

			return szFileName;
		}

		static DWORD currentProcessId() {
			return ::GetCurrentProcessId();
		}

		static bool fileExist(LPCTSTR szPath) {
			DWORD dwAttrib = ::GetFileAttributes(szPath);

			return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
		}

		static bool deleteFile(LPCTSTR lpFileName) {
			return ::DeleteFile(lpFileName) != NULL;
		}

		static bool createDirectory(std::wstring dir) {
			if (CreateDirectory(dir.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
				return true;
			}

			return false;
		}

		static std::string tolower(std::string& str) {
			std::transform(str.begin(), str.end(), str.begin(), ::tolower);
			return str;
		}

		static size_t bytesInWCharStr(const wchar_t *wstr) {
			return wcslen(wstr) * sizeof(wchar_t);
		}

		static int randBetween(const int min = 0, const int max = 1000) {
			return rand() % (max - min + 1) + min;
		}

		static void generateString(wchar_t *s, const int len = 8) {
			constexpr wchar_t alphanum[] = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

			for (int i = 0; i < len; ++i) {
				s[i] = alphanum[rand() % ((sizeof(alphanum) / sizeof(alphanum[0])) - 1)];
			}

			s[len] = 0;
		}

		static wchar_t *chartowchar(const char *str) {
			size_t size = strlen((char *)str) + 1;
			wchar_t *wa = new wchar_t[size];

			size_t outSize;
			mbstowcs_s(&outSize, wa, size, (char *)str, size-1);
			return wa;
		}

		template <typename T> static void rot13(T str[]) {
			for (int i = 0; str[i] != '\0'; i++) {
				if (str[i] >= 'a' && str[i] <= 'm') {
					str[i] += 13;
				} else if (str[i] > 'm' && str[i] <= 'z') {
					str[i] -= 13;
				} else if (str[i] >= 'A' && str[i] <= 'M') {
					str[i] += 13;
				} else if (str[i] > 'M' && str[i] <= 'Z') {
					str[i] -= 13;
				} else if (str[i] == '{') {
					str[i] = '\\';
				} else if (str[i] == '}') {
					str[i] = '(';
				} else if (str[i] == '[') {
					str[i] = ')';
				} else if (str[i] == '?') {
					str[i] = '/';
				} else if (str[i] == '&') {
					str[i] = ' ';
				} else if (str[i] == '*') {
					str[i] = '_';
				} else if (str[i] == '^') {
					str[i] = '.';
				}
			}
		}

		template <typename T> static std::string hex(T data, int len) {
			constexpr char hexmap[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

			std::string hex(len * 2, ' ');
			for (int i = 0; i < len; ++i) {
				hex[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
				hex[2 * i + 1] = hexmap[data[i] & 0x0F];
			}

			return hex;
		}

		template <typename T> static void split(const T& str, const char *delim, std::vector<T>& list) {
			char *dup = _strdup(str.c_str());
			char *next_token = NULL;
			char *token = strtok_s(dup, delim, &next_token);
			while (token != NULL) {
				list.push_back(T(token));
				token = strtok_s(NULL, delim, &next_token);
			}
			free(dup);
		}

		template <typename T> static T trim(T& str) {
			size_t first = str.find_first_not_of(' ');
			size_t last = str.find_last_not_of(' ');
			return str.substr(first, (last - first + 1));
		}

	}

}
