#ifndef AXRSHADER_H
#define AXRSHADER_H

#include "types.h"
#include "amdmodel.h"

#include <unordered_map>
#include <variant>


enum class AXRShaderFeature {
	TransformMVP,
	DiffuseMapping,
	DiffuseUvChannel,
	None
};



struct AXRShaderAttribute {

};



struct AMDModel;

class AXRShader {

public:

	constexpr static inline u32 featureDisabled = 0;
	constexpr static inline u32 featureEnabled = 1;

	AXRShader();

	void setShading(AMDShading shading);
	void setBlendMode(AMDBlendMode blendMode);

	void addFeature(AXRShaderFeature feature);
	void removeFeature(AXRShaderFeature feature);
	void setFeature(AXRShaderFeature feature, u32 value);

	void addAttribute(AMDAttributeType attrType, AMDDataType dataType);

	bool create();
	void destroy();

	void render();

	static AXRShader generateShader(const AMDModel& model, u32 meshID, AMDShading shading, AMDBlendMode blendMode);

	u32 getFeature(AXRShaderFeature feature) const;
	bool hasFeature(AXRShaderFeature feature) const;

private:

	static std::string getShaderVersionString();
	void reset();

	std::unordered_map<AXRShaderFeature, u32> features;
	std::vector<AMDAttribute> attributes;
	AMDShading renderShading;
	AMDBlendMode renderBlendMode;
	bool valid;

};

#endif // AXRSHADER_H
