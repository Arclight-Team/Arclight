/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 model.hpp
 */

#pragma once

#include "filesystem/path.hpp"
#include "math/matrix.hpp"
#include "math/vector.hpp"
#include "util/bits.hpp"
#include "types.hpp"

#include <vector>
#include <deque>
#include <unordered_map>
#include <map>

// in .cpp

#include <numeric>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "render/gle/gle.hpp"

namespace arc
{

	/*
	* @class TextureParser
	*/
	class TextureParser
	{
	public:

		constexpr TextureParser() : data(nullptr), width(0), height(0), channels(0), hdr(false) {}
		TextureParser(const Path& path, bool flipY = false, bool hdr = false) : TextureParser() {
			load(path, flipY, hdr);
		}
		virtual ~TextureParser() { destroy(); }

		bool load(const Path& path, bool flipY = false, bool hdr = false) {

			width = 0;
			height = 0;
			channels = 0;
			this->hdr = hdr;

			stbi_set_flip_vertically_on_load(!flipY);

			if (hdr)
				data = Bits::rcast<u8*>(stbi_loadf(path.toString().c_str(), &width, &height, &channels, 0));
			else
				data = stbi_load(path.toString().c_str(), &width, &height, &channels, 0);

			if (!isLoaded()) {
				return false;
			}

			if (!valid()) {
				destroy();
				return false;
			}

			return true;

		}
		void destroy() {

			if (isLoaded()) {
				stbi_image_free(data);
				data = nullptr;
			}

		}

		constexpr bool isLoaded() const {
			return data != nullptr;
		}
		constexpr bool valid() const {
			return isLoaded() && (channels >= 1 && channels <= 4) && width > 0 && height > 0;
		}

		constexpr const u8* getData() const {
			return data;
		}
		constexpr u64 getSize() const {
			return width * getStride() * height;
		}
		constexpr i32 getWidth() const {
			return width;
		}
		constexpr i32 getHeight() const {
			return height;
		}
		constexpr i32 getStride() const {
			return channels * sizeof(u8);
		}
		constexpr i32 getChannels() const {
			return channels;
		}
		constexpr bool hasRed() const {
			return channels >= 1;
		}
		constexpr bool hasGreen() const {
			return channels >= 2;
		}
		constexpr bool hasBlue() const {
			return channels >= 3;
		}
		constexpr bool hasAlpha() const {
			return channels >= 4;
		}

		GLE::ImageFormat getImageFormat(bool srgb) const {

			switch (channels) {

			case 1:
				return hdr ? GLE::ImageFormat::R16f : GLE::ImageFormat::R8;

			case 2:
				return hdr ? GLE::ImageFormat::RG16f : GLE::ImageFormat::RG8;

			case 3:
				return hdr ? GLE::ImageFormat::RGB16f : srgb ? GLE::ImageFormat::SRGB8 : GLE::ImageFormat::RGB8;

			case 4:
				return hdr ? GLE::ImageFormat::RGBA16f : srgb ? GLE::ImageFormat::SRGBA8 : GLE::ImageFormat::RGBA8;

			}

			arc_force_assert("Invalid channel count");
			return GLE::ImageFormat::RGBA8;

		}

		GLE::TextureSourceFormat getSourceFormat() const {

			switch (channels) {

			case 1:
				return GLE::TextureSourceFormat::Red;

			case 2:
				return GLE::TextureSourceFormat::RG;

			case 3:
				return GLE::TextureSourceFormat::RGB;

			case 4:
				return GLE::TextureSourceFormat::RGBA;

			}

			arc_force_assert("Invalid channel count");
			return GLE::TextureSourceFormat::RGBA;

		}

	private:

		u8* data;
		i32 width;
		i32 height;
		i32 channels;
		bool hdr;

	};


	/*
	* @class Texture2D
	*/
	class Texture2D : public GLE::Texture2D
	{
	public:

		bool load(const Path& path, bool flipY = false, bool srgb = false) {

			TextureParser parser(path, flipY);

			if (!parser.valid()) {

				if (parser.isLoaded()) {

					if (parser.getWidth() <= 0) {
						Log::error("arcTexture2D", "Invalid texture width (%d) in %s", parser.getWidth(), path.toString().c_str());
					}

					if (parser.getHeight() <= 0) {
						Log::error("arcTexture2D", "Invalid texture height (%d) in %s", parser.getHeight(), path.toString().c_str());
					}

					if (parser.getChannels() < 1 || parser.getChannels() > 4) {
						Log::error("arcTexture2D", "Invalid number of channels (%d) in %s", parser.getChannels(), path.toString().c_str());
					}
				}
				else {
					Log::error("arcTexture2D", "Failed to load texture %s", path.toString().c_str());
				}

				return false;
			}

			create();
			bind();
			setData(parser.getWidth(), parser.getHeight(), parser.getImageFormat(srgb), parser.getSourceFormat(), GLE::TextureSourceType::UByte, parser.getData());
			generateMipmaps();

			Log::info("arcTexture2D", "Loaded texture %s", path.toString().c_str());

			return true;

		}

	};


	/*
	* @class TextureHDR 
	*/
	class TextureHDR : public GLE::Texture2D
	{
	public:

