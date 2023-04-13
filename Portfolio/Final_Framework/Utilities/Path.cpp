#include "Framework.h"
#include "Path.h"
#include <string.h>
#include <shlwapi.h>
#include "String.h"
#pragma comment(lib, "shlwapi.lib")

bool Path::ExistFile(std::string path)
{
	return ExistFile(String::ToWString(path));
}

bool Path::ExistFile(std::wstring path)
{
	DWORD fileValue = GetFileAttributes(path.c_str());

	return fileValue < 0xFFFFFFFF;
}

bool Path::ExistDirectory(std::string path)
{
	return ExistDirectory(String::ToWString(path));
}

bool Path::ExistDirectory(std::wstring path)
{
	DWORD attribute = GetFileAttributes(path.c_str());

	BOOL temp = (attribute != INVALID_FILE_ATTRIBUTES &&
		(attribute & FILE_ATTRIBUTE_DIRECTORY));

	return temp == TRUE;
}

std::string Path::Combine(std::string path1, std::string path2)
{
	return path1 + path2;
}

std::wstring Path::Combine(std::wstring path1, std::wstring path2)
{
	return path1 + path2;
}

std::string Path::Combine(std::vector<std::string> paths)
{
	std::string temp = "";
	for (std::string path : paths)
		temp += path;

	return temp;
}

std::wstring Path::Combine(std::vector<std::wstring> paths)
{
	std::wstring temp = L"";
	for (std::wstring path : paths)
		temp += path;

	return temp;
}

std::string Path::GetDirectoryName(std::string path)
{
	String::Replace(&path, "\\", "/");
	size_t index = path.find_last_of('/');

	return path.substr(0, index + 1);
}

std::wstring Path::GetDirectoryName(std::wstring path)
{
	String::Replace(&path, L"\\", L"/");
	size_t index = path.find_last_of('/');

	return path.substr(0, index + 1);
}

std::string Path::GetExtension(std::string path)
{
	String::Replace(&path, "\\", "/");
	size_t index = path.find_last_of('.');

	return path.substr(index + 1, path.length());;
}

std::wstring Path::GetExtension(std::wstring path)
{
	String::Replace(&path, L"\\", L"/");
	size_t index = path.find_last_of('.');

	return path.substr(index + 1, path.length());;
}

std::string Path::GetFileName(std::string path)
{
	String::Replace(&path, "\\", "/");
	size_t index = path.find_last_of('/');

	return path.substr(index + 1, path.length());
}

std::wstring Path::GetFileName(std::wstring path)
{
	String::Replace(&path, L"\\", L"/");
	size_t index = path.find_last_of('/');

	return path.substr(index + 1, path.length());
}

std::string Path::GetFileNameWithoutExtension(std::string path)
{
	std::string fileName = GetFileName(path);

	size_t index = fileName.find_last_of('.');
	return fileName.substr(0, index);
}

std::wstring Path::GetFileNameWithoutExtension(std::wstring path)
{
	std::wstring fileName = GetFileName(path);

	size_t index = fileName.find_last_of('.');
	return fileName.substr(0, index);
}


const WCHAR* Path::ImageFilter = L"Image\0*.png;*.bmp;*.jpg";
const WCHAR* Path::BinModelFilter = L"Binary Model\0*.model";
const WCHAR* Path::FbxModelFilter = L"Fbx Model\0*.fbx;*.obj\0";
const WCHAR* Path::ShaderFilter = L"HLSL file\0*.hlsl";

void Path::OpenFileDialog(std::wstring file, const WCHAR* filter, std::wstring folder, std::function<void(std::wstring)> func, HWND hwnd)
{
	WCHAR name[255];
	wcscpy_s(name, file.c_str());

	std::wstring tempFolder = folder;
	String::Replace(&tempFolder, L"/", L"\\");

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = name;
	ofn.lpstrFileTitle = L"불러오기";
	ofn.nMaxFile = 255;
	ofn.lpstrInitialDir = tempFolder.c_str();
	ofn.Flags = OFN_NOCHANGEDIR;

	if (GetOpenFileName(&ofn) == TRUE)
	{
		if (func != NULL)
		{
			std::wstring loadName = name;
			String::Replace(&loadName, L"\\", L"/");

			func(loadName);
		}
	}
}

void Path::SaveFileDialog(std::wstring file, const WCHAR* filter, std::wstring folder, std::function<void(std::wstring)> func, HWND hwnd)
{
	WCHAR name[255];
	wcscpy_s(name, file.c_str());

	std::wstring tempFolder = folder;
	String::Replace(&tempFolder, L"/", L"\\");

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = name;
	ofn.lpstrFileTitle = L"저장하기";
	ofn.nMaxFile = 255;
	ofn.lpstrInitialDir = tempFolder.c_str();
	ofn.Flags = OFN_NOCHANGEDIR;

	if (GetSaveFileName(&ofn) == TRUE)
	{
		if (func != NULL)
		{
			std::wstring loadName = name;
			String::Replace(&loadName, L"\\", L"/");

			func(loadName);
		}
	}
}

void Path::GetFiles(std::vector<std::string>* files, std::string path, std::string filter, bool bFindSubFolder)
{
	std::vector<std::wstring> wFiles;
	std::wstring wPath = String::ToWString(path);
	std::wstring wFilter = String::ToWString(filter);

	GetFiles(&wFiles, wPath, wFilter, bFindSubFolder);

	for (std::wstring str : wFiles)
		files->push_back(String::ToString(str));
}

//path : ../Temp/
//filter : *.txt
void Path::GetFiles(std::vector<std::wstring>* files, std::wstring path, std::wstring filter, bool bFindSubFolder)
{
	std::wstring file = path + filter;

	WIN32_FIND_DATA findData;
	HANDLE handle = FindFirstFile(file.c_str(), &findData);

	if (handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (findData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{
				if (bFindSubFolder == true && findData.cFileName[0] != '.')
				{
					std::wstring folder = path + std::wstring(findData.cFileName) + L"/";

					GetFiles(files, folder, filter, bFindSubFolder);
				}
			}
			else
			{
				std::wstring fileName = path + std::wstring(findData.cFileName);
				files->push_back(fileName);
			}
		} while (FindNextFile(handle, &findData));

		FindClose(handle);
	}
}

void Path::CreateFolder(std::string path)
{
	CreateFolder(String::ToWString(path));
}

void Path::CreateFolder(std::wstring path)
{
	if (ExistDirectory(path) == false)
		CreateDirectory(path.c_str(), NULL);
}

void Path::CreateFolders(std::string path)
{
	CreateFolders(String::ToWString(path));
}

void Path::CreateFolders(std::wstring path) // 상위폴더서부터 만듬
{
	String::Replace(&path, L"\\", L"/");

	std::vector<std::wstring> folders;
	String::SplitString(&folders, path, L"/");

	std::wstring temp = L"";
	for (std::wstring folder : folders)
	{
		temp += folder + L"/";

		CreateFolder(temp);
	}
}

bool Path::IsRelativePath(std::string path)
{
	return IsRelativePath(String::ToWString(path));
}

bool Path::IsRelativePath(std::wstring path)
{
	BOOL b = PathIsRelative(path.c_str());

	return b >= TRUE;
}
