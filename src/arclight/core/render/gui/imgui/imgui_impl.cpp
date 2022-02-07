/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 imgui_impl.cpp
 */

#include "imgui_impl.hpp"

#include "window/window.hpp"
#include "window/windowhandle.hpp"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "render/gle/gle.hpp"
#include GLE_HEADER

GUI_BEGIN

void IImGuiControl::applyRenderModifiers() {

	if (m_RenderModifiers & (1 << u32(Separator)))
		ImGui::Separator();

	if (m_RenderModifiers & (1 << u32(SameLine)))
		ImGui::SameLine();

	if (m_RenderModifiers & (1 << u32(NewLine)))
		ImGui::NewLine();

	if (m_RenderModifiers & (1 << u32(Spacing)))
		ImGui::Spacing();

	if (m_RenderModifiers & (1 << u32(Indent)))
		ImGui::Indent();

	if (m_RenderModifiers & (1 << u32(Unindent)))
		ImGui::Unindent();

	if (m_RenderModifiers & (1 << u32(BeginGroup)))
		ImGui::BeginGroup();

	if (m_RenderModifiers & (1 << u32(EndGroup)))
		ImGui::EndGroup();

	if (m_RenderModifiers & (1 << u32(AlignTextToFramePadding)))
		ImGui::AlignTextToFramePadding();

}


void initialize(::Window& window) {

	const char* glsl_version = "#version 130";

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window.getInternalHandle().lock()->handle, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

}

void destroy() {

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

}



void prepareRender() {

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

}

void render() {

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

GUI_END