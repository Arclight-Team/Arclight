/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 messagebox.cpp
 */

#include "messagebox.hpp"
#include "util/assert.hpp"
#include "locale/unicode.hpp"

#include <Windows.h>
#undef MessageBox



MessageBox::MessageBox() : type(Type::Accept), icon(Icon::Info), defaultButtonID(0), modal(false) {}



void MessageBox::setType(Type type) {
	this->type = type;
}



void MessageBox::setIcon(Icon icon) {
	this->icon = icon;
}



void MessageBox::setDefaultButton(u32 id) {
	defaultButtonID = id > 2 ? 0 : id;
}



void MessageBox::setModality(bool modal) {
	this->modal = modal;
}



void MessageBox::setTitle(const std::string& title) {
	this->title = title;
}



void MessageBox::setText(const std::string& text) {
	this->text = text;
}



MessageBox::Result MessageBox::show() {

	u32 flags = 0;

	switch (type) {

		case Type::Question:        flags |= MB_YESNO;              break;
		case Type::QuestionCancel:  flags |= MB_YESNOCANCEL;        break;
		case Type::Accept:          flags |= MB_OK;                 break;
		case Type::AcceptCancel:    flags |= MB_OKCANCEL;           break;
		case Type::Retry:           flags |= MB_RETRYCANCEL;        break;
		case Type::RetryContinue:   flags |= MB_CANCELTRYCONTINUE;  break;
		default: arc_force_assert("Illegal message box type");

	}

	switch (icon) {

		case Icon::Info:        flags |= MB_ICONINFORMATION;    break;
		case Icon::Warning:     flags |= MB_ICONWARNING;        break;
		case Icon::Error:       flags |= MB_ICONERROR;          break;
		case Icon::Question:    flags |= MB_ICONQUESTION;       break;
		default: arc_force_assert("Illegal message box icon");

	}

	switch (defaultButtonID) {

		case 0: flags |= MB_DEFBUTTON1; break;
		case 1: flags |= MB_DEFBUTTON2; break;
		case 2: flags |= MB_DEFBUTTON3; break;
		default: arc_force_assert("Illegal message box default button");

	}

	if (modal) {
		flags |= MB_TASKMODAL;
	}

	i32 result = MessageBoxW(nullptr, Unicode::convertString<Unicode::UTF8, Unicode::UTF16>(text).c_str(), Unicode::convertString<Unicode::UTF8, Unicode::UTF16>(title).c_str(), flags);

	switch (result) {

		default:
		case IDABORT:
		case IDCANCEL:
		case IDIGNORE:
			return Result::Cancelled;

		case IDCONTINUE:
			return Result::Continue;

		case IDNO:
			return Result::No;

		case IDOK:
			return Result::Ok;

		case IDRETRY:
		case IDTRYAGAIN:
			return Result::Retry;

		case IDYES:
			return Result::Yes;

	}

}



MessageBox::Result MessageBox::create(const std::string& title, const std::string& text, Type type, Icon icon, u32 defaultButton, bool modal) {

	MessageBox box;
	box.setTitle(title);
	box.setText(text);
	box.setType(type);
	box.setIcon(icon);
	box.setDefaultButton(defaultButton);
	box.setModality(modal);

	return box.show();

}