		bool load(const Path& path, bool flipY = false, bool srgb = false) {

			TextureParser parser(path, flipY, true);

			if (!parser.valid()) {

				if (parser.isLoaded()) {

					if (parser.getWidth() <= 0) {
						Log::error("arcTextureHDR", "Invalid texture width (%d) in %s", parser.getWidth(), path.toString().c_str());
					}

					if (parser.getHeight() <= 0) {
						Log::error("arcTextureHDR", "Invalid texture height (%d) in %s", parser.getHeight(), path.toString().c_str());
					}

					if (parser.getChannels() < 1 || parser.getChannels() > 4) {
						Log::error("arcTextureHDR", "Invalid number of channels (%d) in %s", parser.getChannels(), path.toString().c_str());
					}
				}
				else {
					Log::error("arcTextureHDR", "Failed to load texture %s", path.toString().c_str());
				}

				return false;
			}

			create();
			bind();
			setData(parser.getWidth(), parser.getHeight(), parser.getImageFormat(srgb), parser.getSourceFormat(), GLE::TextureSourceType::Float, parser.getData());

			setWrapU(GLE::TextureWrap::Repeat);
			setWrapV(GLE::TextureWrap::Repeat);
			setMinFilter(GLE::TextureFilter::Bilinear, false);
			setMagFilter(GLE::TextureFilter::Bilinear);

			Log::info("arcTextureHDR", "Loaded texture %s", path.toString().c_str());

			return true;

		}

	};


	/*
	* @class Material
	*/
	class Material
	{
	public:

		Material() { setTextureOptions(); }
		virtual ~Material() { destroy(); }
		Material(const Material& other) = delete;
		Material& operator=(const Material& other) = delete;

		constexpr void setTextureOptions(bool flipY = false, bool sRGB = false) {

			textureFlipY = flipY;
			textureSRGB = sRGB;

		}

		bool parse(const aiMaterial* material, const Path& path) {

			if (!material) {
				Log::error("arcMesh", "Material is null");
				return false;
			}

			destroy();

			for (u32 i = aiTextureType_DIFFUSE; i < aiTextureType_UNKNOWN; i++) {
				aiTextureType type = static_cast<aiTextureType>(i);
				u32 count = material->GetTextureCount(type);

				if (supportedType(type)) {
					for (u32 n = 0; n < count; n++) {

						aiString texPath;
						material->GetTexture(type, n, &texPath);

						auto& target = getTextureContainer(type);
						auto& texture = target.emplace_back();

						Path resPath(path);
						resPath.append(texPath.C_Str());
						
						if (!texture.load(resPath, textureFlipY, textureSRGB)) {
							target.pop_back();
						}
					}
				}
			}

			return true;

		}
		
		void destroy() {

			destroyTextures(diffuse);
			destroyTextures(specular);
			destroyTextures(ambient);
			destroyTextures(emissive);
			destroyTextures(normals);
			destroyTextures(shininess);
			destroyTextures(opacity);
			destroyTextures(displacement);
			destroyTextures(lightmap);
			destroyTextures(reflection);

		}

#define IMPL_TEXTURE(name, field) \
		constexpr std::deque<Texture2D>& get ## name ## Textures() { return field ##; } \
		constexpr const std::deque<Texture2D>& get ## name ## Textures() const { return field ##; } \
		Texture2D& get ## name ## Texture(u32 id) { return field ##.at(id); } \
		const Texture2D& get ## name ## Texture(u32 id) const { return field ##.at(id); } \
		u32 get ## name ## TextureCount() const { return static_cast<u32>(## field ##.size()); } \
		bool has ## name ## Textures() const { return !## field ##.empty(); }

		IMPL_TEXTURE(Diffuse		, diffuse)
		IMPL_TEXTURE(Specular		, specular)
		IMPL_TEXTURE(Ambient		, ambient)
		IMPL_TEXTURE(Emissive		, emissive)
		IMPL_TEXTURE(Normals		, normals)
		IMPL_TEXTURE(Shininess		, shininess)
		IMPL_TEXTURE(Opacity		, opacity)
		IMPL_TEXTURE(Displacement	, displacement)
		IMPL_TEXTURE(Lightmap		, lightmap)
		IMPL_TEXTURE(Reflection		, reflection)
		IMPL_TEXTURE(BumpMap		, bumpmap)

		struct PBRProps {

			Vec3f albedo = Vec3f(1);
			float metallic = 0.0f;
			float roughness = 0.5f;
			float ao = 1.0f;
			float alpha = 1.0f;

		} pbr;

	private:

		bool supportedType(u32 type) const {

			switch (type) {

			case aiTextureType_DIFFUSE:
			case aiTextureType_SPECULAR:
			case aiTextureType_AMBIENT:
			case aiTextureType_EMISSIVE:
			case aiTextureType_NORMALS:
			case aiTextureType_SHININESS:
			case aiTextureType_OPACITY:
			case aiTextureType_DISPLACEMENT:
			case aiTextureType_LIGHTMAP:
			case aiTextureType_REFLECTION:
			case aiTextureType_HEIGHT:
				return true;

			case aiTextureType_BASE_COLOR:
			case aiTextureType_NORMAL_CAMERA:
			case aiTextureType_EMISSION_COLOR:
			case aiTextureType_METALNESS:
			case aiTextureType_DIFFUSE_ROUGHNESS:
			case aiTextureType_AMBIENT_OCCLUSION:
			case aiTextureType_UNKNOWN:
			default:
				return false;

			}

		}
		std::deque<Texture2D>& getTextureContainer(u32 type) {

			switch (type) {

			case aiTextureType_DIFFUSE:
				return diffuse;

			case aiTextureType_SPECULAR:
				return specular;

			case aiTextureType_AMBIENT:
				return ambient;

			case aiTextureType_EMISSIVE:
				return emissive;

			case aiTextureType_NORMALS:
				return normals;

			case aiTextureType_SHININESS:
				return shininess;

			case aiTextureType_OPACITY:
				return opacity;

			case aiTextureType_DISPLACEMENT:
				return displacement;

			case aiTextureType_LIGHTMAP:
				return lightmap;

			case aiTextureType_REFLECTION:
				return reflection;

			case aiTextureType_HEIGHT:
				return bumpmap;

			case aiTextureType_BASE_COLOR:
			case aiTextureType_NORMAL_CAMERA:
			case aiTextureType_EMISSION_COLOR:
			case aiTextureType_METALNESS:
			case aiTextureType_DIFFUSE_ROUGHNESS:
			case aiTextureType_AMBIENT_OCCLUSION:
			case aiTextureType_UNKNOWN:
			default:
				arc_force_assert("Texture type not supported");
				throw std::exception("Texture type not supported");
				
			}

		}
		void destroyTextures(std::deque<Texture2D>& textures) {

			for (auto& t : textures) {
				t.destroy();
			}

			textures.clear();

		}

