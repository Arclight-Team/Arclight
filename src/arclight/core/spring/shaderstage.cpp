/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 shaderstage.cpp
 */

#include "shaderstage.hpp"
#include "shaderhandle.hpp"



ShaderStage::ShaderStage(const std::shared_ptr<ShaderHandle>& program, PipelineFlags flags) : flags(flags) {
	shaderProgram = program;
}



ShaderStage::PipelineFlags ShaderStage::getFlags() const {
	return flags;
}



void ShaderStage::setFlags(PipelineFlags flags) {
	this->flags = flags;
}



bool ShaderStage::onPreRender() {

	if (!shaderProgram) {
		return false;
	}

	if (preRenderCallback && !preRenderCallback()) {
		return false;
	}

	shaderProgram->program.start();

	return true;

}



void ShaderStage::onPostRender() {

	if (postRenderCallback) {
		postRenderCallback();
	}

}