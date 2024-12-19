#include "stdafx.h"
#include "out.h"

namespace Out
{
	OUT getout(const std::wstring& outfile)
	{
		OUT a;
		a.stream = std::make_unique<std::ofstream>();
		a.stream->open(outfile);
		if (a.stream->fail())
		{
			throw ERROR_THROW(113);
		}
		a.outfile = outfile;
		return a;
	}

	void writeOut(OUT& out, In::IN& in)
	{
		if (out.stream)
		{
			*out.stream << in.text << std::endl;
		}
	}

	void writeError(OUT& out, Error::ERROR& error)
	{
		if (out.stream)
		{
			*out.stream << " ----	Ошибка	---- " << std::endl;
			*out.stream << "Ошибка " << error.id << ": " << error.message << std::endl;
			if (error.inext.col >= 0 && error.inext.line >= 0)
			{
				*out.stream << "Строка: " << error.inext.line << std::endl
					<< "Столбец: " << error.inext.col << std::endl
					<< std::endl;
			}
		}
	}

	void close(OUT& out)
	{
		if (out.stream)
		{
			out.stream->close();
			out.stream = NULL;
		}
	}
}