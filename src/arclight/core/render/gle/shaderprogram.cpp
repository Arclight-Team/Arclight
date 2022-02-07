/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 shaderprogram.cpp
 */

#include "shaderprogram.hpp"

#include "glecore.hpp"
#include GLE_HEADER


GLE_BEGIN


bool ShaderProgram::create() {

	if (!isCreated()) {

		id = glCreateProgram();

		if (!id) {

			id = invalidID;
			return false;

		}

	}

	return true;

}



void ShaderProgram::destroy() {

	if (isCreated()) {

		if (isActive()) {
			activeProgramID = invalidBoundID;
		}

		destroyShaders();

		glDeleteProgram(id);
		id = invalidID;
		linked = false;

	}

}



bool ShaderProgram::addShader(const char* source, u32 size, ShaderType type) {

	gle_assert(isCreated(), "Attempted to attach shader without creating a program first");
	gle_assert(!isLinked(), "Attempted to add shader to linked program");
	gle_assert(shaders[static_cast<u32>(type)] == invalidID, "Cannot attach duplicate shader types to the program");

	if ((type == ShaderType::TessCtrlShader || type == ShaderType::TessEvalShader) && !tesselationShadersSupported()) {
		GLE::warn("Tesselation shaders are not supported");
		return false;
	}

	if (type == ShaderType::ComputeShader && !computeShadersSupported()) {
		GLE::warn("Compute shaders are not supported");
		return false;
	}

	u32 shaderEnum = getShaderTypeEnum(type);
	u32 sid = glCreateShader(shaderEnum);

	i32 sourceSize = size;

	glShaderSource(sid, 1, &source, &sourceSize);
	glCompileShader(sid);

	bool success = checkCompilation(sid);

	if (!success) {
		glDeleteShader(sid);
		return false;
	}

	glAttachShader(id, sid);
	shaders[static_cast<u32>(type)] = sid;

	return true;

}



bool ShaderProgram::link() {

	gle_assert(isCreated(), "Attempted to link shader program without creating it");
	gle_assert(!isLinked(), "Attempted to link shader program that has been linked before");

	glLinkProgram(id);

	checkLinking();
	destroyShaders();

	return linked;

}



bool ShaderProgram::loadBinary(void* binary, u32 size) {

	gle_assert(isCreated(), "Attempted to load shader program binary without creating it");
	gle_assert(!isLinked(), "Attempted to load shader program binary into previously linked program");
	gle_assert(size >= 4, "Shader binary has invalid size");

	if (shaderBinariesSupported()) {

		glProgramBinary(id, *static_cast<u32*>(binary), static_cast<u8*>(binary) + 4, size - 4);
		checkLinking();

	} else {
		GLE::warn("Shader binaries are not supported");
	}
	
	return linked;

}



std::vector<u8> ShaderProgram::saveBinary() {

	gle_assert(isLinked(), "Attempted to save shader program binary without linking it");
	
	std::vector<u8> binary;

	if (shaderBinariesSupported()) {

		i32 binarySize = 0;
		u32 binaryFormat = 0;

		glGetProgramiv(id, GL_PROGRAM_BINARY_LENGTH, &binarySize);
		binary.resize(binarySize + 4);
		glGetProgramBinary(id, binarySize, &binarySize, reinterpret_cast<u32*>(&binary[0]), &binary[4]);
		binary.resize(binarySize + 4);

	} else {
		GLE::warn("Shader binaries are not supported");
	}

	return binary;

}



void ShaderProgram::start() {

#if !GLE_PASS_UNLINKED_SHADERS
	gle_assert(isLinked(), "Cannot activate non-linked shader with ID %d", id);
#endif

	if (!isActive()) {
		glUseProgram(id);
		activeProgramID = id;
	}

}



Uniform ShaderProgram::getUniform(const char* name) const {

#if !GLE_PASS_UNLINKED_SHADERS
	gle_assert(isLinked(), "Cannot query uniform from non-linked shader with ID %d", id);
#endif

	u32 uniformID = glGetUniformLocation(id, name);

	if (uniformID == invalidID) {
		GLE::warn("Failed to fetch uniform location for uniform %s (shader program ID=%d)", name, id);
	}

	return Uniform(uniformID);

}



u32 ShaderProgram::getUniformBlockIndex(const char* name) const {

#if !GLE_PASS_UNLINKED_SHADERS
	gle_assert(isLinked(), "Cannot query uniform block from non-linked shader with ID %d", id);
#endif

	u32 uniformID = glGetUniformBlockIndex(id, name);

	if (uniformID == invalidID) {
		GLE::warn("Failed to fetch uniform block index for uniform block %s (shader program ID=%d)", name, id);
	}

	return uniformID;

}



