#include "stdafx.h"
#include "GRB.h"
#include "Rule.h"
#include <cstring>

namespace GRB
{
	Rule::Chain::Chain(short psize, GRBALPHABET s, ...)
	{
		nt = new GRBALPHABET[size = psize];
		int* ptr = (int*)&s;

		for (int i = 0; i < psize; i++)
			nt[i] = (short)ptr[i];
	}

	Rule::Rule(GRBALPHABET pnn, int iderroe, short psize, Chain c, ...)
	{
		nn = pnn;
		iderror = iderroe;
		chains = new Chain[size = psize];
		Chain* ptr = &c;

		for (int i = 0; i < psize; i++)
			chains[i] = ptr[i];
	}

	Greibach::Greibach(GRBALPHABET pstartN, GRBALPHABET pstbottomT, short psize, Rule r, ...)
	{
		startN = pstartN;
		stbottomT = pstbottomT;
		rules = new Rule[size = psize];
		Rule* ptr = &r;

		for (int i = 0; i < psize; i++)
			rules[i] = ptr[i];
	}

#pragma region  Грамматика (Грейбах): функции и методы 

	Greibach getGreibach()
	{
		return greibach;
	}

	short Greibach::getRule(GRBALPHABET pnn, Rule& prule)
	{
		short rc = -1, k = 0;

		while (k < size && rules[k].nn != pnn) k++;

		if (k < size)
			prule = rules[rc = k];

		return rc;
	}

	Rule Greibach::getRule(short n)
	{
		Rule rc;

		if (n >= 0 && n < size)
			rc = rules[n];

		return rc;
	}

	char* Rule::getCRule(char* b, short nchain)
	{
		char buf[200];
		b[0] = Chain::alphabet_to_char(nn);
		b[1] = '-';
		b[2] = '>';
		b[3] = 0x00;
		chains[nchain].getCChain(buf);
		strcat_s(b, sizeof(buf) + 5, buf);

		return b;
	}
#pragma endregion

#pragma region Подбор подходящего правила: 

	short Rule::getNextChain(GRBALPHABET t, Rule::Chain& pchain, short j)
	{
		short rc = -1;

		while (j < size && chains[j].nt[0] != t) j++;

		rc = (j < size ? j : -1);

		if (rc >= 0)
			pchain = chains[rc];

		return rc;
	}

	char* Rule::Chain::getCChain(char* b)
	{
		for (int i = 0; i < size; i++)
			b[i] = Chain::alphabet_to_char(nt[i]);

		b[size] = 0;
		return b;
	}
#pragma endregion
}