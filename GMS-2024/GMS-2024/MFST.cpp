#include "stdafx.h"
#include "MFST.h"
#include <format>

namespace MFST
{
	// ����������� MfstState ��� ����������, �������������� ���� ���������� �� ���������
	MfstState::MfstState()
	{
		lenta_position = 0; // ������� � �����
		nrule = -1;			// ������ �������� ������� (-1 �������� ����������)
		nrulechain = -1;	// ������ ������� ������� (-1 �������� ����������)
	};

	// ����������� MfstState � �����������
	MfstState::MfstState(short pposition, MFSTSTSTACK pst, short pnrulechain)
	{
		lenta_position = pposition; // ������������� ������� � �����
		st = pst;					// ������������� �����
		nrulechain = pnrulechain;	// ������������� ������� ������� �������
	};

	// ����������� MfstState � �������������� ���������� ��� ������� �������
	MfstState::MfstState(short pposition, MFSTSTSTACK pst, short pnrule, short pnrulechain)
	{
		lenta_position = pposition; // ������������� ������� � �����
		st = pst;					// ������������� �����
		nrule = pnrule;				// ������������� ������� �������� �������
		nrulechain = pnrulechain;	// ������������� ������� ������� �������
	};

	// ����������� MfstDiagnosis ��� ����������, �������������� ���� ���������� �� ���������
	Mfst::MfstDiagnosis::MfstDiagnosis()
	{
		lenta_position = -1; // ������� � ����� (-1 �������� ����������)
		rc_step = SURPRISE;	 // ��� ��������������� �������, ������������� � ��������� "�������������"
		nrule = -1;			 // ������ �������� ������� (-1 �������� ����������)
		nrule_chain = -1;	 // ������ ������� ������� (-1 �������� ����������)
	};

	// ����������� MfstDiagnosis � �����������
	Mfst::MfstDiagnosis::MfstDiagnosis(short plenta_position, RC_STEP prc_step, short pnrule, short pnrule_chain)
	{
		lenta_position = plenta_position; // ������� � �����
		rc_step = prc_step;				  // ������� ��� ��������������� �������
		nrule = pnrule;					  // ������ �������� �������
		nrule_chain = pnrule_chain;		  // ������ ������� �������
	};

	// ����������� Mfst ��� ����������, �������������� ����� � �������
	Mfst::Mfst()
	{
		lenta = 0;						 // ��������� �� �����
		lenta_size = lenta_position = 0; // ������ ����� � ������� ���������������� �����
	};

	// ����������� Mfst � ����������� ������� ������ � ���������� ��������
	Mfst::Mfst(LT::LexTable &lextable, GRB::Greibach pgrebach)
	{
		grebach = pgrebach;			   // ������������� ����������
		lex = lextable;				   // ������������� ������� ������
		lenta_size = lex.size;		   // ��������� ������� �����
		lenta = new short[lenta_size]; // �������� �����

		// ���������� ����� ��������� �� �������
		for (int k = 0; k < lenta_size; k++)
			lenta[k] = GRB::Rule::Chain::T(lex.table[k].lexema[0]);

		lenta_position = 0;			// ������������� ������� � �����
		st.push(grebach.stbottomT); // ���������� ������� ��� �����
		st.push(grebach.startN);	// ���������� ���������� �����������
		nrulechain = -1;			// ������ ������� �������
	}

