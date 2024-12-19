#include "stdafx.h"
#include "code_gen.h"

std::vector<std::string> CD::CodeGeneration::parse_function_call(UserDefinedFunctions* function, int params_start_index, int params_end_index)
{
	std::vector<vector<std::string>> params;
	int params_pos = 0;
	vector<int> param;
	bool isInsideFunctionCall = false;
	int countBraces = 0;

	vector< std::string> tmp;
	if (params_start_index > 0 && params_start_index <= params_end_index)
	{
		for (int i = params_start_index; i <= params_end_index; i++)
		{
			LT::Entry* lex = &LEX_TABLE.table[i];
			if (lex->lexema[0] == LEX_COMMA && !isInsideFunctionCall || params_end_index < i + 1)
			{
				if (params_end_index == i)
				{
					param.push_back(i);
				}
				//std::reverse(param.begin(), param.end());
				auto p = parse_expression(param, tmp);

				if (p.isINT && !(function->params[params_pos] == IT::IDDATATYPE::INT) ||
					p.isSTR && !(function->params[params_pos] == IT::IDDATATYPE::STR) ||
					p.isCHAR && !(function->params[params_pos] == IT::IDDATATYPE::CHAR))
				{
					throw "parse_function_call: неверный параметр в вызове функции";
				}
				params_pos++;
				if (p.isSingleVariable)
				{
					if (p.isCHAR)
					{
						tmp.push_back(format("movzx eax, {}", p.resultStorage));
						tmp.push_back("push eax");
					}
					else
					{
						tmp.push_back(format("push {}", p.resultStorage));
					}
				}
				else
				{
					if (p.isResultInEAX)
					{
						tmp.push_back("push ax");
					}
					else if (p.isResultInEBX)
					{
						tmp.push_back("push bx");
					}
					else
					{
						tmp.push_back(p.resultStorage);
					}
				}
				param.clear();
				params.push_back(tmp);
				tmp.clear();
			}
			else
			{
				param.push_back(i);
				if (lex->lexema[0] == LEX_ID && ID_TABLE.table[lex->idxTI].idtype == IT::F)
				{
					isInsideFunctionCall = true;
				}
				if (isInsideFunctionCall && lex->lexema[0] == LEX_LEFTTHESIS)
				{
					countBraces++;
				}
				if (isInsideFunctionCall && lex->lexema[0] == LEX_RIGHTTHESIS)
				{
					countBraces--;
					if (countBraces == 0)
					{
						isInsideFunctionCall = false;
					}
				}
			}
		}

		reverse(params.begin(), params.end());

		for (const auto& pp : params)
		{
			tmp.insert(tmp.end(), pp.begin(), pp.end());
		}
	}
	tmp.push_back(format("call {}", function->name));
	return tmp;
}

void CD::CodeGeneration::__parse_function_body(UserDefinedFunctions* function, int start_index, int end_index)
{
	for (int i = start_index; i < end_index; i++)
	{
		parse_lexem(function->code, i);
	}
}

void CD::CodeGeneration::parse_function(int start_index, int end_index)
{
	UserDefinedFunctions* function = new UserDefinedFunctions();
	currentFunction = function;
	vector<string> instrs;

	std::vector<std::string> params_names;
	bool isMain = false;
	bool f = false;
	int a = 4;
	for (start_index; start_index < end_index && !f; start_index++)
	{
		switch (LEX_TABLE.table[start_index].lexema[0]) {
		case LEX_ID:
			switch (ID_TABLE.table[LEX_TABLE.table[start_index].idxTI].idtype)
			{
			case IT::IDTYPE::F:
				function->name = string(ID_TABLE.table[LEX_TABLE.table[start_index].idxTI].id);
				break;

			case IT::IDTYPE::P:
				function->push_params(ID_TABLE.table[LEX_TABLE.table[start_index].idxTI].iddatatype);

				if (ID_TABLE.table[LEX_TABLE.table[start_index].idxTI].iddatatype == IT::CHAR)
				{
					params_names.push_back(format("mov al, byte ptr [esp + {}]", a));
					params_names.push_back(format("mov {}, al", get_id_name_in_data_segment(ID_TABLE.table[LEX_TABLE.table[start_index].idxTI])));
				}
				else if (ID_TABLE.table[LEX_TABLE.table[start_index].idxTI].iddatatype == IT::INT)
				{
					params_names.push_back(format("mov ax, [esp + {}]", a));
					params_names.push_back(format("mov {}, ax", get_id_name_in_data_segment(ID_TABLE.table[LEX_TABLE.table[start_index].idxTI])));
				}
				else if (ID_TABLE.table[LEX_TABLE.table[start_index].idxTI].iddatatype == IT::STR)
				{
					params_names.push_back(format("mov eax, [esp + {}]", a));
					params_names.push_back(format("mov {}, eax", get_id_name_in_data_segment(ID_TABLE.table[LEX_TABLE.table[start_index].idxTI])));
				}

				a += get_id_size_in_bytes(ID_TABLE.table[LEX_TABLE.table[start_index].idxTI].iddatatype);
				break;

			default:
				break;
			}
			break;

		case LEX_MAIN:
			function->name = "main";
			isMain = true;
			break;

		case LEX_LEFTBRACE:
			f = true;
			break;
		default:
			break;
		}
	}
	function->endLabel = format("{}_END", function->name);
	function->push_code(format("{} proc", function->name));
	function->push_code("start:");
	function->code.insert(function->code.end(), params_names.begin(), params_names.end());
	__parse_function_body(function, start_index, end_index);

	if (isMain)
	{
		function->push_code("push 0");
	}

	function->push_code(format("{}:", function->endLabel));

	if (isMain)
	{
		function->push_code("call ExitProcess");
	}
	else
	{
		function->push_code(format("ret {}", a - 4));
	}

	function->push_code("division_error:");
	function->push_code(format("{}call {}", tab, ExceptionsNames::DivideByZero));

	function->push_code(format("{} endp", function->name));

	if (isMain)
	{
		function->push_code("END main");
	}

	user_functions[function->name] = function;
	__user_functions.push_back(function);
}