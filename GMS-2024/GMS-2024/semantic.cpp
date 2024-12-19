#include "stdafx.h"
#include "semantic.h"
#include <set>

using namespace std;

int errors = 0;

#define ID_ENTRY_BY_LEX_ID(id) ID_Table.table[LEX_Table.table[id].idxTI]

unordered_map<IT::Entry *, std::vector<IT::IDDATATYPE>> protos;
unordered_map<IT::Entry *, bool> isInitialized;

unordered_map<IT::Entry *, std::vector<std::pair<IT::Entry *, int>>> references;
IT::Entry *currentFunction;
Log::LOG *logObject = nullptr;

std::string iddatatype_to_str(IT::IDDATATYPE t)
{
	switch (t)
	{
	case IT::INT:
		return "int";
	case IT::STR:
		return "str";
	case IT::CHAR:
		return "char";
	case IT::ANY:
		return "any";
	default:
		return "undefined";
	}
}

std::string get_id_in_source(const IT::ID_Table &ID_Table, const LT::LexTable &LEX_Table, int i)
{
	return ID_Table.table[LEX_Table.table[i].idxTI].id;
};

struct Expression
{
	IT::IDDATATYPE iddatatype;
	bool isFunctionCallArg = false;
	bool isCompare = false;
	bool isReturn = false;
	std::vector<int> ids;
	int firstLine;
	int dest_IT_index;
	bool isInitialization = false;
	std::string funcName = "";
};

std::vector<std::vector<int>> get_function_params(const IT::ID_Table &ID_Table, const LT::LexTable &LEX_Table, int &start);
void parse_functions(const IT::ID_Table &ID_Table, const LT::LexTable &LEX_Table);
void check_expression(const IT::ID_Table &ID_Table, const LT::LexTable &LEX_Table, Expression &expr);
void check_function_call(const IT::ID_Table &ID_Table, const LT::LexTable &LEX_Table, int funcID, std::vector<std::vector<int>> params);
void detect_cycles_in_references(const IT::ID_Table &ID_Table, const LT::LexTable &LEX_Table);

void printError(std::string msg)
{
	cout << msg << endl;

	if (::logObject != nullptr)
	{
		Log::writeLine((*::logObject), msg);
	}
}

