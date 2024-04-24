/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Console.cpp
 */

#include "Console.hpp"

#include "Util/Range.hpp"
#include "Common/Win32.hpp"


using namespace OS;


void Console::initialize() {

	AllocConsole();
	stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
	stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

}



void Console::destroy() {

	if (stdoutHandle.valid() || stdinHandle.valid()) {

		FreeConsole();
		stdinHandle.close();
		stdoutHandle.close();

	}

}



bool Console::exists() {

	if (stdoutHandle.valid()) {
		return true;
	}

	return GetConsoleWindow();

}



std::string Console::getTitle() {

	if (!stdoutHandle.valid()) {
		return "";
	}

	u32 length = 0x100;
	std::wstring title(length, L'\0');

	while (length = GetConsoleTitleW(title.data(), title.size()), length == 0) {

		if (title.size() < 0x8000) {
			title.resize(title.size() * 2);
		} else {
			return "";
		}

	}

	title.resize(length);

	return OS::String::toUTF8(title);

}



bool Console::isCursorVisible() {

	if (!stdoutHandle.valid()) {
		return false;
	}

	CONSOLE_CURSOR_INFO info;

	if (!GetConsoleCursorInfo(stdoutHandle.handle, &info)) {
		return false;
	}

	return info.bVisible;

}



Vec2i Console::getCursorPosition() {

	if (!stdoutHandle.valid()) {
		return {};
	}

	CONSOLE_SCREEN_BUFFER_INFO info;

	if (!GetConsoleScreenBufferInfo(stdoutHandle.handle, &info)) {
		return {};
	}

	return { info.dwCursorPosition.X, info.dwCursorPosition.Y };

}



Vec2i Console::getBufferSize() {

	if (!stdoutHandle.valid()) {
		return {};
	}

	CONSOLE_SCREEN_BUFFER_INFO info;

	if (!GetConsoleScreenBufferInfo(stdoutHandle.handle, &info)) {
		return {};
	}

	return { info.dwSize.X, info.dwSize.Y };

}



Vec2i Console::getWindowSize() {

	if (!stdoutHandle.valid()) {
		return {};
	}

	CONSOLE_SCREEN_BUFFER_INFO info;

	if (!GetConsoleScreenBufferInfo(stdoutHandle.handle, &info)) {
		return {};
	}

	return { info.srWindow.Right - info.srWindow.Left + 1, info.srWindow.Bottom - info.srWindow.Top + 1 };

}



Console::Flags Console::getFlags() {

	if (!stdoutHandle.valid() || !stdinHandle.valid()) {
		return {};
	}

	DWORD inMode;
	DWORD outMode;
	Flags flags = Flags::None;

	GetConsoleMode(stdinHandle, &inMode);
	GetConsoleMode(stdoutHandle, &outMode);

	if (inMode & ENABLE_INSERT_MODE) {
		flags |= Flags::Insert;
	}

	if (inMode & ENABLE_LINE_INPUT) {
		flags |= Flags::LineInput;
	}

	if (inMode & ENABLE_PROCESSED_INPUT) {
		flags |= Flags::ProcessInput;
	}

	if (inMode & ENABLE_QUICK_EDIT_MODE && inMode & ENABLE_EXTENDED_FLAGS) {
		flags |= Flags::Edit;
	}

	if (outMode & ENABLE_PROCESSED_OUTPUT) {
		flags |= Flags::ProcessOutput;
	}

	if (outMode & ENABLE_WRAP_AT_EOL_OUTPUT) {
		flags |= Flags::Wrap;
	}

	return flags;

}



void Console::setTitle(const std::string& title) {
	SetConsoleTitleW(OS::String::toUTF16(title).data());
}



void Console::setCursorVisibility(bool visible) {

	if (!stdoutHandle.valid()) {
		return;
	}

	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = visible;

	SetConsoleCursorInfo(stdoutHandle.handle, &info);

}



