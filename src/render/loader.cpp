#include "loader.h"
#include "util/file.h"
#include "gle/gle.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


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



	bool loadTexture2D(GLE::Texture2D& texture, const Uri& path, bool flipY) {

		i32 width = 0;
		i32 height = 0;
		i32 channels = 0;

		stbi_set_flip_vertically_on_load(!flipY);

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



	bool loadArrayTexture2D(GLE::ArrayTexture2D& texture, const std::vector<Uri>& paths, bool flipY) {

		i32 width = 0;
		i32 height = 0;
		i32 channels = 0;
		u32 layers = paths.size();
		GLE::TextureFormat format;
		GLE::TextureSourceFormat srcFormat;

		stbi_set_flip_vertically_on_load(!flipY);

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



	bool loadCubemap(GLE::CubemapTexture& cubemap, const std::vector<Uri>& paths, bool flipY) {

		i32 width = 0;
		i32 height = 0;
		i32 channels = 0;
		GLE::TextureFormat format;
		GLE::TextureSourceFormat srcFormat;

		stbi_set_flip_vertically_on_load(!flipY);

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



	void loadNode(const aiNode* sceneNode, ModelNode& node, const Mat4f& transform) {

		aiMatrix4x4 sceneTransform = sceneNode->mTransformation;
		Mat4f thisTransform(sceneTransform[0][0], sceneTransform[1][0], sceneTransform[2][0], sceneTransform[3][0],
							sceneTransform[0][1], sceneTransform[1][1], sceneTransform[2][1], sceneTransform[3][1],
							sceneTransform[0][2], sceneTransform[1][2], sceneTransform[2][2], sceneTransform[3][2],
							sceneTransform[0][3], sceneTransform[1][3], sceneTransform[2][3], sceneTransform[3][3]);
		thisTransform.transpose();

		node.baseTransform = thisTransform * transform;
		node.visible = true;
		node.meshIndices.resize(sceneNode->mNumMeshes);

		for (u32 i = 0; i < sceneNode->mNumMeshes; i++) {
			node.meshIndices[i] = sceneNode->mMeshes[i];
		}

		node.children.resize(sceneNode->mNumChildren);

		for (u32 i = 0; i < sceneNode->mNumChildren; i++) {
			loadNode(sceneNode->mChildren[i], node.children[i], node.baseTransform);
		}

	}



	bool loadModel(Model& model, const Uri& path, bool flipY) {
		
		u32 flags = aiProcess_ValidateDataStructure
			| aiProcess_SortByPType
			| aiProcess_FindInvalidData
			| aiProcess_OptimizeMeshes
			| aiProcess_Triangulate
			| aiProcess_ImproveCacheLocality
			| aiProcess_GenBoundingBoxes
			| aiProcess_LimitBoneWeights
			| aiProcess_GenSmoothNormals;

		Assimp::Importer imp;
		const aiScene* scene = imp.ReadFile(path.getPath().c_str(), flags);

		if (!scene) {
			Log::error("Loader", imp.GetErrorString());
			return false;
		}

		loadNode(scene->mRootNode, model.root, Mat4f());

		for (u32 i = 0; i < scene->mNumMaterials; i++) {

			Material material;
			aiMaterial* sceneMaterial = scene->mMaterials[i];

			for (u32 j = aiTextureType_DIFFUSE; j < aiTextureType_UNKNOWN; j++) {

				aiTextureType type = static_cast<aiTextureType>(j);
				u32 count = sceneMaterial->GetTextureCount(type);
				aiString propPath;

				for (u32 k = 0; k < count; k++) {

					std::string name;
					sceneMaterial->GetTexture(type, k, &propPath, nullptr, nullptr);

					switch (type) {

						case aiTextureType_DIFFUSE:
							name = "diffuse";
							break;

						case aiTextureType_SPECULAR:
							name = "specular";
							break;

						case aiTextureType_AMBIENT:
							name = "ambient";
							break;

						case aiTextureType_EMISSIVE:
							name = "emissive";
							break;

						case aiTextureType_NORMALS:
							name = "normals";
							break;

						case aiTextureType_SHININESS:
							name = "shininess";
							break;

						case aiTextureType_OPACITY:
							name = "opacity";
							break;

						case aiTextureType_DISPLACEMENT:
							name = "displacement";
							break;

						case aiTextureType_LIGHTMAP:
							name = "lightmap";
							break;

						case aiTextureType_REFLECTION:
							name = "reflection";
							break;

						case aiTextureType_HEIGHT:
						case aiTextureType_BASE_COLOR:
						case aiTextureType_NORMAL_CAMERA:
						case aiTextureType_EMISSION_COLOR:
						case aiTextureType_METALNESS:
						case aiTextureType_DIFFUSE_ROUGHNESS:
						case aiTextureType_AMBIENT_OCCLUSION:
						case aiTextureType_UNKNOWN:
						default:
							name = "unknown";
							break;

					}

					name += std::to_string(k);
					Uri texpath(path);
					texpath.move("..");
					texpath.move(propPath.C_Str());

					GLE::Texture2D tex;
					Loader::loadTexture2D(tex, texpath, flipY);

					material.textures.emplace(name, std::move(tex));

				}

			}

			model.materials.emplace_back(std::move(material));

		}

		for (u32 i = 0; i < scene->mNumMeshes; i++) {

			Mesh mesh;
			aiMesh* sceneMesh = scene->mMeshes[i];

			std::vector<float> vertexData;

			mesh.vao.create();
			mesh.vao.bind();

			mesh.vbo.create();
			mesh.vbo.bind();

			u32 faceCount = sceneMesh->mNumFaces;
			u32 vertexCount = sceneMesh->mNumVertices;
			u32 uvChannelCount = sceneMesh->GetNumUVChannels();
			u32 totalSize = vertexCount * 12;

			if (uvChannelCount > 1) {
				Log::error("Loader", "Cannot import model with multiple UV attributes: Only AXR will support it.");
				return false;
			}

			for (u32 j = 0; j < uvChannelCount; j++) {

				u32 uvComps = sceneMesh->mNumUVComponents[j];
				totalSize += vertexCount * uvComps * 4;

			}

			totalSize += vertexCount * 12;

			u32 offset = 0;
			u32 size = vertexCount * 12;
			vertexData.resize(totalSize);
			std::memcpy(&vertexData[offset / 4], sceneMesh->mVertices, size);

			mesh.vao.setAttribute(0, 3, GLE::AttributeType::Float, 0, offset);
			mesh.vao.enableAttribute(0);

			offset += size;

			u32 uvComps = sceneMesh->mNumUVComponents[0];
			size = vertexCount * uvComps * 4;

			for (u32 j = 0; j < vertexCount; j++) {
					
				for (u32 k = 0; k < uvComps; k++) {
					vertexData[offset / 4 + j * uvComps + k] = sceneMesh->mTextureCoords[0][j][k];
				}

			}

			mesh.vao.setAttribute(1, uvComps, GLE::AttributeType::Float, 0, offset);
			mesh.vao.enableAttribute(1);

			offset += size;

			size = vertexCount * 12;
			std::memcpy(&vertexData[offset / 4], sceneMesh->mNormals, size);
			mesh.vao.setAttribute(2, 3, GLE::AttributeType::Float, 0, offset);
			mesh.vao.enableAttribute(2);

			mesh.vbo.allocate(totalSize, vertexData.data());

			mesh.ibo.create();
			mesh.ibo.bind();
			mesh.ibo.allocate(faceCount * 12);

			std::vector<u32> indices(faceCount * 3);

			for (u32 j = 0; j < faceCount; j++) {

				indices[j * 3 + 0] = sceneMesh->mFaces[j].mIndices[0];
				indices[j * 3 + 1] = sceneMesh->mFaces[j].mIndices[1];
				indices[j * 3 + 2] = sceneMesh->mFaces[j].mIndices[2];

			}

			mesh.ibo.update(0, indices.size() * 4, indices.data());

			mesh.vertexCount = faceCount * 3;
			mesh.materialIndex = sceneMesh->mMaterialIndex;

			model.meshes.emplace_back(std::move(mesh));

		}
		
		return true;

	}



	bool saveTexture(u32 w, u32 h, u8* data) {

		Uri fileUri(Config::getURIRootPath());
		fileUri.move("..");
		fileUri.move("screenshots");
		fileUri.createDirectory();
		fileUri.move("arclight_scshot.png");

		return stbi_write_png(fileUri.getPath().c_str(), w, h, 3, data, w);

	}

}