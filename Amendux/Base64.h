#pragma once

#include <string>

std::string base64_encode(std::string const& s);
std::string base64_decode(std::string const& s);

std::wstring base64_encode(std::wstring const& s);
