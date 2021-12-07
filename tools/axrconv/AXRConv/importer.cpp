#include "importer.h"
#include "importdialog.h"
#include "amdmodel.h"

#include <QString>
#include <QImage>
#include <QDir>
#include <QDebug>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


Importer::Importer() : imported(false) {}



AMDModel Importer::import(const QString& path, const ImportConfiguration &config){

	AMDModel model;
	error = "";
	imported = true;

	if(path.isEmpty()){
		setImportError("Path is empty");
		return model;
	}

	Assimp::Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, config.maxWeigths);

	unsigned int flags = aiProcess_ValidateDataStructure
					| aiProcess_SortByPType
                    | aiProcess_OptimizeMeshes
					| aiProcess_Triangulate
					| aiProcess_ImproveCacheLocality
					| aiProcess_GenBoundingBoxes
					| aiProcess_LimitBoneWeights;

    if(!config.flipUVs){
		flags |= aiProcess_FlipUVs;
	}

	if(config.genNormals && !config.smoothNormals){
		flags |= aiProcess_GenNormals;
	}else if(config.genNormals && config.smoothNormals){
		flags |= aiProcess_GenSmoothNormals;
	}

	if(config.genTangents){
		flags |= aiProcess_CalcTangentSpace;
	}


	const aiScene* scene = importer.ReadFile(path.toStdString().c_str(), flags);

	if(!scene){
		setImportError(importer.GetErrorString());
		return model;
	}

	u32 idCounter = 0;
	processAMDNode(model, scene->mRootNode, idCounter, 0);

	std::unordered_map<QString, u32> textureLinks;

	for(u32 i = 0; i < scene->mNumMaterials; i++){

		aiMaterial* sceneMaterial = scene->mMaterials[i];

		float propFloat = 0;
		int propInt = 0;
		aiColor3D propColor;

		AMDMaterial material;
		material.name = sceneMaterial->GetName().length ? sceneMaterial->GetName().C_Str() : "material_" + std::to_string(i);
		material.shared = false;
		material.flags = 0;

		sceneMaterial->Get(AI_MATKEY_OPACITY, &propFloat, nullptr);
		sceneMaterial->Get(AI_MATKEY_TWOSIDED, &propInt, nullptr);
		material.blendMode = (propFloat == 1.0) ? AMDBlendMode::Solid : AMDBlendMode::Transparent;
		material.alphaThreshold = 1.0;

		if(material.blendMode != AMDBlendMode::Solid || propInt){
			material.flags |= AMDMaterialFlags::DisableCulling;
		}

		sceneMaterial->Get(AI_MATKEY_COLOR_AMBIENT, &propColor, nullptr);
		material.ambientColor = convertColor(&propColor);
		sceneMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, &propColor, nullptr);
		material.diffuseColor = convertColor(&propColor);
		sceneMaterial->Get(AI_MATKEY_COLOR_SPECULAR, &propColor, nullptr);
		material.specularColor = convertColor(&propColor);
		sceneMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, &propColor, nullptr);
		material.emissiveColor = convertColor(&propColor);
		sceneMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, &propColor, nullptr);
		material.transparentColor = convertColor(&propColor);
		sceneMaterial->Get(AI_MATKEY_COLOR_REFLECTIVE, &propColor, nullptr);
		material.reflectiveColor = convertColor(&propColor);

		sceneMaterial->Get(AI_MATKEY_SHININESS, &propFloat, nullptr);
		material.shininess = propFloat;
		sceneMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, &propFloat, nullptr);
		material.specularFactor = propFloat;
		sceneMaterial->Get(AI_MATKEY_REFLECTIVITY, &propFloat, nullptr);
		material.reflectivity = propFloat;
		sceneMaterial->Get(AI_MATKEY_REFRACTI, &propFloat, nullptr);
		material.refractionIndex = propFloat;

		for(u32 j = aiTextureType_DIFFUSE; j < aiTextureType_UNKNOWN; j++){

			aiTextureType type = static_cast<aiTextureType>(j);

			u32 count = sceneMaterial->GetTextureCount(type);
			aiString propPath;
			u32 uvChannel;

			for(u32 k = 0; k < count; k++){

				sceneMaterial->GetTexture(type, k, &propPath, nullptr, &uvChannel);
				QString texPath = propPath.C_Str();

				if(!QDir::isAbsolutePath(texPath)){

					QString parentPath = path.left(path.lastIndexOf('/'));
					texPath = parentPath + '/' + texPath;

				}

				if(!textureLinks.count(texPath)){

					AMDTexture texture;
					texture.name = "texture_" + std::to_string(textureLinks.size());
					texture.loaded = false;
                    texture.flags = AMDTextureFlags::GenerateMipmaps;
					texture.width = 0;
					texture.height = 0;
					texture.format = AMDTextureFormat::RGB888;
					texture.mappingU = AMDTextureMapping::Repeat;
					texture.mappingV = AMDTextureMapping::Repeat;
					texture.filterMin = AMDTextureMinFiltering::None;
					texture.filterMag = AMDTextureMagFiltering::None;

					switch(type){

						case aiTextureType_DIFFUSE:
							texture.type = AMDTextureType::Diffuse;
							break;

						case aiTextureType_SPECULAR:
							texture.type = AMDTextureType::Specular;
							break;

						case aiTextureType_AMBIENT:
							texture.type = AMDTextureType::Ambient;
							break;

						case aiTextureType_EMISSIVE:
							texture.type = AMDTextureType::Emissive;
							break;

						case aiTextureType_NORMALS:
							texture.type = AMDTextureType::Normal;
							break;

						case aiTextureType_SHININESS:
							texture.type = AMDTextureType::Shininess;
							break;

						case aiTextureType_OPACITY:
							texture.type = AMDTextureType::Alpha;
							break;

						case aiTextureType_DISPLACEMENT:
							texture.type = AMDTextureType::Displacement;
							break;

						case aiTextureType_LIGHTMAP:
							texture.type = AMDTextureType::Lightmap;
							break;

						case aiTextureType_REFLECTION:
							texture.type = AMDTextureType::Reflection;
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
							texture.type = AMDTextureType::Undefined;
							break;

					}

					loadTexture(texture, texPath);

					textureLinks[texPath] = model.textures.size();
					model.textures.push_back(texture);

				}

				material.textureIDs.push_back(textureLinks[texPath]);
				material.uvChannels.push_back(uvChannel);

			}

		}

		model.materials.push_back(material);

	}

	for(u32 i = 0; i < scene->mNumMeshes; i++) {

		aiMesh* sceneMesh = scene->mMeshes[i];

		if(!sceneMesh->mNumFaces){
			continue;
		}

		u32 primitiveSize = sceneMesh->mFaces[0].mNumIndices;

		AMDMesh mesh;
		mesh.indexed = false;
		mesh.primType = getPrimitiveMode(sceneMesh);
		mesh.vertexCount = sceneMesh->mNumFaces * primitiveSize;
        mesh.name = sceneMesh->mName.C_Str();

		addAttribute(mesh, sceneMesh, AMDAttributeType::Position);

		for(u32 j = 0; j < AI_MAX_NUMBER_OF_COLOR_SETS; j++){

			if(sceneMesh->HasVertexColors(j))   {
				addAttribute(mesh, sceneMesh, AMD_ATTR_COLOR(j));
			}else{
				break;
			}

		}

		for(u32 j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; j++){

			if(sceneMesh->HasTextureCoords(j))   {
				addAttribute(mesh, sceneMesh, AMD_ATTR_UV(j));
			}else{
				break;
			}

		}

		if(sceneMesh->HasNormals()){
			addAttribute(mesh, sceneMesh, AMDAttributeType::Normal);
		}

		if(sceneMesh->HasTangentsAndBitangents()){
			addAttribute(mesh, sceneMesh, AMDAttributeType::Tangent);
			addAttribute(mesh, sceneMesh, AMDAttributeType::Bitangent);
		}

		if(sceneMesh->HasBones()){

			for(u32 j = 0; j < config.maxWeigths; j++){
				addAttribute(mesh, sceneMesh, AMDAttributeType::BoneIndex);
				addAttribute(mesh, sceneMesh, AMDAttributeType::BoneWeight);
			}

		}

		mesh.materialID = sceneMesh->mMaterialIndex;
		model.meshes.emplace_back(mesh);

	}

	return model;

}




