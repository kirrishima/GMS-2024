#include "stdafx.h"
#include "code_gen.h"
#include <vector>

using namespace std;

std::string operator*(const std::string& str, int times) {
	std::string new_str;
	for (int i = 0; i < times; i++)
	{
		new_str += str;
	}
	return new_str;
}

std::string removeComment(const std::string& str) {
	size_t commentPos = str.find(';');
	return (commentPos == std::string::npos) ? str : str.substr(0, commentPos);
}

bool isMASMLabel(const std::string& line) {
	std::string cleanLine = utils::trim(removeComment(line));

	if (cleanLine.empty() || cleanLine.back() != ':') {
		return false;
	}

	std::string identifier = cleanLine.substr(0, cleanLine.size() - 1);

	if (identifier.empty()) {
		return false;
	}

	if (!std::isalpha(identifier[0]) && identifier[0] != '_') {
		return false;
	}

	for (size_t i = 1; i < identifier.size(); ++i) {
		if (!std::isalnum(identifier[i]) && identifier[i] != '_') {
			return false;
		}
	}

	return true;
}

bool isValidIdentifier(const std::string& identifier) {
	if (identifier.empty()) {
		return false;
	}

	if (!std::isalpha(identifier[0]) && identifier[0] != '_') {
		return false;
	}

	for (size_t i = 1; i < identifier.size(); ++i) {
		if (!std::isalnum(identifier[i]) && identifier[i] != '_') {
			return false;
		}
	}

	return true;
}

bool isFunctionStart(const std::string& line) {
	std::string cleanLine = utils::trim(removeComment(line));

	if (cleanLine.size() < 4 || cleanLine.substr(cleanLine.size() - 4) != "proc") {
		return false;
	}

	std::string identifier = utils::trim(cleanLine.substr(0, cleanLine.size() - 4));

	return isValidIdentifier(identifier);
}

bool isFunctionEnd(const std::string& line) {
	std::string cleanLine = utils::trim(removeComment(line));

	if (cleanLine.size() < 4 || cleanLine.substr(cleanLine.size() - 4) != "endp") {
		return false;
	}

	std::string identifier = utils::trim(cleanLine.substr(0, cleanLine.size() - 4));

	return isValidIdentifier(identifier);
}

std::string CD::CodeGeneration::lexems_vector_to_string(const vector<int>& ids, bool comment)
{
	string result = "";
	for (size_t i = 0; i < ids.size(); i++)
	{
		int lexId = ids[i];

		switch (LEX_TABLE.table[lexId].lexema[0])
		{
		case LEX_LITERAL:
			if (ID_TABLE.table[LEX_TABLE.table[lexId].idxTI].iddatatype == IT::INT)
			{
				result += to_string(ID_TABLE.table[LEX_TABLE.table[lexId].idxTI].value.vint);
			}
			else if (ID_TABLE.table[LEX_TABLE.table[lexId].idxTI].iddatatype == IT::STR)
			{
				result += format("\"{}\"", ID_TABLE.table[LEX_TABLE.table[lexId].idxTI].value.vstr->str);
			}
			else if (ID_TABLE.table[LEX_TABLE.table[lexId].idxTI].iddatatype == IT::CHAR)
			{
				result += format("'{}'", ID_TABLE.table[LEX_TABLE.table[lexId].idxTI].value.vstr->str);
			}
			break;
		case LEX_ID:
			if (comment)
			{
				result += ID_TABLE.table[LEX_TABLE.table[lexId].idxTI].id;
			}
			else
			{
				result += get_id_name_in_data_segment(ID_TABLE.table[LEX_TABLE.table[lexId].idxTI]);
			}
			break;
		case LEX_MATH:
			result += LEX_TABLE.table[lexId].v;
			break;
		case LEX_COMPARE:
			result += LEX_TABLE.table[lexId].c;
			break;
		default:
			result += LEX_TABLE.table[lexId].lexema[0];
			break;
		}
	}
	return result;
}

std::string CD::CodeGeneration::lexems_vector_to_source_string(const vector<int>& ids)
{
	string result = "";
	for (const int id : ids)
	{
		result += lexem_to_source(LEX_TABLE.table[id]);
	}
	return result;
}

int get_id_size_in_bytes(IT::IDDATATYPE type)
{
	switch (type)
	{
	case IT::INT:
		return 2;
	case IT::STR:
		return 4;
	case IT::CHAR:
		return 4;
	default:
		throw "get_id_size_in_bytes: неверный тип данных";
	}
}

std::string CD::CodeGeneration::lexem_to_source(LT::Entry& entry)
{
	switch (entry.lexema[0]) {
	case LEX_PRINT:
		return "print";

	case LEX_LITERAL:
		if (ID_TABLE.table[entry.idxTI].iddatatype == IT::IDDATATYPE::INT)
		{
			return to_string(ID_TABLE.table[entry.idxTI].value.vint);
		}
		if (ID_TABLE.table[entry.idxTI].iddatatype == IT::IDDATATYPE::STR)
		{
			return ID_TABLE.table[entry.idxTI].value.vstr->str;
		}
		throw "Литерал неизвестного типа данных";

	case LEX_ID:
		return ID_TABLE.table[entry.idxTI].id;

	case LEX_COMPARE:
		return entry.c;

	case LEX_MATH:
	{
		std::string result = " ";
		result[0] = entry.v;
		return result;
	}

	case LEX_INTEGER:
		if (ID_TABLE.table[entry.idxTI].iddatatype == IT::IDDATATYPE::INT)
		{
			return "int";
		}
		throw "Идентификатор неизвестного типа данных";

	case LEX_WRITE:
		return "write";

	default: {
		std::string result = " ";
		result[0] = entry.lexema[0];
		return result;
	}
	}
}

std::string CD::CodeGeneration::get_string_value(const int lex_id)
{
	int idxIT = IT::search(ID_TABLE, ID_TABLE.table[LEX_TABLE.table[lex_id].idxTI]);
	IT::Entry* id = &ID_TABLE.table[idxIT];

	if (id->iddatatype == IT::IDDATATYPE::STR)
	{
		if (id->idtype == IT::L)
		{
			return string("offset ") + get_id_name_in_data_segment(ID_TABLE.table[idxIT]);
		}
		return get_id_name_in_data_segment(ID_TABLE.table[idxIT]);
	}
	else
	{
		throw "Неожиданный тип при определении значения строковой лексемы <"
			+ to_string(lex_id) + ">: " + to_string(id->iddatatype);
	}
}



bool CD::is_assignment(const std::string& expr) {
	constexpr char operators[] = "+-*/";
	for (char op : operators) {
		if (expr.find(op) != std::string::npos) {
			return false;
		}
	}
	return true;
}

int getEscapeCode(const std::string& input) {
	if (input.size() < 2 || input[0] != '\\') {
		return (int)input[0];
	}

	static const std::unordered_map<char, int> escapeCodes = {
		{'n', (int)'\n'},
		{'t', (int)'\t'},
		{'r', (int)'\r'}
	};

	auto it = escapeCodes.find(input[1]);
	if (it != escapeCodes.end()) {
		return it->second;
	}

	return -1;
}
