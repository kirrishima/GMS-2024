#include "stdafx.h"
#include "code_gen.h"
#include <vector>

using namespace std;

void CD::CodeGeneration::parse_lexem(std::vector<std::string>& result_instructions, int& index_in_lex_table, int tabsize)
{
	switch (LEX_TABLE.table[index_in_lex_table].lexema[0])
	{
	case LEX_EQUAL:
	{
		__parse_lexem_equal(result_instructions, index_in_lex_table, tabsize);
		break;
	}

	case LEX_PRINT:
	case LEX_WRITE:
	{
		parse_print_lexem__(result_instructions, index_in_lex_table, tabsize);
		break;
	}

	case LEX_ID:
		__parse_id_lexem(result_instructions, index_in_lex_table, tabsize);
		break;

	case LEX_RETURN:
	{
		__parse_return_lexem(result_instructions, index_in_lex_table, tabsize);
		break;
	}
	case '?':
		for (const std::string& str : ifElseGen.generate_if_statement(index_in_lex_table))
		{
			result_instructions.push_back(tab * tabsize + str);
		}
		break;
	default:
		break;
	}
}