		std::deque<Texture2D> diffuse;
		std::deque<Texture2D> specular;
		std::deque<Texture2D> ambient;
		std::deque<Texture2D> emissive;
		std::deque<Texture2D> normals;
		std::deque<Texture2D> shininess;
		std::deque<Texture2D> opacity;
		std::deque<Texture2D> displacement;
		std::deque<Texture2D> lightmap;
		std::deque<Texture2D> reflection;
		std::deque<Texture2D> bumpmap;

		bool textureFlipY;
		bool textureSRGB;

	};


	/*
	* @class ITreeNode
	*/
	template<class T>
	class ITreeNode
	{
	protected:

		using Type = T;

	public:

		constexpr ITreeNode() : parent(nullptr), next(nullptr), id(0) {}

		constexpr T* getParent() {
			return parent;
		}
		constexpr const T* getParent() const {
			return parent;
		}

		constexpr T* getNextSibling() {
			return next;
		}
		constexpr const T* getNextSibling() const {
			return next;
		}

		constexpr T* getNext() {

			if (next)
				return next;

			if (children.empty())
				return nullptr;

			return &children[0];

		}
		constexpr const T* getNext() const {

			if (next)
				return next;

			if (children.empty())
				return nullptr;

			return &children[0];

		}

		constexpr const std::vector<T>& getChildren() const {
			return children;
		}
		constexpr std::vector<T>& getChildren() {
			return children;
		}

		constexpr T& getChild(int id) {
			return children.at(id);
		}
		constexpr const T& getChild(int id) const {
			return children.at(id);
		}

		constexpr u32 getChildCount() const {
			return static_cast<u32>(children.size());
		}

		constexpr std::string getName() const {
			return name;
		}

		constexpr u32 getID() const {
			return id;
		}

	protected:

		T* parent;
		T* next;
		std::vector<T> children;

		std::string name;
		u32 id;

	};

	namespace ITreeNodeDumper
	{
		inline int tabLevel = 2;
		inline int level = 0;
		inline std::string indent(tabLevel, ' ');

		template<class T>
		void dumpNode(const ITreeNode<T>& node) {

			std::string indents(level, ' ');
			Log::info("...", indents + "Node { [%d] - '%s' }", node.getID(), node.getName().c_str());

			level += tabLevel;
			indents += indent;

			if (node.getParent()) {
				Log::info("...", indents + "Parent - { [%d] - '%s' }", node.getParent()->getID(), node.getParent()->getName().c_str());
			}
			else {
				Log::info("...", indents + "Parent - none");
			}

			if (node.getNextSibling()) {
				Log::info("...", indents + "Next - { [%d] - '%s' }", node.getNextSibling()->getID(), node.getNextSibling()->getName().c_str());
			}
			else {
				Log::info("...", indents + "Next - none");
			}

			if (node.getChildCount()) {
				Log::info("...", indents + "Children [%d]:", node.getChildCount());

				for (auto& n : node.getChildren()) {
					level += tabLevel;
					dumpNode<T>(n);
					level -= tabLevel;
				}
			}
			
			level -= tabLevel;

		}
	}


	/*
	* @class BoneWeight
	*/
	class BoneWeight
	{
	public:

		static constexpr inline int MaxBones = 4;

		constexpr BoneWeight() : boneID{}, boneWeight{} {}

		bool addBone(int id, float weight) {

			// find free slot
			for (int i = 0; i < MaxBones; i++) {
				if (Math::isZero(boneWeight[i])) {
					
					boneID[i] = id;
					boneWeight[i] = weight;

					return true;
				}
			}

			arc_force_assert("HOW");

			// bone weight is full (MaxBones)
			return false;

		}

		constexpr int getBoneID(int id) const {
			arc_assert(id < MaxBones, "Invalid bone ID specified");
			return boneID[id];
		}
		constexpr float getBoneWeight(int id) const {
			arc_assert(id < MaxBones, "Invalid bone ID specified");
			return boneWeight[id];
		}

	private:

		int boneID[MaxBones];
		float boneWeight[MaxBones];

	};


	/*
	* @class BoneData
	*/
	class BoneData
	{
	public:

		constexpr BoneData() : boneID(0) {}

		constexpr u32 getBoneID() const {
			return boneID;
		}
		constexpr Mat4f getInverseBindTransform() const {
			return inverseBindTransform;
		}

	private:

		friend class NodeCollection;

