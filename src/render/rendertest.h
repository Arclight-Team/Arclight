#pragma once

#include "input/keydefs.h"
#include "render/camera.h"
#include "render/gle/gle.h"
#include "util/matrix.h"


class RenderTest {

public:

	RenderTest();

	void create(u32 w, u32 h);
	void run();
	void destroy();

	void resizeWindowFB(u32 w, u32 h);
	void onCameraKeyAction(KeyAction action);

private:

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

	Mat4f modelMatrix;
	Mat4f viewMatrix;
	Mat4f projectionMatrix;
	Mat4f mvpMatrix;

	Vec3i camMovement;
	Vec3i camRotation;
	Camera camera;

	u64 frameCounter;

	constexpr inline static double fov = 90;
	constexpr inline static double nearPlane = 0.01;
	constexpr inline static double farPlane = 1000;
	constexpr inline static Vec3f camStartPos = Vec3f(0.5, 0.5, 10);
	constexpr inline static double camStartAngleH = Math::toRadians(-90);
	constexpr inline static double camStartAngleV = Math::toRadians(0);
	constexpr inline static double camVelocityScale = 0.2;
	constexpr inline static double camRotationScale = 0.06;

};