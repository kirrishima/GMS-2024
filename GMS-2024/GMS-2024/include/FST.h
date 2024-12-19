#ifndef FST_H
#define FST_H

#include <cstddef>

namespace FST
{
	struct RELATION     // ребро: символ -> вершина графа переходов KA
	{
		char symbol;    // символ перехода
		short nnode;    // номер смежной вершины
		RELATION(char c = 0x00,      // символ перехода
			short ns = NULL)    // новое состояние
			: symbol(c), nnode(ns) {}
	};

	struct NODE         // вершина графа переходов
	{
		short n_relation;    // количество инцидентных ребер
		RELATION* relations; // инцидентные ребра
		NODE() : n_relation(0), relations(nullptr) {}
		NODE(short n, // количество инцидентных ребер
			RELATION rel, ... // список ребер
		);
	};

	// недетерминированный конечный автомат
	struct FST
	{
		const char* string;      // цепочка (строка, завершающаяся 0x00)
		short position;			 // текущая позиция в цепочке
		short nstates;			 // количество состояний автомата
		NODE* nodes;			 // граф переходов: [0] - начальное состояние, [nstate-1] - конечное
		short* rstates;			 // возможные состояния автомата на данной позиции
		FST(
			const char* s,       // цепочка (строка, завершающаяся 0x00)
			short ns,			 // количество состояний автомата
			NODE n, ...			 // список состояний (граф переходов)
		);
	};

	bool execute(         // выполнить распознавание цепочки
		FST& fst);        // недетерминированный конечный автомат

	bool execute(const char* regex, const char* str);
}


#endif // !FST_H