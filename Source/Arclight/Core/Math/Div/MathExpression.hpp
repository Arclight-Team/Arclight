/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 MathExpression.hpp
 */

#pragma once

#include "Parser.hpp"



class MathExpression {

public:

	MathExpression(const std::string& expression) {
		Parser(expression).parse();
	}

};