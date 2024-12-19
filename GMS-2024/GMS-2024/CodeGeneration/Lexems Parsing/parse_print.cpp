#include "stdafx.h"
#include "code_gen.h"
#include <vector>

using namespace std;

void CD::CodeGeneration::parse_print_lexem__(std::vector<std::string>& result_instructions, int& index_in_lex_table, int tabsize)
{
	bool isPrint = false;
	if (LEX_TABLE.table[index_in_lex_table].lexema[0] != LEX_PRINT
		&& LEX_TABLE.table[index_in_lex_table].lexema[0] != LEX_WRITE)
	{
		throw "__parse_print_expression_: передан индекс лексемы, которая не равна LEX_PRINT";
	}
	vector<int> lexems;

	std::string printFunctionPrefix;

	if (LEX_TABLE.table[index_in_lex_table].lexema[0] == LEX_WRITE)
	{
		printFunctionPrefix = "__StreamWrite";
	}
	else
	{
		isPrint = true;
		printFunctionPrefix = "__Print";
	}

	if (isPrint)
	{
		index_in_lex_table += 2;
	}
	else
	{
		index_in_lex_table++;
	}

	while (LEX_TABLE.table[index_in_lex_table].lexema[0] != LEX_SEMICOLON)
	{
		lexems.push_back(index_in_lex_table++);
	};

	if (isPrint)
	{
		lexems.pop_back();
	}

	auto p = parse_expression(lexems, result_instructions, tabsize);

	if (p.isResultInDefaultBool)
	{
		result_instructions.push_back(format("{}movzx ax, {}", tab * tabsize, reservedBoolName));
		result_instructions.push_back(tab * tabsize + "push ax");
		result_instructions.push_back(format("{}call {}Bool", tab * tabsize, printFunctionPrefix));
	}
	else if (p.isMath)
	{
		if (p.isResultInEAX)
		{
			result_instructions.push_back(tab * tabsize + "push eax");
			result_instructions.push_back(format("{}call {}Number", tab * tabsize, printFunctionPrefix));
		}
		else
		{
			result_instructions.push_back(format("{}push {}", tab * tabsize, p.resultStorage));
			result_instructions.push_back(format("{}call {}Number", tab * tabsize, printFunctionPrefix));
		}
	}
	else if (p.isSTR)
	{
		result_instructions.push_back(format("{}push {}", tab * tabsize, p.resultStorage));
		result_instructions.push_back(format("{}call {}Str", tab * tabsize, printFunctionPrefix));
	}
	else if (p.isINT)
	{
		if (!p.isResultInEAX)
		{
			result_instructions.push_back(format("{}movzx eax, {}", tab * tabsize, p.resultStorage, printFunctionPrefix));
			result_instructions.push_back(format("{}push eax", tab * tabsize));
		}
		else
		{
			result_instructions.push_back(format("{}push eax", tab * tabsize));
		}
		result_instructions.push_back(format("{}call {}Number", tab * tabsize, printFunctionPrefix));
	}
	else if (p.isCHAR)
	{
		if (p.isFunctionCall)
		{
			result_instructions.push_back(format("{}push {}", tab * tabsize, p.resultStorage));
		}
		else
		{
			result_instructions.push_back(format("{}push offset {}", tab * tabsize, p.resultStorage));
		}
		result_instructions.push_back(format("{}call {}Char", tab * tabsize, printFunctionPrefix));
	}
}
