/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 slider.hpp
 */

#pragma once

#include "../common.hpp"
#include "../control.hpp"

#include <variant>

GUI_BEGIN

class Slider : public IControl
{
public:

	typedef int DataType;

	enum
	{
		DataType_Int8	= ImGuiDataType_S8,
		DataType_Int16	= ImGuiDataType_S16,
		DataType_Int32	= ImGuiDataType_S32,
		DataType_Int64	= ImGuiDataType_S64,
		DataType_UInt8	= ImGuiDataType_U8,
		DataType_UInt16	= ImGuiDataType_U16,
		DataType_UInt32	= ImGuiDataType_U32,
		DataType_UInt64	= ImGuiDataType_U64,
		DataType_Float	= ImGuiDataType_Float,
		DataType_Double	= ImGuiDataType_Double,

		DataType_Bool	= DataType_UInt8,
	};

	Slider();

	void createBool  (const std::string& text, bool* variable);
	void createInt8  (const std::string& text, i8* variable, i8 min, i8 max);
	void createInt16 (const std::string& text, i16* variable, i16 min, i16 max);
	void createInt32 (const std::string& text, i32* variable, i32 min, i32 max);
	void createInt64 (const std::string& text, i64* variable, i64 min, i64 max);
	void createUInt8 (const std::string& text, u8* variable, u8 min, u8 max);
	void createUInt16(const std::string& text, u16* variable, u16 min, u16 max);
	void createUInt32(const std::string& text, u32* variable, u32 min, u32 max);
	void createUInt64(const std::string& text, u64* variable, u64 min, u64 max);
	void createFloat (const std::string& text, float* variable, float min, float max);
	void createDouble(const std::string& text, double* variable, double min, double max);

	template<Arithmetic T>
	void create(const std::string& text, T* variable, T min = T(0), T max = T(1), const std::string& format = "") {

		if constexpr (std::is_same_v<T, bool>)			createBool  (text, variable);
		else if constexpr (std::is_same_v<T, i8>)		createInt8  (text, variable, min, max);
		else if constexpr (std::is_same_v<T, i16>)		createInt16 (text, variable, min, max);
		else if constexpr (std::is_same_v<T, i32>)		createInt32 (text, variable, min, max);
		else if constexpr (std::is_same_v<T, i64>)		createInt64 (text, variable, min, max);
		else if constexpr (std::is_same_v<T, u8>)		createUInt8 (text, variable, min, max);
		else if constexpr (std::is_same_v<T, u16>)		createUInt16(text, variable, min, max);
		else if constexpr (std::is_same_v<T, u32>)		createUInt32(text, variable, min, max);
		else if constexpr (std::is_same_v<T, u64>)		createUInt64(text, variable, min, max);
		else if constexpr (std::is_same_v<T, float>)	createFloat (text, variable, min, max);
		else if constexpr (std::is_same_v<T, double>)	createDouble(text, variable, min, max);
		else {
			arc_force_assert("Unknown Arithmetic type specified");
			arc_abort();
		}

		setFormat(format);

	}

	void createVecBool  (const std::string& text, int num, bool* variable);
	void createVecInt8  (const std::string& text, int num, i8* variable, i8 min, i8 max);
	void createVecInt16 (const std::string& text, int num, i16* variable, i16 min, i16 max);
	void createVecInt32 (const std::string& text, int num, i32* variable, i32 min, i32 max);
	void createVecInt64 (const std::string& text, int num, i64* variable, i64 min, i64 max);
	void createVecUInt8 (const std::string& text, int num, u8* variable, u8 min, u8 max);
	void createVecUInt16(const std::string& text, int num, u16* variable, u16 min, u16 max);
	void createVecUInt32(const std::string& text, int num, u32* variable, u32 min, u32 max);
	void createVecUInt64(const std::string& text, int num, u64* variable, u64 min, u64 max);
	void createVecFloat (const std::string& text, int num, float* variable, float min, float max);
	void createVecDouble(const std::string& text, int num, double* variable, double min, double max);

	template<Vector V, class T = typename V::Type>
	void createVector(const std::string& text, V* variable, T min = T(0), T max = T(1), const std::string& format = "") {

		using U = typename V::Type;

		constexpr int num = V::Size;
		U* var = &variable->x;

		if constexpr (std::is_same_v<U, bool>)			createVecBool  (text, num, var);
		else if constexpr (std::is_same_v<U, i8>)		createVecInt8  (text, num, var, min, max);
		else if constexpr (std::is_same_v<U, i16>)		createVecInt16 (text, num, var, min, max);
		else if constexpr (std::is_same_v<U, i32>)		createVecInt32 (text, num, var, min, max);
		else if constexpr (std::is_same_v<U, i64>)		createVecInt64 (text, num, var, min, max);
		else if constexpr (std::is_same_v<U, u8>)		createVecUInt8 (text, num, var, min, max);
		else if constexpr (std::is_same_v<U, u16>)		createVecUInt16(text, num, var, min, max);
		else if constexpr (std::is_same_v<U, u32>)		createVecUInt32(text, num, var, min, max);
		else if constexpr (std::is_same_v<U, u64>)		createVecUInt64(text, num, var, min, max);
		else if constexpr (std::is_same_v<U, float>)	createVecFloat (text, num, var, min, max);
		else if constexpr (std::is_same_v<U, double>)	createVecDouble(text, num, var, min, max);
		else {
			arc_force_assert("Unknown Vector<Arithmetic> type specified");
			arc_abort();
		}

		setFormat(format);

	}

	std::string getText() const;
	void setText(const std::string& text);

	std::string getFormat() const;
	void setFormat(const std::string& format);

	IMPL_ACTION_SETTER(OnValueChanged)

protected:

	virtual void update() override;

private:

	std::string m_Text;
	std::string m_Format;
	std::variant<u64, double> m_MinValue;
	std::variant<u64, double> m_MaxValue;
	void* m_Variable;
	DataType m_DataType;
	int m_ComponentCount;

	Action m_OnValueChanged;

};

GUI_END