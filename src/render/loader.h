#pragma once

#include <vector>


namespace GLE {
	class Texture2D;
	class CubemapTexture;
	class ShaderProgram;
}

class Uri;

namespace Loader {

	bool loadShader(GLE::ShaderProgram& program, const Uri& vsPath, const Uri& fsPath);
	bool loadTexture2D(GLE::Texture2D& texture, const Uri& path);
	bool loadCubemap(GLE::CubemapTexture& cubemap, const std::vector<Uri>& paths);

}