bool ShaderProgram::bindUniformBlock(u32 block, u32 index) {

#if !GLE_PASS_UNLINKED_SHADERS
	gle_assert(isLinked(), "Cannot bind uniform block from non-linked shader with ID %d", id);
#endif

	if (block == invalidID) {
		GLE::warn("Attempted to bind invalid uniform block (shader program ID=%d)", id);
		return false;
	}

	if (index >= Limits::getMaxUniformBlockBindings()) {
		GLE::warn("Given uniform block binding index %d exceeds the maximum of %d (shader program ID=%d)", index, Limits::getMaxUniformBlockBindings(), id);
		return false;
	}

	glUniformBlockBinding(id, block, index);

	return true;

}



u32 ShaderProgram::getStorageBlockIndex(const char* name) const {

#if !GLE_PASS_UNLINKED_SHADERS
		gle_assert(isLinked(), "Cannot query uniform block from non-linked shader with ID %d", id);
#endif

	if (!shaderStorageBufferSupported()) {
		GLE::warn("Shader storage buffers are not supported");
		return -1;
	}

	u32 uniformID = glGetProgramResourceIndex(id, GL_SHADER_STORAGE_BLOCK, name);

	if (uniformID == invalidID) {
		GLE::warn("Failed to fetch uniform block index for uniform block %s (shader program ID=%d)", name, id);
	}

	return uniformID;

}



bool ShaderProgram::bindStorageBlock(u32 block, u32 index) {

#if !GLE_PASS_UNLINKED_SHADERS
		gle_assert(isLinked(), "Cannot bind uniform block from non-linked shader with ID %d", id);
#endif

	if (!shaderStorageBufferSupported()) {
		GLE::warn("Shader storage buffers are not supported");
		return false;
	}

	if (block == invalidID) {
		GLE::warn("Attempted to bind invalid storage block (shader program ID=%d)", id);
		return false;
	}

	if (index >= Limits::getMaxUniformBlockBindings()) {
		GLE::warn("Given storage block binding index %d exceeds the maximum of %d (shader program ID=%d)", index, Limits::getMaxStorageBlockBindings(), id);
		return false;
	}

	glShaderStorageBlockBinding(id, block, index);

	return true;

}



bool ShaderProgram::isActive() const {
	return activeProgramID == id;
}



bool ShaderProgram::isLinked() const {
	return linked;
}



bool ShaderProgram::shaderBinariesSupported() {
	return GLE_EXT_SUPPORTED(ARB_get_program_binary);
}



bool ShaderProgram::tesselationShadersSupported() {
	return GLE_EXT_SUPPORTED(ARB_tessellation_shader);
}



bool ShaderProgram::computeShadersSupported() {
	return GLE_EXT_SUPPORTED(ARB_compute_shader);
}



bool ShaderProgram::shaderStorageBufferSupported() {
	return GLE_EXT_SUPPORTED(ARB_shader_storage_buffer_object);
}



bool ShaderProgram::checkCompilation(u32 sid) {

	gle_assert(isCreated(), "Attempted to check compilation status of shader program without creating it");

	i32 compileState = 0;
	glGetShaderiv(sid, GL_COMPILE_STATUS, &compileState);

	if (!compileState) {

		i32 logLength = 0;
		std::string log;
		log.resize(1024);

		glGetShaderInfoLog(sid, log.size(), &logLength, log.data());
		log.resize(logLength);

		error("Failed to compile shader program with ID %d\n" + log, sid);

	}

	return compileState;

}



void ShaderProgram::checkLinking() {

	gle_assert(isCreated(), "Attempted to check link status of shader program without creating it");

	i32 linkState = 0;
	glGetProgramiv(id, GL_LINK_STATUS, &linkState);

	linked = linkState;

	if (!linked) {

		i32 logLength = 0;
		std::string log;
		log.resize(1024);

		glGetProgramInfoLog(id, log.size(), &logLength, log.data());
		log.resize(logLength);

		error("Failed to link shader program with ID %d\n" + log, id);

		destroy();

	}

}



void ShaderProgram::destroyShaders() {

	gle_assert(isCreated(), "Attempted to destroy shaders without creating a program first");

	for (u32 i = 0; i < shaderTypeCount; i++) {

		if (shaders[i] != invalidID) {

			glDetachShader(id, shaders[i]);
			glDeleteShader(shaders[i]);
			shaders[i] = invalidID;

		}

	}

}



u32 ShaderProgram::getShaderTypeEnum(ShaderType type) {

	switch (type) {

		case ShaderType::VertexShader:
			return GL_VERTEX_SHADER;

		case ShaderType::FragmentShader:
			return GL_FRAGMENT_SHADER;

		case ShaderType::GeometryShader:
			return GL_GEOMETRY_SHADER;

		case ShaderType::TessCtrlShader:
			return GL_TESS_CONTROL_SHADER;

		case ShaderType::TessEvalShader:
			return GL_TESS_EVALUATION_SHADER;

		case ShaderType::ComputeShader:
			return GL_COMPUTE_SHADER;

		default:
			gle_force_assert("Invalid shader type 0x%X", type);
			return -1;

	}

}


GLE_END