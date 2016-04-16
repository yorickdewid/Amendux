#pragma once

#include <vector>
#include <string>

#include "Json.h"

class JSON;

enum JSONType {
	JSONType_Null,
	JSONType_String,
	JSONType_Bool,
	JSONType_Number,
	JSONType_Array,
	JSONType_Object
};

class JSONValue
{
	friend class JSON;

public:
	JSONValue(/*NULL*/);
	JSONValue(const wchar_t *m_char_value);
	JSONValue(const std::wstring &m_string_value);
	JSONValue(bool m_bool_value);
	JSONValue(double m_number_value);
	JSONValue(const JSONArray &m_array_value);
	JSONValue(const JSONObject &m_object_value);
	JSONValue(const JSONValue &m_source);
	~JSONValue();

	bool IsNull() const;
	bool IsString() const;
	bool IsBool() const;
	bool IsNumber() const;
	bool IsArray() const;
	bool IsObject() const;

	const std::wstring &AsString() const;
	bool AsBool() const;
	double AsNumber() const;
	const JSONArray &AsArray() const;
	const JSONObject &AsObject() const;

	std::size_t CountChildren() const;
	bool HasChild(std::size_t index) const;
	JSONValue *Child(std::size_t index);
	bool HasChild(const wchar_t* name) const;
	JSONValue *Child(const wchar_t* name);
	std::vector<std::wstring> ObjectKeys() const;

	std::wstring Stringify(bool const prettyprint = false) const;
protected:
	static JSONValue *Parse(const wchar_t **data);

private:
	static std::wstring StringifyString(const std::wstring &str);
	std::wstring StringifyImpl(size_t const indentDepth) const;
	static std::wstring Indent(size_t depth);

	JSONType type;

	union
	{
		bool bool_value;
		double number_value;
		std::wstring *string_value;
		JSONArray *array_value;
		JSONObject *object_value;
	};

};
