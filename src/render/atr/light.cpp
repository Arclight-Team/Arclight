#include "light.h"
#include "render/gle/gle.h"
#include "util/math.h"



namespace {

	GLE::UniformBuffer buffer;
	std::vector<PointLight> pls;
	std::vector<DirectionalLight> dls;
	std::vector<SpotLight> sls;

	u32 maxPointLightID = 0;
	u32 maxDirectionalLightID = 0;
	u32 maxSpotLightID = 0;

}



void Lights::createLightBuffer() {

	buffer.create();
	buffer.bind();
	buffer.allocate(Lights::maxPointLights * Lights::pointLightDataSize + Lights::maxDirectionalLights * Lights::directionalLightDataSize + Lights::maxSpotLights * Lights::spotLightDataSize - Lights::lightEndPadding + 12, GLE::BufferAccess::DynamicDraw);
	buffer.bindRange(Lights::uniformBindingIndex, 0, buffer.getSize());

}



void Lights::destroyLightBuffer() {
	buffer.destroy();
}



void Lights::addLight(const PointLight& light) {

	if (pls.size() == Lights::maxPointLights) {
		Log::warn("Lights", "Point light array is full");
		return;
	}

	pls.push_back(light);

}



void Lights::addLight(const DirectionalLight& light) {

	if (dls.size() == Lights::maxDirectionalLights) {
		Log::warn("Lights", "Directional light array is full");
		return;
	}

	dls.push_back(light);

}



void Lights::addLight(const SpotLight& light) {

	if (sls.size() == Lights::maxSpotLights) {
		Log::warn("Lights", "Spot light array is full");
		return;
	}

	sls.push_back(light);

}



void Lights::setLight(u32 id, const PointLight& light) {

	if (id >= pls.size()) {
		Log::warn("Lights", "Point light array index out of range");
		return;
	}

	pls[id] = light;

}



void Lights::setLight(u32 id, const DirectionalLight& light) {

	if (id >= dls.size()) {
		Log::warn("Lights", "Directional light array index out of range");
		return;
	}

	dls[id] = light;

}



void Lights::setLight(u32 id, const SpotLight& light) {

	if (id >= sls.size()) {
		Log::warn("Lights", "Spot light array index out of range");
		return;
	}

	sls[id] = light;

}



PointLight& Lights::getPointLight(u32 id) {

	if (id >= pls.size()) {
		Log::error("Lights", "Point light array index out of range");
	}

	return pls[id];

}



DirectionalLight& Lights::getDirectionalLight(u32 id) {

	if (id >= dls.size()) {
		Log::error("Lights", "Directional light array index out of range");
	}

	return dls[id];

}



SpotLight& Lights::getSpotLight(u32 id) {

	if (id >= sls.size()) {
		Log::error("Lights", "Spot light array index out of range");
	}

	return sls[id];

}



void Lights::updateLights(const Mat4f& viewMatrix, const Mat3f& normalMatrix) {

	Mat3f lightViewMatrix = viewMatrix.toMat3();
	float* lightData = new float[buffer.getSize() / 4];
	u32 offset = 0;

	for (u32 i = 0; i < pls.size(); i++) {

		Vec4f viewPos = viewMatrix * Vec4f(pls[i].position.x, pls[i].position.y, pls[i].position.z, 1.0);

		lightData[offset + 0] = viewPos.x;
		lightData[offset + 1] = viewPos.y;
		lightData[offset + 2] = viewPos.z;
		lightData[offset + 4] = pls[i].color.x;
		lightData[offset + 5] = pls[i].color.y;
		lightData[offset + 6] = pls[i].color.z;
		lightData[offset + 7] = pls[i].radius;
		lightData[offset + 8] = pls[i].intensity;

		offset += Lights::pointLightDataSize / 4;

	}

	offset = Lights::pointLightDataSize * maxPointLights / 4;

	for (u32 i = 0; i < dls.size(); i++) {

		Vec3f viewDir = Vec3f(normalMatrix * dls[i].direction).normalized();

		lightData[offset + 0] = viewDir.x;
		lightData[offset + 1] = viewDir.y;
		lightData[offset + 2] = viewDir.z;
		lightData[offset + 4] = dls[i].color.x;
		lightData[offset + 5] = dls[i].color.y;
		lightData[offset + 6] = dls[i].color.z;
		lightData[offset + 7] = dls[i].intensity;

		offset += Lights::directionalLightDataSize / 4;

	}

	offset = Lights::pointLightDataSize * maxPointLights / 4 
		   + Lights::directionalLightDataSize * maxDirectionalLights / 4;

	for (u32 i = 0; i < sls.size(); i++) {

		Vec4f viewPos = viewMatrix * Vec4f(sls[i].position.x, sls[i].position.y, sls[i].position.z, 1.0);
		Vec3f viewDir = Vec3f(normalMatrix * sls[i].direction).normalized();

		lightData[offset + 0] = viewPos.x;
		lightData[offset + 1] = viewPos.y;
		lightData[offset + 2] = viewPos.z;
		lightData[offset + 4] = viewDir.x;
		lightData[offset + 5] = viewDir.y;
		lightData[offset + 6] = viewDir.z;
		lightData[offset + 8] = sls[i].color.x;
		lightData[offset + 9] = sls[i].color.y;
		lightData[offset + 10] = sls[i].color.z;
		lightData[offset + 11] = sls[i].outerAngle;
		lightData[offset + 12] = sls[i].innerAngle;
		lightData[offset + 13] = sls[i].radius;
		lightData[offset + 14] = sls[i].intensity;

		offset += Lights::spotLightDataSize / 4;

	}

	offset = Lights::pointLightDataSize * maxPointLights / 4
		+ Lights::directionalLightDataSize * maxDirectionalLights / 4;
		+ Lights::spotLightDataSize * maxSpotLights / 4
		- Lights::lightEndPadding / 4;

	u32 sizes[3];
	sizes[0] = pls.size();
	sizes[1] = dls.size();
	sizes[2] = sls.size();

	buffer.bind();
	buffer.update(0, buffer.getSize() - 12, lightData);
	buffer.update(buffer.getSize() - 12, 12, sizes);

	delete[] lightData;

}