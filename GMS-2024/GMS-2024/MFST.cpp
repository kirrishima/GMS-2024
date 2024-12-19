#include "stdafx.h"
#include "MFST.h"
#include <format>

namespace MFST
{
	// Конструктор MfstState без параметров, инициализирует поля значениями по умолчанию
	MfstState::MfstState()
	{
		lenta_position = 0; // Позиция в ленте
		nrule = -1;			// Индекс текущего правила (-1 означает отсутствие)
		nrulechain = -1;	// Индекс цепочки правила (-1 означает отсутствие)
	};

	// Конструктор MfstState с параметрами
	MfstState::MfstState(short pposition, MFSTSTSTACK pst, short pnrulechain)
	{
		lenta_position = pposition; // Инициализация позиции в ленте
		st = pst;					// Инициализация стека
		nrulechain = pnrulechain;	// Инициализация индекса цепочки правила
	};

	// Конструктор MfstState с дополнительным параметром для индекса правила
	MfstState::MfstState(short pposition, MFSTSTSTACK pst, short pnrule, short pnrulechain)
	{
		lenta_position = pposition; // Инициализация позиции в ленте
		st = pst;					// Инициализация стека
		nrule = pnrule;				// Инициализация индекса текущего правила
		nrulechain = pnrulechain;	// Инициализация индекса цепочки правила
	};

	// Конструктор MfstDiagnosis без параметров, инициализирует поля значениями по умолчанию
	Mfst::MfstDiagnosis::MfstDiagnosis()
	{
		lenta_position = -1; // Позиция в ленте (-1 означает отсутствие)
		rc_step = SURPRISE;	 // Шаг синтаксического анализа, установленный в состояние "неожиданность"
		nrule = -1;			 // Индекс текущего правила (-1 означает отсутствие)
		nrule_chain = -1;	 // Индекс цепочки правила (-1 означает отсутствие)
	};

	// Конструктор MfstDiagnosis с параметрами
	Mfst::MfstDiagnosis::MfstDiagnosis(short plenta_position, RC_STEP prc_step, short pnrule, short pnrule_chain)
	{
		lenta_position = plenta_position; // Позиция в ленте
		rc_step = prc_step;				  // Текущий шаг синтаксического анализа
		nrule = pnrule;					  // Индекс текущего правила
		nrule_chain = pnrule_chain;		  // Индекс цепочки правила
	};

	// Конструктор Mfst без параметров, инициализирует ленту и позицию
	Mfst::Mfst()
	{
		lenta = 0;						 // Указатель на ленту
		lenta_size = lenta_position = 0; // Размер ленты и позиция инициализируются нулем
	};

	// Конструктор Mfst с параметрами таблицы лексем и грамматики Грейбаха
	Mfst::Mfst(LT::LexTable &lextable, GRB::Greibach pgrebach)
	{
		grebach = pgrebach;			   // Инициализация грамматики
		lex = lextable;				   // Инициализация таблицы лексем
		lenta_size = lex.size;		   // Установка размера ленты
		lenta = new short[lenta_size]; // Создание ленты

		// Заполнение ленты лексемами из таблицы
		for (int k = 0; k < lenta_size; k++)
			lenta[k] = GRB::Rule::Chain::T(lex.table[k].lexema[0]);

		lenta_position = 0;			// Инициализация позиции в ленте
		st.push(grebach.stbottomT); // Добавление символа дна стека
		st.push(grebach.startN);	// Добавление стартового нетерминала
		nrulechain = -1;			// Индекс цепочки правила
	}

