#include "stdafx.h" 
#include "error.h"
namespace Error
{
	// серии ошибок: 0 - 99 системные ошибки
	//				 100 - 139 - ошибки параметров
	//				 140 - 170 - ошибки при лексическом анализе

	ERROR errors[ERROR_MAX_ENTRY] = //таблица ошибок
	{
	ERROR_ENTRY(0, "Недопустимый код ошибки"), ERROR_ENTRY(1, "Системный сбой"),
	// код ошибки вне диапазона 0 - ERROR_MAX_ENTRY
	ERROR_ENTRY_NODEF(2), ERROR_ENTRY_NODEF(3), ERROR_ENTRY_NODEF(4), ERROR_ENTRY_NODEF(5), ERROR_ENTRY_NODEF(6), ERROR_ENTRY_NODEF(7), ERROR_ENTRY_NODEF(8), ERROR_ENTRY_NODEF(9),
	ERROR_ENTRY_NODEF10(10), ERROR_ENTRY_NODEF10(20), ERROR_ENTRY_NODEF10(30), ERROR_ENTRY_NODEF10(40), ERROR_ENTRY_NODEF10(50), ERROR_ENTRY_NODEF10(60),
	ERROR_ENTRY_NODEF10(70), ERROR_ENTRY_NODEF10(80), ERROR_ENTRY_NODEF10(90),
	ERROR_ENTRY(100, "Параметр -in должен быть задан"),
	ERROR_ENTRY_NODEF(101), ERROR_ENTRY_NODEF(102), ERROR_ENTRY_NODEF(103), ERROR_ENTRY(104, "Превышена длина входного параметра"),
	ERROR_ENTRY_NODEF(105), ERROR_ENTRY_NODEF(106), ERROR_ENTRY_NODEF(107),
	ERROR_ENTRY_NODEF(108), ERROR_ENTRY(109, "Входной файл пустой."),
	ERROR_ENTRY(110, "Ошибка при открытии файла с исходным кодом (-in)"),
	ERROR_ENTRY(111, "Недопустимый символ в исходном файле (-in)"),
	ERROR_ENTRY(112, "Ошибка при создании файла протокола(-log)"),
	ERROR_ENTRY_NODEF(113), ERROR_ENTRY_NODEF(114), ERROR_ENTRY_NODEF(115),
	// параметры
	ERROR_ENTRY(116, "Директории из указанного пути log файла не существует или не указан выходной файл"),
	ERROR_ENTRY(117, "Директории из указанного пути out файла не существует или не указан выходной файл"),
	ERROR_ENTRY(118, "Директории из указанного пути таблицы лексем не существует или указано неверное расширение: оно должно быть .html"),
	ERROR_ENTRY(119, "Указанный путь таблицы идентификаторов не существует или указано неверное расширение: оно должно быть .html"),
	ERROR_ENTRY(120, "Указан неверный размер стека: минимальный размер 1024, максимальный 8192"),
	ERROR_ENTRY(121, "Значения двух или более параметров совпадают"),
	ERROR_ENTRY(122, "Указан неверный выходной файл в параметре -fo или директория не существует"),ERROR_ENTRY_NODEF(123),ERROR_ENTRY_NODEF(124),ERROR_ENTRY_NODEF(125),
	ERROR_ENTRY_NODEF(126), ERROR_ENTRY_NODEF(127), ERROR_ENTRY_NODEF(128), ERROR_ENTRY_NODEF(129), ERROR_ENTRY_NODEF10(130),
	ERROR_ENTRY(140, "Может быть только одна точка входа main"),
	ERROR_ENTRY(141, "Строковые и символьные литералы не могу быть пустыми"),
	ERROR_ENTRY(142, "Идентификатор использован, но не объявлен"),
	ERROR_ENTRY(143, "Идентификатору целочисленного типа присваевается значение строкового"),
	ERROR_ENTRY(144, "Идентификатору строкового типа присваевается значение целочисленного типа"),
	ERROR_ENTRY(145, "Превышен максимальный размер входного текста"),
	ERROR_ENTRY(146, "Переопределение функции"),
	ERROR_ENTRY(147, "Переопределение параметра функции"),
	ERROR_ENTRY(148, "Переопределение переменной"),
	ERROR_ENTRY(149, "Неккоректный литерал"),
	ERROR_ENTRY(150, "Идентификатор не может начинается с цифры"),
	ERROR_ENTRY(151, "Символьный литерал должен иметь один и только один символ"),
	ERROR_ENTRY(152, "Точка входа не была найдена"), ERROR_ENTRY(153, "Нельзя явно указать тип для точки входа main - он всегда int."),
	ERROR_ENTRY(154, "Нельзя использовать точку входа main как функцию"),	ERROR_ENTRY_NODEF5(155), ERROR_ENTRY_NODEF10(160), ERROR_ENTRY_NODEF10(170),
	ERROR_ENTRY_NODEF10(180), ERROR_ENTRY_NODEF10(190),
	ERROR_ENTRY_NODEF100(200), ERROR_ENTRY_NODEF100(300), ERROR_ENTRY_NODEF100(400), ERROR_ENTRY_NODEF100(500), ERROR_ENTRY(600, "Неверная структура программы"),
	ERROR_ENTRY(601, "Ошибочный оператор"),
	ERROR_ENTRY(602, "Ошибка в выражении"),
	ERROR_ENTRY(603, "Ошибка в параметрах функции или ее использовании"),
	ERROR_ENTRY(604, "Ошибка в параметрах вызваемой функции"),
	ERROR_ENTRY(605, "Ошибка в условии"), ERROR_ENTRY(606, "Ошибка в условном операторе"),ERROR_ENTRY(607, "Ошибка в операторе else"),
	ERROR_ENTRY(608, "Ошибка в арифметическом выражении"),
	ERROR_ENTRY(609, "Ошибка в параметрах функции strcmp"), ERROR_ENTRY(610, "Ошибка в функции strcmp"),ERROR_ENTRY_NODEF(611),
	ERROR_ENTRY_NODEF5(613), ERROR_ENTRY_NODEF(618), ERROR_ENTRY_NODEF(619),
	ERROR_ENTRY_NODEF10(620), ERROR_ENTRY_NODEF10(630), ERROR_ENTRY_NODEF10(640),
	ERROR_ENTRY_NODEF10(650), ERROR_ENTRY_NODEF10(660), ERROR_ENTRY_NODEF10(670), ERROR_ENTRY_NODEF10(680),
	ERROR_ENTRY_NODEF10(690),
	ERROR_ENTRY(700, "Неверная закрывающая скобка"), ERROR_ENTRY(701, "Скобка не была закрыта."),
	ERROR_ENTRY_NODEF5(702), ERROR_ENTRY_NODEF(707), ERROR_ENTRY_NODEF(708), ERROR_ENTRY_NODEF(709),
	ERROR_ENTRY_NODEF50(710),ERROR_ENTRY_NODEF10(760),ERROR_ENTRY_NODEF10(770),ERROR_ENTRY_NODEF10(780),ERROR_ENTRY_NODEF10(790),
	ERROR_ENTRY_NODEF100(800), ERROR_ENTRY_NODEF100(900)
	};
	ERROR geterror(int id, int sourceLine) {

		if (id < 0 || id > ERROR_MAX_ENTRY) {   //если код вне диапазона таблицы ошибок
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