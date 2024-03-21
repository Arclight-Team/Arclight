/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 VertexHelper.hpp
 */

#pragma once

#include "Math/Vector.hpp"

#include <vector>


//Helper functions to quickly create basic geometric shapes
namespace VertexHelper {

	using VertexT = float;
	using VectorT = Vec3<VertexT>;

	//Creates the specified shape and returns its vertex data
	std::vector<VertexT> createTriangle(const VectorT& v0, const VectorT& v1, const VectorT& v2);
	std::vector<VertexT> createQuad(VertexT w, VertexT h, VertexT z = 0);
	std::vector<VertexT> createCuboid(VertexT w, VertexT h, VertexT d);
	std::vector<VertexT> createCube(VertexT s);

	//Inserts the specified shape's vertex data into the buffer v
	void insertTriangle(VertexT* v, const VectorT& v0, const VectorT& v1, const VectorT& v2);
	void insertQuad(VertexT* v, VertexT w, VertexT h, VertexT z = 0);
	void insertCuboid(VertexT* v, VertexT w, VertexT h, VertexT d);
	void insertCube(VertexT* v, VertexT s);

}