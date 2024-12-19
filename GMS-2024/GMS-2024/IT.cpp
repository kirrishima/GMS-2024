#include "stdafx.h"
#include "IT.h"
#include "error.h"


namespace IT
{
	ID_Table Create(int size)
	{
		ID_Table* tabl = new ID_Table;
		if (size > TI_MAXSIZE)
		{
			throw ERROR_THROW(116);
		}
		tabl->maxsize = size;
		tabl->size = 0;
		tabl->table = new Entry[size];
		return *tabl;
	}

	void Add(ID_Table& idtable, Entry entry)
	{
		if (idtable.size + 1 > idtable.maxsize)
		{
			throw ERROR_THROW(117);
		}
		idtable.table[idtable.size] = entry;
		idtable.size += 1;
	}

	Entry GetEntry(const ID_Table& idtable, int n)
	{
		return idtable.table[n];
	}

	int search(const ID_Table& idtable, IT::Entry& entry)
	{
		for (int i = 0; i <= idtable.size; i++)
		{
			if (entry.idtype == IT::IDTYPE::L)
			{
				if (idtable.table[i].idtype == IT::IDTYPE::L) {
					if (entry.iddatatype == IT::IDDATATYPE::INT && idtable.table[i].iddatatype == IT::IDDATATYPE::INT
						&& idtable.table[i].value.vint == entry.value.vint)
					{
						return i;
					}
					if (entry.iddatatype == IT::IDDATATYPE::STR && idtable.table[i].iddatatype == IT::IDDATATYPE::STR
						&& strcmp(idtable.table[i].value.vstr->str, entry.value.vstr->str) == 0)
					{
						return i;
					}
				}
				continue;
			}
			if (strcmp(entry.id, idtable.table[i].id) == 0)
			{
				auto tmp_e = entry.scope;

				while (tmp_e != NULL)
				{
					if (tmp_e == idtable.table[i].scope)
					{
						return i;
					}
					tmp_e = tmp_e->scope;
				}
			}
			if (strcmp(entry.id, idtable.table[i].id) == 0 && idtable.table[i].idtype == IT::F)
			{
				return i;
			}
		}
		return -1;
	}


	int IsId(ID_Table& idtable, char id[ID_MAXSIZE])
	{
		for (int i = 0; i < idtable.size; i++)
			if (strcmp(idtable.table[i].id, id) == 0)
				return i;

		return TI_NULLIDX;
	}

	void Delete(ID_Table& idtable)
	{
		delete[] idtable.table;
		idtable.table = nullptr;
	}
}