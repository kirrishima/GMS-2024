#ifndef LOG_H
#define LOG_H

#include "in.h"
#include "parm.h"
#include "error.h"
#include <string>

namespace Log
{
	struct LOG
	{
		std::wstring logfile;
		std::unique_ptr<std::ofstream> stream;
	};

	static const LOG INIT_LOG{ L"", nullptr };
	LOG getlog(const std::wstring& logfile);
	void writeLine(LOG& log, char* c, ...);
	void writeLine(LOG& log, wchar_t* с, ...);
	void writeLine(LOG& log, const std::string& str);
	void writeLine(LOG& log, const std::wstring& str);
	void writeLog(LOG& log);
	void writeParm(LOG& log, Parm::PARM parm);
	void writeIn(LOG& log, In::IN& in);
	void writeError(LOG& log, Error::ERROR& error);
	void close(LOG& log);
};

#endif  
