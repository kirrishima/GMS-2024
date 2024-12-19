#include "stdafx.h"
#include "code_gen.h"

using namespace std;

std::string CD::CodeGeneration::IfElseGeneration::cmp_op_to_jmp(std::string comparison)
{
	if (comparison == ">") {
		return "jg";
	}
	else if (comparison == "<") {
		return "jl";
	}
	else if (comparison == "==") {
		return "je";
	}
	else if (comparison == "!=") {
		return "jne";
	}
	else if (comparison == ">=") {
		return "jge";
	}
	else if (comparison == "<=") {
		return "jle";
	}
	else {
		throw std::runtime_error("Неожиданная операция сравнения при генерации if-else: " + comparison);
	}
}


void CD::CodeGeneration::IfElseGeneration::__start_if(
	const vector<int>& operands,
	std::vector<std::string>& instructions
) {
	string trueLabel = generate_label("IF_TRUE", IFLabelsCount);
	string endLabel = generate_label("IF_END", IFLabelsCount);
	currentElseLabel = IFLabelsCount++;
	if_stack.push(endLabel);

	vector<string> comments_operands;
	string tmp;
	bool isStringCmp = false;

	for (const int n : operands)
	{
		tmp += parent.lexem_to_source(parent.LEX_TABLE.table[n]);
	}
	auto p = parent.parse_expression(operands, instructions, nestingLevel);
	instructions.push_back(parent.tab * nestingLevel + format("; Начало if{} ({}) ", currentElseLabel, tmp));

	instructions.push_back(parent.tab * nestingLevel + "cmp " + parent.reservedBoolName + ", 1");
	instructions.push_back(parent.tab * nestingLevel + "je " + trueLabel);
	instructions.push_back(parent.tab * nestingLevel + "jmp " + endLabel);
	instructions.push_back(parent.tab * nestingLevel++ + trueLabel + ':');
}

void CD::CodeGeneration::IfElseGeneration::compare_ints(std::vector<std::string>& instructions, const vector<std::vector<int> >& operands)
{
	auto math_instructionsRight = parent.generate_math_expressions(operands[1]);
	auto math_instructionsLeft = parent.generate_math_expressions(operands[0]);

	if (math_instructionsRight.size() == 1 && math_instructionsLeft.size() == 1)
	{
		instructions.push_back(parent.tab * nestingLevel + "mov ax, " + parent.lexems_vector_to_string(operands[0]) + parent.tab + "; lefthand операнд");
		instructions.push_back(parent.tab * nestingLevel + "mov bx, " + parent.lexems_vector_to_string(operands[1]) + parent.tab + "; righthand операнд");
		instructions.push_back("");
	}
	else
	{
		instructions.push_back(parent.tab * nestingLevel + "; Вычисляем righthand операнд:");
		for (const std::string& instr : math_instructionsRight)
		{
			instructions.push_back(parent.tab * nestingLevel + instr);
		}
		if (instructions.back() != parent.tab * nestingLevel + "push ax" && math_instructionsRight.size() != 1)
		{
			instructions.push_back(parent.tab * nestingLevel + "push ax");
		}

		if (math_instructionsLeft.size() != 1)
		{
			instructions.push_back(parent.tab * nestingLevel + "; Вычисляем lefthand операнд:");
			for (const std::string& instr : math_instructionsLeft)
			{
				instructions.push_back(parent.tab * nestingLevel + instr);
			}
			instructions.push_back(parent.tab * nestingLevel + "; значение lefhand уже в eax");
		}
		else
		{
			instructions.push_back("\n" + parent.tab * nestingLevel + "mov ax, " + parent.lexems_vector_to_string(operands[0]) + parent.tab + "; lefthand операнд");
		}

		instructions.push_back(parent.tab * nestingLevel + "pop bx ; загружаем значение righthand операнда");
	}
}

void CD::CodeGeneration::IfElseGeneration::compare_strings(std::vector<std::string>& instructions, const string& str1Name, const string& str2Name)
{
	instructions.push_back(parent.tab * nestingLevel + "push " + str2Name);
	instructions.push_back(parent.tab * nestingLevel + "push " + str1Name);
	instructions.push_back(parent.tab * nestingLevel + "call __StrCmp");
}

void CD::CodeGeneration::IfElseGeneration::__start_else(std::vector<std::string>& instructions) {
	if (if_stack.empty()) {
		throw std::runtime_error("Ошибка: стек if-переходов пуст!");
	}
	string endLabel = if_stack.top();
	string elseLabel = generate_label("ELSE", currentElseLabel);
	for (size_t i = 0; i < instructions.size(); i++)
	{
		if (instructions[i].find(endLabel) != std::string::npos)
		{
			size_t pos = instructions[i].find(endLabel);
			instructions[i].replace(pos, endLabel.length(), elseLabel);
			break;
		}

	}
	instructions.push_back(parent.tab * (nestingLevel - 1) + elseLabel + ':');
}

void CD::CodeGeneration::IfElseGeneration::__end_if_or_else(std::vector<std::string>& instructions) {
	if (if_stack.empty()) {
		throw std::runtime_error("Ошибка: стек if-переходов пуст!");
	}

	string endLabel = if_stack.top();
	instructions.push_back(parent.tab * nestingLevel + "jmp " + endLabel +
		parent.tab + "; Переход к выходу из " + to_string(currentElseLabel) + " if-else");
}

void CD::CodeGeneration::IfElseGeneration::__end_expression(std::vector<std::string>& instructions) {
	if (if_stack.empty()) {
		throw std::runtime_error("Ошибка: стек if-переходов пуст!");
	}

	currentElseLabel--;

	string endLabel = if_stack.top();
	if_stack.pop();

	instructions.push_back(parent.tab * --nestingLevel + endLabel + ':');
}


std::vector<std::string> CD::CodeGeneration::IfElseGeneration::generate_if_statement(int& i)
{
	std::vector<std::string> instructions;
	int ifcounts = 0;
	while (true) {
		if (parent.LEX_TABLE.table[i].lexema[0] == '?')
		{
			i += 2;
			std::vector<int> ops;
			std::string operation;
			int currentOperand = 0;

			while (i < parent.LEX_TABLE.size && parent.LEX_TABLE.table[i].lexema[0] != '{')
			{
				switch (parent.LEX_TABLE.table[i].lexema[0]) {
				case ')':
					if (parent.LEX_TABLE.table[i + 1].lexema[0] == '{')
					{
						break;
					}
				default:
					ops.push_back(i);
					break;
				}
				i++;
			}
			ifcounts++;
			__start_if(ops, instructions);
		}

		vector<int> ids;
		while (i < parent.LEX_TABLE.size && parent.LEX_TABLE.table[i].lexema[0] != '}')
		{
			parent.parse_lexem(instructions, i, nestingLevel);
			i++;
		}

		__end_if_or_else(instructions);

		if (parent.LEX_TABLE.table[i + 1].lexema[0] == ':')
		{
			i += 2;
			__start_else(instructions);

			if (parent.LEX_TABLE.table[i].lexema[0] == '?')
			{
				std::vector<std::string> insts = generate_if_statement(i);
				for (const std::string& ins : insts)
				{
					instructions.push_back(ins);
				}
			}

			continue;
		}
		break;
	}
	__end_expression(instructions);

	return instructions;
}