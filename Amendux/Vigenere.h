#pragma once

#include <string>

namespace Amendux {

	class Vigenere
	{
		std::string key;

	public:
		Vigenere(const std::string key) {
			for (int i = 0; i < key.size(); ++i) {
				if (key[i] >= 'A' && key[i] <= 'Z') {
					this->key += key[i];
				} else if (key[i] >= 'a' && key[i] <= 'z') {
					this->key += key[i] + 'A' - 'a';
				}
			}
		}

		std::wstring Encrypt(std::wstring const& s)
		{
			std::string rs = Encrypt(std::string(s.begin(), s.end()));

			return std::wstring(rs.begin(), rs.end());
		}

		std::wstring Decrypt(std::wstring const& s)
		{
			std::string rs = Decrypt(std::string(s.begin(), s.end()));

			return std::wstring(rs.begin(), rs.end());
		}

		std::string Encrypt(std::string text);
		std::string Decrypt(std::string text);
	};

}
