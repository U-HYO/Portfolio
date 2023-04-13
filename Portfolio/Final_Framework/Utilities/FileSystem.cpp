#include "Framework.h"
#include "FileSystem.h"
#include <filesystem>

using namespace std::filesystem;

// find()				- 주어진 문자열이 존재하는 위치 -> 정방향
// rfind()				- 주어진 문자열이 존재하는 위치 -> 역방향
// find_first_of()		- 주어진 문자중 하나라도 걸리는 첫번째 위치
// find_last_of()		- 주어진 문자중 하나라도 걸리는 마지막 위치
// find_first_not_of()	- 주어진 문자가 아닌 문자가 걸리는 첫번째 위치
// find_last_not_of()	- 주어진 문자가 아닌 문자가 걸리는 마지막 위치

//대소문자 구별한다


const char* FileSystem::EveryFilter = "Every File(*.*)\0*.*;\0";
const char* FileSystem::TextureFilter = "Texture File(*.png, *.bmp)\0*.png;*.bmp;\n";
const char* FileSystem::ShaderFilter = "Shader File(*.hlsl)\0*.hlsl;\n";

std::vector<std::string>  FileSystem::supportImageFormat =
{
	".jpg",
	".png",
	".bmp",
	".tga",
	".dds",
	".exr",
	".raw",
	".gif",
	".hdr",
	".ico",
	".iff",
	".jng",
	".jpeg",
	".koala",
	".kodak",
	".mng",
	".pcx",
	".pbm",
	".pgm",
	".ppm",
	".pfm",
	".pict",
	".psd",
	".raw",
	".sgi",
	".targa",
	".tiff",
	".tif",
	".wbmp",
	".webp",
	".xbm",
	".xpm"
};

std::vector<std::string>  FileSystem::supportAudioFormat =
{
	".aiff",
	".asf",
	".asx",
	".dls",
	".flac",
	".fsb",
	".it",
	".m3u",
	".midi",
	".mod",
	".mp2",
	".mp3",
	".ogg",
	".pls",
	".s3m",
	".vag", // PS2/PSP
	".wav",
	".wax",
	".wma",
	".xm",
	".xma" // XBOX 360
};

std::vector<std::string>  FileSystem::supportModelFormat =
{
	".3ds",
	".obj",
	".fbx",
	".blend",
	".dae",
	".lwo",
	".c4d",
	".ase",
	".dxf",
	".hmp",
	".md2",
	".md3",
	".md5",
	".mdc",
	".mdl",
	".nff",
	".ply",
	".stl",
	".x",
	".smd",
	".lxo",
	".lws",
	".ter",
	".ac3d",
	".ms3d",
	".cob",
	".q3bsp",
	".xgl",
	".csm",
	".bvh",
	".b3d",
	".ndo"
};

void FileSystem::OpenFileDialog(std::function<void(std::string)>&& func, const char * filter, const char * directory)
{
	char buffer[255];
	ZeroMemory(buffer, 255);

	OPENFILENAMEA openFileName;
	ZeroMemory(&openFileName, sizeof(OPENFILENAMEA));
	openFileName.lStructSize = sizeof(OPENFILENAMEA);
	openFileName.hwndOwner = D3D::Get()->GetHandle();
	openFileName.lpstrFilter = filter;
	openFileName.lpstrFile = buffer; // 선택한 파일의 전체경로
	openFileName.nMaxFile = 255;
	openFileName.lpstrInitialDir = directory;//처음 화면에 나올 폴더
	openFileName.Flags = OFN_NOCHANGEDIR; // 마지막으로 닫은 위치 변경x

	if (GetOpenFileNameA(&openFileName) == TRUE)
	{
		//std::string path = GetRelativeFromPath(buffer);
		std::string path = buffer;

		auto function = std::forward<std::function<void(std::string)>>(func);
		if (function != nullptr)
		{
			function(path);
		}
	}

}

void FileSystem::SaveFileDialog(std::function<void(std::string)>&& func, const char * filter, const char * directory)
{
	char buffer[255];
	ZeroMemory(buffer, 255);

	OPENFILENAMEA openFileName;
	ZeroMemory(&openFileName, sizeof(OPENFILENAMEA));
	openFileName.lStructSize = sizeof(OPENFILENAMEA);
	openFileName.hwndOwner = D3D::Get()->GetHandle();
	openFileName.lpstrFilter = filter;
	openFileName.lpstrFile = buffer; // 선택한 파일의 전체경로
	openFileName.nMaxFile = 255;
	openFileName.lpstrInitialDir = directory;//처음 화면에 나올 폴더
	openFileName.Flags = OFN_NOCHANGEDIR; // 마지막으로 닫은 위치 변경x

	if (GetSaveFileNameA(&openFileName) == TRUE)
	{
		std::string path = GetRelativeFromPath(buffer);

		auto function = std::forward<std::function<void(std::string)>>(func);
		if (function != nullptr)
		{
			function(path);
		}
	}
}