	// �������� ����� ���� ��������������� �������
	Mfst::RC_STEP Mfst::step()
	{
		RC_STEP rc = SURPRISE; // ������������� ���������� ��� "�������������"

		if (lenta_position < lenta_size) // ��������, �� ��������� �� ����� �����
		{
			if (GRB::Rule::Chain::isN(st.top())) // ���� ������� ����� - ����������
			{
				GRB::Rule rule;
				if ((nrule = grebach.getRule(st.top(), rule)) >= 0) // ��������� ������� ��� �����������
				{
					GRB::Rule::Chain chain;
					if ((nrulechain = rule.getNextChain(lenta[lenta_position], chain, nrulechain + 1)) >= 0) // ����� ������� ������
					{
#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
						MFST_TRACE1		   // ���������� ����������
#endif									   // !_DISABLE_MFST_DEBUG
#endif									   // _DEBUG
						savestate();	   // ���������� �������� ���������
						st.pop();		   // �������� ����������� �� �����
						push_chain(chain); // ���������� ������� � ����
						rc = NS_OK;		   // �������� ��� � ������������

#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
						MFST_TRACE2 // ���������� ����������
#endif								// !_DISABLE_MFST_DEBUG
#endif								// _DEBUG
					}
					else
					{
#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
						MFST_TRACE4("TNS_NORULECHAIN/NS_NORULE") // ���������� ����������
#endif															 // !_DISABLE_MFST_DEBUG
#endif															 // _DEBUG

						savediagnosis(NS_NORULECHAIN);					// ���������� �������� ������
						rc = resetstate() ? NS_NORULECHAIN : NS_NORULE; // ����� ��������� ��� ������
					};
				}
				else
				{
					rc = NS_ERROR; // ������ ��� ������ �������
				}
			}
			else if ((st.top() == lenta[lenta_position])) // ���� ������� ����� - �������� � ��������� � ������� ��������
			{
				lenta_position++; // ������� � ��������� ������� � �����
				st.pop();		  // �������� ��������� �� �����
				nrulechain = -1;  // ����� ������� ������� �������
				rc = TS_OK;		  // �������� ��� � ����������

#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
				MFST_TRACE3 // ���������� ����������
#endif						// !_DISABLE_MFST_DEBUG
#endif						// _DEBUG
			}
			else
			{
#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
				MFST_TRACE4("TS_NOK/NS_NORULECHAIN") // ���������� ����������
#endif												 // !_DISABLE_MFST_DEBUG
#endif												 // _DEBUG

				rc = resetstate() ? TS_NOK : NS_NORULECHAIN; // ����� ��������� ��� ������
			}
		}
		else
		{
			rc = LENTA_END; // ����� �����
#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
			MFST_TRACE4("LENTA_END") // ���������� ����������
#endif								 // !_DISABLE_MFST_DEBUG
#endif								 // _DEBUG
		};

		return rc; // ������� ���������� ����
	};

	// ����� ��� ���������� ������� �������� � ����
	bool Mfst::push_chain(GRB::Rule::Chain chain)
	{
		for (int k = chain.size - 1; k >= 0; k--) // �������� ������� ���������� ��������
			st.push(chain.nt[k]);				  // ���������� �������� � ����

		return true;
	};

	// ����� ��� ���������� ��������� ��������������� �������
	bool Mfst::savestate()
	{
		storestate.push(MfstState(lenta_position, st, nrule, nrulechain)); // ���������� ��������� � ����

#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
		MFST_TRACE6("SAVESTATE:", storestate.size()); // ���������� ����������
#endif												  // !_DISABLE_MFST_DEBUG
#endif												  // _DEBUG

		return true;
	};

