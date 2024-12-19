#include "stdafx.h"
#include "LT.h"
#include <iostream>

namespace LT
{
	LexTable Create(int size)
	{
		if (size > LT_MAXSIZE)
		{
			throw ERROR_THROW(114);
		}

		LexTable lextable;
		lextable.maxsize = size;
		lextable.size = 0;
		lextable.table = new Entry[size];

		return lextable;
	}

	void Add(LexTable &lextable, Entry entry)
	{
		if (lextable.size > LT_MAXSIZE)
		{
			throw ERROR_THROW(115);
		}
		lextable.table[lextable.size++] = entry;
	}

	Entry GetEntry(const LexTable &lextable, int n)
	{
		if (n > LT_MAXSIZE || n < 0)
		{
			throw ERROR_THROW(13);
		}
		return lextable.table[n];
	}

	bool hasMainLexem(const LexTable &table)
	{
		for (int i = 0; i < table.size; i++)
		{
			if (table.table[i].lexema[0] == LEX_MAIN)
			{
				return true;
			}
		}
		return false;
	}

	void Delete(LexTable &lextable)
	{
		delete[] lextable.table;
		lextable.table = nullptr;
		lextable.size = 0;
		lextable.maxsize = 0;
	}
}