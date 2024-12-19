#ifndef OUT_H
#define OUT_H

#include "in.h"
#include "parm.h"
#include "error.h"
#include <string>

namespace Out
{
	struct OUT
	{
		std::wstring outfile;
		std::unique_ptr<std::ofstream> stream;
	};
	static const OUT INIT_OUT = { L"", NULL };
	OUT getout(const std::wstring& outfile);
	void writeOut(OUT& out, In::IN& in);
	void writeError(OUT& out, Error::ERROR& error);
	void close(OUT& out);
};

#endif  
