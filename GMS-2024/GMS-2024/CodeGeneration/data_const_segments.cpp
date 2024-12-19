#include "stdafx.h"
#include "code_gen.h"

using namespace std;

void CD::CodeGeneration::__s_const()
{
	OUT_ASM_FILE << ".const\n";
	for (int i = 0; i < ID_TABLE.size; i++)
	{
		if (ID_TABLE.table[i].idtype == IT::L)
		{
			if (ID_TABLE.table[i].iddatatype == IT::IDDATATYPE::INT)
			{
				OUT_ASM_FILE << tab << get_id_name_in_data_segment(ID_TABLE.table[i]) << " sword " << ID_TABLE.table[i].value.vint << '\n';
			}
			if (ID_TABLE.table[i].iddatatype == IT::IDDATATYPE::STR)
			{
				OUT_ASM_FILE << tab << get_id_name_in_data_segment(ID_TABLE.table[i]) << " db \"" << ID_TABLE.table[i].value.vstr->str << "\", 0\n";
			}
			if (ID_TABLE.table[i].iddatatype == IT::IDDATATYPE::CHAR)
			{
				OUT_ASM_FILE << tab << get_id_name_in_data_segment(ID_TABLE.table[i]) << " db " << getEscapeCode(ID_TABLE.table[i].value.vstr->str) << "\n";
			}
		}
	}
}

void CD::CodeGeneration::__s_data()
{
	OUT_ASM_FILE << ".data\n" << tab << reservedBoolName << ' ' << "byte" << " ? \n";

	for (int i = 0; i < ID_TABLE.size; i++)
	{
		auto entry = &ID_TABLE.table[i];
		if (entry->idtype == IT::IDTYPE::V || entry->idtype == IT::IDTYPE::P)
		{
			if (entry->iddatatype == IT::INT)
			{
				OUT_ASM_FILE << tab << get_id_name_in_data_segment(*entry)
					<< " sword "
					<< "0\n";
			}
			else if (entry->iddatatype == IT::CHAR)
			{
				OUT_ASM_FILE << tab << get_id_name_in_data_segment(*entry)
					<< " byte ?\n";
			}
			else if (entry->iddatatype == IT::STR && entry->idtype == IT::IDTYPE::P)
			{
				OUT_ASM_FILE << tab << get_id_name_in_data_segment(*entry)
					<< " dword ?\n";
			}
			else if (entry->iddatatype == IT::STR)
			{
				OUT_ASM_FILE << tab << get_id_name_in_data_segment(*entry);
				OUT_ASM_FILE << " dword ?\n";
			}
		}
		else if (entry->idtype == IT::IDTYPE::F || entry->idtype == IT::IDTYPE::L)
		{
			continue;
		}
		else
		{
			throw "__s_data: неизвестный тип переменной";
		}
	}
}

std::string CD::CodeGeneration::get_id_name_in_data_segment(const IT::Entry& entry)
{
	static unordered_map <int, std::string> var;

	if (var.find((unsigned int)entry.id) != var.end())
	{
		return var[(unsigned int)entry.id];
	}
	bool isLocal = false;
	ostringstream ss;

	if (entry.idtype == IT::IDTYPE::L)
	{
		if (entry.iddatatype == IT::IDDATATYPE::INT)
		{
			ss << "__INT_" << entry.id;
		}
		else if (entry.iddatatype == IT::IDDATATYPE::STR)
		{
			ss << "__STR_" << entry.id;
		}
		else if (entry.iddatatype == IT::IDDATATYPE::CHAR)
		{
			ss << "__CHAR_" << entry.id;
		}
	}
	else if (entry.idtype == IT::IDTYPE::V)
	{
		if (entry.iddatatype == IT::IDDATATYPE::INT)
		{
			ss << "_INT_";
		}
		else if (entry.iddatatype == IT::IDDATATYPE::STR)
		{
			ss << "_STR_";
		}
		else if (entry.iddatatype == IT::IDDATATYPE::CHAR)
		{
			ss << "_CHAR_";
		}
		isLocal = true;
	}
	else if (entry.iddatatype == IT::IDDATATYPE::INT && entry.idtype == IT::IDTYPE::P)
	{
		ss << "_INT_PARAM_";

		isLocal = true;
	}
	else if (entry.iddatatype == IT::IDDATATYPE::STR && entry.idtype == IT::IDTYPE::P)
	{
		ss << "_STR_PARAM_";
		isLocal = true;
	}
	else if (entry.iddatatype == IT::IDDATATYPE::CHAR && entry.idtype == IT::IDTYPE::P)
	{
		ss << "_CHAR_PARAM_";
		isLocal = true;
	}
	else if (entry.idtype == IT::IDTYPE::F)
	{
		ss << entry.id;
	}
	else
	{
		throw "Неизвестный ID при генерации имени в сегменте .data";
	}
	if (isLocal)
	{
		ss << '@';
		if (entry.scope != NULL)
		{
			std::stack<IT::Entry*> scope;
			bool isFirst = true;

			auto tmp = entry.scope;
			scope.push(tmp);

			while ((*tmp).scope != nullptr)
			{
				scope.push(tmp->scope);
				tmp = (*tmp).scope;
			}

			while (!scope.empty())
			{
				if (!isFirst)
				{
					ss << "@";
				}
				else
				{
					isFirst = false;
				}
				ss << scope.top()->id;
				scope.pop();
			}
			//ss << entry.scope->id;
		}

		ss << "__" << entry.id;
	}

	std::string result = ss.str();
	var[(int)entry.id] = result;

	return result;
}