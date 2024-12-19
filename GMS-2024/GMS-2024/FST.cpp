#include "stdafx.h"
#include <cstring>
#include "FST.h"
#include <regex>

namespace FST
{
	NODE::NODE(short numRelations, RELATION firstRelation, ...)
	{
		n_relation = numRelations;
		RELATION* relationsArray = &firstRelation;
		relations = new RELATION[numRelations];

		for (short i = 0; i < numRelations; i++) {
			relations[i] = relationsArray[i];
		}
	}

	FST::FST(const char* inputString, short numStates, NODE firstNode, ...)
	{
		string = inputString;
		nstates = numStates;
		nodes = new NODE[numStates];
		NODE* nodesArray = &firstNode;

		for (int i = 0; i < numStates; i++) {
			nodes[i] = nodesArray[i];
		}

		rstates = new short[nstates];

		std::memset(rstates, 0xFF, sizeof(short) * nstates);

		rstates[0] = 0;
	}


	bool execute(FST& fst)
	{
		short* tmpStates = new short[fst.nstates];
		std::memcpy(tmpStates, fst.rstates, fst.nstates * sizeof(short));

		short* rstates = new short[fst.nstates];
		std::memset(rstates, -1, fst.nstates * sizeof(short));

		short lstring = std::strlen(fst.string);

		bool rc = true;

		for (fst.position = 0; fst.position < lstring && rc; fst.position++)
		{
			rc = false;
			std::memcpy(rstates, fst.rstates, fst.nstates * sizeof(short));
			std::memset(fst.rstates, -1, fst.nstates * sizeof(short));

			for (int i = 0; i < fst.nstates; i++)
			{
				if (rstates[i] == fst.position)
				{
					for (int j = 0; j < fst.nodes[i].n_relation; j++)
					{
						if (fst.nodes[i].relations[j].symbol == fst.string[fst.position])
						{
							fst.rstates[fst.nodes[i].relations[j].nnode] = fst.position + 1;
							rc = true;
						}
					}
				}
			}
		}

		delete[] rstates;
		auto result = rc ? fst.rstates[fst.nstates - 1] == lstring : rc;

		std::memcpy(fst.rstates, tmpStates, fst.nstates * sizeof(short));
		return result;
	}

	bool execute(const char* regex, const char* str)
	{
		std::regex pattern(regex);
		return std::regex_match(str, pattern);
	}
}
