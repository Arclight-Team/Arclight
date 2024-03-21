/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 SpringShader.hpp
 */

#pragma once

#include "ShaderStage.hpp"



class SpringShader {

public:

	SpringShader() : enabled(true) {}

	SpringShader(ShaderStage stage) : enabled(true) {
		stages.emplace_back(std::move(stage));
	}

	explicit SpringShader(std::vector<ShaderStage> stages) : enabled(true), stages(std::move(stages)) {}

	bool preRender(u32 stage);
	void postRender(u32 stage);

	u32 getStageCount() const;
	bool hasStageFlag(u32 stage, ShaderStage::PipelineFlags flag) const;

	void enable();
	void disable();
	void setEnabled(bool enabled);
	bool isEnabled() const;

private:

	bool enabled;
	std::vector<ShaderStage> stages;

};