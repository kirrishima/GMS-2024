#include "stdafx.h"
#include "MFST.h"
#include "code_gen.h"
#include "algorithm"

using namespace std;
using namespace MFST;
using namespace GRB;

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "rus");

	int exitCode = 0;

	cout << R"(
*************************************************************
*                  x86 Compiler GMS-2024                    *
*     Course project for Software Engineering at BSTU       *
*************************************************************
)" << '\n';

	Log::LOG log;
	Out::OUT out;

	try
	{
		Parm::PARM parm = Parm::getparm(argc, argv);
		In::IN in = In::getin(parm.in);
		out = Out::getout(parm.out);
		log = Log::getlog(parm.log);

		Log::writeLog(log);
		Log::writeParm(log, parm);
		Out::writeOut(out, in);
		Log::writeIn(log, in);

		auto [LexTable, IdTable] = lex_analysis::lexAnalize(parm, in, log);

		MFST::Mfst mfst(LexTable, GRB::getGreibach());

		if (!mfst.start(log))
		{
			exit(1);
		}
		if (parm.CST)
		{
			mfst.savededucation();
			mfst.printrules();
		}
		cout << endl;
		int x = semantic::check(IdTable, LexTable, &log);
		if (x)
		{
			cout << "\nВсего ошибок: " << x << endl;
			IT::Delete(IdTable);
			LT::Delete(LexTable);
			exit(1);
		}

		CD::CodeGeneration cd(IdTable, LexTable, &parm, &log);

		exitCode = cd.generateCode();

		IT::Delete(IdTable);
		LT::Delete(LexTable);
	}
	catch (const char* e)
	{
		exitCode = 1;
		cout << "Произошла ошибка: " << e;
	}
	catch (std::string& e)
	{
		exitCode = 1;
		cout << "Произошла ошибка: " << e;
	}
	catch (Error::ERROR e)
	{
		exitCode = 1;
		cout << "Ошибка " << e.id << ": " << e.message;
		if (e.sourceLine > 0)
		{
			cout << ". Строка: " << e.sourceLine;
		}
		cout << endl;
		Log::writeError(log, e);
		Out::writeError(out, e);
		Log::close(log);
		Out::close(out);
	}
	catch (std::runtime_error& e)
	{
		exitCode = 1;
		cout << "Ошибка: " << e.what() << endl;
	}
	catch (std::exception& e)
	{
		exitCode = 1;
		cout << "Ошибка: " << e.what() << endl;
	}

	return exitCode;
}