		Mat4f inverseBindTransform;
		u32 boneID;

	};


	/*
	* @class Node
	*/
	class Node : public ITreeNode<Node>
	{
	public:

		Node() : visible(true) {}

		constexpr const std::vector<u32>& getMeshes() const {
			return meshIDs;
		}
		
		constexpr void setTransform(const Mat4f& matrix) {
			transform = matrix;
		}
		constexpr Mat4f getTransform() const {
			return transform;
		}
		constexpr Mat4f getBaseTransform() const {
			return baseTransform;
		}

		bool isBone() const {
			return !boneData.empty();
		}
		BoneData& getBoneData(u32 meshID) {
			arc_assert(boneData.contains(meshID), "Node does not contain bone data for mesh %d", meshID);
			return boneData.at(meshID);
		}
		const BoneData& getBoneData(u32 meshID) const {
			arc_assert(boneData.contains(meshID), "Node does not contain bone data for mesh %d", meshID);
			return boneData.at(meshID);
		}
		const bool containsBoneData(u32 meshID) const {
			return boneData.contains(meshID);
		}

	private:

		friend class NodeCollection;

		Mat4f baseTransform;
		Mat4f transform;
		std::vector<u32> meshIDs;
		std::map<u32, BoneData> boneData;
		bool visible;

	};


	/*
	* @class NodeCollection
	*/
	class NodeCollection
	{
	public:

		NodeCollection() : rootNode(), nodeCount(0) {}
		virtual ~NodeCollection() { destroy(); }
		NodeCollection(const NodeCollection& other) = delete;
		NodeCollection& operator=(const NodeCollection& other) = delete;

		bool parse(const aiNode* rootNode) {

			if (!rootNode) {
				Log::error("arcMesh", "Root node is null");
				return false;
			}

			destroy();

			// build node tree
			nodeCount = 0;
			parseNodes(rootNode, this->rootNode);

			// link node tree
			linkNodes(this->rootNode);

			globalInverseTransform = this->rootNode.getBaseTransform().inverse();
			//globalInverseTransform = this->rootNode.getBaseTransform();

			return true;

		}
		bool parseMeshBones(const aiBone* bones[], u32 count, u32 meshID, std::map<u32, BoneWeight>& weights) {

			std::map<std::string, const aiBone*> boneMap;

			// create bone map [name -> bone]
			for (u32 i = 0; i < count; i++) {
				if (boneMap.contains(bones[i]->mName.C_Str())) {
					Log::error("arcBoneCollection", "Duplicate bone '%s'", bones[i]->mName.C_Str());
					return false;
				}
				boneMap[bones[i]->mName.C_Str()] = bones[i];
			}

			boneCount[meshID] = 0;
			weights.clear();
			parseBones(boneMap, meshID, weights, rootNode);

			return true;

		}
		void destroy() {

			rootNode = {};
			nodeCount = 0;
			boneCount.clear();

		}

		constexpr Node& getRoot() {
			return rootNode;
		}
		constexpr const Node& getRoot() const {
			return rootNode;
		}

		Node* getNode(u32 id) {
			return const_cast<Node*>(findNode(rootNode, id));
		}
		const Node* getNode(u32 id) const {
			return findNode(rootNode, id);
		}

		Node* getNode(const std::string& name) {
			return const_cast<Node*>(findNode(rootNode, name));
		}
		const Node* getNode(const std::string& name) const {
			return findNode(rootNode, name);
		}

		constexpr u32 getNodeCount() const {
			return nodeCount;
		}
		u32 getBoneCount(u32 meshID) const {
			
			if (boneCount.contains(meshID)) {
				return boneCount.at(meshID);
			}

			return 0;

		}

		constexpr Mat4f getGlobalInverseTransform() const {
			return globalInverseTransform;
		}

		//constexpr const std::map<u32, BoneWeight>& getVertexWeights() const {
		//	return vertexWeights;
		//}

	private:

		const Node* findNode(const Node& node, u32 id) const {

			if (node.getID() == id) {
				return &node;
			}

			for (u32 i = 0; i < node.getChildCount(); i++) {

				const Node* match = findNode(node.getChild(i), id);
				if (match)
					return match;

			}

			return nullptr;

		}
		const Node* findNode(const Node& node, const std::string& name) const {

			if (node.getName() == name) {
				return &node;
			}

			for (u32 i = 0; i < node.getChildCount(); i++) {

				const Node* match = findNode(node.getChild(i), name);
				if (match)
					return match;

			}

			return nullptr;

		}

		void parseBones(const std::map<std::string, const aiBone*> boneMap, u32 meshID, std::map<u32, BoneWeight>& weights, Node& node) {
				
			if (boneMap.contains(node.getName())) {
				const aiBone* bone = boneMap.at(node.getName());

				//if (!node.isBone()) {
					// this node is a new bone
					
					BoneData& data = node.boneData[meshID];

					data.boneID = boneCount[meshID]++;
					data.inverseBindTransform = {
						bone->mOffsetMatrix[0][0], bone->mOffsetMatrix[0][1], bone->mOffsetMatrix[0][2], bone->mOffsetMatrix[0][3],
						bone->mOffsetMatrix[1][0], bone->mOffsetMatrix[1][1], bone->mOffsetMatrix[1][2], bone->mOffsetMatrix[1][3],
						bone->mOffsetMatrix[2][0], bone->mOffsetMatrix[2][1], bone->mOffsetMatrix[2][2], bone->mOffsetMatrix[2][3],
						bone->mOffsetMatrix[3][0], bone->mOffsetMatrix[3][1], bone->mOffsetMatrix[3][2], bone->mOffsetMatrix[3][3]
					};

					// add the bone weight to the vertex weight map
					for (u32 i = 0; i < bone->mNumWeights; i++) {
						const aiVertexWeight* weight = bone->mWeights + i;
						//vertexWeights[weight->mVertexId].addBone(node.boneID, weight->mWeight);
						weights[weight->mVertexId].addBone(data.boneID, weight->mWeight);
					}
				//}
			}

			for (u32 i = 0; i < node.getChildCount(); i++) {
				parseBones(boneMap, meshID, weights, node.getChild(i));
			}

		}

