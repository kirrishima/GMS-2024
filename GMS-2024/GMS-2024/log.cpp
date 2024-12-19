#include "stdafx.h"
#include "log.h"
#include "utils.h"
#include <iomanip>
#include <string>
#pragma warning(disable : 4996)

using namespace std;

namespace Log
{
	const int fieldWidth = 8;

	LOG getlog(const std::wstring& logfile)
	{
		LOG log;
		log.stream = std::make_unique<std::ofstream>(logfile);
		if (!log.stream->is_open())
		{
			throw ERROR_THROW(112);
		}

		log.logfile = logfile;
		return log;
	}

	void writeLine(LOG& log, char* c, ...)
	{
		if (log.stream)
		{
			char** currentArg = &c;
			while (*currentArg != nullptr && *currentArg != "")
			{
				*log.stream << *currentArg;
				currentArg++;
			}
		}
	}

	void writeLine(LOG& log, const std::string& str)
	{
		if (log.stream)
		{
			(*log.stream) << str << std::endl;
		}
	}

	void writeLine(LOG& log, const std::wstring& str)
	{
		if (log.stream)
		{
			*log.stream << utils::wstring_to_string(str) << '\n';
		}
	}

	void writeLine(LOG& log, wchar_t* c, ...)
	{
		if (log.stream)
		{
			wchar_t** currentArg = &c;

			while (*currentArg != nullptr && *currentArg != L"")
			{
				size_t len = wcstombs(nullptr, *currentArg, 0) + 1;
				if (len == (size_t)-1)
				{
					break;
				}

				char* tempBuffer = (char*)malloc(len);
				if (tempBuffer != nullptr)
				{
					wcstombs(tempBuffer, *currentArg, len);
					*log.stream << tempBuffer;
					free(tempBuffer);
				}
				currentArg++;
			}
		}
	}

	void writeLog(LOG& log)
	{
		if (log.stream)
		{
			char date[100];
			tm local;
			time_t currentTime = time(NULL);
			localtime_s(&local, &currentTime);
			strftime(date, 100, " %d.%m.%Y %H:%M:%S    ----", &local);
			*log.stream << " ----    Протокол" << date << endl;
		}
	}

	void writeParm(LOG& log, Parm::PARM parm)
	{
		if (log.stream)
		{
			*log.stream << "\n ----    Параметры    ---- " << std::endl;

			*log.stream << std::left;
			*log.stream << std::setw(fieldWidth) << "-log:" << "\"" << utils::wstring_to_string(parm.log) << "\"" << std::endl;
			*log.stream << std::setw(fieldWidth) << "-out:" << "\"" << utils::wstring_to_string(parm.out) << "\"" << std::endl;
			*log.stream << std::setw(fieldWidth) << "-in:" << "\"" << utils::wstring_to_string(parm.in) << "\"" << std::endl;
			*log.stream << std::setw(fieldWidth) << "-lt:" << "\"" << utils::wstring_to_string(parm.lt) << "\"" << std::endl;
			*log.stream << std::setw(fieldWidth) << "-it:" << "\"" << utils::wstring_to_string(parm.it) << "\"" << std::endl;
			*log.stream << std::setw(fieldWidth) << "-fo:" << "\"" << utils::wstring_to_string(parm.exeName) << "\"" << std::endl;
			*log.stream << std::setw(fieldWidth) << "-stack:" << parm.stackSize << std::endl;

			*log.stream << "\n ----    Опции    ---- " << std::endl;

			*log.stream << std::setw(fieldWidth) << "/lex:" << (parm.enableLexAnSave ? "True" : "False") << std::endl;
			*log.stream << std::setw(fieldWidth) << "/cst:" << (parm.CST ? "True" : "False") << std::endl
				<< std::endl;
		}
	}

	void writeIn(LOG& log, In::IN& in)
	{
		if (log.stream)
		{
			*log.stream << " ----    Исходные данные    ---- " << endl;
			*log.stream << "Количество символов : " << in.size << endl;
			*log.stream << "Проигнорировано     : " << in.ignore << endl;
			*log.stream << "Количество строк    : " << in.lines << endl << endl;
		}
	}

	void writeError(LOG& log, Error::ERROR& error)
	{
		if (log.stream)
		{
			*log.stream << " ----    Ошибка    ---- " << endl;
			*log.stream << "Ошибка " << error.id << ": " << error.message;
			if (error.inext.col >= 0 && error.inext.line >= 0)
			{
				*log.stream << ". Строка: " << error.inext.line << ", cтолбец: " << error.inext.col << endl
					<< endl;
			}
			else if (error.sourceLine > 0)
			{
				*log.stream << ". Строка: " << error.sourceLine;
			}
			*log.stream << endl;
		}
	}

	void close(LOG& log)
	{
		if (log.stream)
		{
			log.stream->close();
		}
	}
}