void Console::setCursorPosition(u32 x, u32 y) {

	if (!stdoutHandle.valid()) {
		return;
	}

	COORD c;
	c.X = x;
	c.Y = y;

	SetConsoleCursorPosition(stdoutHandle.handle, c);

}



void Console::setBufferSize(u32 width, u32 height) {

	if (!stdoutHandle.valid()) {
		return;
	}

	COORD c;
	c.X = width;
	c.Y = height;

	SetConsoleScreenBufferSize(stdoutHandle.handle, c);

}



void Console::setWindowSize(u32 width, u32 height) {

	if (!stdoutHandle.valid()) {
		return;
	}

	SMALL_RECT rect;
	rect.Left = 0;
	rect.Top = 0;
	rect.Right = width - 1;
	rect.Bottom = height - 1;

	SetConsoleWindowInfo(stdoutHandle.handle, true, &rect);

}



void Console::setFlags(Flags flags) {

	if (!stdoutHandle.valid() || !stdinHandle.valid()) {
		return;
	}

	DWORD inMode = 0;
	DWORD outMode = 0;

	if (static_cast<bool>(flags & Flags::Insert)) {
		inMode |= ENABLE_INSERT_MODE;
	}

	if (static_cast<bool>(flags & Flags::LineInput)) {
		inMode |= ENABLE_LINE_INPUT;
	}

	if (static_cast<bool>(flags & Flags::ProcessInput)) {
		inMode |= ENABLE_PROCESSED_INPUT;
	}

	if (static_cast<bool>(flags & Flags::Edit)) {
		inMode |= ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS;
	} else {
		inMode |= ENABLE_EXTENDED_FLAGS;
	}

	if (static_cast<bool>(flags & Flags::ProcessOutput)) {
		outMode |= ENABLE_PROCESSED_OUTPUT;
	}

	if (static_cast<bool>(flags & Flags::Wrap)) {
		outMode |= ENABLE_WRAP_AT_EOL_OUTPUT;
	}

	SetConsoleMode(stdinHandle, inMode);
	SetConsoleMode(stdoutHandle, outMode);

}



void Console::clear() {
	fill(' ');
}



void Console::fill(char16_t c) {

	if (!stdoutHandle.valid()) {
		return;
	}

	DWORD written = 0;
	FillConsoleOutputCharacterW(stdoutHandle.handle, c, -1, COORD { 0, 0 }, &written);

}



std::string Console::read(u32 count) {

	if (!stdinHandle.valid()) {
		return "";
	}

	DWORD readCount = 0;
	std::wstring buffer(count, '\0');

	if (!ReadConsoleW(stdinHandle.handle, buffer.data(), count, &readCount, nullptr)) {
		return "";
	}

	buffer.resize(readCount);
	return OS::String::toUTF8(buffer);

}



SizeT Console::write(const std::string& text) {

	if (!stdoutHandle.valid()) {
		return 0;
	}

	std::wstring wtext = OS::String::toUTF16(text);
	DWORD written = 0;

	if (!WriteConsoleW(stdoutHandle.handle, wtext.data(), wtext.size(), &written, nullptr)) {
		return 0;
	}

	return written;

}



bool Console::write(const std::string& text, const Vec2i& pos, const Vec2i& size) {

	if (!stdoutHandle.valid()) {
		return false;
	}

	std::wstring wtext = OS::String::toUTF16(text);
	std::vector<CHAR_INFO> buffer(wtext.size());

	for (SizeT i : Range(wtext.size())) {

		buffer[i].Char.UnicodeChar = wtext[i];
		buffer[i].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;

	}

	COORD bufferSize;
	bufferSize.X = size.x;
	bufferSize.Y = size.y;

	SMALL_RECT dest;
	dest.Left = pos.x;
	dest.Top = pos.y;
	dest.Right = pos.x + size.x;
	dest.Bottom = pos.y + size.y;

	return WriteConsoleOutputW(stdoutHandle.handle, buffer.data(), bufferSize, COORD { 0, 0 }, &dest);

}