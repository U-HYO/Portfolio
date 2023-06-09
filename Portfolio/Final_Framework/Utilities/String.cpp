#include "Framework.h"
#include "String.h"

#pragma warning(disable : 4996)

//////////////////////////////////////////////////////////////////////////
///@brief 문자열 자르기
///@param orgin : 원본 문자열
///@param tok : 자를 기준이 되는 문자열
///@return 완료된 문자열 배열
//////////////////////////////////////////////////////////////////////////
void String::SplitString(std::vector<std::string>* result, std::string origin, std::string tok)
{
	result->clear();

	int cutAt = 0; //자를 위치s
	while ((cutAt = (int)origin.find_first_of(tok)) != origin.npos)
	{
		if (cutAt > 0) //자르는 위치가 0보다크면
			result->push_back(origin.substr(0, cutAt));

		origin = origin.substr(cutAt + 1);
	}

	if (origin.length() > 0) //자르고도 남은 것이 있다면
		result->push_back(origin.substr(0, cutAt));
}

void String::SplitString(std::vector<std::wstring>* result, std::wstring origin, std::wstring tok)
{
	result->clear();

	int cutAt = 0; //자를 위치s
	while ((cutAt = (int)origin.find_first_of(tok)) != origin.npos)
	{
		if (cutAt > 0) //자르는 위치가 0보다크면
			result->push_back(origin.substr(0, cutAt));

		origin = origin.substr(cutAt + 1);
	}

	if (origin.length() > 0) //자르고도 남은 것이 있다면
		result->push_back(origin.substr(0, cutAt));
}


//////////////////////////////////////////////////////////////////////////
///@brief 시작 문자열이 같은지 체크
///@param str : 체크하려는 문자열
///@param comp : 시작 비교문자열
//////////////////////////////////////////////////////////////////////////
bool String::StartsWith(std::string str, std::string comp)
{
	std::wstring::size_type index = str.find(comp);
	if (index != std::wstring::npos && (int)index == 0)
		return true;

	return false;
}

bool String::StartsWith(std::wstring str, std::wstring comp)
{
	std::wstring::size_type index = str.find(comp);
	if (index != std::wstring::npos && (int)index == 0)
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
///@brief 해당 문자열이 포함되어 있는지
///@param str : 체크하려는 문자열
///@param comp : 비교문자열
//////////////////////////////////////////////////////////////////////////
bool String::Contain(std::string str, std::string comp)
{
	size_t found = str.find(comp);

	return found != std::wstring::npos;
}

bool String::Contain(std::wstring str, std::wstring comp)
{
	size_t found = str.find(comp);

	return found != std::wstring::npos;
}


//////////////////////////////////////////////////////////////////////////
///@brief 해당 문자열에서 comp 문자를 rep로 변경
///@param str : 체크하려는 문자열
///@param comp : 비교문자열
///@param rep : 바꿀문자열
//////////////////////////////////////////////////////////////////////////
void String::Replace(std::string * str, std::string comp, std::string rep)
{
	std::string temp = *str;

	size_t start_pos = 0;
	while ((start_pos = temp.find(comp, start_pos)) != std::wstring::npos)
	{
		temp.replace(start_pos, comp.length(), rep);
		start_pos += rep.length();
	}

	*str = temp;
}

void String::Replace(std::wstring* str, std::wstring comp, std::wstring rep)
{
	std::wstring temp = *str;

	size_t start_pos = 0;
	while ((start_pos = temp.find(comp, start_pos)) != std::wstring::npos)
	{
		temp.replace(start_pos, comp.length(), rep);
		start_pos += rep.length();
	}

	*str = temp;
}

//////////////////////////////////////////////////////////////////////////
///@brief std::string형을 std::wstring형으로 변경
///@param value : 변환할 문자열
///@return 변환 완료 문자열
//////////////////////////////////////////////////////////////////////////
std::wstring String::ToWString(std::string value)
{
	std::wstring temp = L"";
	temp.assign(value.begin(), value.end());

	return temp;
}

//////////////////////////////////////////////////////////////////////////
///@brief std::wstring형을 std::string형으로 변경
///@param value : 변환할 문자열
///@return 변환 완료 문자열
//////////////////////////////////////////////////////////////////////////
std::string String::ToString(std::wstring value)
{
	std::string temp = "";
	temp.assign(value.begin(), value.end());

	return temp;
}
std::string String::Format(const std::string format, ...)
{
	va_list args;

	va_start(args, format);
	size_t len = vsnprintf(NULL, 0, format.c_str(), args);
	va_end(args);

	std::vector<char> vec(len + 1);

	va_start(args, format);
	vsnprintf(&vec[0], len + 1, format.c_str(), args);
	va_end(args);

	return &vec[0];
}

std::wstring String::Format(const std::wstring format, ...)
{
	va_list args;

	va_start(args, format);
	size_t len = _vsnwprintf(NULL, 0, format.c_str(), args);
	va_end(args);

	std::vector<WCHAR> vec(len + 1);

	va_start(args, format);
	_vsnwprintf(&vec[0], len + 1, format.c_str(), args);
	va_end(args);

	return &vec[0];
}

//////////////////////////////////////////////////////////////////////////