#pragma once

#include "gle/gle.h"
#include <vector>


struct Mesh {

	GLE::VertexArray vao;
	GLE::VertexBuffer vbo;

};


struct Model {
	std::vector<Mesh> meshes;
};


class Uri;

namespace Loader {

	bool loadShader(GLE::ShaderProgram& program, const Uri& vsPath, const Uri& fsPath);
	bool loadTexture2D(GLE::Texture2D& texture, const Uri& path);
	bool loadArrayTexture2D(GLE::ArrayTexture2D& texture, const std::vector<Uri>& paths);
	bool loadCubemap(GLE::CubemapTexture& cubemap, const std::vector<Uri>& paths);
	bool loadModel(Model& model, const Uri& path);

}