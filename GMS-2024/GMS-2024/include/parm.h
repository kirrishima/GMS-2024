#ifndef PARAM_H
#define PARAM_H

#include <string>

#define PARM_IN L"in"      
#define PARM_OUT L"out"      
#define PARM_LOG L"log"     
#define PARM_EXE L"fo"     
#define PARM_LT L"lt"     
#define PARM_IT L"it"     
#define PARM_STACK L"stack"     
#define PARM_LEX L"lex"     
#define PARM_CST L"cst"      
#define PARM_OUT_DEFAULT_EXT L".out"       
#define PARM_LOG_DEFAULT_EXT L".log"       
#define PARM_LT_TABLE_DEFAULT L".LT.html"       
#define PARM_IT_TABLE_DEFAULT L".IT.html"       
#define PARM_OBJ_EXTENSION L".obj"     
#define PARM_EXE_EXTENSION L".exe"     
#define PARM_STACK_DEFAULT_SIZE 8192       
#define STACK_MIN_SIZE 1024
#define STACK_MAX_SIZE STACK_MIN_SIZE * STACK_MIN_SIZE

namespace Parm
{
	struct PARM
	{
		std::wstring in;
		std::wstring out;
		std::wstring log;
		std::wstring it;
		std::wstring lt;
		std::wstring _masmDest;
		std::wstring _objName;
		std::wstring exeName;
		size_t stackSize;
		bool enableLexAnSave = false;
		bool CST = false;
	};

	PARM getparm(int argc, _TCHAR* argv[]);

};

#endif  