		void parseNodes(const aiNode* ainode, Node& node, const Mat4f& transform = Mat4f()) {

			node = {};
			node.name = ainode->mName.C_Str();
			node.id = nodeCount++;
			node.visible = true;

			node.transform = {  ainode->mTransformation[0][0], ainode->mTransformation[0][1], ainode->mTransformation[0][2], ainode->mTransformation[0][3],
								ainode->mTransformation[1][0], ainode->mTransformation[1][1], ainode->mTransformation[1][2], ainode->mTransformation[1][3],
								ainode->mTransformation[2][0], ainode->mTransformation[2][1], ainode->mTransformation[2][2], ainode->mTransformation[2][3],
								ainode->mTransformation[3][0], ainode->mTransformation[3][1], ainode->mTransformation[3][2], ainode->mTransformation[3][3] };

			node.baseTransform = node.transform;

			if (ApplyRelativeNodeTransformation) {
				//node.transform = transform * node.transform;
				node.transform *= transform;
			}

			node.meshIDs.resize(ainode->mNumMeshes);
			for (u32 i = 0; i < ainode->mNumMeshes; i++) {
				node.meshIDs[i] = ainode->mMeshes[i];
			}

			node.children.resize(ainode->mNumChildren);
			for (u32 i = 0; i < ainode->mNumChildren; i++) {
				Node& newNode = node.children[i];
				parseNodes(ainode->mChildren[i], newNode, node.transform);
			}

		}
		void linkNodes(Node& node) {

			node.parent = nullptr;
			node.next = nullptr;

			for (u32 i = 0; i < node.getChildCount(); i++) {
				Node& child = node.getChild(i);
				linkNodes(child);

				child.parent = &node;
				child.next = (i + 1 < node.getChildCount()) ? &node.getChild(i + 1) : nullptr;
			}

		}

		//static constexpr inline bool ApplyRelativeNodeTransformation = true;
		static constexpr inline bool ApplyRelativeNodeTransformation = false;

		Mat4f globalInverseTransform;
		//std::map<u32, BoneWeight> vertexWeights;
		Node rootNode;
		u32 nodeCount;

		std::map<u32, u32> boneCount;

	};


	/*
	* @class Bone
	*/
	class Bone : public ITreeNode<Bone>
	{
	public:

		constexpr void setOffset(const Mat4f& matrix) {
			offset = matrix;
			dirty = true;
			recalculateInverseBindTransform(matrix);
		}
		constexpr Mat4f getOffset() const {
			return offset;
		}

		constexpr Mat4f getBaseOffset() const {
			return baseOffset;
		}
		constexpr Mat4f getInverseBaseOffset() const {
			return inverseBaseOffset;
		}

	private:

		constexpr void recalculateInverseBindTransform(const Mat4f& transform) {
			Mat4f boneTransform = transform * baseOffset;
			inverseBaseOffset = boneTransform.inverse();
			
			for (auto& b : children) {
				b.recalculateInverseBindTransform(boneTransform);
			}
		}

		friend class BoneCollection;

		Mat4f baseOffset;
		Mat4f inverseBaseOffset;
		Mat4f offset;
		bool dirty;

	};

	static constexpr inline auto MaxBoneCount = 1000;

	/*
	* @class Mesh
	*/
	class Mesh
	{
	public:

		Mesh() : meshID(0), materialID(0), faceCount(0), vertexCount(0), nodes(nullptr) {}
		virtual ~Mesh() { destroy(); }
		Mesh(const Mesh& other) = delete;
		Mesh& operator=(const Mesh& other) = delete;

