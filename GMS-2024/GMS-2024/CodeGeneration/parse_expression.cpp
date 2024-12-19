#include "stdafx.h"
#include "code_gen.h"
#include <vector>

using namespace std;

CD::CodeGeneration::ParseExpressionReturnParms CD::CodeGeneration::parse_expression(vector<int> ids, vector<string>& instructions, int tabsize)
{
	ParseExpressionReturnParms params;
	params.stringRepresentation = lexems_vector_to_string(ids);
	int countBraces = 0;
	bool insideFunction = false;
	bool isStrCmp = false;

	for (int id : ids)
	{
		LT::Entry& lt_entry = LEX_TABLE.table[id];
		switch (LEX_TABLE.table[id].lexema[0])
		{
		case LEX_LITERAL:
		case LEX_ID:
			if (!insideFunction)
			{
				switch (ID_TABLE.table[lt_entry.idxTI].iddatatype)
				{
				case IT::IDDATATYPE::INT:
					params.isINT = true;
					break;
				case IT::IDDATATYPE::STR:
					params.isSTR = true;
					break;
				case IT::IDDATATYPE::CHAR:
					params.isCHAR = true;
					break;
				}
			}
			if (ID_TABLE.table[lt_entry.idxTI].idtype == IT::IDTYPE::F)
			{
				params.isFunctionCall = true;
				insideFunction = true;
			}
			break;

		case LEX_COMPARE:
			params.isCompare = true;
			params.isSingleVariable = false;
			break;

		case LEX_MATH:
			params.isMath = true;
			params.isSingleVariable = false;
			break;

		case LEX_STRCMP:
			insideFunction = true;
			params.isFunctionCall = true;
			params.isSTR = true;
			isStrCmp = true;
			break;

		case LEX_LEFTTHESIS:
			if (insideFunction)
				countBraces++;
			break;

		case LEX_RIGHTTHESIS:
			if (insideFunction && --countBraces == 0)
				insideFunction = false;
			break;

		default:
			break;
		}
	}

	instructions.push_back(tab * tabsize + "; " + params.stringRepresentation);
	if (params.isCompare)
	{
		vector<vector<int>> operands(2, vector<int>());
		int pos = 0;
		int operationID = 0;

		for (int id : ids)
		{
			if (LEX_TABLE.table[id].lexema[0] == LEX_COMPARE)
			{
				operationID = id;
				pos++;
				continue;
			}
			operands[pos].push_back(id);
		}

		if (params.isSTR)
		{
			params.isStringCompare = true;
			if (operands[0].size() != 1 || operands[1].size() != 1)
			{
				auto ps1 = parse_expression(operands[0], instructions, tabsize);
				if (ps1.isResultInEAX)
				{
					instructions.push_back(format("{}push {}", tab * tabsize, ps1.resultStorage));
				}
				else if (ps1.isINT && ps1.isSingleVariable)
				{
					instructions.push_back(format("{}movzx eax, {}", tab * tabsize, ps1.resultStorage));
					instructions.push_back(format("{}push eax", tab * tabsize));
				}

				auto ps2 = parse_expression(operands[1], instructions, tabsize);

				if (ps2.isResultInEAX)
				{
					instructions.push_back(format("{}push {}", tab * tabsize, ps2.resultStorage));
				}
				else if (ps2.isINT && ps2.isSingleVariable && !ps2.isResultInEAX)
				{
					instructions.push_back(format("{}movzx eax, {}", tab * tabsize, ps2.resultStorage));
					instructions.push_back(format("{}push eax", tab * tabsize));
				}

				instructions.push_back(format("{}pop ebx", tab * tabsize));
				instructions.push_back(format("{}pop eax", tab * tabsize));

				instructions.push_back(tab * tabsize + "cmp eax, ebx");
			}
			else
			{
				ifElseGen.compare_strings(instructions, get_string_value(operands[0][0]), get_string_value(operands[1][0]));
				instructions.push_back(tab * tabsize + "cmp ax, 0");
			}
		}
		else if (params.isINT)
		{
			params.isIntCompare = true;

			ifElseGen.compare_ints(instructions, operands);
			instructions.push_back(tab * tabsize + "cmp ax, bx");
		}
		else
		{
			throw "Неизвестный тип данных в сравнении";
		}

		instructions.push_back(format("{}mov {}, 0", tab * tabsize, reservedBoolName));
		instructions.push_back(format("{}{} @true{}", tab * tabsize, ifElseGen.cmp_op_to_jmp(LEX_TABLE.table[operationID].c), trueLabelsCount));
		instructions.push_back(format("{}jmp @false{}", tab * tabsize, trueLabelsCount));
		instructions.push_back(format("{}@true{}:", tab * tabsize, trueLabelsCount));
		instructions.push_back(format("{}mov {}, 1", tab * tabsize, reservedBoolName));
		instructions.push_back(format("{}@false{}:", tab * tabsize, trueLabelsCount++));

		params.isResultInDefaultBool = true;
		params.isResultComputed = true;
		params.isSingleVariable = false;
		params.resultStorage = reservedBoolName;
	}
	else if (params.isMath)
	{
		auto v = generate_math_expressions(ids);
		for (const std::string& str : v)
		{
			instructions.push_back(tab * tabsize + str);
		}
		params.isSingleVariable = false;
		params.isResultComputed = true;
		params.isResultInEAX = true;
		params.resultStorage = "ax";
	}
	else if (params.isSingleVariable && params.isFunctionCall)
	{
		if (isStrCmp)
		{
			if (ids.size() > 6)
			{
				vector<vector<int>> operands(2, vector<int>());
				int pos = 0;
				int operationID = 0;

				int countBraces = 0;
				insideFunction = false;
				for (int id = ids[2]; id <= ids.back(); id++)
				{
					switch (LEX_TABLE.table[id].lexema[0])
					{
					case LEX_COMMA:
						if (!insideFunction)
						{
							operationID = id;
							pos++;
							continue;
						}
						break;

					case LEX_ID:
						if (ID_TABLE.table[LEX_TABLE.table[id].idxTI].idtype == IT::F)
							insideFunction = true;
						break;

					case LEX_LEFTTHESIS:
						if (insideFunction)
							countBraces++;
						break;

					case LEX_RIGHTTHESIS:
						if (insideFunction && --countBraces == 0)
							insideFunction = false;
						break;

					default:
						break;
					}
					operands[pos].push_back(id);
				}
				operands[pos].pop_back();

				auto ps1 = parse_expression(operands[0], instructions, tabsize);
				if (ps1.isResultInEAX || ps1.isSingleVariable)
				{
					instructions.push_back(format("{}push {}", tab * tabsize, ps1.resultStorage));
				}

				auto ps2 = parse_expression(operands[1], instructions, tabsize);
				if (ps2.isResultInEAX || ps2.isSingleVariable)
				{
					instructions.push_back(format("{}push {}", tab * tabsize, ps2.resultStorage));
				}
				instructions.push_back(format("{}call __StrCmp", tab * tabsize, ps2.resultStorage));
			}
			else
			{
				ifElseGen.compare_strings(instructions, get_string_value(ids[2]), get_string_value(ids[4]));
			}
		}
		else
		{
			parse_lexem(instructions, ids.front(), tabsize);
		}

		params.isResultInEAX = true;
		params.resultStorage = "eax";
	}
	else if (params.isSingleVariable)
	{
		if (params.isINT || params.isCHAR)
		{
			params.resultStorage = get_id_name_in_data_segment(ID_TABLE.table[LEX_TABLE.table[ids[0]].idxTI]);
		}
		else if (params.isSTR)
		{
			params.resultStorage = get_string_value(ids[0]);
		}
	}

	return params;
}