int semantic::check(const IT::ID_Table &ID_Table, const LT::LexTable &LEX_Table, Log::LOG *log)
{
	::logObject = log;
	parse_functions(ID_Table, LEX_Table);
	detect_cycles_in_references(ID_Table, LEX_Table);

	auto handle_expression = [&](int &i, int dest_IT_index, bool isReturn, bool isCompare, IT::IDDATATYPE iddatatype)
	{
		Expression expression;
		expression.dest_IT_index = dest_IT_index;
		expression.firstLine = LEX_Table.table[i].sn;
		expression.isReturn = isReturn;
		expression.isCompare = isCompare;
		expression.iddatatype = iddatatype;

		int bracesCount = 0;

		while (LEX_Table.table[i].lexema[0] != LEX_SEMICOLON && LEX_Table.table[i].lexema[0] != LEX_LEFTBRACE)
		{
			if (LEX_Table.table[i].lexema[0] == LEX_ID)
			{
				expression.ids.push_back(i);
				if (ID_ENTRY_BY_LEX_ID(i).idtype == IT::F)
				{
					if (LEX_Table.table[i + 1].lexema[0] != LEX_LEFTTHESIS)
					{
						printError(format("Ошибка 710. Строка {}: Присваивание значения функции непосредственно переменной недопустимо",
										  LEX_Table.table[i].sn));
						errors++;
					}
					else
					{
						int x = i;
						i += 2;
						check_function_call(ID_Table, LEX_Table, x, get_function_params(ID_Table, LEX_Table, i));
					}
				}
			}
			else if (LEX_Table.table[i].lexema[0] == LEX_STRCMP)
			{
				expression.ids.push_back(i);
				i += 2;
				check_function_call(ID_Table, LEX_Table, -1, get_function_params(ID_Table, LEX_Table, i));
			}
			else
				expression.ids.push_back(i);

			if (LEX_Table.table[i].lexema[0] == LEX_LEFTTHESIS)
				bracesCount++;
			else if (LEX_Table.table[i].lexema[0] == LEX_RIGHTTHESIS)
				bracesCount--;

			i++;
		}

		if ((bracesCount != 0 && LEX_Table.table[expression.ids.back()].lexema[0] == LEX_RIGHTTHESIS) ||
			LEX_Table.table[expression.ids.back()].lexema[0] == LEX_LEFTBRACE)
		{
			expression.ids.pop_back();
		}

		check_expression(ID_Table, LEX_Table, expression);
	};

	for (int i = 0; i < LEX_Table.size; i++)
	{
		switch (LEX_Table.table[i].lexema[0])
		{
		case LEX_MAIN:
		case LEX_ID:
			if (LEX_Table.table[i].lexema[0] == LEX_MAIN)
			{
				currentFunction = &ID_Table.table[LEX_Table.table[i].idxTI];
			}
			else if (LEX_Table.table[i - 1].lexema[0] == LEX_TYPE && ID_Table.table[LEX_Table.table[i].idxTI].idtype == IT::F)
			{
				currentFunction = &ID_Table.table[LEX_Table.table[i].idxTI];
			}
			else if (LEX_Table.table[i + 1].lexema[0] == LEX_LEFTTHESIS && LEX_Table.table[i - 1].lexema[0] != LEX_TYPE)
			{
				int x = i;
				i += 2;
				check_function_call(ID_Table, LEX_Table, x, get_function_params(ID_Table, LEX_Table, i));
			}
			if (ID_Table.table[LEX_Table.table[i].idxTI].idtype == IT::V)
			{
				if (LEX_Table.table[i - 1].lexema[0] == LEX_TYPE && LEX_Table.table[i + 1].lexema[0] != LEX_EQUAL)
				{
					isInitialized[&ID_Table.table[LEX_Table.table[i].idxTI]] = false;
				}
			}
			else if (ID_Table.table[LEX_Table.table[i].idxTI].idtype == IT::P || ID_Table.table[LEX_Table.table[i].idxTI].idtype == IT::F)
			{
				isInitialized[&ID_Table.table[LEX_Table.table[i].idxTI]] = true;
			}
			break;

		case LEX_EQUAL:
		{
			int tmpIDindex = i++;
			handle_expression(i, LEX_Table.table[i - 1].idxTI, false, false, ID_ENTRY_BY_LEX_ID(i - 1).iddatatype);
			isInitialized[&ID_Table.table[LEX_Table.table[tmpIDindex].idxTI]] = true;
			break;
		}

		case LEX_IF:
		case LEX_PRINT:
			i += 2;
			handle_expression(i, -1, false, true, IT::ANY);
			break;
		case LEX_STRCMP:
			i += 2;
			check_function_call(ID_Table, LEX_Table, -1, get_function_params(ID_Table, LEX_Table, i));
			break;
		case LEX_WRITE:
			i += 1;
			handle_expression(i, -1, false, true, IT::ANY);
			break;
		case LEX_RETURN:
			i++;
			int z = -1;
			for (int i = 0; i < LEX_Table.size; i++)
			{
				if (LEX_Table.table[i].lexema[0] == LEX_ID && &ID_Table.table[LEX_Table.table[i].idxTI] == currentFunction)
				{
					z = i;
				}
			}
			handle_expression(i, z, true, false, currentFunction->iddatatype);
			break;
		}
	}

	return errors;
}

void parse_functions(const IT::ID_Table &ID_Table, const LT::LexTable &LEX_Table)
{
	auto extract_parameters = [&](int &index) -> std::vector<IT::IDDATATYPE>
	{
		std::vector<IT::IDDATATYPE> parameters;
		int counter = 1;
		bool exit = false;

		while (!exit)
		{
			switch (LEX_Table.table[index].lexema[0])
			{
			case LEX_RIGHTTHESIS:
				if (--counter == 0)
				{
					exit = true;
				}
				break;
			case LEX_ID:
			case LEX_LITERAL:
				parameters.push_back(ID_ENTRY_BY_LEX_ID(index).iddatatype);
				break;
			}
			index++;
		}
		return parameters;
	};

	auto extract_references = [&](int &index) -> std::vector<std::pair<IT::Entry *, int>>
	{
		std::vector<std::pair<IT::Entry *, int>> refs;
		int braces = 1;

		while (LEX_Table.table[index].lexema[0] != LEX_LEFTBRACE)
		{
			index++;
		}
		index++;

		while (braces > 0)
		{
			switch (LEX_Table.table[index].lexema[0])
			{
			case LEX_ID:
				if (ID_Table.table[LEX_Table.table[index].idxTI].idtype == IT::F)
				{
					refs.push_back({&ID_Table.table[LEX_Table.table[index].idxTI], LEX_Table.table[index].sn});
				}
				break;

			case LEX_LEFTBRACE:
				braces++;
				break;

			case LEX_BRACELET:
				braces--;
				break;
			}
			index++;
		}
		return refs;
	};

	int start = 0;

	for (int i = 0; i < LEX_Table.size; i++)
	{
		if (LEX_Table.table[i].lexema[0] == LEX_ID && LEX_Table.table[i - 1].lexema[0] == LEX_TYPE && ID_ENTRY_BY_LEX_ID(i).idtype == IT::F)
		{
			int tmpI = i;
			i += 2;
			protos[&ID_Table.table[LEX_Table.table[tmpI].idxTI]] = extract_parameters(i);
		}
	}

	for (int i = 0; i < LEX_Table.size; i++)
	{
		if (LEX_Table.table[i].lexema[0] == LEX_ID && LEX_Table.table[i - 1].lexema[0] == LEX_TYPE && ID_ENTRY_BY_LEX_ID(i).idtype == IT::F)
		{

			int tmpI = i;
			i += 2;
			references[&ID_Table.table[LEX_Table.table[tmpI].idxTI]] = extract_references(i);
		}
	}
}

