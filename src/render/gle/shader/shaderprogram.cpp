#include "shaderprogram.h"

#include GLE_HEADER


GLE_BEGIN


void ShaderProgram::create() {

	if (!isCreated()) {
		id = glCreateProgram();
	}

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

#if GLE_VERSION_MIN(4, 1)

	glProgramBinary(id, *static_cast<u32*>(binary), static_cast<u8*>(binary) + 4, size - 4);
	checkLinking();

#else
	gle_assert(false, "Shader binaries are not supported in this version");
#endif

	return linked;

}



std::vector<u8> ShaderProgram::saveBinary() {

	gle_assert(isLinked(), "Attempted to save shader program binary without linking it");
	
	std::vector<u8> binary;

#if GLE_VERSION_MIN(4, 1)

	i32 binarySize = 0;
	u32 binaryFormat = 0;

	glGetProgramiv(id, GL_PROGRAM_BINARY_LENGTH, &binarySize);
	binary.resize(binarySize + 4);
	glGetProgramBinary(id, binarySize, &binarySize, reinterpret_cast<u32*>(&binary[0]), &binary[4]);
	binary.resize(binarySize + 4);

#else
	gle_assert(false, "Shader binaries are not supported in this version");
#endif

	return binary;

}



void ShaderProgram::start() {

	gle_assert(isLinked(), "Cannot activate non-linked shader with ID %d", id);

	if (!isActive()) {
		glUseProgram(id);
		activeProgramID = id;
	}

}



bool ShaderProgram::isCreated() const {
	return id != invalidID;
}



bool ShaderProgram::isActive() const {
	return activeProgramID == id;
}



bool ShaderProgram::isLinked() const {
	return linked;
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

		GLE::error("Failed to compile shader program with ID %d\n" + log, sid);

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

		GLE::error("Failed to link shader program with ID %d\n" + log, id);

		destroy();

	}

}



void ShaderProgram::destroyShaders() {

	gle_assert(isCreated(), "Attempted to destroy shaders without creating a program first");

	for (u32 i = 0; i < 6; i++) {

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

#if GLE_VERSION_MIN(3, 3)
		case ShaderType::GeometryShader:
			return GL_GEOMETRY_SHADER;
#endif

#if GLE_VERSION_MIN(4, 0)
		case ShaderType::TessCtrlShader:
			return GL_TESS_CONTROL_SHADER;

		case ShaderType::TessEvalShader:
			return GL_TESS_EVALUATION_SHADER;
#endif

#if GLE_VERSION_MIN(4, 3)
		case ShaderType::ComputeShader:
			return GL_COMPUTE_SHADER;
#endif

		default:
			gle_assert(false, "Invalid shader type 0x%X", type);
			return -1;

	}

}


GLE_END