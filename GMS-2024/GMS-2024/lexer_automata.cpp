#include "stdafx.h"
#include "FST.h"
#include "lexer_automata.h"
#include "regex.h"
#include <memory>
#include <stack>
#include <string>
#include "utils.h"
#include "IT.h"
#include <format>

using namespace REGEX;
using FST::execute;
using lex_analysis::utils::printToFile;

bool stringFlag = false;
bool charFlag = false;
bool _isDeclare = false;

LT::LexTable LexTable = LT::Create(LT_MAXSIZE - 1);
IT::ID_Table ID_Table = IT::Create(TI_MAXSIZE - 1);

using namespace std;

unordered_map<string, bool> long_ids;

char* str = new char[MAX_LEX_SIZE];

FST::FST* IntegerFST(CreateIntegerFST(str));
FST::FST* StringFST(CreateStringFST(str));
FST::FST* CharFST(CreateCharFST(str));

FST::FST* PrintFST(CreatePrintFST(str));
FST::FST* WriteFST(CreateWriteFST(str));
FST::FST* StrcmpFST(CreateStrcmpFST(str));
FST::FST* MainFST(CreateMainFST(str));
FST::FST* ReturnFST(CreateReturnFST(str));

FST::FST* IntDECIMALLiteralFST(CreateIntDECIMALLiteralFST(str));
FST::FST* IntBINARYLiteralFST(CreateIntBINARYLiteralFST(str));
FST::FST* IntOCTALLiteralFST(CreateIntOCTALLiteralFST(str));
FST::FST* IntHEXLiteralFST(CreateIntHEXLiteralFST(str));

FST::FST* IdentifierFST(CreateIdentifierFST(str));

FST::FST* ifFST(CreateIfFST(str));
FST::FST* elseFST(CreateElseFST(str));
char lex_analysis::determineLexeme()
{
	if (execute(*IntegerFST))
	{
		return LEX_INTEGER;
	}

	if (execute(*StringFST))
	{
		stringFlag = true;
		return LEX_STRING;
	}

	if (execute(*IntDECIMALLiteralFST) || execute(*IntHEXLiteralFST) || execute(*IntOCTALLiteralFST) || execute(*IntBINARYLiteralFST))
	{
		return LEX_LITERAL;
	}

	if (execute(*PrintFST))
	{
		return LEX_PRINT;
	}

	if (execute(*WriteFST))
	{
		return LEX_WRITE;
	}

	if (execute(*ifFST))
	{
		return LEX_IF;
	}

	if (execute(*elseFST))
	{
		return LEX_ELSE;
	}

	if (execute(*StrcmpFST))
	{
		return LEX_STRCMP;
	}

	if (execute(*CharFST))
	{
		charFlag = true;
		return LEX_CHAR;
	}

	if (execute(*MainFST))
	{
		return LEX_MAIN;
	}

	if (execute(*ReturnFST))
	{
		return LEX_RETURN;
	}

	if (execute(*IdentifierFST))
	{
		return LEX_ID;
	}

	return '\0';
}

void processLiteral(int& i, unsigned char* text, int& currentLine, char delimiter, int& literalsCount, IT::Entry& IT_entry, LT::Entry& LT_entry, In::IN& in);

