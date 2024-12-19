#include "stdafx.h"
#include "utils.h"
#include <iomanip>
#include <format>
#include <bitset>
#include <optional>

namespace utils
{
	std::pair<int, int> trimmed_pos(const std::string& str)
	{
		size_t first = str.find_first_not_of(' ');
		if (first == std::string::npos) return std::make_pair<int, int>(-1, -1);

		size_t last = str.find_last_not_of(' ');
		return std::make_pair<int, int>(first, last);
	}

	std::string trim(const std::string& str) {
		size_t start = str.find_first_not_of(" \t\r\n");
		size_t end = str.find_last_not_of(" \t\r\n");
		return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
	}

	std::string wstring_to_string(const std::wstring& wstr) {
		size_t required_size;
		wcstombs_s(&required_size, nullptr, 0, wstr.c_str(), _TRUNCATE);

		std::vector<char> buffer(required_size);

		size_t converted_size;
		wcstombs_s(&converted_size, buffer.data(), buffer.size(), wstr.c_str(), _TRUNCATE);

		std::string str(buffer.data(), converted_size - 1);

		return str;
	}
}

namespace lex_analysis::utils
{
	void printToFile(const IT::ID_Table& ID_Table, const std::wstring& IT_filename,
		const LT::LexTable& LexTable, const std::wstring& LT_filename, const In::IN& in) {
		std::ofstream IT_file(IT_filename);
		std::ofstream LT_file(LT_filename);

		if (!IT_file.is_open()) {
			std::wcerr << L"Не удалось открыть файл \"" << IT_filename << L"\"\n";
			return;
		}

		if (!LT_file.is_open()) {
			std::wcerr << L"Не удалось открыть файл \"" << LT_filename << L"\"\n";
			return;
		}

		IT_file << "<!DOCTYPE html><html><head><title>Identifier Table</title></head><body>\n";
		IT_file << "<table border=\"1\" cellpadding=\"5\" cellspacing=\"0\">\n";
		IT_file << "<tr><th>ID</th><th>Datatype</th><th>ID Type</th><th>Line</th><th>Value</th><th>Scope</th></tr>\n";

		for (int i = 0; i < ID_Table.size; i++) {
			const auto& entry = IT::GetEntry(ID_Table, i);

			IT_file << "<tr><td>" << entry.id << "</td>";

			IT_file << "<td>";
			switch (entry.iddatatype) {
			case IT::INT:  IT_file << "INT"; break;
			case IT::STR:  IT_file << "STR"; break;
			case IT::CHAR: IT_file << "CHAR"; break;
			default:       IT_file << "-"; break;
			}
			IT_file << "</td>";

			IT_file << "<td>";
			switch (entry.idtype) {
			case IT::V: IT_file << "V"; break;
			case IT::L: IT_file << "L"; break;
			case IT::F: IT_file << "F"; break;
			case IT::P: IT_file << "P"; break;
			default:    IT_file << "-"; break;
			}
			IT_file << "</td>";

			IT_file << "<td>" << entry.idxfirstLE << "</td>";

			IT_file << "<td>";
			if (entry.iddatatype == IT::INT) {
				IT_file << entry.value.vint;
			}
			else if (entry.iddatatype == IT::STR || entry.iddatatype == IT::CHAR) {
				IT_file << entry.value.vstr->str;
			}
			else {
				IT_file << "-";
			}
			IT_file << "</td>";

			IT_file << "<td>";
			if (entry.scope != nullptr) {
				std::stack<IT::Entry*> scope_stack;
				auto tmp = entry.scope;
				while (tmp) {
					scope_stack.push(tmp);
					tmp = tmp->scope;
				}

				bool first = true;
				while (!scope_stack.empty()) {
					if (!first) IT_file << '.';
					IT_file << scope_stack.top()->id;
					scope_stack.pop();
					first = false;
				}
			}
			else {
				IT_file << "-";
			}
			IT_file << "</td></tr>\n";
		}

		IT_file << "</table></body></html>\n";

		LT_file << "<!DOCTYPE html><html><head><title>Lexem Table</title></head><body>\n";
		LT_file << "<table border=\"1\" cellpadding=\"5\" cellspacing=\"0\">\n";
		LT_file << "<tr><th>Line</th><th>Lexem</th></tr>\n";

		int currentLine = 1;
		std::string tmp;
		for (int i = 0; i < LexTable.size; i++)
		{
			auto LT_entry = LT::GetEntry(LexTable, i);
			if (currentLine != LT_entry.sn)
			{
				LT_file << "<tr><td>" << (currentLine > 9 ? std::to_string(currentLine) : ("0" + std::to_string(currentLine))) << "</td>";
				LT_file << "<td>" << tmp << "</td></tr>";
				tmp.clear();
				currentLine = LT_entry.sn;
				tmp += LT_entry.lexema[0];
				continue;
			}

			tmp += LT_entry.lexema[0];
		}
		LT_file << "<tr><td>" << (currentLine > 9 ? std::to_string(currentLine) : ("0" + std::to_string(currentLine))) << "</td>";
		LT_file << "<td>" << tmp << "</td></tr>";
		LT_file << "</table></body></html>" << std::endl;
		LT_file.close();
	}


	std::optional<int16_t> parseSignedTwoByteNumber(const std::string& str) {
		using namespace boost::spirit::qi;

		int16_t result = 0;
		auto begin = str.begin();
		auto end = str.end();

		bool parsed = parse(begin, end,
			(lit("0b") >> bin
				| lit("0x") >> hex
				| '0' >> oct
				| int_
				), result);

		if (!parsed || begin != end) {
			throw "Строка не распознана как число заданной системы счисления";
		}

		if (result < std::numeric_limits<int16_t>::min() || result > std::numeric_limits<int16_t>::max()
			|| str.starts_with("0x") && str.length() > 6
			|| str.starts_with("0b") && str.length() > 18) {
			throw "Число выходит за пределы диапазона для signed 16-битного целого числа";
		}

		if (parsed && begin == end) {
			if (result >= std::numeric_limits<int16_t>::min() && result <= std::numeric_limits<int16_t>::max()) {
				return static_cast<int16_t>(result);
			}
		}

		return std::nullopt;
	}

	bool isSingleCharacter(const unsigned char* input, std::size_t endPos) {
		if (endPos < 0 || endPos > 1) {
			return false;
		}

		if (endPos == 0 && input[0] != '\\') {
			return true;
		}

		if (input[0] == '\\' && endPos == 1) {
			unsigned char nextChar = input[1];
			switch (nextChar) {
			case 'n':
			case 't':
			case '\\':
			case '"':
			case '\'':
				return true;
			default:
				return false;
			}
		}

		return false;
	}
}