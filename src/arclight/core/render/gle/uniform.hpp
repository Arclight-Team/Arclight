/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 uniform.hpp
 */

#pragma once

#include "gc.hpp"

#include GLE_MATRIX_HEADER
GLE_DEFINE_MATRIX


GLE_BEGIN


class Uniform {

public:

	constexpr Uniform() : id(invalidID) {}
	constexpr explicit Uniform(u32 location) : id(location) {}
	
	void setInt(i32 v0);
	void setInt(i32 v0, i32 v1);
	void setInt(i32 v0, i32 v1, i32 v2);
	void setInt(i32 v0, i32 v1, i32 v2, i32 v3);

	void setUnsigned(u32 v0);
	void setUnsigned(u32 v0, u32 v1);
	void setUnsigned(u32 v0, u32 v1, u32 v2);
	void setUnsigned(u32 v0, u32 v1, u32 v2, u32 v3);

	void setFloat(float v0);
	void setFloat(float v0, float v1);
	void setFloat(float v0, float v1, float v2);
	void setFloat(float v0, float v1, float v2, float v3);

	void setIntArray(const i32* v, u32 elements, u32 count);
	void setUnsignedArray(const u32* v, u32 elements, u32 count);
	void setFloatArray(const float* v, u32 elements, u32 count);

	void setVec2(const float* m);
	void setVec2(const GLEVec2& m);
	void setVec3(const float* m);
	void setVec3(const GLEVec3& m);
	void setVec4(const float* m);
	void setVec4(const GLEVec4& m);

	void setVec2Array(const float* m, u32 count);
	void setVec2Array(const GLEVec2* m, u32 count);
	void setVec3Array(const float* m, u32 count);
	void setVec3Array(const GLEVec3* m, u32 count);
	void setVec4Array(const float* m, u32 count);
	void setVec4Array(const GLEVec4* m, u32 count);

	void setMat2(const float* m);
	void setMat2(const GLEMat2& m);
	void setMat3(const float* m);
	void setMat3(const GLEMat3& m);
	void setMat4(const float* m);
	void setMat4(const GLEMat4& m);

	void setMat2Array(const float* m, u32 count);
	void setMat2Array(const GLEMat2* m, u32 count);
	void setMat3Array(const float* m, u32 count);
	void setMat3Array(const GLEMat3* m, u32 count);
	void setMat4Array(const float* m, u32 count);
	void setMat4Array(const GLEMat4* m, u32 count);

	bool isValid() const;

private:

	u32 id;

};

GLE_END