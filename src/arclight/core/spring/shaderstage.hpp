/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 shaderstage.hpp
 */

#pragma once

#include "util/bitmaskenum.hpp"

#include <string>
#include <functional>
#include <memory>



class Path;
class ShaderHandle;

class ShaderStage {

public:

	enum class PipelineFlags {
		None = 0x0,
		Depth = 0x1,
		Blending = 0x2,
		Culling = 0x4,
		Textures = 0x8
	};

	ARC_CREATE_MEMBER_BITMASK_ENUM(PipelineFlags)

	ShaderStage(const std::shared_ptr<ShaderHandle>& program, PipelineFlags flags);

	PipelineFlags getFlags() const;
	void setFlags(PipelineFlags flags);

	template<class Func>
	void setPreRenderCallback(Func&& func) requires CC::Returns<Func, bool> {
		preRenderCallback = func;
	}

	template<class Func>
	void setPostRenderCallback(Func&& func) requires CC::Returns<Func, void> {
		postRenderCallback = func;
	}

	bool onPreRender();
	void onPostRender();

private:

	ShaderStage() = default;

	PipelineFlags flags;
	std::shared_ptr<ShaderHandle> shaderProgram;

	std::function<bool()> preRenderCallback;
	std::function<void()> postRenderCallback;

};