	// ����� ��� �������������� ��������� ��������������� �������
	bool Mfst::resetstate()
	{
		bool rc = false;
		MfstState state;
#pragma warning(push)
#pragma warning(disable : 4706)
		if (rc = (storestate.size() > 0)) // ��������, ���� �� ����������� ���������
		{
			state = storestate.top();			   // ��������� �������� ��������� �� �����
			lenta_position = state.lenta_position; // �������������� ������� � �����
			st = state.st;						   // �������������� �����
			nrule = state.nrule;				   // �������������� ������� �������
			nrulechain = state.nrulechain;		   // �������������� ������� ������� �������

			storestate.pop(); // �������� ���������������� ��������� �� �����

#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
			MFST_TRACE5("RESSTATE") // ���������� ����������
			MFST_TRACE2				// ���������� ����������
#endif								// !_DISABLE_MFST_DEBUG
#endif								// _DEBUG

				rc = true;
		}

		return rc;
	};
#pragma warning(pop)
	// ����� ��� ���������� �������� ������ ��������������� �������
	bool Mfst::savediagnosis(RC_STEP prc_step)
	{
		bool rc = false;
		short k = 0;

		while (k < MFST_DIAGN_NUMBER && lenta_position <= diagnosis[k].lenta_position)
			k++; // ����� ��������� ������� ��� ��������
#pragma warning(push)
#pragma warning(disable : 4706)
		if (rc = (k < MFST_DIAGN_NUMBER))
		{
			diagnosis[k] = MfstDiagnosis(lenta_position, prc_step, nrule, nrulechain); // ���������� ��������

			for (int i = k + 1; i < MFST_DIAGN_NUMBER; i++)
				diagnosis[i].lenta_position = -1; // ������� ���������� ���������
		}

		return rc;
	};
#pragma warning(pop)
	// ����� ��� ������ ��������������� �������
	bool Mfst::start(Log::LOG &log)
	{
		bool rc = false;
		RC_STEP rc_step = SURPRISE;
		char buf[MFST_DIAGN_MAXSIZE] = {};
		rc_step = step(); // ���������� ������� ����

		while (rc_step == NS_OK || rc_step == NS_NORULECHAIN || rc_step == TS_OK || rc_step == TS_NOK)
			rc_step = step(); // ���������� ����� ��������������� �������, ���� ��� �������

		switch (rc_step) // ��������� ����������� �������
		{
		case LENTA_END:
		{
#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
			MFST_TRACE4("------>LENTA_END") // ���������� ����������
			std::cout << "---------------------------------------------------------------------------------------------------" << std::endl;
#endif // !_DISABLE_MFST_DEBUG
#endif // _DEBUG

			sprintf_s(buf, MFST_DIAGN_MAXSIZE, "����� ����� %d, �������������� ������ �������� ��� ������", lex.table[lex.size - 1].sn);
			Log::writeLine(log, buf, "\n", nullptr);
			std::cout << "����� ����� " << lex.table[lex.size - 1].sn << ", �������������� ������ ����� ��� ������" << std::endl;
			rc = true;
			break;
		}

		case NS_NORULE:
		{
#ifdef _DEBUG
#ifndef _DISABLE_MFST_DEBUG
			MFST_TRACE4("------>NS_NORULE") // ���������� ����������
			std::cout << "---------------------------------------------------------------------------------------------------" << std::endl;
#endif // !_DISABLE_MFST_DEBUG
#endif // _DEBUG
			char *diagnosis1 = getDiagnosis(0, buf);
			char *diagnosis2 = getDiagnosis(1, buf);
			char *diagnosis3 = getDiagnosis(2, buf);

			std::cout << diagnosis1 << std::endl;
			std::cout << diagnosis2 << std::endl;
			std::cout << diagnosis3 << std::endl;

			Log::writeLine(log, "----    ������    ----");
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

	// ����� ��� ��������� ����������� ����� � ���� ������
	char *Mfst::getCSt(char *buf)
	{
		short p;
		for (int k = static_cast<signed>(st.size()) - 1; k >= 0; --k)
		{
			p = (st.c[k]);													// ��������� ������� �� �����
			buf[st.size() - 1 - k] = GRB::Rule::Chain::alphabet_to_char(p); // �������������� � ������
		}
		buf[st.size()] = '\0'; // ���������� ������
		return buf;
	}

	// ����� ��� ��������� ����� ����� � ���� ������
	char *Mfst::getCLenta(char *buf, short pos, short n)
	{
		short i, k = (pos + n < lenta_size) ? pos + n : lenta_size; // ����������� ������
		for (i = pos; i < k; i++)
			buf[i - pos] = GRB::Rule::Chain::alphabet_to_char(lenta[i]); // �������������� � �������
		buf[i - pos] = 0x00;											 // ���������� ������
		return buf;
	}

	// ����� ��� ��������� ������ �������� ������
	char *Mfst::getDiagnosis(short n, char *buf)
	{
		char *rc = new char[200]{}; // �������� ������ ��� ��������
		int errid = 0;
		int lpos = -1;

		if (n < MFST_DIAGN_NUMBER && (lpos = diagnosis[n].lenta_position) >= 0) // �������� ��������
		{
			errid = grebach.getRule(diagnosis[n].nrule).iderror; // ��������� �������������� ������
			Error::ERROR err = Error::geterror(errid);			 // ��������� ��������� �� ������
			sprintf_s(buf, MFST_DIAGN_MAXSIZE, "������ %d: ������ %d, %s", err.id, lex.table[lpos].sn, err.message);
			rc = buf; // ������������ ������ ������
		}

		return rc;
	}

	// ����� ��� ������ ������ ��������������� �������
	void Mfst::printrules()
	{
		MfstState state;
		GRB::Rule rule;
		for (unsigned short i = 0; i < storestate.size(); i++) // ������� ���� ����������� ���������
		{
			state = storestate.c[i];			 // ��������� ���������
			rule = grebach.getRule(state.nrule); // ��������� �������

			// #ifdef _DEBUG
			// #ifndef _DISABLE_MFST_DEBUG
			MFST_TRACE7 // ���������� ����������
						// #endif // !_DISABLE_MFST_DEBUG
			// #endif // _DEBUG
		};
	};

	// ����� ��� ���������� ������� ��������������� �������
	bool Mfst::savededucation()
	{
		deducation.nrules = new short[deducation.size = storestate.size()]; // �������� ������� ������
		deducation.nrulechains = new short[deducation.size];				// �������� ������� ������� ������

		for (unsigned short i = 0; i < storestate.size(); i++) // ������� ���� ����������� ���������
		{
			deducation.nrules[i] = storestate.c[i].nrule;			// ���������� ������� �������
			deducation.nrulechains[i] = storestate.c[i].nrulechain; // ���������� ������� �������
		}

		return true;
	}
}
