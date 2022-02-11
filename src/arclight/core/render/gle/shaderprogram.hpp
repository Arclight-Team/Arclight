/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 shaderprogram.hpp
 */

#pragma once

#include "gc.hpp"
#include "globject.hpp"
#include "uniform.hpp"

#include <vector>


GLE_BEGIN


enum class ShaderType {
	VertexShader	= 0x8B31, // GL_VERTEX_SHADER
	FragmentShader	= 0x8B30, // GL_FRAGMENT_SHADER
	GeometryShader	= 0x8DD9, // GL_GEOMETRY_SHADER
	TessCtrlShader	= 0x8E88, // GL_TESS_CONTROL_SHADER
	TessEvalShader	= 0x8E87, // GL_TESS_EVALUATION_SHADER
	ComputeShader	= 0x91B9, // GL_COMPUTE_SHADER
};


class ShaderProgram : public GLObject {

public:

	constexpr static u32 shaderTypeCount = static_cast<u32>(ShaderType::ComputeShader) + 1;

	constexpr ShaderProgram() : linked(false), shaders { invalidID, invalidID , invalidID , invalidID , invalidID, invalidID } {
		
		for (u32 i = 0; i < shaderTypeCount; i++) {
			shaders[i] = invalidID;
		}

	}

	//Creates a shader program if none has been created yet
	virtual bool create() override;

	//Destroys a shader program if it was created once
	virtual void destroy() override;

	//Attaches a shader of the given type to the program. Returns true if compilation was successful, false otherwise.
	bool addShader(const char* source, u32 size, ShaderType type);
	
	//Links the compiled shaders to the program and returns true upon success. ON failure, false is returned and the program destroyed.
	bool link();

	//Loads/saves the compiled shader program binary. If not successful, the program will be destroyed.
	bool loadBinary(void* binary, u32 size);
	std::vector<u8> saveBinary();

	//Activates the shader
	void start();

	//Aquires a uniform handle
	Uniform getUniform(const char* name) const;
	u32 getUniformBlockIndex(const char* name) const;
	bool bindUniformBlock(u32 block, u32 index);

	u32 getStorageBlockIndex(const char* name) const;
	bool bindStorageBlock(u32 block, u32 index);

	//Checks the given states
	bool isActive() const;
	bool isLinked() const;

	static bool shaderBinariesSupported();
	static bool tesselationShadersSupported();
	static bool computeShadersSupported();
	static bool shaderStorageBufferSupported();

private:

	bool checkCompilation(u32 sid);
	void checkLinking();
	void destroyShaders();

	bool linked;	//True if it has been linked
	u32 shaders[shaderTypeCount];	//Array of shader ids

	inline static u32 activeProgramID = invalidBoundID;

};


GLE_END