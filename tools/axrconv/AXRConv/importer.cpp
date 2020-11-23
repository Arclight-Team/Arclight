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

    unsigned int flags = aiProcess_ValidateDataStructure
                    | aiProcess_SortByPType
                    | aiProcess_FindInvalidData
                    | aiProcess_OptimizeMeshes
                    | aiProcess_Triangulate
                    | aiProcess_ImproveCacheLocality
                    | aiProcess_GenBoundingBoxes;

    if(config.flipUVs){
        flags |= aiProcess_FlipUVs;
    }

    if(config.genNormals && !config.smoothNormals){
        flags |= aiProcess_GenNormals;
    }else if(config.genNormals && config.smoothNormals){
        flags |= aiProcess_GenSmoothNormals;
    }

    if(config.limitWeigths){
        importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, config.maxWeigths);
        flags |= aiProcess_LimitBoneWeights;
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

        mesh.attributes.push_back(AMDAttribute { AMDAttributeType::Position, AMD_DATA_TYPE(AMDDataType::Float, 3) });
        mesh.meshData.push_back(std::vector<u8>(mesh.vertexCount * 4 * 3));

        for(u32 j = 0; j < sceneMesh->mNumFaces; j++){

            for(u32 k = 0; k < primitiveSize; k++){

                for(u32 l = 0; l < 3; l++){
                    addAttributeData(mesh.meshData[0], ((j * primitiveSize + k) * 3 + l) * sizeof(float), sceneMesh->mVertices[sceneMesh->mFaces->mIndices[k]]);
                }

            }

        }

        for(u32 j = 0; j < AI_MAX_NUMBER_OF_COLOR_SETS; j++){

            if(sceneMesh->HasVertexColors(j))   {
                mesh.attributes.push_back(AMDAttribute { AMD_ATTR_COLOR(j), AMD_DATA_TYPE(AMDDataType::Float, 4) });
            }else{
                break;
            }

        }

        for(u32 j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; j++){

            if(sceneMesh->HasTextureCoords(j))   {
                mesh.attributes.push_back(AMDAttribute { AMD_ATTR_UV(j), AMD_DATA_TYPE(AMDDataType::Float, 2) });
            }else{
                break;
            }

        }

        if(sceneMesh->HasNormals()){
            mesh.attributes.push_back(AMDAttribute { AMDAttributeType::Normal, AMD_DATA_TYPE(AMDDataType::Float, 3) });
        }

        if(sceneMesh->HasTangentsAndBitangents()){
            mesh.attributes.push_back(AMDAttribute { AMDAttributeType::Tangent, AMD_DATA_TYPE(AMDDataType::Float, 3) });
            mesh.attributes.push_back(AMDAttribute { AMDAttributeType::Bitangent, AMD_DATA_TYPE(AMDDataType::Float, 3) });
        }

        if(sceneMesh->HasBones()){

            for(u32 j = 0; j < config.maxWeigths; j++){
                mesh.attributes.push_back(AMDAttribute { AMDAttributeType::BoneWeight, AMD_DATA_TYPE(AMDDataType::Float, 1) });
                mesh.attributes.push_back(AMDAttribute { AMDAttributeType::BoneIndex, AMD_DATA_TYPE(AMDDataType::UInt, 1) });
            }

        }

        for(u32 j = 0; j < sceneMesh->mNumFaces; j++){

        }

        mesh.meshData.resize(mesh.attributes.size());
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
        model.nodes[i].meshIDs.push_back(sceneNode->mMeshes[i]);
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
