/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Application.cpp
 */

#include "App/Application.hpp"
#include "System/Process.hpp"



bool Application::initialize() {
	return true;
}



void Application::finalize() {

}



Path Application::getExecutablePath() {
	return System::Process::getCurrentExecutablePath();
}



Path Application::getExecutableDirectory() {

	try {
		return getExecutablePath().parent();
	} catch (...) {
		return "";
	}

}



Path Application::getCurrentWorkingDirectory() {

	std::error_code ec;
	return Path(std::filesystem::current_path(ec));

}



void Application::setCurrentWorkingDirectory(const Path& path) {

	std::error_code ec;
	std::filesystem::current_path(path.getHandle(), ec);

}