void FileSystem::OpenDirectoryWindow(const std::string & directory)
{
	//ShellExecuteA(nullptr, "open","explorer", directory.c_str(), nullptr, SW_SHOW);
	
	ShellExecuteA(nullptr, "open", nullptr, nullptr, directory.c_str(), SW_SHOW);
}


const bool FileSystem::Create_Directory(const std::string & path)
{
	bool bResult = false;

	try
	{
		bResult = create_directories(path);
	}
	catch (filesystem_error error)
	{
		MessageBoxA(nullptr, error.what(), "ERROR", MB_OK);

	}

	return bResult;
}

const bool FileSystem::Delete_Directory(const std::string & directory)
{
	bool bResult = false;

	try
	{
		bResult = remove_all(directory) > 0; // 삭제시킨 파일의 갯수가 나옴
	}
	catch (filesystem_error error)
	{
		MessageBoxA(nullptr, error.what(), "ERROR", MB_OK);

	}

	return bResult;
}

const bool FileSystem::Delete_File(const std::string & path)
{
	if (IsDirectory(path)) return false; // 경로가들어오면 false;

	bool bResult = false;

	try
	{
		bResult = remove(path);
	}
	catch (filesystem_error error)
	{
		MessageBoxA(nullptr, error.what(), "ERROR", MB_OK);

	}

	return bResult;
}

const bool FileSystem::Copy_File(const std::string & src, const std::string & dst)
{
	if (src == dst) return false; // 같은경로이면 false
	if (!ExistDirectory(GetDirectoryFromPath(dst)))
	{
		Create_Directory(GetDirectoryFromPath(dst));
	}

	bool bResult = false;

	try
	{
		bResult = copy_file(src, dst, copy_options::overwrite_existing); // overwrite_existing 덮어쓰기
	}
	catch (filesystem_error error)
	{
		MessageBoxA(nullptr, error.what(), "ERROR", MB_OK);
	}

	return bResult;
}

const bool FileSystem::IsDirectory(const std::string & path)
{
	bool bResult = false;

	try
	{
		bResult = is_directory(path); // 삭제시킨 파일의 갯수가 나옴
	}
	catch (filesystem_error error)
	{
		MessageBoxA(nullptr, error.what(), "ERROR", MB_OK);
	}

	return bResult;
}

const bool FileSystem::IsDirectory(const std::wstring & path)
{
	bool bResult = false;

	try
	{
		bResult = is_directory(path); // 삭제시킨 파일의 갯수가 나옴
	}
	catch (filesystem_error error)
	{
		MessageBoxA(nullptr, error.what(), "ERROR", MB_OK);
	}

	return bResult;
}

const bool FileSystem::ExistDirectory(const std::string & directory)
{
	bool bResult = false;
	try
	{
		bResult = exists(directory);
	}
	catch (filesystem_error error)
	{
		MessageBoxA(nullptr, error.what(), "ERROR", MB_OK);
	}
	return bResult;
}

const bool FileSystem::ExistFile(const std::string & path)
{
	bool bResult = false;
	try
	{
		bResult = exists(path);
	}
	catch (filesystem_error error)
	{
		MessageBoxA(nullptr, error.what(), "ERROR", MB_OK);
	}
	return bResult;
}

const std::string FileSystem::GetFileNameFromPath(const std::string & path)
{
	//../_Assets/Texture/Tree.png ------------> Tree.png를 찾는다

	auto lastIndex = path.find_last_of("\\/");
	auto fileName = path.substr(lastIndex + 1, path.length()); // lastIndex = /, lastIndex+1 = T

	return fileName;
}

const std::string FileSystem::GetIntactFileNameFromPath(const std::string & path)
{
	//../_Assets/Texture/Tree.png ------------> Tree 를 찾는다
	auto fileName = GetFileNameFromPath(path); // Tree.png
	auto lastIndex = fileName.find_last_of('.'); // 
	auto intactFileName = fileName.substr(0, lastIndex);

	return intactFileName;
}

const std::string FileSystem::GetDirectoryFromPath(const std::string & path)
{
	// ../_Assets/Texture/Tree.png ------------> ../_Assets/Texture/ 를 찾는다
	auto lastIndex = path.find_last_of("\\/");
	auto directory = path.substr(0, lastIndex + 1);

	return directory;
}