		bool parse(const aiMesh* mesh, u32 meshID, NodeCollection& nodes) {

			if (!mesh) {
				Log::error("arcMesh", "Mesh is null");
				return false;
			}

			destroy();

			this->nodes = &nodes;
			this->meshID = meshID;

			// parse bones
			if (mesh->HasBones()) {
				// TODO: make this run in a parallel thread for better performance?
				// NOTE: this can stop parsing prematurely if the bone structure is invalid, so multithreading would not work here
				//if (!bones.parse(const_cast<const aiBone**>(mesh->mBones), mesh->mNumBones, rootNode)) {
				//	return false;
				//}

				if (!nodes.parseMeshBones(const_cast<const aiBone**>(mesh->mBones), mesh->mNumBones, meshID, vertexWeights)) {
					return false;
				}
			}

			u32 uvComps = mesh->mNumUVComponents[0];
			uvComps = Math::min(uvComps, 2U); // TODO

			u32 vertexSize = mesh->mNumVertices * 3 * sizeof(float);
			u32 normalSize = mesh->mNumVertices * 3 * sizeof(float);
			u32 tangentSize = mesh->mNumVertices * 3 * sizeof(float);
			u32 texCoordSize = mesh->mNumVertices * uvComps * sizeof(float);
			u32 boneIDSize = mesh->mNumVertices * sizeof(Vec4i);
			u32 boneWeightSize = mesh->mNumVertices * sizeof(Vec4f);

			// calculate total size (in bytes)
			u32 totalSize = vertexSize +
				(mesh->HasNormals() ? normalSize : 0) +
				(mesh->HasTangentsAndBitangents() ? tangentSize : 0) +
				(mesh->HasTextureCoords(0) ? texCoordSize : 0) +
				(mesh->HasBones() ? boneIDSize + boneWeightSize : 0);

			// create vertex array
			vertexArray.create();
			vertexArray.bind();

			// create vertex buffer
			vertexBuffer.create();
			vertexBuffer.bind();

			std::vector<u8> buffer;
			buffer.resize(totalSize);

			u8* dest = buffer.data();
			u64 offset = 0;

			// load vertices
			vertexArray.enableAttribute(0);
			vertexArray.setAttribute(0, 3, GLE::AttributeType::Float, 0, offset);
			std::memcpy(dest + offset, mesh->mVertices, vertexSize);
			offset += vertexSize;

			// load normals
			if (mesh->HasNormals()) {
				vertexArray.enableAttribute(1);
				vertexArray.setAttribute(1, 3, GLE::AttributeType::Float, 0, offset);
				std::memcpy(dest + offset, mesh->mNormals, normalSize);
				offset += normalSize;
			}

			// load tangents
			if (mesh->HasTangentsAndBitangents()) {
				vertexArray.enableAttribute(2);
				vertexArray.setAttribute(2, 3, GLE::AttributeType::Float, 0, offset);
				std::memcpy(dest + offset, mesh->mTangents, tangentSize);
				offset += tangentSize;
			}

			// load texture coordinates
			if (mesh->HasTextureCoords(0)) {
				//vertexArray.enableAttribute(3);
				//vertexArray.setAttribute(3, 2, GLE::AttributeType::Float, 0, offset);
				//std::memcpy(dest + offset, mesh->mTextureCoords[0], texCoordSize);
				//offset += texCoordSize;

				std::vector<float> texCoords;
				texCoords.resize(mesh->mNumVertices * uvComps);

				for (u32 i = 0; i < mesh->mNumVertices; i++) {
					for (u32 j = 0; j < uvComps; j++) {
						texCoords[i * uvComps + j] = mesh->mTextureCoords[0][i][j];
					}
				}

				vertexArray.setAttribute(3, uvComps, GLE::AttributeType::Float, 0, offset);
				vertexArray.enableAttribute(3);
				std::memcpy(dest + offset, texCoords.data(), texCoordSize);
				offset += texCoordSize;
			}

			// load bone weights
			if (mesh->HasBones()) {
				std::vector<Vec4i> boneIDs;
				std::vector<Vec4f> boneWeights;

				boneIDs.resize(mesh->mNumVertices);
				boneWeights.resize(mesh->mNumVertices);

				//auto& vertexWeights = bones.getVertexWeights();
				//auto& vertexWeights = nodes.getVertexWeights();

				for (const auto& [vertex, weight] : vertexWeights) {

					if (vertex >= mesh->mNumVertices) {
						Log::error("arcMesh", "Invalid vertex ID in vertex weight map");
						return false;
						//continue;
					}

					for (u32 i = 0; i < BoneWeight::MaxBones; i++) {
						boneIDs[vertex][i] = Math::min(weight.getBoneID(i), MaxBoneCount);
						boneWeights[vertex][i] = weight.getBoneWeight(i);
					}
				}

				/*for (auto& b : boneIDs) {
					if (b[0] > 8) arc_force_assert("");
					if (b[1] > 8) arc_force_assert("");
					if (b[2] > 8) arc_force_assert("");
					if (b[3] > 8) arc_force_assert("");
				}*/

				vertexArray.setAttribute(4, 4, GLE::AttributeType::Int, 0, offset, GLE::AttributeClass::Int);
				vertexArray.enableAttribute(4);
				std::memcpy(dest + offset, boneIDs.data(), boneIDSize);
				offset += boneIDSize;

				vertexArray.setAttribute(5, 4, GLE::AttributeType::Float, 0, offset);
				vertexArray.enableAttribute(5);
				std::memcpy(dest + offset, boneWeights.data(), boneWeightSize);
				offset += boneWeightSize;
			}

			vertexBuffer.allocate(totalSize, buffer.data());

			// create index buffer
			std::vector<int> indices;
			//indices.resize(mesh->mNumVertices);
			//std::iota(indices.begin(), indices.end(), 0);

			indices.resize(mesh->mNumFaces * 3);
			for (u32 i = 0; i < mesh->mNumFaces; i++) {
				indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
				indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
				indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
			}

			indexBuffer.create();
			indexBuffer.bind();
			indexBuffer.allocate(indices.size() * sizeof(int), indices.data());

			faceCount = mesh->mNumFaces;
			vertexCount = /*mesh->mNumVertices*/ mesh->mNumFaces * 3;
			materialID = mesh->mMaterialIndex;

			return true;

		}
		void destroy() {

			//bones.destroy();
			nodes = nullptr;
			vertexArray.destroy();
			vertexBuffer.destroy();
			indexBuffer.destroy();

			boneTransforms.clear();
			meshID = 0;
			materialID = 0;
			faceCount = 0;
			vertexCount = 0;

		}

