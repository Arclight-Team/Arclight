#pragma once

#include "render/gle/gle.h"
#include "util/uri.h"
#include <vector>
#include <unordered_map>



struct Material {

	GLE::ShaderProgram program;
	std::unordered_map<std::string, GLE::Texture2D> textures;

	void destroy();

};


struct Mesh {

	u32 vertexCount;
	GLE::VertexArray vao;
	GLE::VertexBuffer vbo;
	GLE::IndexBuffer ibo;
	u32 materialIndex;

	void destroy();

};


struct ModelNode {

	std::vector<ModelNode> children;
	std::vector<u32> meshIndices;
	std::string name;
	Mat4f baseTransform;
	bool visible;

};


struct Model {

	std::vector<Mesh> meshes;
	std::vector<Material> materials;
	ModelNode root;
	Mat4f transform;

	void destroy();

};


namespace Loader {

	bool loadShader(GLE::ShaderProgram& program, const Uri& vsPath, const Uri& fsPath);
	bool loadShader(GLE::ShaderProgram& program, const Uri& vsPath, const Uri& gsPath, const Uri& fsPath);

	bool loadTexture2D(GLE::Texture2D& texture, const Uri& path, bool flipY = false);
	bool loadArrayTexture2D(GLE::ArrayTexture2D& texture, const std::vector<Uri>& paths, bool flipY = false);
	bool loadCubemap(GLE::CubemapTexture& cubemap, const std::vector<Uri>& paths, bool flipY = false);
	bool loadModel(Model& model, const Uri& path, bool flipY = false);

}