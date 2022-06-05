/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 mathexpression.hpp
 */

#pragma once

#include "parser.hpp"



class MathExpression {

public:

	MathExpression(const std::string& expression) {
		Parser(expression).parse();
	}

};