		void createTransformTree(std::vector<Mat4f>& transforms) {

			if (!nodes || !nodes->getBoneCount(meshID))
				return;

			//transforms.resize(boneCount);
			//transforms.resize(100);
			transforms.resize(MaxBoneCount);
			transforms.emplace_back(); // Set identity matrix as transform no.201
			populateTransformTree(transforms, nodes->getRoot(), false);

		}
		bool updateTransformTree(std::vector<Mat4f>& transforms) {

			if (transforms.size() < nodes->getBoneCount(meshID)) {
				createTransformTree(transforms);
				return true;
			}

			return populateTransformTree(transforms, nodes->getRoot(), true);

		}

		void bind() {

			vertexArray.bind();
			//indexBuffer.bind();

		}

		constexpr u32 getFaceCount() const {
			return faceCount;
		}
		constexpr u32 getVertexCount() const {
			return vertexCount;
		}
		bool hasBones() const {
			return !vertexWeights.empty();
		}

		constexpr u32 getMaterialID() const {
			return materialID;
		}

	private:

		bool populateTransformTree(std::vector<Mat4f>& transforms, const Node& node, bool update, const Mat4f& transform = Mat4f()) {

			Mat4f baseTrans = transform;
				baseTrans *= node.getTransform();

			//auto bone = getBone(node.getName());
			if (node.isBone() && node.containsBoneData(meshID)) {

				BoneData bone = node.getBoneData(meshID);
				if (bone.getBoneID() < MaxBoneCount) {
					//Mat4f boneTrans = /*bone->getInverseBaseOffset() **/ bone->getOffset();
					//Mat4f boneTrans = bone->getInverseBaseOffset();
					//Mat4f boneTrans = node.getInverseBindTransform();
					Mat4f boneTrans = bone.getInverseBindTransform();

					//transforms[node.getBoneID()] = nodes->getGlobalInverseTransform() * baseTrans * boneTrans;
					transforms[bone.getBoneID()] = nodes->getGlobalInverseTransform() * baseTrans * boneTrans;
					//transforms[node.getBoneID()] = nodes->getRoot().getBaseTransform().inverse() * baseTrans * boneTrans;
				}
			}

			for (auto& n : node.getChildren()) {
				populateTransformTree(transforms, n, update, baseTrans);
			}

			return true;

		}

		//BoneCollection bones;
		NodeCollection* nodes;
		std::map<u32, BoneWeight> vertexWeights;

		GLE::VertexArray vertexArray;
		GLE::VertexBuffer vertexBuffer;
		GLE::IndexBuffer indexBuffer;
		std::vector<Mat4f> boneTransforms;
		u32 meshID;
		u32 materialID;
		u32 faceCount;
		u32 vertexCount;

	};


	/*
	* @class ModelParser
	*/
	class ModelParser
	{
	public:

		static constexpr inline auto DefaultProcessFlags = 0
			| aiProcess_ValidateDataStructure
			| aiProcess_SortByPType
			| aiProcess_FindInvalidData
			| aiProcess_OptimizeMeshes
			| aiProcess_Triangulate
			| aiProcess_ImproveCacheLocality
			| aiProcess_GenBoundingBoxes
			| aiProcess_LimitBoneWeights
			| aiProcess_GenSmoothNormals
			| aiProcess_CalcTangentSpace;


		ModelParser() : scene(nullptr) {}
		ModelParser(const Path& path, u32 processFlags = DefaultProcessFlags) {
			load(path, processFlags);
		}
		virtual ~ModelParser() { destroy(); }

		bool load(const Path& path, u32 processFlags = DefaultProcessFlags) {

			importer.FreeScene();
			scene = importer.ReadFile(path.toString(), processFlags);

			filePath = path;
			rootPath = path.parent();

			if (!isLoaded()) {
				return false;
			}

			return true;

		}
		void destroy() {

			importer.FreeScene();
			scene = nullptr;

		}

		constexpr bool isLoaded() const {
			return scene != nullptr;
		}

		constexpr aiAnimation**	getAnimations() { return scene->mAnimations; }
		constexpr aiCamera**	getCameras()	{ return scene->mCameras;	 }
		constexpr aiLight**		getLights()		{ return scene->mLights;	 }
		constexpr aiMaterial**	getMaterials()	{ return scene->mMaterials;	 }
		constexpr aiMesh**		getMeshes()		{ return scene->mMeshes;	 }
		constexpr aiMetadata*	getMetadata()	{ return scene->mMetaData;	 }
		constexpr aiNode*		getRootNode()	{ return scene->mRootNode;	 }
		constexpr aiTexture**	getTextures()	{ return scene->mTextures;	 }

		constexpr aiAnimation**	const	getAnimations() const { return scene->mAnimations;	}
		constexpr aiCamera**	const 	getCameras()	const { return scene->mCameras;		}
		constexpr aiLight**		const	getLights()		const { return scene->mLights;		}
		constexpr aiMaterial**	const	getMaterials()	const { return scene->mMaterials;	}
		constexpr aiMesh**		const	getMeshes()		const { return scene->mMeshes;		}
		constexpr aiMetadata*	const 	getMetadata()	const { return scene->mMetaData;	}
		constexpr aiNode*		const 	getRootNode()	const { return scene->mRootNode;	}
		constexpr aiTexture**	const	getTextures()	const { return scene->mTextures;	}

		constexpr u32 getFlags() const	{ return scene->mFlags; }