const std::string FileSystem::GetExtensionFromPath(const std::string & path)
{
	//../_Assets/Texture/Tree.png ------------> .png 를 찾는다
	auto lastIndex = path.find_last_of('.');
	if (lastIndex != std::string::npos) // 찾았다
	{
		return path.substr(lastIndex, path.length()); // .png // lastIndex + 1 = png
	}

	return std::string();
}

const std::string FileSystem::GetPathWithoutExtension(const std::string & path)
{
	auto directory = GetDirectoryFromPath(path); // 경로
	auto fileName = GetIntactFileNameFromPath(path); // 파일이름

	return directory + fileName;
}

const std::wstring FileSystem::GetPathWithoutExtension(const std::wstring & path)
{
	auto directory = GetDirectoryFromPath(path); // 경로
	auto fileName = GetIntactFileNameFromPath(path); // 파일이름

	return directory + fileName;
}

const std::wstring FileSystem::GetRelativeFromPath(const std::wstring & absolutePath)
{
	//절대 경로를 만듬
	path p = absolute(absolutePath);
	path r = absolute(GetWorkingDirectory());

	//루트 경로가 다를경우 절대경로 반환
	if (p.root_path() != r.root_path())
		return p.generic_wstring();

	path result;

	//두 경로가 갈라지는 지점을 체크
	path::const_iterator iter_path = p.begin();
	path::const_iterator iter_relative = r.begin();

	while (
		*iter_path == *iter_relative &&
		iter_path != p.end() &&
		iter_relative != r.end())
	{
		iter_path++;
		iter_relative++;
	}

	//relative에 남은 각 토큰에대해 ..을 추가
	if (iter_relative != r.end())
	{
		iter_relative++;
		while (iter_relative != r.end())
		{
			result /= "..";
			iter_relative++;
		}
	}

	//남은 경로 추가
	while (iter_path != p.end())
	{
		result /= *iter_path;
		iter_path++;
	}

	return result.generic_wstring();
}

const std::wstring FileSystem::GetParentDirectory(const std::wstring & directory)
{
	auto found = directory.find_last_of(L"\\/");
	auto result = directory;

	if (found == std::string::npos)
		return directory;

	if (found == directory.length() - 1)
	{
		result = result.substr(0, found - 1);
		return GetParentDirectory(result);
	}

	return result.substr(0, found) + L"/";
}

const std::string FileSystem::GetRelativeFromPath(const std::string & absolutePath)
{
	//절대 경로를 만듬
	path p = absolute(absolutePath);
	path r = absolute(GetWorkingDirectory());

	//루트 경로가 다를경우 절대경로 반환
	if (p.root_path() != r.root_path())
		return p.generic_string();

	path result;

	//두 경로가 갈라지는 지점을 체크
	path::const_iterator iter_path = p.begin();
	path::const_iterator iter_relative = r.begin();

	while (
		*iter_path == *iter_relative &&
		iter_path != p.end() &&
		iter_relative != r.end())
	{
		iter_path++;
		iter_relative++;
	}

	//relative에 남은 각 토큰에대해 ..을 추가
	if (iter_relative != r.end())
	{
		iter_relative++;
		while (iter_relative != r.end())
		{
			result /= "..";
			iter_relative++;
		}
	}

	//남은 경로 추가
	while (iter_path != p.end())
	{
		result /= *iter_path;
		iter_path++;
	}

	return result.generic_string();
}

const std::string FileSystem::GetWorkingDirectory()
{
	return current_path().generic_string() + "/";
}

const std::string FileSystem::GetParentDirectory(const std::string & directory)
{
	auto found = directory.find_last_of("\\/");
	auto result = directory;

	if (found == std::string::npos)
		return directory;

	if (found == directory.length() - 1)
	{
		result = result.substr(0, found - 1);
		return GetParentDirectory(result);
	}

	return result.substr(0, found) + "/";
}

const std::wstring FileSystem::GetFileNameFromPath(const std::wstring & path)
{
	//../_Assets/Texture/Tree.png ------------> Tree.png를 찾는다

	auto lastIndex = path.find_last_of(L"\\/");
	auto fileName = path.substr(lastIndex + 1, path.length()); // lastIndex = /, lastIndex+1 = T

	return fileName;
}

const std::wstring FileSystem::GetIntactFileNameFromPath(const std::wstring & path)
{
	//../_Assets/Texture/Tree.png ------------> Tree 를 찾는다
	auto fileName = GetFileNameFromPath(path); // Tree.png
	auto lastIndex = fileName.find_last_of('.'); // 
	auto intactFileName = fileName.substr(0, lastIndex);

	return intactFileName;
}