	// Основной метод шага синтаксического анализа
	Mfst::RC_STEP Mfst::step()
	{
		RC_STEP rc = SURPRISE; // Инициализация результата как "неожиданность"

		if (lenta_position < lenta_size) // Проверка, не достигнут ли конец ленты
		{
			if (GRB::Rule::Chain::isN(st.top())) // Если вершина стека - нетерминал
			{
				GRB::Rule rule;
				if ((nrule = grebach.getRule(st.top(), rule)) >= 0) // Получение правила для нетерминала
				{
					GRB::Rule::Chain chain;
					if ((nrulechain = rule.getNextChain(lenta[lenta_position], chain, nrulechain + 1)) >= 0) // Поиск цепочки правил
					{
#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
						MFST_TRACE1		   // Отладочная информация
#endif									   // !_DISABLE_MFST_DEBUG
#endif									   // _DEBUG
						savestate();	   // Сохранение текущего состояния
						st.pop();		   // Удаление нетерминала из стека
						push_chain(chain); // Добавление цепочки в стек
						rc = NS_OK;		   // Успешный шаг с нетерминалом

#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
						MFST_TRACE2 // Отладочная информация
#endif								// !_DISABLE_MFST_DEBUG
#endif								// _DEBUG
					}
					else
					{
#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
						MFST_TRACE4("TNS_NORULECHAIN/NS_NORULE") // Отладочная информация
#endif															 // !_DISABLE_MFST_DEBUG
#endif															 // _DEBUG

						savediagnosis(NS_NORULECHAIN);					// Сохранение диагноза ошибки
						rc = resetstate() ? NS_NORULECHAIN : NS_NORULE; // Сброс состояния или ошибка
					};
				}
				else
				{
					rc = NS_ERROR; // Ошибка при поиске правила
				}
			}
			else if ((st.top() == lenta[lenta_position])) // Если вершина стека - терминал и совпадает с текущей лексемой
			{
				lenta_position++; // Переход к следующей позиции в ленте
				st.pop();		  // Удаление терминала из стека
				nrulechain = -1;  // Сброс индекса цепочки правила
				rc = TS_OK;		  // Успешный шаг с терминалом

#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
				MFST_TRACE3 // Отладочная информация
#endif						// !_DISABLE_MFST_DEBUG
#endif						// _DEBUG
			}
			else
			{
#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
				MFST_TRACE4("TS_NOK/NS_NORULECHAIN") // Отладочная информация
#endif												 // !_DISABLE_MFST_DEBUG
#endif												 // _DEBUG

				rc = resetstate() ? TS_NOK : NS_NORULECHAIN; // Сброс состояния или ошибка
			}
		}
		else
		{
			rc = LENTA_END; // Конец ленты
#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
			MFST_TRACE4("LENTA_END") // Отладочная информация
#endif								 // !_DISABLE_MFST_DEBUG
#endif								 // _DEBUG
		};

		return rc; // Возврат результата шага
	};

	// Метод для добавления цепочки символов в стек
	bool Mfst::push_chain(GRB::Rule::Chain chain)
	{
		for (int k = chain.size - 1; k >= 0; k--) // Обратный порядок добавления символов
			st.push(chain.nt[k]);				  // Добавление символов в стек

		return true;
	};

	// Метод для сохранения состояния синтаксического анализа
	bool Mfst::savestate()
	{
		storestate.push(MfstState(lenta_position, st, nrule, nrulechain)); // Сохранение состояния в стек

#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
		MFST_TRACE6("SAVESTATE:", storestate.size()); // Отладочная информация
#endif												  // !_DISABLE_MFST_DEBUG
#endif												  // _DEBUG

		return true;
	};

	// Метод для восстановления состояния синтаксического анализа
	bool Mfst::resetstate()
	{
		bool rc = false;
		MfstState state;
#pragma warning(push)
#pragma warning(disable : 4706)
		if (rc = (storestate.size() > 0)) // Проверка, есть ли сохраненные состояния
		{
			state = storestate.top();			   // Получение верхнего состояния из стека
			lenta_position = state.lenta_position; // Восстановление позиции в ленте
			st = state.st;						   // Восстановление стека
			nrule = state.nrule;				   // Восстановление индекса правила
			nrulechain = state.nrulechain;		   // Восстановление индекса цепочки правила

			storestate.pop(); // Удаление восстановленного состояния из стека

#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
			MFST_TRACE5("RESSTATE") // Отладочная информация
			MFST_TRACE2				// Отладочная информация
#endif								// !_DISABLE_MFST_DEBUG
#endif								// _DEBUG

				rc = true;
		}

		return rc;
	};
#pragma warning(pop)
	// Метод для сохранения диагноза ошибки синтаксического анализа
	bool Mfst::savediagnosis(RC_STEP prc_step)
	{
		bool rc = false;
		short k = 0;

		while (k < MFST_DIAGN_NUMBER && lenta_position <= diagnosis[k].lenta_position)
			k++; // Поиск свободной позиции для диагноза
#pragma warning(push)
#pragma warning(disable : 4706)
		if (rc = (k < MFST_DIAGN_NUMBER))
		{
			diagnosis[k] = MfstDiagnosis(lenta_position, prc_step, nrule, nrulechain); // Сохранение диагноза

			for (int i = k + 1; i < MFST_DIAGN_NUMBER; i++)
				diagnosis[i].lenta_position = -1; // Очистка оставшихся диагнозов
		}

		return rc;
	};
#pragma warning(pop)
	// Метод для начала синтаксического анализа
	bool Mfst::start(Log::LOG &log)
	{
		bool rc = false;
		RC_STEP rc_step = SURPRISE;
		char buf[MFST_DIAGN_MAXSIZE] = {};
		rc_step = step(); // Выполнение первого шага

		while (rc_step == NS_OK || rc_step == NS_NORULECHAIN || rc_step == TS_OK || rc_step == TS_NOK)
			rc_step = step(); // Выполнение шагов синтаксического анализа, пока они успешны

		switch (rc_step) // Обработка результатов анализа
		{
		case LENTA_END:
		{
#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
			MFST_TRACE4("------>LENTA_END") // Отладочная информация
			std::cout << "---------------------------------------------------------------------------------------------------" << std::endl;
#endif // !_DISABLE_MFST_DEBUG
#endif // _DEBUG

			sprintf_s(buf, MFST_DIAGN_MAXSIZE, "всего строк %d, синтаксический анализ выполнен без ошибок", lex.table[lex.size - 1].sn);
			Log::writeLine(log, buf, "\n", nullptr);
			std::cout << "всего строк " << lex.table[lex.size - 1].sn << ", синтаксический анализ выпол без ошибок" << std::endl;
			rc = true;
			break;
		}

		case NS_NORULE:
		{
#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
			MFST_TRACE4("------>NS_NORULE") // Отладочная информация
			std::cout << "---------------------------------------------------------------------------------------------------" << std::endl;
#endif // !_DISABLE_MFST_DEBUG
#endif // _DEBUG
			char *diagnosis1 = getDiagnosis(0, buf);
			char *diagnosis2 = getDiagnosis(1, buf);
			char *diagnosis3 = getDiagnosis(2, buf);

			std::cout << diagnosis1 << std::endl;
			std::cout << diagnosis2 << std::endl;
			std::cout << diagnosis3 << std::endl;

			Log::writeLine(log, "----    Ошибка    ----");
			Log::writeLine(log, diagnosis1, "\n", nullptr);
			Log::writeLine(log, diagnosis2, "\n", nullptr);
			Log::writeLine(log, diagnosis3, "\n", nullptr);

			break;
		}

		case NS_NORULECHAIN:

#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
			MFST_TRACE4("------>NS_NORULECHAIN");
#endif // !_DISABLE_MFST_DEBUG
#endif // _DEBUG
			break;

		case NS_ERROR:
#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
			MFST_TRACE4("------>NS_ERROR");
#endif // !_DISABLE_MFST_DEBUG
#endif // _DEBUG
			break;

		case SURPRISE:
#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
			MFST_TRACE4("------>NS_SURPRISE");
#endif // !_DISABLE_MFST_DEBUG
#endif // _DEBUG
			break;
		}

		return rc;
	};

