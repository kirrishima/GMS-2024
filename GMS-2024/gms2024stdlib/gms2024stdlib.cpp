#include "pch.h"
#include "framework.h"

extern "C" {
	void __stdcall __StreamWriteStr(char* arr)
	{
		int n = strlen(arr);
		for (int i = 0; i < n; ++i) {
			if (arr[i] == '\\' && i + 1 < n) {
				char nextChar = arr[++i];
				switch (nextChar) {
				case 'n': std::cout << '\n'; break;
				case 't': std::cout << '\t'; break;
				case '\\': std::cout << '\\'; break;
				case '"': std::cout << '"'; break;
				case '\'': std::cout << '\''; break;
				default:
					std::cout << '\\' << nextChar;
					break;
				}
			}
			else {
				std::cout << arr[i];
			}
		}
	}

	void __stdcall __NullDevisionException() {
		SetConsoleOutputCP(1251);
		std::cout << "Ошибка: попытка деления на ноль. Программа аварийно завершена.";
		ExitProcess(1);
	}

	void __stdcall __StreamWriteChar(char* arr)
	{
		std::cout << arr[0];
	}

	void __stdcall __StreamWriteNumber(short n)
	{
		std::cout << n;
	}

	void __stdcall __StreamWriteBool(bool b)
	{
		std::cout << (b ? "True" : "False");
	}

	void __stdcall __PrintNumber(short n)
	{
		std::cout << "Number: ";
		__StreamWriteNumber(n);
		std::cout << std::endl;
	}

	void __stdcall __PrintBool(bool b)
	{
		std::cout << "Condition: ";
		__StreamWriteBool(b);
		std::cout << std::endl;
	}

	void __stdcall __PrintStr(char* arr)
	{
		SetConsoleOutputCP(1251);
		std::cout << "Str: ";
		__StreamWriteStr(arr);
		std::cout << std::endl;
	}

	void __stdcall __PrintChar(char* arr)
	{
		SetConsoleOutputCP(1251);
		std::cout << "Char: ";
		__StreamWriteChar(arr);
		std::cout << "\n";
	}

	int __stdcall __StrCmp(const char* _str1, const char* _str2)
	{
		int result = strcmp(_str1, _str2);
		return (result < 0) ? -1 : (result > 0) ? 1 : 0;
	}
}
