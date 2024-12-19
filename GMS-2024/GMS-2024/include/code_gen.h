#pragma once
#include <string>
#include "LT.h"
#include "IT.h"
#include <fstream>
#include <format>
#include <stack>
#include "parm.h"

using namespace std;

bool isMASMLabel(const string& line);
string removeComment(const string& str);
bool isValidIdentifier(const string& identifier);
bool isFunctionStart(const string& line);
bool isFunctionEnd(const string& line);
int get_id_size_in_bytes(IT::IDDATATYPE type);
int getEscapeCode(const string& input);

string operator*(const string& str, int times);

namespace CD
{
	bool is_assignment(const string& expr);

	struct UserDefinedFunctions;
	struct ExceptionsNames;

	struct CodeGeneration {
		string tab = "    ";
		const string reservedBoolName = "@bool_RESERVED";
		int trueLabelsCount = 0;

		IT::ID_Table ID_TABLE;
		LT::LexTable LEX_TABLE;
		ofstream OUT_ASM_FILE;
		const Parm::PARM* parm;
		Log::LOG* log;

		UserDefinedFunctions* currentFunction = nullptr;

		unordered_map<string, UserDefinedFunctions*> user_functions;
		vector<UserDefinedFunctions*> __user_functions;

		CodeGeneration(const IT::ID_Table& ID_TABLE, const LT::LexTable& LEX_TABLE, const Parm::PARM* parm, Log::LOG* log)
			: ID_TABLE(ID_TABLE), LEX_TABLE(LEX_TABLE), parm(parm), log(log), ifElseGen(*this)
		{
			this->OUT_ASM_FILE.open(parm->_masmDest);
		}

		static string get_id_name_in_data_segment(const IT::Entry& entry);
		vector<string> __parse_math_expression(const vector<int>& expression);
		vector<string> generate_math_expressions(const vector<int>& expr);

		void __s_const();
		void __s_data();

		string lexems_vector_to_source_string(const vector<int>& ids);
		string lexem_to_source(LT::Entry& entry);

		vector<string> parse_function_call(UserDefinedFunctions* function, int params_start_index, int params_end_index);
		void parse_function(int start_index, int end_index);
		void __parse_function_body(UserDefinedFunctions* function, int start_index, int end_index);

		void parse_lexem(vector<string>& result_instructions, int& index_in_lex_table, int tabsize = 0);
		void __parse_lexem_equal(vector<string>& result_instructions, int& index_in_lex_table, int tabsize = 0);
		void __parse_return_lexem(vector<string>& result_instructions, int& index_in_lex_table, int tabsize);
		void __parse_id_lexem(vector<string>& result_instructions, int& index_in_lex_table, int tabsize);

		struct ParseExpressionReturnParms
		{
			bool isSingleVariable = true;
			bool isStringPTR = false;

			bool isResultComputed = false;
			bool isResultInDefaultBool = false;
			bool isResultInEAX = false;
			bool isResultInEBX = false;
			bool isResultInSTACK = false;

			bool isStringCompare = false;
			bool isIntCompare = false;
			bool isCompare = false;

			bool isMath = false;
			bool isINT = false;
			bool isSTR = false;
			bool isCHAR = false;
			bool isFunctionCall = false;

			IT::IDDATATYPE returnDataType;
			string stringRepresentation;
			string resultStorage;
		};

		ParseExpressionReturnParms parse_expression(vector<int> ids, vector<string>& instructions, int tabsize = 0);
		void parse_print_lexem__(vector<string>& result_instructions, int& i, int tabsize = 0);

		string get_string_value(const int lex_id);
		string lexems_vector_to_string(const vector<int>& ids, bool comments = false);

		int generateCode();

		struct IfElseGeneration {
			CodeGeneration& parent;

			IfElseGeneration(CodeGeneration& p) : parent(p) {}

			int IFLabelsCount = 0;
			int currentElseLabel = 0;
			int nestingLevel = 0;

			stack<string> if_stack;

			string cmp_op_to_jmp(string comparison);

			void compare_ints(vector<string>& instructions, const vector<vector<int> >& operands);
			void compare_strings(vector<string>& instructions, const string& str1Name, const string& str2Name);

			void __start_if(const vector<int>& operands,
				vector<string>&
			);

			void __start_else(vector<string>& instructions);
			void __end_if_or_else(vector<string>& instructions);
			void __end_expression(vector<string>& instructions);

			vector<string> generate_if_statement(int& i);

			string generate_label(const string& prefix, int n) {
				return prefix + "_" + to_string(n);
			}
		} ifElseGen;
	};


	struct UserDefinedFunctions
	{
		string name;
		string endLabel;
		vector<IT::IDDATATYPE> params;
		vector<string> code;

		void push_code(string code_instruction) {
			code.push_back(code_instruction);
		}

		void push_code(const char* code_instruction) {
			code.push_back(string(code_instruction));
		}

		void push_params(const IT::IDDATATYPE type) {
			params.push_back(type);
		}
	};

	struct ExceptionsNames {
		static const string DivideByZero;
	};

#define COMPILE_COMMAND R"(cmd /c "ml /c /nologo /Zi /Fo {} {}")"
#define LINK_COMMAND R"(cmd /c "link /nologo /DEBUG /subsystem:console /OUT:{} {} libucrt.lib libcmt.lib libvcruntime.lib kernel32.lib /OPT:REF /OPT:ICF /NODEFAULTLIB:libcmtd /NODEFAULTLIB:MSVCRTD")"
#define BASE R"(.586
.model flat, stdcall
includelib  gms2024stdlib.lib
ExitProcess PROTO : DWORD
__PrintNumber PROTO :SDWORD
__PrintBool PROTO :BYTE
__PrintArray PROTO :DWORD, :DWORD, :DWORD
__PrintStr PROTO :DWORD
__PrintChar PROTO :DWORD

__StreamWriteNumber PROTO :SDWORD
__StreamWriteBool PROTO :BYTE
__StreamWriteArray PROTO :DWORD, :DWORD, :DWORD
__StreamWriteStr PROTO :DWORD
__StreamWriteChar PROTO :DWORD

__StrCmp PROTO :DWORD, :DWORD

__NullDevisionException PROTO)";
}