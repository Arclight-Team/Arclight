/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 SpringShader.cpp
 */

#include "Spring/SpringShader.hpp"



bool SpringShader::preRender(u32 stage) {

	if (!enabled || stage >= getStageCount()) {
		return false;
	}

	return stages[stage].onPreRender();

}



void SpringShader::postRender(u32 stage) {

	if (stage >= getStageCount()) {
		return;
	}

	stages[stage].onPostRender();

}



u32 SpringShader::getStageCount() const {
	return stages.size();
}



bool SpringShader::hasStageFlag(u32 stage, ShaderStage::PipelineFlags flag) const {
	return (stages[stage].getFlags() & flag) == flag;
}



void SpringShader::enable() {
	enabled = true;
}



void SpringShader::disable() {
	enabled = false;
}



void SpringShader::setEnabled(bool enabled) {
	this->enabled = enabled;
}



bool SpringShader::isEnabled() const {
	return enabled;
}