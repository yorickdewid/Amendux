#pragma once

namespace Amendux {

	class Util
	{
	public:
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

		template <typename T> static std::string Hex(T data, int len) {
			constexpr char hexmap[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

			std::string hex(len * 2, ' ');
			for (int i = 0; i < len; ++i) {
				hex[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
				hex[2 * i + 1] = hexmap[data[i] & 0x0F];
			}

			return hex;
		}

	};

}