u32 Importer::processAMDNode(AMDModel& model, const aiNode* sceneNode, u32& nodeID, u32 parentID){

	u32 id = nodeID;

	model.nodes[id].name = sceneNode->mName.C_Str();
	model.nodes[id].parentID = parentID;

	nodeID++;

	for(u32 i = 0; i < sceneNode->mNumChildren; i++){

		u32 childID = processAMDNode(model, sceneNode->mChildren[i], nodeID, id);
		model.nodes[id].childIDs.push_back(childID);

	}

	for(u32 i = 0; i < sceneNode->mNumMeshes; i++){
		model.nodes[id].meshIDs.push_back(sceneNode->mMeshes[i]);
	}

	return id;

}



void Importer::loadTexture(AMDTexture& texture, const QString& path){

	QImage image;

    QString baseName = QFileInfo(path).fileName();
    baseName.truncate(baseName.indexOf('.'));

    texture.name = baseName.toStdString();

	if(!image.load(path)){
		return;
	}

	texture.width = image.width();
	texture.height = image.height();

	switch(image.format()){

		case QImage::Format_RGB16:
			texture.format = AMDTextureFormat::RGB565;
			break;

		case QImage::Format_RGB888:
			texture.format = AMDTextureFormat::RGB888;
			break;

		case QImage::Format_RGBA8888:
			texture.format = AMDTextureFormat::RGBA8888;
			break;

		default:

			if(image.hasAlphaChannel()){
				image = image.convertToFormat(QImage::Format_RGBA8888);
				texture.format = AMDTextureFormat::RGBA8888;
			}else{
				image = image.convertToFormat(QImage::Format_RGB888);
				texture.format = AMDTextureFormat::RGB888;
			}

			break;

	}

	u32 size = image.sizeInBytes();
	texture.data.resize(size);
	std::copy(image.bits(), image.bits() + size, texture.data.data());

	texture.loaded = true;

}



