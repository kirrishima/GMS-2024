#include "stdafx.h"
#include "code_gen.h"
#include <filesystem>
#include "console.h"

using namespace std;

namespace CD {
	int CodeGeneration::generateCode()
	{
		if (!OUT_ASM_FILE.is_open() || OUT_ASM_FILE.fail() || OUT_ASM_FILE.bad()) {
			cout << "�� ������� ������� ���� .asm ��� ������\n";
			return 1;
		}

		OUT_ASM_FILE << BASE;
		OUT_ASM_FILE << '\n' << ".stack " << parm->stackSize;
		OUT_ASM_FILE << "\n\n";
		__s_const();
		OUT_ASM_FILE << '\n';
		__s_data();
		OUT_ASM_FILE << "\n\n.code";

		int bracesCount = 0;
		int start = 0;
		int end = LEX_TABLE.size;

		vector<pair<int, int>> functions;

		for (size_t i = 0; i < end; i++) {
			if (LEX_TABLE.table[i].lexema[0] == LEX_LEFTBRACE) {
				bracesCount++;
			}
			else if (LEX_TABLE.table[i].lexema[0] == LEX_BRACELET) {
				if (--bracesCount == 0) {
					functions.push_back({ start, i });
					start = i + 1;
				}
			}
		}

		for (const auto& p : functions) {
			parse_function(p.first, p.second);
		}

		for (const auto& function : __user_functions) {
			OUT_ASM_FILE << '\n';
			for (const auto& cl : function->code) {
				if (isMASMLabel(cl) || isFunctionEnd(cl) || isFunctionStart(cl)) {
					OUT_ASM_FILE << cl << '\n';
					continue;
				}
				OUT_ASM_FILE << tab << cl << '\n';
			}
		}
		OUT_ASM_FILE.close();

		SetConsoleCP(1251);
		SetConsoleOutputCP(1251);

		PersistentConsole console;
		if (!console.initialize()) {
			*log->stream << "�� ������� ���������������� ������� ������������.\n";
			cout << "�� ������� ���������������� ������� ������������.\n";
			return 1;
		}

		string output;

		*log->stream << "\n----    ������ ���������� � ������    ----\n";

		auto executeStep = [&](const  string& stepName, const  string& command) -> bool {
			*log->stream << "\n" << stepName << " ��������� " << command << endl;
			cout << stepName << "...\n";

			if (!console.executeCommand(command, output)) {
				*log->stream << stepName << " �� �������:\n" << output;
				cout << stepName << " �� �������. ��������� ��� ���� ��� ������������.\n";
				return false;
			}

			*log->stream << output;
			output.clear();
			return true;
			};

		if (!executeStep("��������� ����� ������������ Visual Studio",
			R"("C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat")")) {
			return 1;
		}

		string compileCmd = format(COMPILE_COMMAND, utils::wstring_to_string(parm->_objName), utils::wstring_to_string(parm->_masmDest));
		if (!executeStep("����������", compileCmd)) {
			return 1;
		}

		string linkCmd = format(LINK_COMMAND, utils::wstring_to_string(parm->exeName), utils::wstring_to_string(parm->_objName));
		if (!executeStep("��������", linkCmd)) {
			return 1;
		}

		*log->stream << "\n���������� � �������� ��������� �������.\n";
		*log->stream << "�������� ����: " << utils::wstring_to_string(parm->exeName) << '\n';
		cout << "���������� � �������� ��������� �������.\n";
		cout << "�������� ����: " << utils::wstring_to_string(parm->exeName) << '\n';
		return 0;
	}
}
