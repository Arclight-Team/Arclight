#pragma once

#include <vector>


namespace GLE {
	class Texture2D;
	class ArrayTexture2D;
	class CubemapTexture;
	class ShaderProgram;
}

class Uri;

namespace Loader {

	bool loadShader(GLE::ShaderProgram& program, const Uri& vsPath, const Uri& fsPath);
	bool loadTexture2D(GLE::Texture2D& texture, const Uri& path);
	bool loadArrayTexture2D(GLE::ArrayTexture2D& texture, const std::vector<Uri>& paths);
	bool loadCubemap(GLE::CubemapTexture& cubemap, const std::vector<Uri>& paths);

}