void Importer::setImportError(const QString& msg){
	error = msg;
	imported = false;
}



QString Importer::getErrorString() const {
	return error;
}



bool Importer::validImport() const {
	return imported;
}



AMDPrimitiveMode Importer::getPrimitiveMode(const aiMesh* mesh) {

	switch(mesh->mPrimitiveTypes) {

		case aiPrimitiveType_POINT:
			return AMDPrimitiveMode::Points;

		case aiPrimitiveType_LINE:
			return AMDPrimitiveMode::Lines;

		default:
		case aiPrimitiveType_TRIANGLE:
			return AMDPrimitiveMode::Triangles;

	}

}



void Importer::addAttribute(AMDMesh& mesh, const aiMesh* sceneMesh, AMDAttributeType type){

	u32 primitiveSize = sceneMesh->mFaces[0].mNumIndices;
	u32 faceCount = sceneMesh->mNumFaces;
	u32 vertexCount = faceCount * primitiveSize;

	u8 attrElements = 0;
	u32 attrSize = 0;
	u32 attrStride = 0;
	AMDDataType dataType = AMDDataType::Float;
	void* attrData = nullptr;

	switch(type){

		case AMDAttributeType::Position:

			attrSize = sizeof(float);
			attrElements = 3;
			attrData = sceneMesh->mVertices;
			dataType = AMDDataType::Float;

			break;

		case AMDAttributeType::Normal:

			attrSize = sizeof(float);
			attrElements = 3;
			attrData = sceneMesh->mNormals;
			dataType = AMDDataType::Float;

			break;

		case AMDAttributeType::Tangent:

			attrSize = sizeof(float);
			attrElements = 3;
			attrData = sceneMesh->mTangents;
			dataType = AMDDataType::Float;

			break;

		case AMDAttributeType::Bitangent:

			attrSize = sizeof(float);
			attrElements = 3;
			attrData = sceneMesh->mBitangents;
			dataType = AMDDataType::Float;

			break;

		case AMD_ATTR_COLOR(0):
		case AMD_ATTR_COLOR(1):
		case AMD_ATTR_COLOR(2):
		case AMD_ATTR_COLOR(3):
		case AMD_ATTR_COLOR(4):
		case AMD_ATTR_COLOR(5):
		case AMD_ATTR_COLOR(6):
		case AMD_ATTR_COLOR(7):

			attrSize = sizeof(float);
			attrElements = 4;
			attrData = sceneMesh->mColors[AMD_ATTR_GET_COLOR(type)];
			dataType = AMDDataType::Float;

			break;

		case AMD_ATTR_UV(0):
		case AMD_ATTR_UV(1):
		case AMD_ATTR_UV(2):
		case AMD_ATTR_UV(3):
		case AMD_ATTR_UV(4):
		case AMD_ATTR_UV(5):
		case AMD_ATTR_UV(6):
		case AMD_ATTR_UV(7):

			attrSize = sizeof(float);
			attrElements = 2;
			attrStride = sizeof(float);
			attrData = sceneMesh->mTextureCoords[AMD_ATTR_GET_UV(type)];
			dataType = AMDDataType::Float;

			break;
/*
		case AMDAttributeType::BoneIndex:

			attrSize = sizeof(float);
			attrElements = 3;
			attrData = sceneMesh->mBones;
			dataType = AMDDataType::UInt;

			break;

		case AMDAttributeType::BoneWeight:

			attrSize = sizeof(float);
			attrElements = 3;
			attrData = sceneMesh->mBones;
			dataType = AMDDataType::UInt;

			break;
*/
		default:
			return;

	}

	u32 attrBytes = attrSize * attrElements;
	u32 attrTotal = attrBytes + attrStride;

	mesh.attributes.push_back(AMDAttribute { type, AMD_DATA_TYPE(dataType, attrElements) });
	mesh.meshData.push_back(std::vector<u8>(vertexCount * attrBytes));

	std::vector<u8>& meshAttrData = mesh.meshData[mesh.meshData.size() - 1];

	for(u32 i = 0; i < faceCount; i++){

		for(u32 j = 0; j < primitiveSize; j++){
			std::memcpy(&meshAttrData[(i * primitiveSize + j) * attrBytes], static_cast<u8*>(attrData) + sceneMesh->mFaces[i].mIndices[j] * attrTotal, attrBytes);
		}

	}

}



AMDVector3 Importer::convertColor(const aiColor3D* color){
	return AMDVector3{color->r, color->g, color->b};
}
