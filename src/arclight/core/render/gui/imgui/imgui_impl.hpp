/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 imgui_impl.hpp
 */

#pragma once
#include "../common.hpp"

class Window;


GUI_BEGIN

enum class RenderModifier
{
	Separator,
	SameLine,
	NewLine,
	Spacing,
	Indent,
	Unindent,
	BeginGroup,
	EndGroup,
	AlignTextToFramePadding
};

using enum RenderModifier;

class IImGuiControl
{
public:

	constexpr IImGuiControl() :
		m_RenderModifiers(0)
	{}

	constexpr void addRenderModifier(RenderModifier rm) {
		m_RenderModifiers |= (1 << u32(rm));
	}

	constexpr void removeRenderToken(RenderModifier rm) {
		m_RenderModifiers &= ~(1 << u32(rm));
	}

	constexpr void clearRenderModifiers() {
		m_RenderModifiers = 0;
	}

	void applyRenderModifiers();

protected:

	u32 m_RenderModifiers;

};

void initialize(::Window& window);
void destroy();

void prepareRender();
void render();

GUI_END