#pragma once

#include "util/vector.h"


//PL = Point Light
//SL = Spot Light
//DL = Directional Light
struct PointLight {

	constexpr PointLight(const Vec3f& position, const Vec3f& color, float radius, float intensity) :
		position(position), color(color), radius(radius), intensity(intensity) {};

	Vec3f position;
	Vec3f color;
	float radius;
	float intensity;

};

struct DirectionalLight {

	constexpr DirectionalLight(const Vec3f& direction, const Vec3f& color, float intensity) :
		direction(direction), color(color), intensity(intensity) {};

	Vec3f direction;
	Vec3f color;
	float intensity;

};

struct SpotLight {

	constexpr SpotLight(const Vec3f& position, const Vec3f& direction, const Vec3f& color, float outerAngle, float innerAngle, float radius, float intensity) :
		position(position), direction(direction), color(color), outerAngle(outerAngle), innerAngle(innerAngle), radius(radius), intensity(intensity) {};

	Vec3f position;
	Vec3f direction;
	Vec3f color;
	float outerAngle;
	float innerAngle;
	float radius;
	float intensity;

};

namespace Lights {

	constexpr u32 maxPointLights = 4;
	constexpr u32 maxDirectionalLights = 4;
	constexpr u32 maxSpotLights = 4;
	constexpr u32 pointLightDataSize = 48;
	constexpr u32 directionalLightDataSize = 32;
	constexpr u32 spotLightDataSize = 64;
	constexpr u32 lightEndPadding = 4;
	constexpr u32 uniformBindingIndex = 0;

	void createLightBuffer();
	void destroyLightBuffer();

	void addLight(const PointLight& light);
	void addLight(const DirectionalLight& light);
	void addLight(const SpotLight& light);
	void setLight(u32 id, const PointLight& light);
	void setLight(u32 id, const DirectionalLight& light);
	void setLight(u32 id, const SpotLight& light);
	void updateLights();


}