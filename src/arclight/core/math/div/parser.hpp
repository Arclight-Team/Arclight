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


struct BinaryOperator {

	u32 symbol;
	u32 precedence;

};


constexpr static std::array operators = []() {
	return std::array<BinaryOperator, 1>{{'+', 1}};
}();


class Parser {

public:

	enum class TokenMode {
		Operand,
		Operator
	};

	Parser() : expression("") {}
	Parser(const std::string& exp) : expression(exp) {}

	void parse() {

		ArcDebug() << expression;

		TokenMode mode = TokenMode::Operand;
		cursor = 0;

		u32 level = 0;

		while (cursor < expression.size()) {

			if (cursorAtSpace()) {

				cursor++;
				continue;

			}

			auto c = charAtCursor();

			if (c == '(') {

				level++;
				cursor++;

			} else if (c == ')'){

				if (level) {
					level--;
				} else {
					throw MathSyntaxException("Unexpected ')'", expression, cursor);
				}

				cursor++;

			} else {

				switch (mode) {

					case TokenMode::Operand:

						parseConstant();
						mode = TokenMode::Operator;

						break;

					case TokenMode::Operator:

						Log::info("MathExpr", std::string("Operator ") + c);
						mode = TokenMode::Operand;
						cursor++;

						break;

				}

			}

/*
			if (mode == TokenMode::Value && ((c >= '0' && c <= '9') || c == '.' || c == '+' || c == '-')) {

				parseConstant();
				mode = TokenMode::Operator;

			} else {

				switch (c) {

					case ' ':
						cursor++;
						break;

					default:
						throw MathSyntaxException(std::string("Invalid token '") + c + "'", expression, cursor);

				}

			}
*/
		}

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

};