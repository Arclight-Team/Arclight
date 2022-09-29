/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 shaderloader.hpp
 */

#pragma once

#include "common/exception.hpp"
#include "render/gle/shaderprogram.hpp"

#include <stdexcept>



//Exception that indicates that an error in the shader loader occured
class ShaderLoaderException : public ArclightException {

public:
	using ArclightException::ArclightException;
	virtual const char* name() const noexcept override { return "Shader Loader Exception"; }

};


class Path;

namespace ShaderLoader {

	//Loads the shaders from files and creates a shader program
	GLE::ShaderProgram fromFiles(const Path& vsPath, const Path& fsPath);
	GLE::ShaderProgram fromFiles(const Path& vsPath, const Path& gsPath, const Path& fsPath);

	//Loads the shaders from strings and creates a shader program
	GLE::ShaderProgram fromString(const std::string& vs, const std::string& fs);
	GLE::ShaderProgram fromString(const std::string& vs, const std::string& fs, const std::string& gs);

	//Loads the shader from a file and adds it to the given program
	void addShader(GLE::ShaderProgram& shader, const Path& path, GLE::ShaderType type);

}