std::vector<std::vector<int>> get_function_params(const IT::ID_Table &ID_Table, const LT::LexTable &LEX_Table, int &start)
{
	std::vector<std::vector<int>> params;
	std::vector<int> tmp;

	int counter = 1;
	bool exit = false;

	while (!exit)
	{
		switch (LEX_Table.table[start].lexema[0])
		{
		case LEX_ID:
			tmp.push_back(start);
			if (ID_ENTRY_BY_LEX_ID(start).idtype == IT::F)
			{
				int tmpStart = start;
				start += 2;
				check_function_call(ID_Table, LEX_Table, tmpStart, get_function_params(ID_Table, LEX_Table, start));
			}
			break;

		case LEX_SEMICOLON:
			exit = true;
			break;

		case LEX_COMMA:
			params.push_back(tmp);
			tmp.clear();
			break;

		case LEX_LEFTTHESIS:
			counter++;
			break;

		case LEX_RIGHTTHESIS:
			if (--counter == 0)
			{
				exit = true;
				break;
			}
		default:
			tmp.push_back(start);
			break;
		}
		start++;
	}
	start--;
	if (tmp.size())
	{
		params.push_back(tmp);
	}
	return params;
}

void check_expression(const IT::ID_Table &ID_Table, const LT::LexTable &LEX_Table, Expression &expr)
{
	if (expr.ids.empty())
		return;
	bool insideFunction = false;
	int countBraces = 0;
	bool metCompare = false;
	bool metMath = false;
	if (expr.ids.size() == 1)
	{
		const auto &id_entry = ID_ENTRY_BY_LEX_ID(expr.ids[0]);
		const int line_number = LEX_Table.table[expr.ids[0]].sn;
		const std::string id_in_source = get_id_in_source(ID_Table, LEX_Table, expr.ids[0]);

		if (expr.iddatatype != id_entry.iddatatype && expr.iddatatype != IT::ANY)
		{
			if (expr.isFunctionCallArg)
			{
				printError(format(
					"Ошибка 709. Строка {}: аргумент функции {} имеет тип {}, но ожидается {}",
					line_number,
					expr.funcName,
					iddatatype_to_str(id_entry.iddatatype),
					iddatatype_to_str(expr.iddatatype)));
			}
			else
			{
				printError(format(
					"Ошибка 708. Строка {}: тип {} не совместим с типом {}",
					line_number,
					id_entry.idtype == IT::L ? std::string("литерала") : "идентификатора",
					iddatatype_to_str(expr.iddatatype)));
			}
			errors++;
		}
		return;
	}

	unordered_map<IT::IDDATATYPE, int> types = {
		{IT::CHAR, 0},
		{IT::INT, 0},
		{IT::STR, 0}};

	for (int id = expr.ids.front(); id <= expr.ids.back(); id++)
	{
		const auto &lex_entry = LEX_Table.table[id];
		switch (lex_entry.lexema[0])
		{
		case LEX_LITERAL:
		case LEX_ID:
			if (!insideFunction)
			{
				types[ID_ENTRY_BY_LEX_ID(id).iddatatype]++;
			}
			if (ID_ENTRY_BY_LEX_ID(id).idtype == IT::F)
			{
				insideFunction = true;
			}
			if (LEX_Table.table[id].lexema[0] != LEX_LITERAL && (isInitialized.find(&ID_Table.table[LEX_Table.table[id].idxTI]) == isInitialized.end() || (isInitialized.find(&ID_Table.table[LEX_Table.table[id].idxTI]) != isInitialized.end() &&
																																						   isInitialized[&ID_Table.table[LEX_Table.table[id].idxTI]] == false)))
			{
				printError(format("Ошибка 700. Строка {}: переменная {} не может быть использована в выражении, так как она неинициализирована.",
								  lex_entry.sn, ID_Table.table[LEX_Table.table[id].idxTI].id));
				errors++;
			}
			break;

		case LEX_MATH:
			if (types[IT::STR] != 0 || types[IT::CHAR] != 0 || (id != expr.ids.back() && ID_ENTRY_BY_LEX_ID(id + 1).iddatatype != IT::INT))
			{
				printError(format("Ошибка 707. Строка {}: выражение содержит тип(ы), которые не поддерживают операцию {}",
								  lex_entry.sn, lex_entry.v));
				errors++;
			}
			break;

		case LEX_COMPARE:
			if (!expr.isCompare)
			{
				printError(format("Ошибка 706. Строка {}: результат сравнения используется вне контекста сравнения.",
								  lex_entry.sn));
				errors++;
			}
			metCompare = true;
			break;

		case LEX_PRINT:
			printError(format("Ошибка 705. Строка {}: функция print не может участвовать в выражениях.",
							  lex_entry.sn));
			errors++;
			break;

		case LEX_WRITE:
			printError(format("Ошибка 704. Строка {}: оператор write не может участвовать в выражениях.",
							  lex_entry.sn));
			errors++;
			break;

		case LEX_LEFTTHESIS:
			if (insideFunction)
			{
				countBraces++;
			}
			break;

		case LEX_RIGHTTHESIS:
			if (insideFunction && --countBraces == 0)
			{
				insideFunction = false;
			}
			break;

		case LEX_STRCMP:
			types[IT::IDDATATYPE::INT]++;
			insideFunction = true;
			break;

		default:
			break;
		}
	}

	int non_zero_count = 0;
	int charCount = 0;
	for (const auto &[type, count] : types)
	{
		if (type == IT::CHAR && count && metCompare)
		{
			printError(format("Ошибка 703. Строка {}: операции сравнения не могут быть применены данным типа char",
							  LEX_Table.table[expr.ids[0]].sn));
			errors++;
		}
		if (count > 0)
		{
			if (++non_zero_count > 1)
			{
				if (expr.isCompare && metCompare)
				{
					printError(format("ошибка 702. Строка {}: операции сравнения не могут быть применены к данным разных типов.",
									  LEX_Table.table[expr.ids[0]].sn));
					errors++;
				}
				if (metMath)
				{
					printError(format("Ошибка 711. Строка {}: арифметические операции не могут быть применены к данным разных типов или сравнениям.",
									  LEX_Table.table[expr.ids[0]].sn));
					errors++;
				}
				break;
			}
		}
	}
}

