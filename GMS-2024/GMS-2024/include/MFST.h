#pragma once
#include "stdafx.h"
#include "MFST.h"
#include "GRB.h"
#include <iomanip>
#include <stack>

class my_stack_SHORT : public std::stack<short> { // стек автомата
public:
	using std::stack<short>::c;
};

#define MFST_DIAGN_MAXSIZE 2 * ERROR_MAXSIZE_MESSAGE
#define MFST_DIAGN_NUMBER 3

static int FST_TRACE_n = -1; // номер текущего шага трассировки
static char rbuf[205], sbuf[205], lbuf[1024];

// Макросы для трассировки (отладки)
#define MFST_TRACE_START std::cout << std::setw(4) << std::left << "Шаг" << ":" \
                                  << std::setw(20) << std::left << " Правило" \
                                  << std::setw(30) << std::left << " Входная лента" \
                                  << std::setw(20) << std::left << " Стек" \
                                  << std::endl;

#define MFST_TRACE1 std::cout << std::setw(4) << std::left << ++FST_TRACE_n << ": " \
                               << std::setw(20) << std::left << rule.getCRule(rbuf, nrulechain) \
                               << std::setw(30) << std::left << getCLenta(lbuf, lenta_position) \
                               << std::setw(20) << std::left << getCSt(sbuf) \
                               << std::endl;

#define MFST_TRACE2 std::cout << std::setw(4) << std::left << FST_TRACE_n << ": " \
                               << std::setw(20) << std::left << " " \
                               << std::setw(30) << std::left << getCLenta(lbuf, lenta_position) \
                               << std::setw(20) << std::left << getCSt(sbuf) \
                               << std::endl;

#define MFST_TRACE3 std::cout << std::setw(4) << std::left << ++FST_TRACE_n << ": " \
                               << std::setw(20) << std::left << " " \
                               << std::setw(30) << std::left << getCLenta(lbuf, lenta_position) \
                               << std::setw(20) << std::left << getCSt(sbuf) \
                               << std::endl;

#define MFST_TRACE4(c) std::cout << std::setw(4) << std::left << ++FST_TRACE_n << ": " << std::setw(20) << std::left << c << std::endl;
#define MFST_TRACE5(c) std::cout << std::setw(4) << std::left << FST_TRACE_n << ": " << std::setw(20) << std::left << c << std::endl;
#define MFST_TRACE6(c, k) std::cout << std::setw(4) << std::left << FST_TRACE_n << ": " << std::setw(20) << std::left << c << k << std::endl;
#define MFST_TRACE7 std::cout << std::setw(4) << std::left << state.lenta_position << ": " \
                               << std::setw(20) << std::left << rule.getCRule(rbuf, state.nrulechain) \
                               << '\n';

typedef my_stack_SHORT MFSTSTSTACK;

namespace MFST {
	struct MfstState { // Структура состояния для управления автоматом
		short lenta_position; // Текущая позиция на входной ленте
		short nrule; // Номер правила применённого правила
		short nrulechain; // Номер цепочки правил применённого правила
		MFSTSTSTACK st; // Стек автомата
		MfstState();
		MfstState(short pposition, MFSTSTSTACK pst, short pnrulechain); // (позиция на ленте; стек автомата; номер применённой цепочки правил)
		MfstState(short pposition, MFSTSTSTACK pst, short pnrule, short pnrulechain); // (позиция на ленте; стек автомата; номер применённого правила; номер применённой цепочки правил)
	};

	class my_stack_MfstState : public std::stack<MfstState> {
	public:
		using std::stack<MfstState>::c;
	};

	struct Mfst { // Основная структура автомата
		enum RC_STEP { // Коды результата для функции шага
			NS_OK, // Успешно обработано правило и стек; стек возвращается в предыдущее состояние
			NS_NORULE, // Не найдено подходящее правило (ошибка в правилах)
			NS_NORULECHAIN, // Не найдена применимая цепочка правил для обработанного правила (ошибка в цепочках правил)
			NS_ERROR, // Встретилась непоправимая ошибка
			TS_OK, // Токен совпадает с ожидаемым стеком, возвращает токен; извлекает из стека
			TS_NOK, // Токен не совпадает с ожидаемым стеком, возвращает токен; извлекает из стека
			LENTA_END, // Достигнут конец входной ленты (>= размер ленты)
			SURPRISE, // Встретился неожиданный ввод (ошибка в шаге)
		};

		struct MfstDiagnosis { // Структура для диагностики
			short lenta_position; // Позиция на входной ленте
			RC_STEP rc_step; // Код результата последней операции
			short nrule; // Номер применённого правила
			short nrule_chain; // Номер цепочки правил
			MfstDiagnosis(); // Конструктор по умолчанию
			MfstDiagnosis(short plenta_position, RC_STEP prc_step, short pnrule, short pnrule_chain);
		} diagnosis[MFST_DIAGN_NUMBER]; // Массив для хранения последних диагнозов

		GRBALPHABET* lenta; // Указатель на входную ленту (TN/NS)
		short lenta_position; // Текущая позиция на входной ленте
		short nrule; // Номер текущего применённого правила
		short nrulechain; // Номер текущей применённой цепочки правил
		short lenta_size; // Размер входной ленты
		GRB::Greibach grebach; // Нормальная форма Грейбаха
		LT::LexTable lex; // Лексическая таблица для распознанного автомата
		MFSTSTSTACK st; // Стек автомата
		my_stack_MfstState storestate; // Стек для хранения состояния автомата

		Mfst();
		Mfst(LT::LexTable& plex, GRB::Greibach pgrebach);
		char* getCSt(char* buf); // Выводит текущее состояние в виде строки
		char* getCLenta(char* buf, short pos, short n = 25); // Лента: n токенов с позиции pos
		char* getDiagnosis(short n, char* buf); // Выводит n-ый элемент массива диагнозов или 0
		bool savestate(); // Сохраняет текущее состояние автомата
		bool resetstate(); // Восстанавливает сохранённое состояние автомата
		bool push_chain(GRB::Rule::Chain chain); // Добавляет цепочку правил в стек
		RC_STEP step(); // Выполняет шаг в автомате
		bool start(Log::LOG& log); // Запускает автомат
		bool savediagnosis(RC_STEP pprc_step); // Сохраняет результат последней операции
		void printrules(); // Печатает применённые правила

		struct Deducation {
			short size;
			short* nrules;
			short* nrulechains;
			Deducation() {
				size = 0;
				nrules = nullptr;
				nrulechains = nullptr;
			}
		} deducation;

		bool savededucation(); // Сохраняет информацию о выводах
	};
}
