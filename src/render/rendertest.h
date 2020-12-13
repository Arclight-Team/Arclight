#pragma once

#include "input/keydefs.h"
#include "render/camera.h"
#include "render/gle/gle.h"
#include "util/matrix.h"

#include "loader.h"


class RenderTest {

public:

	RenderTest();

	void create(u32 w, u32 h);
	void run();
	void destroy();

	void resizeWindowFB(u32 w, u32 h);
	void onKeyAction(KeyAction action);

private:

	void renderModels();
	void renderNode(Model& model, ModelNode& node);

	void setTextureFilters(u32 modelID, GLE::TextureFilter min, GLE::TextureFilter mag);
	void setTextureWrap(u32 modelID, GLE::TextureWrap wrapU, GLE::TextureWrap wrapV);

	void recalculateMVPMatrix();

	GLE::VertexBuffer squareVertexBuffer;
	GLE::VertexArray squareVertexArray;

	GLE::VertexBuffer skyboxVertexBuffer;
	GLE::VertexArray skyboxVertexArray;

	GLE::ShaderProgram basicShader;
	GLE::ShaderProgram cubemapShader;
	GLE::Uniform mvpBasicUniform;
	GLE::Uniform diffuseTextureUniform;
	GLE::Uniform mvpCubemapUniform;
	GLE::Uniform cubemapUniform;
	GLE::Uniform srtUniform;
	GLE::Uniform amongUsFrameUniform;
	GLE::Uniform amongUsTextureUniform;

	GLE::Texture2D eugeneTexture;
	GLE::ArrayTexture2D amongUsTextureArray;
	GLE::Texture2D diffuseTexture;
	GLE::CubemapTexture skyboxTexture;

	GLE::ShaderProgram modelShader;
	GLE::Uniform modelMUniform;
	GLE::Uniform modelMVPUniform;
	GLE::Uniform modelDiffuseUniform;
	GLE::Uniform modelLightUniform;
	GLE::Uniform modelViewUniform;
	std::vector<Model> models;

	Mat4f modelMatrix;
	Mat4f viewMatrix;
	Mat4f projectionMatrix;
	Mat4f mvpMatrix;

	Vec3i camMovement;
	Vec3i camRotation;
	Camera camera;

	u64 frameCounter;
	u32 fbWidth;
	u32 fbHeight;

	constexpr inline static double fov = 90;
	constexpr inline static double nearPlane = 0.01;
	constexpr inline static double farPlane = 1000;
	constexpr inline static Vec3f camStartPos = Vec3f(0.5, 0.5, 10);
	constexpr inline static double camStartAngleH = Math::toRadians(-90);
	constexpr inline static double camStartAngleV = Math::toRadians(0);
	constexpr inline static double camVelocityScale = 0.2;
	constexpr inline static double camRotationScale = 0.06;

};