const std::wstring FileSystem::GetDirectoryFromPath(const std::wstring & path)
{
	// ../_Assets/Texture/Tree.png ------------> ../_Assets/Texture/ 를 찾는다
	auto lastIndex = path.find_last_of(L"\\/");
	auto directory = path.substr(0, lastIndex + 1);

	return directory;
}

const std::wstring FileSystem::GetExtensionFromPath(const std::wstring & path)
{
	//../_Assets/Texture/Tree.png ------------> .png 를 찾는다
	auto lastIndex = path.find_last_of(L'.');
	if (lastIndex != std::wstring::npos) // 찾았다
	{
		return path.substr(lastIndex, path.length()); // .png // lastIndex + 1 = png
	}

	return std::wstring();
}

const std::vector<std::string> FileSystem::GetDirectoriesInDirectory(const std::string & directory)
{
	std::vector<std::string> subDirectories;

	directory_iterator endIter; // 자동초기화로 null이 들어간다
	for (directory_iterator iter(directory); iter != endIter; iter++)
	{
		if (!is_directory(iter->status())) // 파일인지 경로인지 분석
			continue;

		subDirectories.emplace_back(iter->path().generic_string()); // 경로를 뺴옴
	}

	return subDirectories;
}

const std::vector<std::string> FileSystem::GetFilesInDirectory(const std::string & directory)
{
	std::vector<std::string> files;

	directory_iterator endIter; // 자동초기화로 null이 들어간다
	for (directory_iterator iter(directory); iter != endIter; iter++)
	{
		if (!is_regular_file(iter->status())) // 기본 파일만 빼옴
			continue;

		files.emplace_back(iter->path().generic_string()); // 경로를 뺴옴
	}

	return files;
}

const bool FileSystem::IsSupportedImageFile(const std::string & path)
{
	std::string fileExtension = GetExtensionFromPath(path);
	auto supportFormats = GetSupportedImageFormats();

	for (const auto iter : supportFormats) // Range Based for loop <--> while - Entry Condition Loop
	{
		if (fileExtension == iter || fileExtension == ToUpper(iter)) return true;
	}

	return false;
}

const bool FileSystem::IsSupportedAudioFile(const std::string & path)
{
	std::string fileExtension = GetExtensionFromPath(path);
	auto supportFormats = GetSupportedAudioFormats();

	for (const auto iter : supportFormats)
	{
		if (fileExtension == iter || fileExtension == ToUpper(iter)) return true;
	}

	return false;
}

const bool FileSystem::IsSupportedModelFile(const std::string & path)
{
	std::string fileExtension = GetExtensionFromPath(path);
	auto supportFormats = GetSupportedModelFormats();

	for (const auto iter : supportFormats)
	{
		if (fileExtension == iter || fileExtension == ToUpper(iter)) return true;
	}

	return false;
}

const bool FileSystem::IsSupportedFilterFile(std::string & filter, const std::string & path)
{
	std::string fileExtension = GetExtensionFromPath(path);
	
	if (fileExtension == filter || fileExtension == ToUpper(filter))
		return true;

	return false;
}

const bool FileSystem::IsSupportedFilter(std::string & filter, const std::string & path)
{
	std::string fileName = GetFileNameFromPath(path);

	if (fileName.find(filter) != std::string::npos)
		return true;

	return false;
}

const std::string FileSystem::ToUpper(const std::string & lower)
{
	std::string upper;

	for (const auto& character : lower)
	{
		upper += toupper(character);
	}

	return upper;
}

const std::string FileSystem::ToLower(const std::string & upper)
{
	std::string lower;

	for (const auto& character : upper)
	{
		lower += tolower(character);
	}
	return lower;
}

//const std::string FileSystem::ToString(const std::wstring & str)
//{
//	//TODO : 과제
//	return ToString(str.c_str());
//}

const std::string FileSystem::ToString(std::wstring const& str, std::locale const& loc)
{

	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
	codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
	std::mbstate_t state = std::mbstate_t();
	std::vector<char> buf((str.size() + 1) * codecvt.max_length());
	wchar_t const* in_next = str.c_str();
	char* out_next = &buf[0];
	std::codecvt_base::result r = codecvt.out(state,
		str.c_str(), str.c_str() + str.size(), in_next,
		&buf[0], &buf[0] + buf.size(), out_next);
	if (r == std::codecvt_base::error)
		throw std::runtime_error("can't convert wstring to string");
	return std::string(&buf[0]);

}

