#pragma once
#include "IT.h"
#include "LT.h"

namespace semantic {
	int check(const IT::ID_Table& ID_Table, const LT::LexTable& LEX_Table, Log::LOG* log);
}