void check_function_call(const IT::ID_Table &ID_Table, const LT::LexTable &LEX_Table, int funcID, std::vector<std::vector<int>> params)
{
	IT::Entry *entry;
	int paramsCount = 0;

	if (funcID >= 0)
	{
		entry = &ID_Table.table[LEX_Table.table[funcID].idxTI];
		paramsCount = protos[entry].size();
	}
	else
	{
		entry = new IT::Entry();
		strcpy(entry->id, "strcmp");
		entry->iddatatype = IT::INT;
		paramsCount = 2;
	}

	if (params.size() != paramsCount)
	{
		printError(format("Ошибка 712. Строка {}, число передаваемых аргументов в функцию {} не соответствует ее прототипу\n",
						  LEX_Table.table[funcID].sn, entry->id));
		errors++;
	}

	int size = params.size() > paramsCount ? paramsCount : params.size();

	for (size_t i = 0; i < size; i++)
	{
		Expression expression;

		if (funcID >= 0)
		{
			expression.iddatatype = protos[entry][i];
		}
		else
		{
			expression.iddatatype = IT::STR;
		}

		expression.dest_IT_index = funcID;
		expression.isFunctionCallArg = true;
		expression.ids = params[i];
		expression.funcName = entry->id;
		check_expression(ID_Table, LEX_Table, expression);
	}

	if (funcID < 0)
	{
		delete entry;
	}
}

void detect_cycles_in_references(const IT::ID_Table &ID_Table, const LT::LexTable &LEX_Table)
{
	std::set<IT::Entry *> visited;
	std::set<IT::Entry *> recursion_stack;

	auto has_cycle = [&](IT::Entry *node, auto &has_cycle_ref) -> bool
	{
		recursion_stack.insert(node);
		if (references.count(node))
		{
			for (const auto &callee : references.at(node))
			{
				if (recursion_stack.count(callee.first) || !visited.count(callee.first) && has_cycle_ref(callee.first, has_cycle_ref))
				{
					printError(format(
						"Ошибка 713. Строка {}: Функция {} ссылается на {} и образует цикл.",
						callee.second,
						node->id,
						callee.first->id));
					errors++;
					return true;
				}
			}
		}
		recursion_stack.erase(node);
		visited.insert(node);
		return false;
	};

	for (const auto &[func, calls] : references)
	{
		if (!visited.count(func))
		{
			has_cycle(func, has_cycle);
		}
	}
}
