#include "stdafx.h"
#include "code_gen.h"
#include <vector>

using namespace std;

void CD::CodeGeneration::__parse_id_lexem(std::vector<std::string>& result_instructions, int& index_in_lex_table, int tabsize)
{
	if (LEX_TABLE.table[index_in_lex_table + 1].lexema[0] == LEX_LEFTTHESIS)
	{
		std::string name = ID_TABLE.table[LEX_TABLE.table[index_in_lex_table].idxTI].id;
		index_in_lex_table += 2;
		std::vector<int> ids;

		stack<char> parenthesis;
		parenthesis.push(LEX_LEFTTHESIS);

		while (index_in_lex_table < LEX_TABLE.size)
		{
			if (LEX_TABLE.table[index_in_lex_table].lexema[0] == LEX_RIGHTTHESIS)
			{
				parenthesis.pop();
				if (parenthesis.size() == 0)
				{
					break;
				}
			}
			if (LEX_TABLE.table[index_in_lex_table].lexema[0] == LEX_LEFTTHESIS)
			{
				parenthesis.push(index_in_lex_table);
			}
			ids.push_back(index_in_lex_table++);
		}
		auto v = parse_function_call(user_functions[name], ids.size() ? ids.front() : -1, ids.size() ? ids.back() : -1);
		for (const std::string& str : v)
		{
			result_instructions.push_back(tab * tabsize + str);
		}
	}
}