const std::wstring FileSystem::ToWString(const std::string & str)
{
	//std::wstring result;
	//result.assign(str.begin(), str.end());
	//
	//return result;
	return ToWString(str.c_str());
}

const std::wstring FileSystem::ToWString(const char* _char)
{
	// mbsrtowcs : mbsr - multibyte string
	//             to
	//             wcs - wide character string

	if (_char == NULL || strlen(_char) == 0)
		return L""; // return empty string

	std::mbstate_t state{};

	const char* p = _char;
	// character length 
	// when converstion successful.
	size_t clen = mbsrtowcs(NULL, &p, 0 /* ignored */, &state);

	// cannot convert or error occurred
	if (clen == 0 || clen == static_cast<size_t>(-1))
	{
		return L"";
	}

	std::wstring rlt(clen, L'\0');

	clen = mbsrtowcs(&rlt[0], &_char, rlt.size(), &state);

	// cannot convert or error occurred
	if (clen == 0 || clen == static_cast<size_t>(-1))
	{
		return L"";
	}

	return rlt;
}

const char * FileSystem::UTF8ToANSI(const char * pszCode)
{
	BSTR    bstrWide;
	char*   pszAnsi;
	int     nLength;
	// Get nLength of the Wide Char buffer
	nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlenA(pszCode) + 1,
		NULL, NULL);
	bstrWide = SysAllocStringLen(NULL, nLength);
	// Change UTF-8 to Unicode (UTF-16)
	MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlenA(pszCode) + 1, bstrWide,
		nLength);
	// Get nLength of the multi byte buffer 
	nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
	pszAnsi = Debug_new char[nLength];
	// Change from unicode to mult byte
	WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
	SysFreeString(bstrWide);
	return pszAnsi;
}

//const char * FileSystem::ToUTF8(const char * target)
//{
//	size_t szIn = strlen(target) + 10;
//	size_t szOut = szIn * 3;
//
//	char *pIn = new char[szIn];
//	char *pOut = new char[szOut];
//
//	memset(pIn, '\0', sizeof(char)*szIn);
//	memset(pOut, '\0', sizeof(char)*szOut);
//	strcpy(pIn, target);
//
//	char *pIn2 = pIn;
//	char *pOut2 = pOut;
//
//	iconv_t cd;
//	int ret;
//	
//	if ((cd = iconv_open("UTF-8//IGNORE", "EUC-KR")) < 0)
//	{
//		// iconv_open 에서 error 나면 errno 을 셋팅한다.
//		fprintf(stderr, "Fail to iconv_open() errorno : %d\n", errno);
//		exit(-1);
//	}
//
//	if ((ret = iconv(cd, &pIn2, &szIn, &pOut2, &szOut)) < 0)
//	{
//		// iconv 에서 error 나면 errno 을 셋팅한다.
//		// E2BIG *outbuf에 공간이 부족합니다
//		// EILSEQ 부정확한 다중바이트 문자열이 입력값으로 들어왔습니다.
//		// EINVAL 완료되지않은 다중바이트문자열이 입력값으로 들어왔습니다. 
//		char errmsg[10] = "";
//		if (errno == E2BIG) sprintf(errmsg, "E2BIG");
//		if (errno == EILSEQ) sprintf(errmsg, "EILSEQ");
//		if (errno == EINVAL) sprintf(errmsg, "EINVAL");
//		fprintf(stderr, "Fail to iconv() errorno : %s(%d)\n", errmsg, errno);
//	}
//	iconv_close(cd);
//
//	szOut = pOut2 - pOut;
//	FILE *fp = fopen("test.txt", "wb");
//	fwrite(pOut, 1, szOut, fp);
//	delete[] pIn;
//	delete[] pOut;
//	fclose(fp);
//}

const std::string FileSystem::ToUTF8(const std::string target)
{

	WCHAR szUnicode[256] = L"";
	char szUTF8char[256] = "";

	std::string strConver;
	char* szSrc = (LPSTR)(LPCTSTR)target.c_str();


	char szRetVale[128] = "";

	//EUC-KR 캐릭터셋의 ANSI문자를 유니코드로 변환
	int UnicodeSize = MultiByteToWideChar(CP_ACP, 0, szSrc, (int)strlen(szSrc), szUnicode, sizeof(szUnicode));

	// 유니코드를 UTF-8 캐릭터셋의 ANSI문자로 변환
	int UTF8CodeSize = WideCharToMultiByte(CP_UTF8, 0, szUnicode, UnicodeSize, szUTF8char, sizeof(szUTF8char), NULL, NULL);

	memcpy(szRetVale, szUTF8char, UTF8CodeSize);
	strConver = szRetVale;

	return strConver;
}