std::pair<LT::LexTable, IT::ID_Table> lex_analysis::lexAnalize(Parm::PARM param, In::IN in, Log::LOG& log)
{
	int indexIT;

	int bufferIndex = 0;

	LT::Entry LT_entry;
	LT_entry.sn = 0;
	LT_entry.idxTI = 0;
	LT_entry.lexema[0] = NULL;

	std::stack<IT::Entry*> scope;
	scope.push(NULL);
	int block = 0;
	int openningParentthesis = 0;

	int literalsCount = 0;
	bool addedToITFlag = false;
	bool declareFunctionflag = false;

	IT::Entry IT_entry;
	LexTable.size = 0;
	int currentLine = 1;

	for (int i = 0; i < in.size; i++)
	{
		if (((in.text[i] >= 'A' && in.text[i] <= 'Z') || (in.text[i] >= 'a' && in.text[i] <= 'z') ||
			(in.text[i] >= '0' && in.text[i] <= '9') || in.text[i] == '_') &&
			in.text[i] != ' ')
		{
			str[bufferIndex++] = in.text[i];

			if (bufferIndex >= MAX_LEX_SIZE)
			{
				throw ERROR_THROW(145);
			}
		}
		else
		{
			str[bufferIndex] = '\0';
			LT_entry.lexema[0] = determineLexeme();

			switch (LT_entry.lexema[0])
			{

			case LEX_MAIN:
			{
				if (LexTable.size > 0 && LexTable.table[LexTable.size - 1].lexema[0] == LEX_TYPE)
				{
					throw ERROR_THROW_LINE(153, currentLine);
				}

				if (in.text[i] == LEFTTHESIS)
				{
					throw ERROR_THROW_LINE(154, currentLine);
				}

				memcpy(IT_entry.id, str, ID_SIZE);
				IT_entry.id[ID_SIZE] = '\0';
				IT_entry.iddatatype = IT::INT;
				IT_entry.idtype = IT::F;
				IT_entry.value.vint = NULL;
				IT_entry.idxfirstLE = currentLine;
				IT_entry.scope = NULL;
				indexIT = IT::search(ID_Table, IT_entry);

				if (indexIT != -1)
				{
					throw ERROR_THROW_LINE(140, currentLine);
				}

				LT_entry.idxTI = ID_Table.size;
				IT::Add(ID_Table, IT_entry);
				scope.push(&ID_Table.table[ID_Table.size - 1]);
				break;
			}
			case LEX_LITERAL:
			{
				IT_entry.iddatatype = IT::IDDATATYPE::INT;
				IT_entry.idtype = IT::IDTYPE::L;

				try
				{
					IT_entry.value.vint = utils::parseSignedTwoByteNumber(str).value();
				}
				catch (const std::out_of_range& ex)
				{
					std::cout << "Ошибка в числовом литерале '" << str << "': " << ex.what() << std::endl;
					throw ERROR_THROW_LINE(149, currentLine);
				}
				catch (const std::string& ex)
				{
					std::cout << "Ошибка в числовом литерале '" << str << "'. " << ex << std::endl;
					throw ERROR_THROW_LINE(149, currentLine);
				}
				catch (const char* ex)
				{
					std::cout << "Ошибка в числовом литерале '" << str << "'. " << ex << std::endl;
					throw ERROR_THROW_LINE(149, currentLine);
				}
				catch (const std::exception& ex)
				{
					std::cout << "Ошибка в числовом литерале '" << str << "'" << std::endl;
					std::cout << "Допустимые значения: " << format("[{}; {}]", SHRT_MIN, SHRT_MAX)
						<< ". Допустимые форматы записи: десятичная, двоичная (префикс '0b'), шестнадцатеричная (префикс '0x'), восьмеричная (префикс '0')" << std::endl;
					throw ERROR_THROW_LINE(149, currentLine);
				}

				int index = i - 1;
				while (index > 0 && isdigit(in.text[index]))
					index--;

				if (in.text[index] == '-' && (in.text[index - 1] == EQUAL || in.text[index - 1] == LEFTTHESIS || in.text[index - 1] == COMMA || (LexTable.table[LexTable.size - 1].v == MINUS && LexTable.table[LexTable.size - 2].lexema[0] == LEX_RETURN)))
				{
					IT_entry.value.vint *= -1;
					if (LexTable.table[LexTable.size - 1].lexema[0] == LEX_MATH && LexTable.table[LexTable.size - 1].v == MINUS)
					{
						LexTable.size--;
					}
				}

				int pos = IT::search(ID_Table, IT_entry);
				if (pos >= 0)
				{
					IT_entry = ID_Table.table[pos];
					LT_entry.idxTI = pos;
					break;
				}

				sprintf_s(IT_entry.id, "L%d", literalsCount++);
				IT_entry.id[ID_SIZE] = '\0';

				IT_entry.idxfirstLE = currentLine;
				IT_entry.scope = NULL;

				LT_entry.idxTI = ID_Table.size;
				IT::Add(ID_Table, IT_entry);
				break;
			}
			case LEX_IF:
				LT_entry.lexema[0] = LEX_IF;

				break;
			case LEX_ELSE:
				LT_entry.lexema[0] = LEX_ELSE;
				break;
			case LEX_ID:
			{
				if (isdigit(str[0]))
				{
					throw ERROR_THROW_LINE(150, currentLine);
				}

				if (scope.empty())
					IT_entry.scope = NULL;
				else
					IT_entry.scope = scope.top();

				LT_entry.idxTI = ID_Table.size;
				memcpy(IT_entry.id, str, ID_SIZE);
				IT_entry.id[ID_SIZE] = '\0';

				if (strlen(str) > ID_SIZE)
				{
					if (long_ids.find(string(str)) == long_ids.end())
					{
						long_ids[string(str)] = true;
						std::cout << str << " слишком длинное имя идентификатора - допустимый размер - 5 символов."
							"Оно будет обрезано до "
							<< IT_entry.id << ". Возможен конфликт имен и сопутствущие ошибки." << std::endl;
					}
				}

				IT_entry.iddatatype = IT::INT;
				IT_entry.value.vint = NULL;
				IT_entry.idxfirstLE = currentLine;
				IT_entry.idtype = IT::V;

				if (LexTable.table[LexTable.size - 1].lexema[0] == LEX_TYPE &&
					(LexTable.size == 1 || LexTable.table[LexTable.size - 2].lexema[0] != LEX_COMMA))
				{
					if (LexTable.table[LexTable.size - 1].lexema[0] == LEX_STRING && stringFlag)
					{
						IT_entry.iddatatype = IT::STR;
						strcpy_s(IT_entry.value.vstr->str, "");
						stringFlag = false;
					}
					else if (LexTable.table[LexTable.size - 1].lexema[0] == LEX_CHAR && charFlag)
					{
						IT_entry.iddatatype = IT::CHAR;
						strcpy_s(IT_entry.value.vstr->str, "");
						charFlag = false;
					}
					indexIT = IT::search(ID_Table, IT_entry);
					if (indexIT != -1)
					{
						std::cout << "Ошибка в строке " << currentLine << std::endl;
						throw ERROR_THROW_LINE(148, currentLine);
					}

					LT_entry.idxTI = ID_Table.size;
					IT::Add(ID_Table, IT_entry);
					addedToITFlag = true;
				}
				else if (in.text[i] == LEX_EQUAL)
				{
					if (IT::search(ID_Table, IT_entry) == -1 && LexTable.size > 1 && LexTable.table[LexTable.size - 2].lexema[0] != LEX_TYPE)
					{
						std::cout << "Идентификатор " << IT_entry.id
							<< " использован до объявления в строке " << currentLine << std::endl;
						throw ERROR_THROW_LINE(142, currentLine);
					}
				}
				if (LexTable.size >= 3 && LexTable.table[LexTable.size - 1].lexema[0] == LEX_TYPE &&
					LexTable.table[LexTable.size - 2].lexema[0] == LEX_LEFTTHESIS &&
					LexTable.table[LexTable.size - 3].lexema[0] == LEX_ID && ID_Table.table[ID_Table.size - 2].idtype == IT::F)
				{
					ID_Table.table[ID_Table.size - 1].idtype = IT::P;
				}

				if (LexTable.size >= 3 && LexTable.table[LexTable.size - 2].lexema[0] == LEX_COMMA && ID_Table.table[LexTable.table[LexTable.size - 3].idxTI].idtype == IT::P)
				{
					IT_entry.idtype = IT::P;

					if (LexTable.table[LexTable.size - 1].lexema[0] == LEX_STRING && stringFlag)
					{
						IT_entry.iddatatype = IT::STR;
						strcpy_s(IT_entry.value.vstr->str, "");
						stringFlag = false;
					}
					else if (LexTable.table[LexTable.size - 1].lexema[0] == LEX_CHAR && charFlag)
					{
						IT_entry.iddatatype = IT::CHAR;
						strcpy_s(IT_entry.value.vstr->str, "");
						charFlag = false;
					}

					indexIT = IT::search(ID_Table, IT_entry);
					if (indexIT != -1)
					{
						throw ERROR_THROW_LINE(147, currentLine);
					}

					IT::Add(ID_Table, IT_entry);
					addedToITFlag = true;
				}

				if (!addedToITFlag)
				{
					indexIT = IT::search(ID_Table, IT_entry);
					if (indexIT >= 0)
					{
						LT_entry.idxTI = indexIT;
					}
					else
					{
						throw ERROR_THROW_LINE(142, currentLine);
					}
				}

				std::memset(IT_entry.id, NULL, ID_SIZE);

				IT_entry.iddatatype = IT::INT;
				IT_entry.value.vint = NULL;
				addedToITFlag = false;

				break;
			}
			default:
				break;
			}

			bufferIndex = 0;
			std::memset(str, NULL, 50);
		}

		if (LT_entry.lexema[0] != NULL)
		{
			LT_entry.sn = currentLine;
			LT::Add(LexTable, LT_entry);
			LT_entry.lexema[0] = NULL;
		}

		switch (in.text[i])
		{
		case SEMICOLON:
			LT_entry.lexema[0] = LEX_SEMICOLON;
			goto add_LT_entry;
			break;
		case COMMA:
			LT_entry.lexema[0] = LEX_COMMA;
			goto add_LT_entry;
			break;
		case PLUS:
			LT_entry.lexema[0] = LEX_PLUS;
			goto add_LT_entry;
			break;
		case MINUS:
			if (i > 0 && in.text[i - 1] == EQUAL)
			{
				break;
			}
			LT_entry.lexema[0] = LEX_MINUS;
			goto add_LT_entry;
			break;
		case STAR:
			LT_entry.lexema[0] = LEX_STAR;
			goto add_LT_entry;
			break;
		case DIRSLASH:
			LT_entry.lexema[0] = LEX_DIRSLASH;
			goto add_LT_entry;
			break;
		case '<':
		case '>':
			if (i + 1 < in.size && in.text[i + 1] != '=')
			{
				LT_entry.lexema[0] = LEX_COMPARE_LESS;
				LT_entry.sn = currentLine;
				LT_entry.c = in.text[i];
				LT::Add(LexTable, LT_entry);
				LT_entry.lexema[0] = NULL;
			}

			break;
		case EQUAL:
			if (i + 1 < in.size && in.text[i + 1] == '=')
			{
				LT_entry.lexema[0] = LEX_COMPARE_LESS;
				LT_entry.c = "==";
				LT_entry.sn = currentLine;
				LT::Add(LexTable, LT_entry);
				LT_entry.lexema[0] = NULL;
				i += 1;
				break;
			}
			if (in.text[i - 1] == '>' || in.text[i - 1] == '<' || in.text[i - 1] == '!')
			{
				LT_entry.lexema[0] = LEX_COMPARE_LESS;
				LT_entry.c = "01";
				LT_entry.c[1] = '=';
				LT_entry.c[0] = in.text[i - 1];
				LT_entry.sn = currentLine;
				LT::Add(LexTable, LT_entry);
				LT_entry.lexema[0] = NULL;
				break;
			}
			LT_entry.lexema[0] = LEX_EQUAL;

		add_LT_entry:
			LT_entry.sn = currentLine;
			LT_entry.v = in.text[i];
			LT::Add(LexTable, LT_entry);
			LT_entry.lexema[0] = NULL;
			break;

		case DOUBLE_QUOTES:
			processLiteral(i, in.text, currentLine, DOUBLE_QUOTES, literalsCount, IT_entry, LT_entry, in);
			break;

		case MARK:
			processLiteral(i, in.text, currentLine, MARK, literalsCount, IT_entry, LT_entry, in);
			break;

		case NEW_LINE:
		{
			LT_entry.sn = currentLine++;
			LT_entry.lexema[0] = NULL;
			break;
		}

		case LEFT_BRACE:
		{
			if (!scope.empty() && openningParentthesis != 0)
			{
				IT::Entry* code_of_block = new IT::Entry();
				code_of_block->scope = NULL;
				code_of_block->scope = scope.top();
				scope.push(code_of_block);
				sprintf_s(code_of_block->id, "IF%d", block++);
			}

			openningParentthesis++;

			LT_entry.lexema[0] = LEX_LEFTBRACE;
			LT_entry.sn = currentLine;
			LT::Add(LexTable, LT_entry);
			LT_entry.lexema[0] = NULL;

			break;
		}

		case RIGHT_BRACE:
		{
			openningParentthesis--;
			LT_entry.lexema[0] = RIGHT_BRACE;
			LT_entry.sn = currentLine;
			LT::Add(LexTable, LT_entry);
			LT_entry.lexema[0] = NULL;

			if (!scope.empty())
				scope.pop();

			break;
		}

		case LEFTTHESIS:
		{
			LT_entry.lexema[0] = LEX_LEFTTHESIS;
			LT_entry.sn = currentLine;
			LT::Add(LexTable, LT_entry);
			LT_entry.lexema[0] = NULL;

			if (LexTable.table[LexTable.size - 2].lexema[0] == LEX_ID && LexTable.table[LexTable.size - 3].lexema[0] == LEX_TYPE)
			{
				ID_Table.table[LexTable.table[LexTable.size - 2].idxTI].idtype = IT::F;
				scope.push(&ID_Table.table[LexTable.table[LexTable.size - 2].idxTI]);
			}
			break;
		}
		case RIGHTTHESIS:
		{
			LT_entry.lexema[0] = LEX_RIGHTTHESIS;
			LT_entry.sn = currentLine;
			LT::Add(LexTable, LT_entry);
			LT_entry.lexema[0] = NULL;

			if (!scope.empty() && declareFunctionflag)
			{
				scope.pop();
				declareFunctionflag = false;
			}
			break;
		}
		}
	}

	try
	{
		if (param.enableLexAnSave)
		{
			printToFile(ID_Table, param.it, LexTable, param.lt, in);
		}
	}

	catch (...)
	{
		cout << "Внимание: не удалось сохранить таблицу лексем и/или идентификаторов.";
	}

	delete[] str;

	delete IntegerFST;
	delete StringFST;
	delete CharFST;
	delete PrintFST;
	delete MainFST;
	delete ReturnFST;
	delete IntDECIMALLiteralFST;
	delete IntBINARYLiteralFST;
	delete IntOCTALLiteralFST;
	delete IntHEXLiteralFST;
	delete IdentifierFST;
	delete ifFST;
	delete elseFST;

	long_ids.clear();

	if (!hasMainLexem(LexTable))
	{
		throw ERROR_THROW(152);
	}

	return std::make_pair(LexTable, ID_Table);
}

