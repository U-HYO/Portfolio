#pragma once

class String
{
public:
	static void SplitString(std::vector<std::string>* result, std::string origin, std::string tok);
	static void SplitString(std::vector<std::wstring>* result, std::wstring origin, std::wstring tok);

	static bool StartsWith(std::string str, std::string comp);
	static bool StartsWith(std::wstring str, std::wstring comp);

	static bool Contain(std::string str, std::string comp);
	static bool Contain(std::wstring str, std::wstring comp);

	static void Replace(std::string* str, std::string comp, std::string rep);
	static void Replace(std::wstring* str, std::wstring comp, std::wstring rep);

	static std::wstring ToWString(std::string value);
	static std::string ToString(std::wstring value);

	static std::string Format(const std::string format, ...);
	static std::wstring Format(const std::wstring format, ...);
};