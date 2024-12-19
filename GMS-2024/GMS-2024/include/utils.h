#pragma once
#include <string>
#include <IT.h>
#include <LT.h>
#include <optional>

namespace utils
{
	std::pair<int, int> trimmed_pos(const std::string& str);
	std::string trim(const std::string& str);
	std::string wstring_to_string(const std::wstring& wstr);
}

namespace lex_analysis::utils
{
	void printToFile(const IT::ID_Table& ID_Table, const std::wstring& IT_filename,
		const LT::LexTable& LexTable, const std::wstring& LT_filename, const In::IN& in);

	int stringToNumber(const std::string& str);
	std::optional<int16_t> parseSignedTwoByteNumber(const std::string& str);
	bool isSingleCharacter(const unsigned char* input, std::size_t endPos);
}