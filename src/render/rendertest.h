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
	void onScroll(float s);

	enum ActionID : int {
		CameraRotLeft = 1,
		CameraRotRight,
		CameraRotDown,
		CameraRotUp,
		CameraMoveLeft,
		CameraMoveRight,
		CameraMoveBackward,
		CameraMoveForward,
		CameraMoveDown,
		CameraMoveUp,
		CameraSpeedUp,
		CameraSlowDown,
		FovIn,
		FovOut,
		QuickScreenshot,
		ReloadShaders,
		ReloadResources,
		ToggleDebug
	};

	enum ModelID : int {
		Mario,
		Luigi,
		CastleGrounds,
		Melascula,
		Galand
	};

private:

	enum class ShaderPass {
		Shadow,
		Main,
		Debug
	};

	void loadShaders();
	void loadResources();

	void saveScreenshot();

	void renderModels(ShaderPass pass);
	void renderNode(Model& model, ModelNode& node, ShaderPass pass);

	void setTextureFilters(u32 modelID, GLE::TextureFilter min, GLE::TextureFilter mag);
	void setTextureWrap(u32 modelID, GLE::TextureWrap wrapU, GLE::TextureWrap wrapV);

	void recalculateProjection();
	void setupFramebuffers();

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
	GLE::Uniform modelNUniform;
	GLE::Uniform modelMVUniform;
	GLE::Uniform modelMVPUniform;
	GLE::Uniform modelDiffuseUniform;
	GLE::Uniform modelLightUniform;
	GLE::Uniform modelShadowMatrixUniform;
	GLE::Uniform modelShadowMapUniform;
	GLE::Uniform modelViewUniform;
	GLE::Uniform modelSrtUniform;
	GLE::Uniform modelBaseColUniform;
	std::vector<Model> models;

	GLE::ShaderProgram debugShader;
	GLE::Uniform debugPUniform;
	GLE::Uniform debugUPUniform;
	GLE::Uniform debugNUniform;
	GLE::Uniform debugMVPUniform;

	GLE::VertexArray screenVertexArray;
	GLE::VertexBuffer screenVertexBuffer;
	GLE::ShaderProgram pprocessShader;
	GLE::Uniform pprocessTextureUniform;
	GLE::Uniform pprocessExposureUniform;

	GLE::ShaderProgram shadowShader;
	GLE::Uniform lightMatrixUniform;

	GLE::Framebuffer renderFramebuffer;
	GLE::Texture2D renderColorTexture;
	GLE::Renderbuffer renderDepthBuffer;

	GLE::Framebuffer shadowFramebuffer;
	GLE::Texture2D shadowDepthTexture;

	Mat4f viewMatrix;
	Mat4f projectionMatrix;

	Mat3f waterSrtMatrix;
	Vec2f waterBaseCol;

	Vec3i camMovement;
	Vec3i camRotation;
	Camera camera;

	u64 frameCounter;
	u32 fbWidth;
	u32 fbHeight;

	float exposure;

	bool showNormals;

	constexpr inline static double nearPlane = 0.1;
	constexpr inline static double farPlane = 1000;
	constexpr inline static Vec3f camStartPos = Vec3f(0.5, 0.5, 10);
	constexpr inline static double camStartAngleH = Math::toRadians(-90);
	constexpr inline static double camStartAngleV = Math::toRadians(0);
	constexpr inline static double camVelocityFast = 0.51;
	constexpr inline static double camVelocitySlow = 0.08;
	constexpr inline static double camRotationScale = 0.06;
	constexpr inline static double fovNormal = 90;
	constexpr inline static double fovZoom = 30;
	constexpr inline static u32 shadowMapSize = 2048;

	static inline double fov = fovNormal;
	static inline double camVelocity = camVelocityFast;

};