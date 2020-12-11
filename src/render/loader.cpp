#include "loader.h"
#include "util/file.h"
#include "gle/gle.h"
/*
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
*/
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
			stbi_image_free(data);
			return false;
		}

		bool hasAlpha = (channels == 4);

		texture.create();
		texture.bind();
		texture.setData(width, height,	hasAlpha ? GLE::TextureFormat::RGBA8 : GLE::TextureFormat::RGB8, 
										hasAlpha ? GLE::TextureSourceFormat::RGBA : GLE::TextureSourceFormat::RGB, GLE::TextureSourceType::UByte, data);

		stbi_image_free(data);
		texture.generateMipmaps();

		Log::info("Loader", "Loaded 2D texture %s", path.getPath().c_str());

		return true;

	}



	bool loadArrayTexture2D(GLE::ArrayTexture2D& texture, const std::vector<Uri>& paths) {

		i32 width = 0;
		i32 height = 0;
		i32 channels = 0;
		u32 layers = paths.size();
		GLE::TextureFormat format;
		GLE::TextureSourceFormat srcFormat;

		stbi_set_flip_vertically_on_load(true);

		texture.create();
		texture.bind();

		for (u32 i = 0; i < layers; i++) {

			u8* data = nullptr;
			const Uri& path = paths[i];

			if (!width) {

				data = stbi_load(path.getPath().c_str(), &width, &height, &channels, 0);

				if (!data) {
					Log::error("Loader", "Failed to load 2D texture %s", path.getPath().c_str());
					return false;
				}

				if (channels != 3 && channels != 4) {
					Log::error("Loader", "Invalid number of channels (%d) in %s", channels, path.getPath().c_str());
					stbi_image_free(data);
					return false;
				}

				format = (channels == 4) ? GLE::TextureFormat::RGBA8 : GLE::TextureFormat::RGB8;
				srcFormat = (channels == 4) ? GLE::TextureSourceFormat::RGBA : GLE::TextureSourceFormat::RGB;
				texture.setData(width, height, layers, format, srcFormat, GLE::TextureSourceType::UByte, nullptr);

			} else {

				i32 w = 0;
				i32 h = 0;
				i32 c = 0;
				data = stbi_load(path.getPath().c_str(), &w, &h, &c, 0);

				if (!data) {
					Log::error("Loader", "Failed to load 2D texture %s", path.getPath().c_str());
					return false;
				}

				if (width != w || height != h || channels != c) {
					Log::error("Loader", "2D array texture configuration mismatch of %s", path.getPath().c_str());
					stbi_image_free(data);
					return false;
				}

			}

			texture.update(0, 0, width, height, i, srcFormat, GLE::TextureSourceType::UByte, data);

			stbi_image_free(data);

		}

		texture.generateMipmaps();

		Log::info("Loader", "Loaded 2D array texture (size = %d)", layers);

		return true;

	}



	bool loadCubemap(GLE::CubemapTexture& cubemap, const std::vector<Uri>& paths) {

		i32 width = 0;
		i32 height = 0;
		i32 channels = 0;
		GLE::TextureFormat format;
		GLE::TextureSourceFormat srcFormat;

		stbi_set_flip_vertically_on_load(false);

		if (paths.size() != 6) {
			Log::error("Loader", "Cubemap requires 6 paths, got %d", paths.size());
			return false;
		}

		cubemap.create();
		cubemap.bind();

		for (u32 i = 0; i < 6; i++) {

			u8* data = nullptr;
			const Uri& path = paths[i];

			if (!width) {

				data = stbi_load(path.getPath().c_str(), &width, &height, &channels, 0);

				if (!data) {
					Log::error("Loader", "Failed to load cubemap texture %s", path.getPath().c_str());
					return false;
				}

				if (width != height) {
					Log::error("Loader", "Width and height of cubemap face %s are not equal", path.getPath().c_str());
					stbi_image_free(data);
					return false;
				}

				if (channels != 3 && channels != 4) {
					Log::error("Loader", "Invalid number of channels (%d) in %s", channels, path.getPath().c_str());
					stbi_image_free(data);
					return false;
				}

				format = (channels == 4) ? GLE::TextureFormat::RGBA8 : GLE::TextureFormat::RGB8;
				srcFormat = (channels == 4) ? GLE::TextureSourceFormat::RGBA : GLE::TextureSourceFormat::RGB;
				cubemap.setData(i, width, format, srcFormat, GLE::TextureSourceType::UByte, data);

			} else {

				i32 w = 0;
				i32 h = 0;
				i32 c = 0;
				data = stbi_load(path.getPath().c_str(), &w, &h, &c, 0);

				if (!data) {
					Log::error("Loader", "Failed to load cubemap texture %s", path.getPath().c_str());
					return false;
				}

				if (width != w || height != h || channels != c) {
					Log::error("Loader", "Cubemap texture configuration mismatch of %s", path.getPath().c_str());
					stbi_image_free(data);
					return false;
				}

			}


			cubemap.setData(i, width, format, srcFormat, GLE::TextureSourceType::UByte, data);

			stbi_image_free(data);

		}

		cubemap.generateMipmaps();

		Log::info("Loader", "Loaded cubemap texture");

		return true;

	}



	bool Loader::loadModel(Model& model, const Uri& path) {
		/*
		u32 flags = aiProcess_ValidateDataStructure
			| aiProcess_SortByPType
			| aiProcess_FindInvalidData
			| aiProcess_OptimizeMeshes
			| aiProcess_Triangulate
			| aiProcess_ImproveCacheLocality
			| aiProcess_GenBoundingBoxes
			| aiProcess_LimitBoneWeights;

		Assimp::Importer imp;
		const aiScene* scene = imp.ReadFile(path.getPath().c_str(), flags);

		if (!scene) {
			Log::error("Loader", imp.GetErrorString());
			return false;
		}

		for (u32 i = 0; i < scene->mNumMeshes; i++) {

			aiMesh* mesh = scene->mMeshes[i];

			mesh->mFace

		}
		*/
		return true;

	}

}