#include "stdafx.h"
#include "error.h"
#include "in.h"
#include "utils.h"

namespace In
{
	IN getin(std::wstring infile)
	{
		IN in;

		in.size = 0;
		in.lines = 0;
		in.ignore = 0;

		std::ifstream file(infile);
		if (!file.is_open())
		{
			throw ERROR_THROW(110);
		}

		in.text = new unsigned char[IN_MAX_LEN_TEXT];

		std::string tmp;
		int trimmedLength = 0;

		while (std::getline(file, tmp))
		{
			bool insideString = false;

			std::pair<int, int> p = utils::trimmed_pos(tmp);

			if (p.first == -1 || p.second == -1)
			{
				in.lines++;
				in.text[in.size++] = IN_CODE_REPLACE_ENDL;
				in.ignore += tmp.length();
				continue;
			}

			in.ignore += p.first + tmp.length() - p.second - 1;

			for (int position = p.first; position <= p.second; position++)
			{
				auto ch = static_cast<unsigned char>(tmp[position]);
				switch (in.code[ch])
				{
				case IN::T:
					in.text[in.size++] = ch;
					break;

				case IN::F:
					in.text[in.size] = '\0';
					throw ERROR_THROW_IN(111, in.lines, position + 1, in.text);

				case IN::I:
					in.ignore++;
					break;

				case IN::Space:
					if (!insideString && in.size > 0 && in.text[in.size - 1] == SPACE)
					{
						in.ignore++;
						break;
					}
					in.text[in.size++] = ch;
					break;

				case IN::DoubleQuotes:
				case IN::Mark:
				{
					in.text[in.size++] = ch;
					insideString = !insideString;
					break;
				}
				case IN::Slash:
				{
					if (!insideString && position < p.second && tmp[position + 1] == '/')
					{
						in.ignore += p.second - position;
						position = p.second + 1;
						break;
					}
					in.text[in.size++] = ch;
					break;
				}
				case IN::Asterisk:
				case IN::Equal:
				case IN::LeftBrace:
				case IN::LeftParen:
				case IN::Minus:
				case IN::Plus:
				case IN::RightBrace:
				case IN::RightParen:
				case IN::Semicolon:
				case IN::Comma:
				case IN::Compare:
				{
					int tmpPos = position;

					if (!insideString && position + 1 <= p.second && tmp[position + 1] == SPACE)
					{
						while (position + 1 <= p.second && tmp[position + 1] == SPACE) {
							position++;
							in.ignore++;
						}
					}
					if (!insideString && in.size > 0 && in.text[in.size - 1] == SPACE)
					{
						in.text[in.size - 1] = ch;
						in.ignore++;
					}
					else
					{
						in.text[in.size++] = ch;
					}

					break;
				}
				default:
					in.text[in.size] = in.code[tmp[position]];
					in.size++;
				}
			}
			in.lines++;
			in.text[in.size++] = IN_CODE_REPLACE_ENDL;
		}
		in.text[--in.size] = '\0';
		return in;
	}
}
