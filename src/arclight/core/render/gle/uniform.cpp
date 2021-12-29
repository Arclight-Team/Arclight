/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 uniform.cpp
 */

#include "uniform.hpp"

#include GLE_HEADER


GLE_BEGIN


void Uniform::setInt(i32 v0) {
	glUniform1i(id, v0);
}



void Uniform::setInt(i32 v0, i32 v1) {
	glUniform2i(id, v0, v1);
}



void Uniform::setInt(i32 v0, i32 v1, i32 v2) {
	glUniform3i(id, v0, v1, v2);
}



void Uniform::setInt(i32 v0, i32 v1, i32 v2, i32 v3) {
	glUniform4i(id, v0, v1, v2, v3);
}




void Uniform::setUnsigned(u32 v0) {
	glUniform1ui(id, v0);
}



void Uniform::setUnsigned(u32 v0, u32 v1) {
	glUniform2ui(id, v0, v1);
}



void Uniform::setUnsigned(u32 v0, u32 v1, u32 v2) {
	glUniform3ui(id, v0, v1, v2);
}



void Uniform::setUnsigned(u32 v0, u32 v1, u32 v2, u32 v3) {
	glUniform4ui(id, v0, v1, v2, v3);
}




void Uniform::setFloat(float v0) {
	glUniform1f(id, v0);
}



void Uniform::setFloat(float v0, float v1) {
	glUniform2ui(id, v0, v1);
}



void Uniform::setFloat(float v0, float v1, float v2) {
	glUniform3f(id, v0, v1, v2);
}



void Uniform::setFloat(float v0, float v1, float v2, float v3) {
	glUniform4f(id, v0, v1, v2, v3);
}




void Uniform::setIntArray(const i32* v, u32 elements, u32 count) {

	switch (elements) {
		
		case 1:
			glUniform1iv(id, count, v);
			break;

		case 2:
			glUniform2iv(id, count, v);
			break;

		case 3:
			glUniform3iv(id, count, v);
			break;

		case 4:
			glUniform4iv(id, count, v);
			break;

		default:
			gle_force_assert("Illegal element count of %d for uniform int array", elements);
			break;

	}

}



void Uniform::setUnsignedArray(const u32* v, u32 elements, u32 count) {

	switch (elements) {

		case 1:
			glUniform1uiv(id, count, v);
			break;

		case 2:
			glUniform2uiv(id, count, v);
			break;

		case 3:
			glUniform3uiv(id, count, v);
			break;

		case 4:
			glUniform4uiv(id, count, v);
			break;

		default:
			gle_force_assert("Illegal element count of %d for uniform unsigned int array", elements);
			break;

	}

}



void Uniform::setFloatArray(const float* v, u32 elements, u32 count) {

	switch (elements) {

		case 1:
			glUniform1fv(id, count, v);
			break;

		case 2:
			glUniform2fv(id, count, v);
			break;

		case 3:
			glUniform3fv(id, count, v);
			break;

		case 4:
			glUniform4fv(id, count, v);
			break;

		default:
			gle_force_assert("Illegal element count of %d for uniform int array", elements);
			break;

	}

}




void Uniform::setVec2(const float* m) {
	setFloatArray(m, 2, 1);
}



void Uniform::setVec2(const GLEVec2& m) {
	setFloatArray(GLE_VECTOR_VALUE_PTR(m), 2, 1);
}



void Uniform::setVec3(const float* m) {
	setFloatArray(m, 3, 1);
}



void Uniform::setVec3(const GLEVec3& m) {
	setFloatArray(GLE_VECTOR_VALUE_PTR(m), 3, 1);
}



void Uniform::setVec4(const float* m) {
	setFloatArray(m, 4, 1);
}



void Uniform::setVec4(const GLEVec4& m) {
	setFloatArray(GLE_VECTOR_VALUE_PTR(m), 4, 1);
}




void Uniform::setVec2Array(const float* m, u32 count) {
	setFloatArray(m, 2, count);
}



void Uniform::setVec2Array(const GLEVec2* m, u32 count) {
	setFloatArray(GLE_VECTOR_ARRAY_PTR(m), 2, count);
}



void Uniform::setVec3Array(const float* m, u32 count) {
	setFloatArray(m, 3, count);
}



void Uniform::setVec3Array(const GLEVec3* m, u32 count) {
	setFloatArray(GLE_VECTOR_ARRAY_PTR(m), 3, count);
}



void Uniform::setVec4Array(const float* m, u32 count) {
	setFloatArray(m, 4, count);
}



void Uniform::setVec4Array(const GLEVec4* m, u32 count) {
	setFloatArray(GLE_VECTOR_ARRAY_PTR(m), 4, count);
}




void Uniform::setMat2(const float* m) {
	glUniformMatrix2fv(id, 1, false, m);
}



void Uniform::setMat2(const GLEMat2& m) {
	glUniformMatrix2fv(id, 1, false, GLE_MATRIX_VALUE_PTR(m));
}



void Uniform::setMat3(const float* m) {
	glUniformMatrix3fv(id, 1, false, m);
}



void Uniform::setMat3(const GLEMat3& m) {
	glUniformMatrix3fv(id, 1, false, GLE_MATRIX_VALUE_PTR(m));
}



void Uniform::setMat4(const float* m) {
	glUniformMatrix4fv(id, 1, false, m);
}



void Uniform::setMat4(const GLEMat4& m) {
	glUniformMatrix4fv(id, 1, false, GLE_MATRIX_VALUE_PTR(m));
}




void Uniform::setMat2Array(const float* m, u32 count) {
	glUniformMatrix2fv(id, count, false, m);
}



void Uniform::setMat2Array(const GLEMat2* m, u32 count) {
	glUniformMatrix2fv(id, count, false, GLE_MATRIX_ARRAY_PTR(m));
}



void Uniform::setMat3Array(const float* m, u32 count) {
	glUniformMatrix3fv(id, count, false, m);
}



void Uniform::setMat3Array(const GLEMat3* m, u32 count) {
	glUniformMatrix3fv(id, count, false, GLE_MATRIX_ARRAY_PTR(m));
}



void Uniform::setMat4Array(const float* m, u32 count) {
	glUniformMatrix4fv(id, count, false, m);
}



void Uniform::setMat4Array(const GLEMat4* m, u32 count) {
	glUniformMatrix4fv(id, count, false, GLE_MATRIX_ARRAY_PTR(m));
}



bool Uniform::isValid() const {
	return id != invalidID;
}


GLE_END