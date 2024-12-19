#include "stdafx.h"
#include "code_gen.h"
#include <vector>

using namespace std;

void CD::CodeGeneration::__parse_return_lexem(std::vector<std::string>& result_instructions, int& index_in_lex_table, int tabsize)
{
	std::vector<int> ids;
	index_in_lex_table++;
	while (LEX_TABLE.table[index_in_lex_table].lexema[0] != LEX_SEMICOLON)
	{
		ids.push_back(index_in_lex_table++);
	}

	auto p = parse_expression(ids, result_instructions, tabsize);

	if (p.isResultInDefaultBool)
	{
		result_instructions.push_back(format("{}mov ax, {}", tab * tabsize, reservedBoolName));
	}
	else if (p.isResultInEAX)
	{
		return;
	}
	else if (p.isResultInSTACK)
	{
		result_instructions.push_back(format("{}pop eax", tab * tabsize));
	}
	else if (p.isSingleVariable)
	{
		if (p.isSTR)
		{
			result_instructions.push_back(format("{}mov eax, {}", tab * tabsize, p.resultStorage));
		}
		else if (p.isCHAR)
		{
			result_instructions.push_back(format("{}mov eax, offset {}", tab * tabsize, p.resultStorage));
		}
		else
		{
			result_instructions.push_back(format("{}movzx eax, {}", tab * tabsize, p.resultStorage));
		}
	}
	if (currentFunction->name == "main")
	{
		result_instructions.push_back(format("{}push eax", tab * tabsize));
	}
	result_instructions.push_back(format("{}jmp {}", tab * tabsize, currentFunction->endLabel));
}
