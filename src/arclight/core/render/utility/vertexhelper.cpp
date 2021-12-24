#include "vertexhelper.hpp"


namespace VertexHelper {


	constexpr void flattenVector(VertexT* dst, const VectorT& src) {

		for (u32 i = 0; i < 3; i++) {
			dst[i] = src[i];
		}

	}


	std::vector<VertexT> createTriangle(const VectorT& v0, const VectorT& v1, const VectorT& v2) {

		std::vector<VertexT> v;
		v.resize(9);

		insertTriangle(v.data(), v0, v1, v2);

		return v;

	}



	std::vector<VertexT> createQuad(VertexT w, VertexT h, VertexT z) {

		std::vector<VertexT> v;
		v.resize(18);

		insertQuad(v.data(), w, h, z);

		return v;

	}



	std::vector<VertexT> createCuboid(VertexT w, VertexT h, VertexT d) {

		std::vector<VertexT> v;
		v.resize(108);

		insertCuboid(v.data(), w, h, d);

		return v;

	}



	std::vector<VertexT> createCube(VertexT s) {
		return createCuboid(s, s, s);
	}


	void insertTriangle(VertexT* v, const VectorT& v0, const VectorT& v1, const VectorT& v2) {

		flattenVector(&v[0], v0);
		flattenVector(&v[3], v1);
		flattenVector(&v[6], v2);

	}



	void insertQuad(VertexT* v, VertexT w, VertexT h, VertexT z) {

		VertexT x = w / 2;
		VertexT y = h / 2;

		VectorT vtxs[4] = {
			{-x, -y, z},
			{+x, -y, z},
			{+x, +y, z},
			{-x, +y, z}
		};

		flattenVector(&v[0], vtxs[0]);
		flattenVector(&v[3], vtxs[1]);
		flattenVector(&v[6], vtxs[2]);
		flattenVector(&v[9], vtxs[0]);
		flattenVector(&v[12], vtxs[2]);
		flattenVector(&v[15], vtxs[3]);

	}



	void insertCuboid(VertexT* v, VertexT w, VertexT h, VertexT d) {

		VertexT x = w / 2;
		VertexT y = h / 2;
		VertexT z = d / 2;

		VectorT vtxs[8] = {
			{-x, -y, -z},
			{+x, -y, -z},
			{+x, +y, -z},
			{-x, +y, -z},
			{-x, -y, +z},
			{+x, -y, +z},
			{+x, +y, +z},
			{-x, +y, +z}
		};

		//Front
		flattenVector(&v[0], vtxs[0]);
		flattenVector(&v[3], vtxs[1]);
		flattenVector(&v[6], vtxs[2]);
		flattenVector(&v[9], vtxs[0]);
		flattenVector(&v[12], vtxs[2]);
		flattenVector(&v[15], vtxs[3]);

		//Left
		flattenVector(&v[18], vtxs[4]);
		flattenVector(&v[21], vtxs[0]);
		flattenVector(&v[24], vtxs[3]);
		flattenVector(&v[27], vtxs[4]);
		flattenVector(&v[30], vtxs[3]);
		flattenVector(&v[33], vtxs[7]);

		//Right
		flattenVector(&v[36], vtxs[1]);
		flattenVector(&v[39], vtxs[5]);
		flattenVector(&v[42], vtxs[6]);
		flattenVector(&v[45], vtxs[1]);
		flattenVector(&v[48], vtxs[6]);
		flattenVector(&v[51], vtxs[2]);

		//Top
		flattenVector(&v[54], vtxs[3]);
		flattenVector(&v[57], vtxs[2]);
		flattenVector(&v[60], vtxs[6]);
		flattenVector(&v[63], vtxs[3]);
		flattenVector(&v[66], vtxs[6]);
		flattenVector(&v[69], vtxs[7]);

		//Bottom
		flattenVector(&v[72], vtxs[4]);
		flattenVector(&v[75], vtxs[5]);
		flattenVector(&v[78], vtxs[1]);
		flattenVector(&v[81], vtxs[4]);
		flattenVector(&v[84], vtxs[1]);
		flattenVector(&v[87], vtxs[0]);

		//Back
		flattenVector(&v[90], vtxs[5]);
		flattenVector(&v[93], vtxs[4]);
		flattenVector(&v[96], vtxs[7]);
		flattenVector(&v[99], vtxs[5]);
		flattenVector(&v[102], vtxs[7]);
		flattenVector(&v[105], vtxs[6]);

	}



	void insertCube(VertexT* v, VertexT s) {
		insertCuboid(v, s, s, s);
	}

}