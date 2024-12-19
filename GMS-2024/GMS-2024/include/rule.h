#pragma once
#include "error.h"
#include "GRB.h"

#define GRB_ERROR_SERIES 600
#define NS(n) GRB::Rule::Chain::N(n)
#define TS(n) GRB::Rule::Chain::T(n)
#define ISNS(n)	GRB::Rule::Chain::isN(n)

namespace GRB
{
	Greibach greibach(NS('S'), TS('$'),
		10,
		Rule(NS('S'), GRB_ERROR_SERIES + 0,
			6,
			Rule::Chain(12, TS('t'), TS('i'), TS('('), NS('F'), TS(')'), TS('{'), NS('N'), TS('r'), NS('E'), TS(';'), TS('}'), NS('S')),
			Rule::Chain(11, TS('t'), TS('i'), TS('('), NS('F'), TS(')'), TS('{'), TS('r'), NS('E'), TS(';'), TS('}'), NS('S')),
			Rule::Chain(11, TS('t'), TS('i'), TS('('), TS(')'), TS('{'), NS('N'), TS('r'), NS('E'), TS(';'), TS('}'), NS('S')),
			Rule::Chain(10, TS('t'), TS('i'), TS('('), TS(')'), TS('{'), TS('r'), NS('E'), TS(';'), TS('}'), NS('S')),
			Rule::Chain(7, TS('m'), TS('{'), NS('N'), TS('r'), NS('E'), TS(';'), TS('}')),
			Rule::Chain(4, TS('m'), TS('{'), NS('N'), TS('}'))
		),
		Rule(NS('N'), GRB_ERROR_SERIES + 1,
			22,
			Rule::Chain(6, TS('t'), TS('i'), TS('='), NS('E'), TS(';'), NS('N')),
			Rule::Chain(4, TS('t'), TS('i'), TS(';'), NS('N')),
			Rule::Chain(5, TS('i'), TS('='), NS('E'), TS(';'), NS('N')),
			Rule::Chain(4, TS('r'), NS('E'), TS(';'), NS('N')),
			Rule::Chain(6, TS('p'), TS('('), NS('E'), TS(')'), TS(';'), NS('N')),
			Rule::Chain(8, TS('p'), TS('('), NS('E'), TS('c'), NS('E'), TS(')'), TS(';'), NS('N')),
			Rule::Chain(4, TS('w'), NS('E'), TS(';'), NS('N')),
			Rule::Chain(6, TS('w'), NS('E'), TS('c'), NS('E'), TS(';'), NS('N')),
			Rule::Chain(3, TS('?'), NS('Y'), NS('N')),
			Rule::Chain(6, TS('i'), TS('('), NS('W'), TS(')'), TS(';'), NS('N')),
			Rule::Chain(5, TS('i'), TS('('), TS(')'), TS(';'), NS('N')),
			Rule::Chain(5, TS('t'), TS('i'), TS('='), NS('E'), TS(';')),
			Rule::Chain(3, TS('t'), TS('i'), TS(';')),
			Rule::Chain(4, TS('i'), TS('='), NS('E'), TS(';')),
			Rule::Chain(3, TS('r'), NS('E'), TS(';')),
			Rule::Chain(5, TS('p'), TS('('), NS('E'), TS(')'), TS(';')),
			Rule::Chain(7, TS('p'), TS('('), NS('E'), TS('c'), NS('E'), TS(')'), TS(';')),
			Rule::Chain(3, TS('w'), NS('E'), TS(';')),
			Rule::Chain(5, TS('w'), NS('E'), TS('c'), NS('E'), TS(';')),
			Rule::Chain(2, TS('?'), NS('Y')),
			Rule::Chain(5, TS('i'), TS('('), NS('W'), TS(')'), TS(';')),
			Rule::Chain(4, TS('i'), TS('('), TS(')'), TS(';'))
		),

		Rule(NS('E'), GRB_ERROR_SERIES + 2,
			10,
			Rule::Chain(1, TS('l')),
			Rule::Chain(1, TS('i')),
			Rule::Chain(2, TS('i'), NS('M')),
			Rule::Chain(2, TS('l'), NS('M')),
			Rule::Chain(5, TS('i'), TS('('), NS('W'), TS(')'), NS('M')),
			Rule::Chain(4, TS('i'), TS('('), NS('W'), TS(')')),
			Rule::Chain(4, TS('i'), TS('('), TS(')'), NS('M')),
			Rule::Chain(3, TS('i'), TS('('), TS(')')),
			Rule::Chain(4, TS('('), NS('E'), TS(')'), NS('M')),
			Rule::Chain(3, TS('('), NS('E'), TS(')'))
		),
		Rule(NS('A'), GRB_ERROR_SERIES + 9,
			4,
			Rule::Chain(1, TS('l')),
			Rule::Chain(1, TS('i')),
			Rule::Chain(4, TS('i'), TS('('), NS('W'), TS(')')),
			Rule::Chain(3, TS('i'), TS('('), TS(')'))
		),

		Rule(NS('B'), GRB_ERROR_SERIES + 10,
			1,
			Rule::Chain(6, TS('s'), TS('('), NS('A'), TS(','), NS('A'), TS(')'))
		),

		Rule(NS('Y'), GRB_ERROR_SERIES + 6,
			4,
			Rule::Chain(9, TS('('), NS('E'), TS('c'), NS('E'), TS(')'), TS('{'), NS('N'), TS('}'), NS('X')),
			Rule::Chain(8, TS('('), NS('E'), TS('c'), NS('E'), TS(')'), TS('{'), NS('N'), TS('}')),
			Rule::Chain(9, TS('('), NS('B'), TS('c'), NS('E'), TS(')'), TS('{'), NS('N'), TS('}'), NS('X')),
			Rule::Chain(8, TS('('), NS('B'), TS('c'), NS('E'), TS(')'), TS('{'), NS('N'), TS('}'))
		),

		Rule(NS('X'), GRB_ERROR_SERIES + 7,
			2,
			Rule::Chain(4, TS(':'), TS('{'), NS('N'), TS('}')),
			Rule::Chain(3, TS(':'), TS('?'), NS('Y'))
		),

		Rule(NS('W'), GRB_ERROR_SERIES + 3,
			4,
			Rule::Chain(3, TS('i'), TS(','), NS('W')),
			Rule::Chain(3, TS('l'), TS(','), NS('W')),
			Rule::Chain(1, TS('i')),
			Rule::Chain(1, TS('l'))
		),

		Rule(NS('F'), GRB_ERROR_SERIES + 4,
			2,
			Rule::Chain(4, TS('t'), TS('i'), TS(','), NS('F')),
			Rule::Chain(2, TS('t'), TS('i'))
		),

		Rule(NS('M'), GRB_ERROR_SERIES + 8,
			2,
			Rule::Chain(3, TS('v'), NS('E'), NS('M')),
			Rule::Chain(2, TS('v'), NS('E'))
		)
	);
}

#undef GRB_ERROR_SERIES 
#undef NS(n)
#undef TS(n)
#undef ISNS(n)