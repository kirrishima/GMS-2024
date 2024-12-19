#pragma once

#include <iostream>

#define ID_MAXSIZE 26
#define TI_MAXSIZE 4096
#define TI_INT_DEFAULT 0x00000000
#define TI_STR_DEFAULT 0x00
#define TI_NULLIDX 0xffffffff
#define TI_STR_MAXSIZE 255

namespace IT
{
	enum IDDATATYPE { INT = 1, STR, CHAR, ANY };
	enum IDTYPE { V = 1024, F, P, L };

	struct Entry
	{
		int			idxfirstLE;
		char		id[ID_MAXSIZE];
		IDDATATYPE	iddatatype;
		IDTYPE		idtype;
		Entry* scope;

		union
		{
			int	vint;
			struct
			{
				int len;
				char str[TI_STR_MAXSIZE - 1];
			}	vstr[TI_STR_MAXSIZE];
		} value;
	};

	struct ID_Table
	{
		int maxsize;
		int size;
		Entry* table;
	};

	ID_Table Create(
		int size
	);

	void Add(
		ID_Table& idtable,
		Entry entry
	);

	Entry GetEntry(
		const ID_Table& idtable,
		int n
	);

	int IsId(
		ID_Table& idtable,
		char id[ID_MAXSIZE]
	);

	int search(const ID_Table& idtable, Entry& entry);

	void Delete(ID_Table& idtable);
}