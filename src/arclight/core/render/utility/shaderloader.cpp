/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 shaderloader.cpp
 */

#include "shaderloader.hpp"
#include "filesystem/file.hpp"
#include "filesystem/path.hpp"



GLE::ShaderProgram ShaderLoader::fromFiles(const Path& vsPath, const Path& fsPath) {

	File vsFile(vsPath, File::In | File::Text);

	if (!vsFile.isOpen()) {
		throw ShaderLoaderException(std::string("Failed to open vertex shader file ") + vsPath.toString().c_str());
	}

	const std::string vs = vsFile.readAllText();
	vsFile.close();

	File fsFile(fsPath, File::In | File::Text);

	if (!fsFile.isOpen()) {
		throw ShaderLoaderException(std::string("Failed to open fragment shader file ") + fsPath.toString().c_str());
	}

	const std::string fs = fsFile.readAllText();
	fsFile.close();

	return fromString(vs, fs);

}



GLE::ShaderProgram ShaderLoader::fromFiles(const Path& vsPath, const Path& gsPath, const Path& fsPath) {

	File vsFile(vsPath, File::In | File::Text);

	if (!vsFile.isOpen()) {
		throw ShaderLoaderException(std::string("Failed to open vertex shader file ") + vsPath.toString().c_str());
	}

	const std::string vs = vsFile.readAllText();
	vsFile.close();

	File fsFile(fsPath, File::In | File::Text);

	if (!fsFile.isOpen()) {
		throw ShaderLoaderException(std::string("Failed to open fragment shader file ") + fsPath.toString().c_str());
	}

	const std::string fs = fsFile.readAllText();
	fsFile.close();

	File gsFile(gsPath, File::In | File::Text);

	if (!gsFile.isOpen()) {
		throw ShaderLoaderException(std::string("Failed to open geometry shader file ") + gsPath.toString().c_str());
	}

	const std::string gs = gsFile.readAllText();
	gsFile.close();

	return fromString(vs, fs, gs);

}



GLE::ShaderProgram ShaderLoader::fromString(const std::string& vs, const std::string& fs) {

	GLE::ShaderProgram program;
	program.create();

	if (!program.addShader(vs.c_str(), static_cast<u32>(vs.length()), GLE::ShaderType::VertexShader)) {
		throw ShaderLoaderException(std::string("Compilation of vertex shader failed"));
	}

	if (!program.addShader(fs.c_str(), static_cast<u32>(fs.length()), GLE::ShaderType::FragmentShader)) {
		throw ShaderLoaderException(std::string("Compilation of fragment shader failed"));
	}

	if (!program.link()) {
		throw ShaderLoaderException(std::string("Failed to link shaders"));
	}

	return program;

}



GLE::ShaderProgram ShaderLoader::fromString(const std::string& vs, const std::string& fs, const std::string& gs) {

	GLE::ShaderProgram program;
	program.create();

	if (!program.addShader(vs.c_str(), static_cast<u32>(vs.length()), GLE::ShaderType::VertexShader)) {
		throw ShaderLoaderException(std::string("Compilation of vertex shader failed"));
	}

	if (!program.addShader(fs.c_str(), static_cast<u32>(fs.length()), GLE::ShaderType::FragmentShader)) {
		throw ShaderLoaderException(std::string("Compilation of fragment shader failed"));
	}

	if (!program.addShader(gs.c_str(), static_cast<u32>(gs.length()), GLE::ShaderType::GeometryShader)) {
		throw ShaderLoaderException(std::string("Compilation of geometry shader failed"));
	}

	if (!program.link()) {
		throw ShaderLoaderException(std::string("Failed to link shaders"));
	}

	return program;

}



void ShaderLoader::addShader(GLE::ShaderProgram& program, const Path& path, GLE::ShaderType type) {

	File file(path, File::In | File::Text);

	if (!file.isOpen()) {
		throw ShaderLoaderException(std::string("Failed to open shader file ") + path.toString().c_str());
	}

	const std::string s = file.readAllText();
	file.close();

	if (!program.isCreated()) {
		throw ShaderLoaderException(std::string("Cannot attach a shader to a non-created program"));
	}

	if (!program.addShader(s.c_str(), s.size(), type)) {
		throw ShaderLoaderException(std::string("Compilation of shader ") + path.toString().c_str() + " failed");
	}

}