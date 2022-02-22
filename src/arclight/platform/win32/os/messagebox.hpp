/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 messagebox.hpp
 */

#pragma once

#include "types.hpp"

#include <string>



class MessageBox {

public:

	enum class Type {
		Question,
		QuestionCancel,
		Accept,
		AcceptCancel,
		Retry,
		RetryContinue
	};

	enum class Icon {
		Info,
		Warning,
		Error,
		Question
	};

	enum class Result {
		Ok,
		Cancelled,
		Yes,
		No,
		Retry,
		Continue
	};

	MessageBox();

	void setType(Type type);
	void setIcon(Icon icon);
	void setDefaultButton(u32 id);

	void setModality(bool modal);

	void setTitle(const std::string& title);
	void setText(const std::string& text);

	Result show();

	static Result create(const std::string& title, const std::string& text, Type type, Icon icon, u32 defaultButton = 0, bool modal = false);

private:

	std::string title;
	std::string text;
	Type type;
	Icon icon;
	u32 defaultButtonID;
	bool modal;

};