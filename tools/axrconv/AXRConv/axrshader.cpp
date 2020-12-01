#include "axrshader.h"

#define AXRS_ADD_FEATURE(f, v) {AXRShaderFeature::f, v}



AXRShader::AXRShader() : renderShading(AMDShading::BlinnPhong), renderBlendMode(AMDBlendMode::Solid), valid(false) {

	for(u32 i = static_cast<u32>(AXRShaderFeature::TransformMVP); i < static_cast<u32>(AXRShaderFeature::None); i++){
		features[static_cast<AXRShaderFeature>(i)] = featureDisabled;
	}

}



void AXRShader::setShading(AMDShading shading){
	renderShading = shading;
}



void AXRShader::setBlendMode(AMDBlendMode blendMode){
	renderBlendMode = blendMode;
}



void AXRShader::addFeature(AXRShaderFeature feature){
	features[feature] = featureEnabled;
}



void AXRShader::removeFeature(AXRShaderFeature feature){
	features[feature] = featureDisabled;
}



void AXRShader::setFeature(AXRShaderFeature feature, u32 value){
	features[feature] = value;
}



bool AXRShader::create(){
	return false;
}



void AXRShader::destroy(){

}



void AXRShader::render(){

}



AXRShader AXRShader::generateShader(const AMDModel& model, u32 meshID, AMDShading shading, AMDBlendMode blendMode){

	AXRShader shader;

	const AMDMesh& mesh = model.meshes[meshID];
	const AMDMaterial& material = model.materials[mesh.materialID];
	bool hasVertexAttributes = false;
	u8 uvMask = 0;

	for(u32 i = 0; i < mesh.attributes.size(); i++){

		const AMDAttribute& attr = mesh.attributes[i];

		//If we have duplicate vertices, abort since we don't know what to do with them
		if(hasVertexAttributes){
			return AXRShader();
		}

		//Vertex positions are common across all
		if(attr.type == AMDAttributeType::Position && AMD_ATTR_GET_TYPE(attr.dataType) == AMDDataType::Float){

			switch(AMD_ATTR_GET_ELEMENTS(attr.dataType)){

				case 2:
					//Must be screen space
					shader.removeFeature(AXRShaderFeature::TransformMVP);
					break;

				case 3:
					//Must be world space
					shader.addFeature(AXRShaderFeature::TransformMVP);
					break;

				default:
					//1 or 4 is weird
					return AXRShader();

			}

			hasVertexAttributes = true;

		}

		if(AMD_ATTR_IS_UV(attr.type)){
			uvMask |= 1 << AMD_ATTR_GET_UV(attr.type);
		}

	}

	for(u32 i = 0; i < material.textureIDs.size(); i++){

		const AMDTexture& texture = model.textures[material.textureIDs[i]];
		u8 uvChannel = material.uvChannels[i];

		//Check if we got a diffuse map and whether it has a linked UV channel
		if(texture.type == AMDTextureType::Diffuse && (uvMask & (1 << uvChannel))){

			shader.addFeature(AXRShaderFeature::DiffuseMapping);
			shader.setFeature(AXRShaderFeature::DiffuseUvChannel, uvChannel);

		}

	}

	return AXRShader();
}



u32 AXRShader::getFeature(AXRShaderFeature feature){
	return 0;
}
