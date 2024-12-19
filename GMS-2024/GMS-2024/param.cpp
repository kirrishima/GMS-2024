#include "stdafx.h"
#include "parm.h"
#include <tchar.h>
#include "error.h"
#include <filesystem>
#include <map>
#include <algorithm>
#include <set>

namespace fs = std::filesystem;
using namespace std;

bool validatePath(const std::wstring& filePath);
bool validateHtmlPath(const std::filesystem::path& path);
void validatePaths(const Parm::PARM& parameters);
bool areDirectoriesValid(const fs::path& path);

namespace Parm
{

	PARM getparm(int argc, _TCHAR* argv[])
	{
		PARM parm;

		bool in = false, out = false, log = false;

		std::map<std::wstring, std::wstring> namedParams;
		std::map<std::wstring, bool> flags;

		std::set<std::wstring> validKeys = {
			PARM_IN, PARM_OUT, PARM_LOG, PARM_EXE, PARM_LT, PARM_IT, PARM_STACK
		};

		std::set<std::wstring> validFlags = {
			PARM_CST, PARM_LEX
		};

		for (int i = 1; i < argc; i++)
		{
			std::wstring arg = argv[i];

			if (arg[0] == L'-' && arg.find(L':') != std::wstring::npos)
			{
				size_t colonPos = arg.find(L':');
				std::wstring key = arg.substr(1, colonPos - 1);

				std::transform(key.begin(), key.end(), key.begin(),
					[](TCHAR c)
					{ return std::tolower(c, std::locale()); });

				std::wstring value = arg.substr(colonPos + 1);
				namedParams[key] = value;

				if (validKeys.find(key) != validKeys.end()) {
					namedParams[key] = value;
				}
				else {
					wcout << L"Нераспознанный параметр " << arg << L" пропущен.\n";
				}
			}
			else if (arg[0] == L'/' && arg.length() > 1)
			{
				std::wstring flag = arg.substr(1);

				std::transform(flag.begin(), flag.end(), flag.begin(),
					[](TCHAR c)
					{ return std::tolower(c, std::locale()); });

				if (validFlags.find(flag) != validFlags.end()) {
					flags[flag] = true;
				}
				else {
					wcout << L"Нераспознанный флаг " << arg << L" пропущен.\n";
				}
			}
			else
			{
				std::wcout << L"Неверный формат аргумента: " << arg << std::endl;
			}
		}
		if (namedParams.find(PARM_IN) == namedParams.end())
		{
			throw ERROR_THROW(100);
		}
		else
		{
			parm.in = namedParams[PARM_IN];
		}

		if (namedParams.find(PARM_LOG) == namedParams.end())
		{
			parm.log = parm.in + PARM_LOG_DEFAULT_EXT;
		}
		else
		{
			if (!areDirectoriesValid(namedParams[PARM_LOG]))
			{
				throw ERROR_THROW(116);
			}
			parm.log = namedParams[PARM_LOG];
		}

		if (namedParams.find(PARM_EXE) == namedParams.end())
		{
			parm.exeName = parm.in + PARM_EXE_EXTENSION;
			parm._objName = parm.in + PARM_OBJ_EXTENSION;
		}
		else
		{
			if (!areDirectoriesValid(namedParams[PARM_EXE]) || !namedParams[PARM_EXE].ends_with(PARM_EXE_EXTENSION))
			{
				throw ERROR_THROW(122);
			}
			parm.exeName = namedParams[PARM_EXE];
			parm._objName = namedParams[PARM_EXE] + PARM_OBJ_EXTENSION;
		}

		if (namedParams.find(PARM_OUT) == namedParams.end())
		{
			parm.out = parm.in + PARM_OUT_DEFAULT_EXT;
		}
		else
		{
			if (!areDirectoriesValid(namedParams[PARM_OUT]))
			{
				throw ERROR_THROW(117);
			}
			parm.out = namedParams[PARM_OUT];
		}

		if (namedParams.find(PARM_LT) != namedParams.end())
		{
			std::filesystem::path ltPath(namedParams[PARM_LT]);
			if (!validateHtmlPath(ltPath))
			{
				throw ERROR_THROW(118);
			}
			parm.lt = namedParams[PARM_LT];
		}
		else
		{
			parm.lt = parm.in + PARM_LT_TABLE_DEFAULT;
		}

		if (namedParams.find(PARM_IT) != namedParams.end())
		{
			std::filesystem::path itPath(namedParams[PARM_IT]);
			if (!validateHtmlPath(itPath))
			{
				throw ERROR_THROW(119);
			}
			parm.it = namedParams[PARM_IT];
		}
		else
		{
			parm.it = parm.in + PARM_IT_TABLE_DEFAULT;
		}

		if (namedParams.find(PARM_STACK) != namedParams.end())
		{
			try
			{
				size_t stackSize = std::stoul(namedParams[PARM_STACK]);

				if (stackSize < STACK_MIN_SIZE || stackSize > STACK_MAX_SIZE)
				{
					throw ERROR_THROW(120);
				}

				parm.stackSize = stackSize;
			}
			catch (const std::exception&)
			{
				throw ERROR_THROW(121);
			}
		}
		else
		{
			parm.stackSize = PARM_STACK_DEFAULT_SIZE;
		}

		if (flags.find(PARM_LEX) != flags.end())
		{
			parm.enableLexAnSave = true;
		}

		if (flags.find(PARM_CST) != flags.end())
		{
			parm.CST = true;
		}

		parm._masmDest = (fs::path(fs::absolute(parm.in)).parent_path() / (fs::path(parm.in).filename().string() + ".asm")).wstring();
		validatePaths(parm);

		return parm;
	}
}

bool areDirectoriesValid(const fs::path& path)
{
	fs::path parentPath = path.parent_path();
	while (!parentPath.empty())
	{
		if (!fs::exists(parentPath))
		{
			return false;
		}
		parentPath = parentPath.parent_path();
	}
	return true;
}

bool isFilePath(const fs::path& path)
{
	return path.has_filename();
}

bool validatePath(const std::wstring& filePath)
{
	std::filesystem::path path(filePath);

	if (fs::is_directory(path))
	{
		return false;
	}

	std::filesystem::path dirPath = path.parent_path();
	if (!std::filesystem::exists(dirPath))
	{
		return false;
	}

	return true;
}

bool validateHtmlPath(const std::filesystem::path& path)
{

	if (isFilePath(path) && areDirectoriesValid(path))
	{
		return path.extension() == ".html";
	}

	return false;
}

void validatePaths(const Parm::PARM& parameters)
{
	std::unordered_map<std::wstring, std::vector<std::wstring>> pathMap;

	pathMap[parameters.in].push_back(L"in");
	pathMap[parameters.out].push_back(L"out");
	pathMap[parameters.log].push_back(L"log");
	pathMap[parameters.it].push_back(L"it");
	pathMap[parameters.lt].push_back(L"lt");

	bool hasDuplicates = false;
	for (const auto& [path, fields] : pathMap)
	{
		if (fields.size() > 1)
		{
			hasDuplicates = true;
			std::wcout << L"Ошибка: путь '" << path << L"' совпадает в параметрах: ";
			for (const auto& field : fields)
			{
				std::wcout << field << L" ";
			}
			std::wcout << std::endl;
		}
	}

	if (hasDuplicates)
	{
		throw ERROR_THROW(121);
	}
}
