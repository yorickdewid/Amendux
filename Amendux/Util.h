#pragma once

#include <vector>
#include <algorithm>

namespace Amendux {

	class Util
	{
	public:
		// TODO move to config
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
		};

		static PWCHAR getDirectory(Util::Directory folderId);
		static std::wstring generateUUID();
		static std::wstring Util::user();
		static std::wstring Util::machine();

		static std::string tolower(std::string& str) {
			std::transform(str.begin(), str.end(), str.begin(), ::tolower);
			return str;
		}

		static std::wstring tolower(std::wstring& str) {
			std::transform(str.begin(), str.end(), str.begin(), ::tolower);
			return str;
		}

		static size_t bytesInWCharStr(const wchar_t *wstr) {
			return wcslen(wstr) * sizeof(wchar_t);
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

	};

}
