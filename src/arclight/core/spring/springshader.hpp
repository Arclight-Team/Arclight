/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 springshader.hpp
 */

#pragma once

#include "shaderstage.hpp"



class SpringShader {

public:

	SpringShader() : enabled(true), invocationID(0) {}

	explicit SpringShader(ShaderStage stage, u32 siid) : enabled(true), invocationID(siid) {
		stages.emplace_back(std::move(stage));
	}

	explicit SpringShader(std::vector<ShaderStage> stages, u32 siid) : enabled(true), invocationID(siid), stages(std::move(stages)) {}

	bool preRender(u32 stage);
	void postRender(u32 stage);

	u32 getInvocationID() const;
	u32 getStageCount() const;
	bool hasStageFlag(u32 stage, ShaderStage::PipelineFlags flag) const;

	void enable();
	void disable();
	void setEnabled(bool enabled);
	bool isEnabled() const;

private:

	bool enabled;
	u32 invocationID;
	std::vector<ShaderStage> stages;

};