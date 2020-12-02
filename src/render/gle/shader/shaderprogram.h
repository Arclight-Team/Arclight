#pragma once

#include "../gc.h"
#include "uniform.h"

#include <vector>


GLE_BEGIN


enum class ShaderType {
	VertexShader,
	FragmentShader,
	GeometryShader,
	TessCtrlShader,
	TessEvalShader,
	ComputeShader
};


class ShaderProgram {

public:

	constexpr static u32 shaderTypeCount = static_cast<u32>(ShaderType::ComputeShader) + 1;

	constexpr ShaderProgram() : id(invalidID), linked(false), shaders { invalidID, invalidID , invalidID , invalidID , invalidID, invalidID } {
		
		for (u32 i = 0; i < shaderTypeCount; i++) {
			shaders[i] = invalidID;
		}

	}

	ShaderProgram(const ShaderProgram& program) = delete;
	ShaderProgram& operator=(const ShaderProgram& program) = delete;

	//Creates a shader program if none has been created yet
	void create();

	//Destroys a shader program if it was created once
	void destroy();

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

	//Checks the given states
	bool isCreated() const;
	bool isActive() const;
	bool isLinked() const;

	static bool shaderBinariesSupported();
	static bool tesselationShadersSupported();
	static bool computeShadersSupported();

private:

	bool checkCompilation(u32 sid);
	void checkLinking();
	void destroyShaders();

	static u32 getShaderTypeEnum(ShaderType type);

	u32 id;			//ID of the shader program
	bool linked;	//True if it has been linked
	u32 shaders[shaderTypeCount];	//Array of shader ids

	inline static u32 activeProgramID = invalidBoundID;

};


GLE_END