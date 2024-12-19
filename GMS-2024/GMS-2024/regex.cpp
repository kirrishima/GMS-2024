#include "stdafx.h"
#include "regex.h"
#include "FST.h"

namespace REGEX
{
	FST::FST *CreateIntegerFST(const char *str)
	{
		return new FST::FST(str,
							4,
							FST::NODE(1, FST::RELATION('i', 1)),
							FST::NODE(1, FST::RELATION('n', 2)),
							FST::NODE(1, FST::RELATION('t', 3)),
							FST::NODE());
	}
	FST::FST *CreateCharFST(const char *str)
	{
		return new FST::FST(str,
							5,
							FST::NODE(1, FST::RELATION('c', 1)),
							FST::NODE(1, FST::RELATION('h', 2)),
							FST::NODE(1, FST::RELATION('a', 3)),
							FST::NODE(1, FST::RELATION('r', 4)),
							FST::NODE());
	}

	FST::FST *CreateStringFST(const char *str)
	{
		return new FST::FST(str,
							4,
							FST::NODE(1, FST::RELATION('s', 1)),
							FST::NODE(1, FST::RELATION('t', 2)),
							FST::NODE(1, FST::RELATION('r', 3)),
							FST::NODE());
	}

	FST::FST *CreatePrintFST(const char *str)
	{
		return new FST::FST(str,
							6,
							FST::NODE(1, FST::RELATION('p', 1)),
							FST::NODE(1, FST::RELATION('r', 2)),
							FST::NODE(1, FST::RELATION('i', 3)),
							FST::NODE(1, FST::RELATION('n', 4)),
							FST::NODE(1, FST::RELATION('t', 5)),
							FST::NODE());
	}

	FST::FST *CreateStrcmpFST(const char *str)
	{
		return new FST::FST(str,
							7,
							FST::NODE(1, FST::RELATION('s', 1)),
							FST::NODE(1, FST::RELATION('t', 2)),
							FST::NODE(1, FST::RELATION('r', 3)),
							FST::NODE(1, FST::RELATION('c', 4)),
							FST::NODE(1, FST::RELATION('m', 5)),
							FST::NODE(1, FST::RELATION('p', 6)),
							FST::NODE());
	}

	FST::FST *CreateWriteFST(const char *str)
	{
		return new FST::FST(str,
							6,
							FST::NODE(1, FST::RELATION('w', 1)),
							FST::NODE(1, FST::RELATION('r', 2)),
							FST::NODE(1, FST::RELATION('i', 3)),
							FST::NODE(1, FST::RELATION('t', 4)),
							FST::NODE(1, FST::RELATION('e', 5)),
							FST::NODE());
	}

	FST::FST *CreateMainFST(const char *str)
	{
		return new FST::FST(str,
							5,
							FST::NODE(1, FST::RELATION('m', 1)),
							FST::NODE(1, FST::RELATION('a', 2)),
							FST::NODE(1, FST::RELATION('i', 3)),
							FST::NODE(1, FST::RELATION('n', 4)),
							FST::NODE());
	}

	FST::FST *CreateReturnFST(const char *str)
	{
		return new FST::FST(str,
							7,
							FST::NODE(1, FST::RELATION('r', 1)),
							FST::NODE(1, FST::RELATION('e', 2)),
							FST::NODE(1, FST::RELATION('t', 3)),
							FST::NODE(1, FST::RELATION('u', 4)),
							FST::NODE(1, FST::RELATION('r', 5)),
							FST::NODE(1, FST::RELATION('n', 6)),
							FST::NODE());
	}

#define nums(state) FST::RELATION('0', state), FST::RELATION('1', state), FST::RELATION('2', state), \
					FST::RELATION('3', state), FST::RELATION('4', state), FST::RELATION('5', state), \
					FST::RELATION('6', state), FST::RELATION('7', state), FST::RELATION('8', state), \
					FST::RELATION('9', state)

#define octal(state) FST::RELATION('0', state), FST::RELATION('1', state), FST::RELATION('2', state), \
					 FST::RELATION('3', state), FST::RELATION('4', state), FST::RELATION('5', state), \
					 FST::RELATION('6', state), FST::RELATION('7', state)