	// Метод для получения содержимого стека в виде строки
	char *Mfst::getCSt(char *buf)
	{
		short p;
		for (int k = static_cast<signed>(st.size()) - 1; k >= 0; --k)
		{
			p = (st.c[k]);													// Получение символа из стека
			buf[st.size() - 1 - k] = GRB::Rule::Chain::alphabet_to_char(p); // Преобразование в символ
		}
		buf[st.size()] = '\0'; // Завершение строки
		return buf;
	}

	// Метод для получения части ленты в виде строки
	char *Mfst::getCLenta(char *buf, short pos, short n)
	{
		short i, k = (pos + n < lenta_size) ? pos + n : lenta_size; // Определение границ
		for (i = pos; i < k; i++)
			buf[i - pos] = GRB::Rule::Chain::alphabet_to_char(lenta[i]); // Преобразование в символы
		buf[i - pos] = 0x00;											 // Завершение строки
		return buf;
	}

	// Метод для получения текста диагноза ошибки
	char *Mfst::getDiagnosis(short n, char *buf)
	{
		char *rc = new char[200]{}; // Создание строки для диагноза
		int errid = 0;
		int lpos = -1;

		if (n < MFST_DIAGN_NUMBER && (lpos = diagnosis[n].lenta_position) >= 0) // Проверка диагноза
		{
			errid = grebach.getRule(diagnosis[n].nrule).iderror; // Получение идентификатора ошибки
			Error::ERROR err = Error::geterror(errid);			 // Получение сообщения об ошибке
			sprintf_s(buf, MFST_DIAGN_MAXSIZE, "Ошибка %d: строка %d, %s", err.id, lex.table[lpos].sn, err.message);
			rc = buf; // Формирование текста ошибки
		}

		return rc;
	}

	// Метод для вывода правил синтаксического анализа
	void Mfst::printrules()
	{
		MfstState state;
		GRB::Rule rule;
		for (unsigned short i = 0; i < storestate.size(); i++) // Перебор всех сохраненных состояний
		{
			state = storestate.c[i];			 // Получение состояния
			rule = grebach.getRule(state.nrule); // Получение правила

			// #ifdef _DEBUG
			// #ifndef _DISABLE_MFST_DEBUG
			MFST_TRACE7 // Отладочная информация
						// #endif // !_DISABLE_MFST_DEBUG
			// #endif // _DEBUG
		};
	};

	// Метод для сохранения выводов синтаксического анализа
	bool Mfst::savededucation()
	{
		deducation.nrules = new short[deducation.size = storestate.size()]; // Создание массива правил
		deducation.nrulechains = new short[deducation.size];				// Создание массива цепочек правил

		for (unsigned short i = 0; i < storestate.size(); i++) // Перебор всех сохраненных состояний
		{
			deducation.nrules[i] = storestate.c[i].nrule;			// Сохранение индекса правила
			deducation.nrulechains[i] = storestate.c[i].nrulechain; // Сохранение индекса цепочки
		}

		return true;
	}
}
