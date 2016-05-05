#pragma once

// Win32 incompatibilities
#if defined(WIN32) && !defined(__GNUC__)
static inline bool isnan(double x) { return x != x; }
static inline bool isinf(double x) { return !isnan(x) && isnan(x - x); }
#endif

#include <vector>
#include <string>
#include <map>

// Simple function to check a string 's' has at least 'n' characters
static inline bool simplejson_wcsnlen(const wchar_t *s, size_t n) {
	if (s == 0)
		return false;

	const wchar_t *save = s;
	while (n-- > 0)
	{
		if (*(save++) == 0) return false;
	}

	return true;
}

// Custom types
class JSONValue;
typedef std::vector<JSONValue*> JSONArray;
typedef std::map<std::wstring, JSONValue*> JSONObject;

#include "JSONValue.h"

class JSON
{
	friend class JSONValue;

public:
	static JSONValue *Parse(const char *data);
	static JSONValue *Parse(const wchar_t *data);
	static std::wstring Stringify(const JSONValue *value);

protected:
	static bool SkipWhitespace(const wchar_t **data);
	static bool ExtractString(const wchar_t **data, std::wstring &str);
	static double ParseInt(const wchar_t **data);
	static double ParseDecimal(const wchar_t **data);

private:
	JSON();
};