		constexpr i32 getAnimationCount()	const { return scene->mNumAnimations;	}
		constexpr i32 getCameraCount()		const { return scene->mNumCameras;		}
		constexpr i32 getLightCount()		const { return scene->mNumLights;		}
		constexpr i32 getMaterialCount()	const { return scene->mNumMaterials;	}
		constexpr i32 getMeshCount()		const { return scene->mNumMeshes;		}
		constexpr i32 getTextureCount()		const { return scene->mNumTextures;		}

		constexpr bool hasAnimations()	const { return getAnimations()	&& getAnimationCount()	> 0; }
		constexpr bool hasCameras()		const { return getCameras()		&& getCameraCount()		> 0; }
		constexpr bool hasLights()		const { return getLights()		&& getLightCount()		> 0; }
		constexpr bool hasMaterials()	const { return getMaterials()	&& getMaterialCount()	> 0; }
		constexpr bool hasMeshes()		const { return getMeshes()		&& getMeshCount()		> 0; }
		constexpr bool hasTextures()	const { return getTextures()	&& getTextureCount()	> 0; }

		std::string getErrorString() const {
			return importer.GetErrorString();
		}
		
		Path getFilePath() const {
			return filePath;
		}
		Path getRootPath() const {
			return rootPath;
		}

	private:

		Assimp::Importer importer;
		const aiScene* scene;
		Path filePath;
		Path rootPath;

	};


	/*
	* @class Model
	*/
	class Model
	{
	public:

		bool load(const Path& path, bool flipY = false, bool sRGB = false) {

			ModelParser parser(path, ModelParser::DefaultProcessFlags/* | (flipY ? aiProcess_FlipUVs : 0)*/);

			textureFlipY = flipY;
			textureSRGB = sRGB;

			/*aiScene* scene = (aiScene*)"tua madre";*/

			if (!parser.isLoaded()) {
				Log::error("arcModel", parser.getErrorString());
				return false;
			}

			if (!nodes.parse(parser.getRootNode())) {
				Log::error("arcModel", "Failed to parse nodes");
				return false;
			}

#if ARC_DEBUG
			//Log::info("arcModel", "Node tree:");
			//ITreeNodeDumper::dumpNode(nodes.getRoot());
#endif

			if (!parseMaterials(parser)) {
				Log::error("arcModel", "Failed to parse materials");
				return false;
			}

			if (!parseMeshes(parser)) {
				Log::error("arcModel", "Failed to parse meshes");
				return false;
			}

#if ARC_DEBUG
			//Log::info("arcModel", "Bone tree for first mesh:");
			//ITreeNodeDumper::dumpNode(meshes[0].getRootBone());
#endif

			return true;

		}

		void destroy() {

			for (auto& mesh : meshes) {
				mesh.destroy();
			}

			for (auto& material : materials) {
				material.destroy();
			}

		}

		constexpr NodeCollection& getNodes() {
			return nodes;
		}
		constexpr const NodeCollection& getNodes() const {
			return nodes;
		}

		constexpr Node& getRootNode() {
			return nodes.getRoot();
		}
		constexpr const Node& getRootNode() const {
			return nodes.getRoot();
		}

		constexpr std::deque<Material>& getMaterials() {
			return materials;
		}
		constexpr const std::deque<Material>& getMaterials() const {
			return materials;
		}

		constexpr std::deque<Mesh>& getMeshes() {
			return meshes;
		}
		constexpr const std::deque<Mesh>& getMeshes() const {
			return meshes;
		}

		Material& getMaterial(u32 id) {
			return materials.at(id);
		}
		const Material& getMaterial(u32 id) const {
			return materials.at(id);
		}

		Mesh& getMesh(u32 id) {
			return meshes.at(id);
		}
		const Mesh& getMesh(u32 id) const {
			return meshes.at(id);
		}

		constexpr u32 getNodeCount() const {
			return nodes.getNodeCount();
		}
		u32 getMaterialCount() const {
			return static_cast<u32>(materials.size());
		}
		u32 getMeshCount() const {
			return static_cast<u32>(meshes.size());
		}

	private:

		bool parseMaterials(ModelParser& parser) {

			materials.clear();
			materials.resize(parser.getMaterialCount());

			for (u32 i = 0; i < parser.getMaterialCount(); i++) {
				const aiMaterial* material = parser.getMaterials()[i];

				materials[i].setTextureOptions(textureFlipY, textureSRGB);
				if (!materials[i].parse(material, parser.getRootPath()))
					return false;
			}

			return true;

		}
		bool parseMeshes(ModelParser& parser) {

			meshes.clear();
			meshes.resize(parser.getMeshCount());

			for (u32 i = 0; i < parser.getMeshCount(); i++) {
				const aiMesh* mesh = parser.getMeshes()[i];

				//if (!meshes[i].parse(mesh, nodes.getRoot()))
				//	return false;

				if (!meshes[i].parse(mesh, i, nodes))
					return false;
			}
			
			return true;

		}

	private:

		NodeCollection nodes;
		std::deque<Material> materials;
		std::deque<Mesh> meshes;
		bool textureFlipY;
		bool textureSRGB;

	};

}

#if true

using arcTexture2D			= arc::Texture2D;
using arcTextureHDR			= arc::TextureHDR;
using arcMaterial			= arc::Material;

using arcNode				= arc::Node;
using arcNodeCollection		= arc::NodeCollection;
using arcBone				= arc::Bone;
using arcBoneWeight			= arc::BoneWeight;
//using arcBoneCollection		= arc::BoneCollection;
using arcMesh				= arc::Mesh;
using arcModel				= arc::Model;

using arcTextureParser		= arc::TextureParser;
using arcModelParser		= arc::ModelParser;

#endif