#include "stdafx.h" 
#include "error.h"
namespace Error
{
	// ����� ������: 0 - 99 ��������� ������
	//				 100 - 139 - ������ ����������
	//				 140 - 170 - ������ ��� ����������� �������

	ERROR errors[ERROR_MAX_ENTRY] = //������� ������
	{
	ERROR_ENTRY(0, "������������ ��� ������"), ERROR_ENTRY(1, "��������� ����"),
	// ��� ������ ��� ��������� 0 - ERROR_MAX_ENTRY
	ERROR_ENTRY_NODEF(2), ERROR_ENTRY_NODEF(3), ERROR_ENTRY_NODEF(4), ERROR_ENTRY_NODEF(5), ERROR_ENTRY_NODEF(6), ERROR_ENTRY_NODEF(7), ERROR_ENTRY_NODEF(8), ERROR_ENTRY_NODEF(9),
	ERROR_ENTRY_NODEF10(10), ERROR_ENTRY_NODEF10(20), ERROR_ENTRY_NODEF10(30), ERROR_ENTRY_NODEF10(40), ERROR_ENTRY_NODEF10(50), ERROR_ENTRY_NODEF10(60),
	ERROR_ENTRY_NODEF10(70), ERROR_ENTRY_NODEF10(80), ERROR_ENTRY_NODEF10(90),
	ERROR_ENTRY(100, "�������� -in ������ ���� �����"),
	ERROR_ENTRY_NODEF(101), ERROR_ENTRY_NODEF(102), ERROR_ENTRY_NODEF(103), ERROR_ENTRY(104, "��������� ����� �������� ���������"),
	ERROR_ENTRY_NODEF(105), ERROR_ENTRY_NODEF(106), ERROR_ENTRY_NODEF(107),
	ERROR_ENTRY_NODEF(108), ERROR_ENTRY(109, "������� ���� ������."),
	ERROR_ENTRY(110, "������ ��� �������� ����� � �������� ����� (-in)"),
	ERROR_ENTRY(111, "������������ ������ � �������� ����� (-in)"),
	ERROR_ENTRY(112, "������ ��� �������� ����� ���������(-log)"),
	ERROR_ENTRY_NODEF(113), ERROR_ENTRY_NODEF(114), ERROR_ENTRY_NODEF(115),
	// ���������
	ERROR_ENTRY(116, "���������� �� ���������� ���� log ����� �� ���������� ��� �� ������ �������� ����"),
	ERROR_ENTRY(117, "���������� �� ���������� ���� out ����� �� ���������� ��� �� ������ �������� ����"),
	ERROR_ENTRY(118, "���������� �� ���������� ���� ������� ������ �� ���������� ��� ������� �������� ����������: ��� ������ ���� .html"),
	ERROR_ENTRY(119, "��������� ���� ������� ��������������� �� ���������� ��� ������� �������� ����������: ��� ������ ���� .html"),
	ERROR_ENTRY(120, "������ �������� ������ �����: ����������� ������ 1024, ������������ 8192"),
	ERROR_ENTRY(121, "�������� ���� ��� ����� ���������� ���������"),
	ERROR_ENTRY(122, "������ �������� �������� ���� � ��������� -fo ��� ���������� �� ����������"),ERROR_ENTRY_NODEF(123),ERROR_ENTRY_NODEF(124),ERROR_ENTRY_NODEF(125),
	ERROR_ENTRY_NODEF(126), ERROR_ENTRY_NODEF(127), ERROR_ENTRY_NODEF(128), ERROR_ENTRY_NODEF(129), ERROR_ENTRY_NODEF10(130),
	ERROR_ENTRY(140, "����� ���� ������ ���� ����� ����� main"),
	ERROR_ENTRY(141, "��������� � ���������� �������� �� ���� ���� �������"),
	ERROR_ENTRY(142, "������������� �����������, �� �� ��������"),
	ERROR_ENTRY(143, "�������������� �������������� ���� ������������� �������� ����������"),
	ERROR_ENTRY(144, "�������������� ���������� ���� ������������� �������� �������������� ����"),
	ERROR_ENTRY(145, "�������� ������������ ������ �������� ������"),
	ERROR_ENTRY(146, "��������������� �������"),
	ERROR_ENTRY(147, "��������������� ��������� �������"),
	ERROR_ENTRY(148, "��������������� ����������"),
	ERROR_ENTRY(149, "������������ �������"),
	ERROR_ENTRY(150, "������������� �� ����� ���������� � �����"),
	ERROR_ENTRY(151, "���������� ������� ������ ����� ���� � ������ ���� ������"),
	ERROR_ENTRY(152, "����� ����� �� ���� �������"), ERROR_ENTRY(153, "������ ���� ������� ��� ��� ����� ����� main - �� ������ int."),
	ERROR_ENTRY(154, "������ ������������ ����� ����� main ��� �������"),	ERROR_ENTRY_NODEF5(155), ERROR_ENTRY_NODEF10(160), ERROR_ENTRY_NODEF10(170),
	ERROR_ENTRY_NODEF10(180), ERROR_ENTRY_NODEF10(190),
	ERROR_ENTRY_NODEF100(200), ERROR_ENTRY_NODEF100(300), ERROR_ENTRY_NODEF100(400), ERROR_ENTRY_NODEF100(500), ERROR_ENTRY(600, "�������� ��������� ���������"),
	ERROR_ENTRY(601, "��������� ��������"),
	ERROR_ENTRY(602, "������ � ���������"),
	ERROR_ENTRY(603, "������ � ���������� ������� ��� �� �������������"),
	ERROR_ENTRY(604, "������ � ���������� ��������� �������"),
	ERROR_ENTRY(605, "������ � �������"), ERROR_ENTRY(606, "������ � �������� ���������"),ERROR_ENTRY(607, "������ � ��������� else"),
	ERROR_ENTRY(608, "������ � �������������� ���������"),
	ERROR_ENTRY(609, "������ � ���������� ������� strcmp"), ERROR_ENTRY(610, "������ � ������� strcmp"),ERROR_ENTRY_NODEF(611),
	ERROR_ENTRY_NODEF5(613), ERROR_ENTRY_NODEF(618), ERROR_ENTRY_NODEF(619),
	ERROR_ENTRY_NODEF10(620), ERROR_ENTRY_NODEF10(630), ERROR_ENTRY_NODEF10(640),
	ERROR_ENTRY_NODEF10(650), ERROR_ENTRY_NODEF10(660), ERROR_ENTRY_NODEF10(670), ERROR_ENTRY_NODEF10(680),
	ERROR_ENTRY_NODEF10(690),
	ERROR_ENTRY(700, "�������� ����������� ������"), ERROR_ENTRY(701, "������ �� ���� �������."),
	ERROR_ENTRY_NODEF5(702), ERROR_ENTRY_NODEF(707), ERROR_ENTRY_NODEF(708), ERROR_ENTRY_NODEF(709),
	ERROR_ENTRY_NODEF50(710),ERROR_ENTRY_NODEF10(760),ERROR_ENTRY_NODEF10(770),ERROR_ENTRY_NODEF10(780),ERROR_ENTRY_NODEF10(790),
	ERROR_ENTRY_NODEF100(800), ERROR_ENTRY_NODEF100(900)
	};
	ERROR geterror(int id, int sourceLine) {

		if (id < 0 || id > ERROR_MAX_ENTRY) {   //���� ��� ��� ��������� ������� ������
			return errors[0];
		}

		else {
			errors[id].sourceLine = sourceLine;
			return errors[id];
		}
	}
	ERROR geterrorin(int id, int line = -1, int col = -1) {

		if (id < 0 || id > ERROR_MAX_ENTRY) {
			return errors[0];
		}

		else {
			errors[id].inext.line = line;
			errors[id].inext.col = col;
			return errors[id];
		}
	}
};