/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 slider.cpp
 */

#include "slider.hpp"

GUI_BEGIN

Slider::Slider() {}



void Slider::createBool(const std::string& text, bool* variable) {

	m_Text = text;
	m_Variable = variable;
	m_MinValue = 0ui64;
	m_MaxValue = 1ui64;
	m_DataType = DataType_Bool;
	m_ComponentCount = 1;

}

void Slider::createInt8(const std::string& text, i8* variable, i8 min, i8 max) {

	m_Text = text;
	m_Variable = variable;
	m_MinValue = u64(min);
	m_MaxValue = u64(max);
	m_DataType = DataType_Int8;
	m_ComponentCount = 1;

}

void Slider::createInt16(const std::string& text, i16* variable, i16 min, i16 max) {

	m_Text = text;
	m_Variable = variable;
	m_MinValue = u64(min);
	m_MaxValue = u64(max);
	m_DataType = DataType_Int16;
	m_ComponentCount = 1;

}

void Slider::createInt32(const std::string& text, i32* variable, i32 min, i32 max) {

	m_Text = text;
	m_Variable = variable;
	m_MinValue = u64(min);
	m_MaxValue = u64(max);
	m_DataType = DataType_Int32;
	m_ComponentCount = 1;

}

void Slider::createInt64(const std::string& text, i64* variable, i64 min, i64 max) {

	m_Text = text;
	m_Variable = variable;
	m_MinValue = u64(min);
	m_MaxValue = u64(max);
	m_DataType = DataType_Int64;
	m_ComponentCount = 1;

}

void Slider::createUInt8(const std::string& text, u8* variable, u8 min, u8 max) {

	m_Text = text;
	m_Variable = variable;
	m_MinValue = u64(min);
	m_MaxValue = u64(max);
	m_DataType = DataType_UInt8;
	m_ComponentCount = 1;

}

void Slider::createUInt16(const std::string& text, u16* variable, u16 min, u16 max) {

	m_Text = text;
	m_Variable = variable;
	m_MinValue = u64(min);
	m_MaxValue = u64(max);
	m_DataType = DataType_UInt16;
	m_ComponentCount = 1;

}

void Slider::createUInt32(const std::string& text, u32* variable, u32 min, u32 max) {

	m_Text = text;
	m_Variable = variable;
	m_MinValue = u64(min);
	m_MaxValue = u64(max);
	m_DataType = DataType_UInt32;
	m_ComponentCount = 1;

}

void Slider::createUInt64(const std::string& text, u64* variable, u64 min, u64 max) {

	m_Text = text;
	m_Variable = variable;
	m_MinValue = u64(min);
	m_MaxValue = u64(max);
	m_DataType = DataType_UInt64;
	m_ComponentCount = 1;

}

void Slider::createFloat(const std::string& text, float* variable, float min, float max) {

	m_Text = text;
	m_Variable = variable;
	m_MinValue = double(min);
	m_MaxValue = double(max);
	m_DataType = DataType_Float;
	m_ComponentCount = 1;

}

void Slider::createDouble(const std::string& text, double* variable, double min, double max) {

	m_Text = text;
	m_Variable = variable;
	m_MinValue = min;
	m_MaxValue = max;
	m_DataType = DataType_Double;
	m_ComponentCount = 1;

}



void Slider::createVecBool(const std::string& text, int num, bool* variable) {

	createBool(text, variable);
	m_ComponentCount = num;

}

void Slider::createVecInt8(const std::string& text, int num, i8* variable, i8 min, i8 max) {

	createInt8(text, variable, min, max);
	m_ComponentCount = num;

}

void Slider::createVecInt16(const std::string& text, int num, i16* variable, i16 min, i16 max) {

	createInt16(text, variable, min, max);
	m_ComponentCount = num;

}

void Slider::createVecInt32(const std::string& text, int num, i32* variable, i32 min, i32 max) {

	createInt32(text, variable, min, max);
	m_ComponentCount = num;

}

void Slider::createVecInt64(const std::string& text, int num, i64* variable, i64 min, i64 max) {

	createInt64(text, variable, min, max);
	m_ComponentCount = num;

}

void Slider::createVecUInt8(const std::string& text, int num, u8* variable, u8 min, u8 max) {

	createUInt8(text, variable, min, max);
	m_ComponentCount = num;

}

void Slider::createVecUInt16(const std::string& text, int num, u16* variable, u16 min, u16 max) {

	createUInt16(text, variable, min, max);
	m_ComponentCount = num;

}

void Slider::createVecUInt32(const std::string& text, int num, u32* variable, u32 min, u32 max) {

	createUInt32(text, variable, min, max);
	m_ComponentCount = num;

}

void Slider::createVecUInt64(const std::string& text, int num, u64* variable, u64 min, u64 max) {

	createUInt64(text, variable, min, max);
	m_ComponentCount = num;

}

void Slider::createVecFloat(const std::string& text, int num, float* variable, float min, float max) {

	createFloat(text, variable, min, max);
	m_ComponentCount = num;

}

void Slider::createVecDouble(const std::string& text, int num, double* variable, double min, double max) {

	createDouble(text, variable, min, max);
	m_ComponentCount = num;

}



std::string Slider::getText() const {
	return m_Text;
}

void Slider::setText(const std::string& text) {
	m_Text = text;
}



std::string Slider::getFormat() const {
	return m_Format;
}

void Slider::setFormat(const std::string& format) {
	m_Format = format;
}



void Slider::update() {

	IImGuiControl::applyRenderModifiers();

	bool bfloat = m_DataType == DataType_Float || m_DataType == DataType_Double;
	bool changed = false;

	if (m_Format.empty()) {
		m_Format = bfloat ? "%.3f" : "%d";
	}

	if (m_DataType == DataType_Float) {
		float min = std::get<double>(m_MinValue);
		float max = std::get<double>(m_MaxValue);
		if (m_ComponentCount > 1) {
			changed = ImGui::SliderScalarN(m_Text.c_str(), m_DataType, m_Variable, m_ComponentCount, &min, &max, m_Format.c_str(), ImGuiSliderFlags(0));
		}
		else {
			changed = ImGui::SliderScalar(m_Text.c_str(), m_DataType, m_Variable, &min, &max, m_Format.c_str(), ImGuiSliderFlags(0));
		}
	}
	else if (m_DataType == DataType_Double) {
		double min = std::get<double>(m_MinValue);
		double max = std::get<double>(m_MaxValue);
		if (m_ComponentCount > 1) {
			changed = ImGui::SliderScalarN(m_Text.c_str(), m_DataType, m_Variable, m_ComponentCount, &min, &max, m_Format.c_str(), ImGuiSliderFlags(0));
		}
		else {
			changed = ImGui::SliderScalar(m_Text.c_str(), m_DataType, m_Variable, &min, &max, m_Format.c_str(), ImGuiSliderFlags(0));
		}
	}
	else {
		u64 min = std::get<u64>(m_MinValue);
		u64 max = std::get<u64>(m_MaxValue);
		if (m_ComponentCount > 1) {
			changed = ImGui::SliderScalarN(m_Text.c_str(), m_DataType, m_Variable, m_ComponentCount, &min, &max, m_Format.c_str(), ImGuiSliderFlags(0));
		}
		else {
			changed = ImGui::SliderScalar(m_Text.c_str(), m_DataType, m_Variable, &min, &max, m_Format.c_str(), ImGuiSliderFlags(0));
		}
	}

	if (changed) {
		m_OnValueChanged();
	}

}

GUI_END