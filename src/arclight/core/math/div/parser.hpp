/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 parser.hpp
 */

#pragma once

#include "debug.hpp"

#include <array>
#include <string_view>



class MathSyntaxException : public std::runtime_error {

public:

	explicit MathSyntaxException(const std::string& cause, std::string_view expr, SizeT pos) noexcept : std::runtime_error("Syntax error: " + cause), expression(expr), position(pos) {}

	constexpr std::string_view getExpression() const noexcept {
		return expression;
	}

	constexpr SizeT getErrorPosition() const noexcept {
		return position;
	}

private:

	std::string_view expression;
	SizeT position;

};


enum class TokenType {

	Constant,
	Operator,
	LeftBracket,
	RightBracket,
	End

};


struct Token {

	constexpr Token() noexcept : Token("", TokenType::End) {}
	constexpr Token(std::string_view str, TokenType type) : str(str), type(type) {}

	std::string_view str;
	TokenType type;

};


class Parser {

	enum class State {
		Expression
	};

public:

	Parser() : Parser("") {}
	explicit Parser(const std::string& exp) : expression(exp), cursor(0), tokenStart(0) {}

	void parse() {

		ArcDebug() << expression;

		u32 level = 0;
		cursor = 0;
		tokenStart = 0;

		Token token = tokenize();
		ArcDebug() << "Token" << token.str;

		while (token.type != TokenType::End) {

			token = tokenize();
			ArcDebug() << "Token" << token.str;

		}

	}



	Token tokenize() {

		tokenStart = cursor;
		TokenType type = scanNextToken();

		return { std::string_view(expression.begin() + tokenStart, expression.begin() + cursor), type };

	}


	TokenType scanNextToken() {

		TokenType type;

		while (cursor < expression.size()) {

			char c = charAtCursor();
			cursor++;

			switch (c) {

				case '(':   return TokenType::LeftBracket;
				case ')':   return TokenType::RightBracket;

				case ' ':
					tokenStart++;
					break;

				default:
					throw MathSyntaxException(std::string("Illegal '") + c + "'", expression, cursor - 1);

			}

		}

		return type;

	}


	void parseConstant() {

		bool hasDecimal = false;
		bool hasDigits = false;
		bool negative = false;
		u32 x = 0;
		u32 y = 0;

		auto c = charAtCursor();

		if (c == '-') {

			negative = true;
			cursor++;

		} else if (c == '+') {

			cursor++;

		}

		for (cursor; cursor < expression.size(); cursor++) {

			c = charAtCursor();

			if (c >= '0' && c <= '9') {

				u32& v = hasDecimal ? y : x;
				v *= 10;
				v += c - '0';

				hasDigits = true;

			} else if (c == '.') {

				if (hasDecimal) {
					throw MathSyntaxException("Double decimal point in number", expression, cursor);
				}

				hasDecimal = true;

			} else {

				break;

			}
			
		}

		if (!hasDigits) {
			throw MathSyntaxException("Bad decimal point in expression", expression, cursor);
		}

		if (!hasDecimal || (hasDecimal && !y)) {
			Log::info("MathExpr", "Constant [Int]   %c%d", negative ? '-' : '+', x);
		} else {
			Log::info("MathExpr", "Constant [Float] %c%d.%d", negative ? '-' : '+', x, y);
		}

	}


	constexpr bool cursorAtSpace() {
		return charAtCursor() == ' ';
	}


	constexpr char charAtCursor() const {
		return expression[cursor];
	}


private:

	std::string_view expression;
	SizeT cursor;
	SizeT tokenStart;

};