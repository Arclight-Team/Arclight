#pragma once

#include "gle/gle.h"
#include "atr/camera.h"
#include "renderer.h"
#include "util/matrix.h"
#include "util/profiler.h"
#include "input/keydefs.h"


class ActorManager;

class PhysicsRenderer : public Renderer {

public:

	enum ActionID {
		CameraRotLeft = 1,
		CameraRotRight,
		CameraRotDown,
		CameraRotUp,
		CameraMoveLeft,
		CameraMoveRight,
		CameraMoveBackward,
		CameraMoveForward,
		CameraMoveDown,
		CameraMoveUp
	};

	PhysicsRenderer(ActorManager& actorManager);

	virtual bool init() override;
	virtual void render() override;
	virtual void destroy() override;

	void setAspectRatio(float aspect);
	void setCameraMatrix(const Vec3f& pos, const Vec3f& lookat);

	void onKeyAction(KeyAction action);

private:

	ActorManager& actorManager;

	GLE::ShaderProgram objectShader;
	GLE::VertexArray objectVA;
	GLE::VertexBuffer objectVB;
	GLE::VertexBuffer offsetVB;

	GLE::Uniform mvpMatrixUniform;

	Camera camera;
	Vec3i camMovement;
	Vec3i camRotation;

	Mat4f projMatrix;
	Mat4f viewMatrix;

	Profiler profiler;

	u32 prevObjects;
	std::vector<float> modelMatrixBuffer;

	constexpr static double camRotationScale = 0.06;
	constexpr static double camVelocity = 1;

};