void processLiteral(int& i, unsigned char* text, int& currentLine, char delimiter, int& literalsCount, IT::Entry& IT_entry, LT::Entry& LT_entry, In::IN& in)
{
	int index = i + 1;

	while (index < MAX_LEX_SIZE - 1 && in.size > index &&
		(text[index] != delimiter || (text[index - 1] == '\\' && text[index - 2] != '\\')))
	{
		index++;
	}

	if (text[index] != delimiter)
	{
		throw ERROR_THROW_LINE(151, currentLine);
	}
	if (index - i - 1 > 255)
	{
		std::cout << "Значение литерала превышает допустим (1 символ для char и 255 для str)\n";
		throw ERROR_THROW_LINE(149, currentLine);
	}
	strncpy(IT_entry.value.vstr->str, reinterpret_cast<const char*>(text + i + 1), index - i - 1);
	IT_entry.value.vstr->str[index - i - 1] = '\0';
	IT_entry.value.vstr->len = strlen(IT_entry.value.vstr->str);

	if (IT_entry.value.vstr->len <= 0)
	{
		std::cout << "Пустой литерал в строке " << currentLine << std::endl;
		throw ERROR_THROW_LINE(141, currentLine);
	}

	sprintf_s(IT_entry.id, "L%d", literalsCount++);
	IT_entry.iddatatype = (delimiter == DOUBLE_QUOTES) ? IT::STR : IT::CHAR;
	IT_entry.idtype = IT::L;

	int pos = IT::search(ID_Table, IT_entry);
	if (pos != -1)
	{
		LT_entry.idxTI = pos;
	}
	else
	{
		IT::Add(ID_Table, IT_entry);
		LT_entry.idxTI = ID_Table.size - 1;
	}

	LT_entry.lexema[0] = LEX_LITERAL;
	LT_entry.sn = currentLine;
	LT::Add(LexTable, LT_entry);

	i = index;
}
