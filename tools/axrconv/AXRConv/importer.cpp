#include "importer.h"
#include "importdialog.h"
#include "amdmodel.h"

#include <QString>

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
                    | aiProcess_FindInvalidData
                    | aiProcess_OptimizeMeshes
                    | aiProcess_Triangulate
                    | aiProcess_ImproveCacheLocality
                    | aiProcess_GenBoundingBoxes
                    | aiProcess_LimitBoneWeights;

    if(config.flipUVs){
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

    mesh.attributes.push_back(AMDAttribute { type, AMD_DATA_TYPE(dataType, attrElements) });
    mesh.meshData.push_back(std::vector<u8>(vertexCount * attrBytes));

    std::vector<u8>& meshAttrData = mesh.meshData[mesh.meshData.size() - 1];

    for(u32 i = 0; i < faceCount; i++){

        for(u32 j = 0; j < primitiveSize; j++){
            std::memcpy(&meshAttrData[(i * primitiveSize + j) * attrBytes], static_cast<u8*>(attrData) + sceneMesh->mFaces[i].mIndices[j] * attrBytes, attrBytes);
        }

    }

}
