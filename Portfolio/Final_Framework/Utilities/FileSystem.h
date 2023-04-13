#pragma once
#include <WTypes.h >
#include <oleauto.h>
//#include <errno.h>
//#include "include/iconv.h"

class FileSystem final
{
public:
	static const char* EveryFilter;
	static const char* TextureFilter;
	static const char* ShaderFilter;

public:
	static void OpenFileDialog
	(
		std::function<void(std::string)>&& func,
		const char* filter = EveryFilter,
		const char* directory = ""
	);

	static void OpenFileDialogW
	(
		std::function<void(std::wstring)>&& func,
		const char* filter = EveryFilter,
		const wchar_t* directory = L""
	);

	static void SaveFileDialog
	(
		std::function<void(std::string)>&& func,
		const char* filter = EveryFilter,
		const char* directory = ""
	);

	static void SaveFileDialogW
	(
		std::function<void(std::string)>&& func,
		const char* filter = EveryFilter,
		const char* directory = ""
	);



public:
	static void OpenDirectoryWindow(const std::string& directory);

	static const bool Create_Directory(const std::string& path);
	static const bool Delete_Directory(const std::string& directory);
	static const bool Delete_File(const std::string& path);
	static const bool Copy_File(const std::string& src, const std::string& dst);
	static const bool IsDirectory(const std::string& path); // 파일인지 폴더인지 체크
	static const bool IsDirectory(const std::wstring& path); // 파일인지 폴더인지 체크
	static const bool ExistDirectory(const std::string& directory); // 폴더존재하는지
	static const bool ExistFile(const std::string& path); // 파일존재하는지

	static const std::string GetFileNameFromPath(const std::string& path); // tree.png
	static const std::string GetIntactFileNameFromPath(const std::string& path); // tree
	static const std::string GetDirectoryFromPath(const std::string& path); // ../Assets/Texture/
	static const std::string GetExtensionFromPath(const std::string& path); // png
	static const std::string GetPathWithoutExtension(const std::string& path); // png
	static const std::string GetRelativeFromPath(const std::string& absolutePath); // 절대경로를 상대경로로바꿔줌
	static const std::string GetWorkingDirectory(); // 작업중인 폴더
	static const std::string GetParentDirectory(const std::string& directory); // 상위폴더

	static const std::wstring GetFileNameFromPath(const std::wstring& path); // tree.png
	static const std::wstring GetIntactFileNameFromPath(const std::wstring& path); // tree
	static const std::wstring GetDirectoryFromPath(const std::wstring& path); // ../Assets/Texture/
	static const std::wstring GetExtensionFromPath(const std::wstring& path); // png
	static const std::wstring GetPathWithoutExtension(const std::wstring& path); // png
	static const std::wstring GetRelativeFromPath(const std::wstring& absolutePath); // 절대경로를 상대경로로바꿔줌
	static const std::wstring GetParentDirectory(const std::wstring& directory); // 상위폴더

	static const std::vector<std::string> GetDirectoriesInDirectory(const std::string& directory);
	static const std::vector<std::string> GetFilesInDirectory(const std::string& directory);

	static const std::vector<std::string>& GetSupportedImageFormats() { return supportImageFormat; }
	static const std::vector<std::string>& GetSupportedAudioFormats() { return supportAudioFormat; }
	static const std::vector<std::string>& GetSupportedModelFormats() { return supportModelFormat; }

	static const bool IsSupportedImageFile(const std::string& path);
	static const bool IsSupportedAudioFile(const std::string& path);
	static const bool IsSupportedModelFile(const std::string& path);
	static const bool IsSupportedFilterFile(std::string& filter, const std::string& path);
	static const bool IsSupportedFilter(std::string& filter, const std::string& path);
	static const std::string ToUpper(const std::string& lower);
	static const std::string ToLower(const std::string& upper);
	//static const std::string ToString(const std::wstring& str);
	//static const std::string ToString(const wchar_t* _wchar);
	static const std::string ToString(std::wstring const & str, std::locale const & loc = std::locale());
	static const std::wstring ToWString(const std::string& str);
	static const std::wstring ToWString(std::string const & str, std::locale const & loc);
	static const std::wstring ToWString(const char* _char);

	static const char* UTF8ToANSI(const char *pszCode);

	static const std::string ToUTF8(const std::string target);


private:
	static std::vector<std::string> supportImageFormat;
	static std::vector<std::string> supportAudioFormat;
	static std::vector<std::string> supportModelFormat;
};

