#include "loader.h"
#include "util/file.h"
#include "gle/gle.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


namespace Loader {


	bool loadShader(GLE::ShaderProgram& program, const Uri& vsPath, const Uri& fsPath) {

		File vsFile;
		vsFile.open(vsPath);

		if (!vsFile.isOpen()) {
			Log::error("Loader", "Failed to open vertex shader file %s", vsPath.getPath().c_str());
			return false;
		}

		const std::string vs = vsFile.readAll();
		vsFile.close();

		File fsFile;
		fsFile.open(fsPath);

		if (!fsFile.isOpen()) {
			Log::error("Loader", "Failed to open fragment shader file %s", fsPath.getPath().c_str());
			return false;
		}

		const std::string fs = fsFile.readAll();
		fsFile.close();

		program.create();

		if (!program.addShader(vs.c_str(), vs.size(), GLE::ShaderType::VertexShader)) {
			Log::error("Loader", "Compilation of vertex shader %s failed", vsPath.getPath().c_str());
			return false;
		}

		if (!program.addShader(fs.c_str(), fs.size(), GLE::ShaderType::FragmentShader)) {
			Log::error("Loader", "Compilation of fragment shader %s failed", fsPath.getPath().c_str());
			return false;
		}

		if (!program.link()) {
			Log::error("Loader", "Linking of shader program failed", fsPath.getPath().c_str());
			return false;
		}

		Log::info("Loader", "Shader program successfully compiled (vs = %s, fs = %s)", vsPath.getPath().c_str(), fsPath.getPath().c_str());

		return true;

	}


	bool loadTexture2D(GLE::Texture2D& texture, const Uri& path) {

		i32 width = 0;
		i32 height = 0;
		i32 channels = 0;

		stbi_set_flip_vertically_on_load(true);
		u8* data = stbi_load(path.getPath().c_str(), &width, &height, &channels, 0);

		if (!data) {
			Log::error("Loader", "Failed to load 2D texture %s", path.getPath().c_str());
			return false;
		}

		if (channels < 3 || channels > 4) {
			Log::error("Loader", "Invalid number of channels (%d) in %s", channels, path.getPath().c_str());
			return false;
		}

		bool hasAlpha = (channels == 4);

		GLE::setRowUnpackAlignment(GLE::Alignment::None);

		texture.create();
		texture.bind();
		texture.setData(width, height,	hasAlpha ? GLE::TextureFormat::RGBA8 : GLE::TextureFormat::RGB8, 
										hasAlpha ? GLE::TextureSourceFormat::RGBA : GLE::TextureSourceFormat::RGB, GLE::TextureSourceType::UByte, data);

		stbi_image_free(data);
		texture.generateMipmaps();

		Log::info("Loader", "Loaded 2D texture %s", path.getPath().c_str());

		return true;

	}


	bool loadCubemap(GLE::CubemapTexture& cubemap, const std::vector<Uri>& paths) {

		i32 width = 0;
		i32 height = 0;
		i32 channels = 0;

		stbi_set_flip_vertically_on_load(false);

		if (paths.size() != 6) {
			Log::error("Loader", "Cubemap requires 6 paths, got %d", paths.size());
			return false;
		}

		cubemap.create();
		cubemap.bind();

		for (u32 i = 0; i < 6; i++) {

			const Uri& path = paths[i];

			u8* data = stbi_load(path.getPath().c_str(), &width, &height, &channels, 0);

			if (!data) {
				Log::error("Loader", "Failed to load cubemap texture %s", path.getPath().c_str());
				return false;
			}

			if (width != height) {
				Log::error("Loader", "Width and height of cubemap face %s are not equal", path.getPath().c_str());
				return false;
			}

			if (channels < 3 || channels > 4) {
				Log::error("Loader", "Invalid number of channels (%d) in %s", channels, path.getPath().c_str());
				return false;
			}

			bool hasAlpha = (channels == 4);

			GLE::setRowUnpackAlignment(GLE::Alignment::None);

			cubemap.setData(i, width, hasAlpha ? GLE::TextureFormat::RGBA8 : GLE::TextureFormat::RGB8,
							hasAlpha ? GLE::TextureSourceFormat::RGBA : GLE::TextureSourceFormat::RGB, GLE::TextureSourceType::UByte, data);

			stbi_image_free(data);

		}

		cubemap.generateMipmaps();

		Log::info("Loader", "Loaded cubemap texture");

		return true;

	}

}