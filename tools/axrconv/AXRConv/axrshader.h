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

	bool create();
	void destroy();

	void render();

	static AXRShader generateShader(const AMDModel& model, u32 meshID, AMDShading shading, AMDBlendMode blendMode);

	u32 getFeature(AXRShaderFeature feature);

private:

	std::unordered_map<AXRShaderFeature, u32> features;
	AMDShading renderShading;
	AMDBlendMode renderBlendMode;
	bool valid;

};

#endif // AXRSHADER_H