	FST::FST *CreateIntDECIMALLiteralFST(const char *str)
	{
		return new FST::FST(str,
							2,
							FST::NODE(20, nums(0), nums(1)),
							FST::NODE());
	}

#define R(c, x) FST::RELATION(c, x)

#define nums(state) R('0', state), R('1', state), R('2', state), \
					R('3', state), R('4', state), R('5', state), \
					R('6', state), R('7', state), R('8', state), \
					R('9', state)

#define HEX(x) R('a', x), R('b', x), R('c', x), R('d', x), R('e', x), R('f', x), \
			   R('A', x), R('B', x), R('C', x), R('D', x), R('E', x), R('F', x)

	FST::FST *CreateIntHEXLiteralFST(const char *str)
	{
		return new FST::FST(str,
							4,
							FST::NODE(1, R('0', 1)),
							FST::NODE(2, R('x', 2), R('X', 2)),
							FST::NODE(44, HEX(2), nums(2), nums(3), HEX(3)),
							FST::NODE());
	}

	FST::FST *CreateIntOCTALLiteralFST(const char *str)
	{
		return new FST::FST(str,
							3,
							FST::NODE(1, R('0', 1)),
							FST::NODE(16, octal(1), octal(2)),
							FST::NODE());
	}

	FST::FST *CreateIntBINARYLiteralFST(const char *str)
	{
		return new FST::FST(str,
							4,
							FST::NODE(1, R('0', 1)),
							FST::NODE(2, R('b', 2), R('B', 2)),
							FST::NODE(4, R('0', 2), R('1', 2), R('0', 3), R('1', 3)),
							FST::NODE());
	}

	FST::FST *CreateIfFST(const char *str)
	{
		return new FST::FST(str,
							3,
							FST::NODE(1, FST::RELATION('i', 1)),
							FST::NODE(1, FST::RELATION('f', 2)),
							FST::NODE());
	}

	FST::FST *CreateCompareFST(const char *str)
	{
		return new FST::FST(str,
							3,
							FST::NODE(6, FST::RELATION('>', 2), FST::RELATION('<', 2), FST::RELATION('>', 1), FST::RELATION('<', 1),
									  FST::RELATION('=', 1), FST::RELATION('!', 1)),
							FST::NODE(1, FST::RELATION('=', 2)),
							FST::NODE());
	}

	FST::FST *CreateElseFST(const char *str)
	{
		return new FST::FST(str,
							5,
							FST::NODE(1, FST::RELATION('e', 1)),
							FST::NODE(1, FST::RELATION('l', 2)),
							FST::NODE(1, FST::RELATION('s', 3)),
							FST::NODE(1, FST::RELATION('e', 4)),
							FST::NODE());
	}

	constexpr int NUM_LETTERS = 26 * 2 + 1;

#define relationsForState(state)                                    \
	R('a', state), R('b', state), R('c', state), R('d', state),     \
		R('e', state), R('f', state), R('g', state), R('h', state), \
		R('i', state), R('j', state), R('k', state), R('l', state), \
		R('m', state), R('n', state), R('o', state), R('p', state), \
		R('q', state), R('r', state), R('s', state), R('t', state), \
		R('u', state), R('v', state), R('w', state), R('x', state), \
		R('y', state), R('z', state), R('A', state), R('B', state), \
		R('C', state), R('D', state), R('E', state), R('F', state), \
		R('G', state), R('H', state), R('I', state), R('J', state), \
		R('K', state), R('L', state), R('M', state), R('N', state), \
		R('O', state), R('P', state), R('Q', state), R('R', state), \
		R('S', state), R('T', state), R('U', state), R('V', state), \
		R('W', state), R('X', state), R('Y', state), R('Z', state), R('_', state)

	FST::FST *CreateIdentifierFST(const char *str)
	{
		return new FST::FST(str,
							2,
							FST::NODE(NUM_LETTERS * 2 + 20,
									  relationsForState(0), nums(0),
									  relationsForState(1), nums(1)),
							FST::NODE());
	}

#undef nums
#undef relationsForState
#undef octal
#undef R
#undef HEX
}