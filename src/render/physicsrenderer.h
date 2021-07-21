#pragma once

#include "gle/gle.h"
#include "atr/camera.h"
#include "renderer.h"
#include "util/matrix.h"
#include "util/profiler.h"
#include "input/keydefs.h"
#include "model/model.h"
#include "input/inputhandler.h"
#include "atr/simplecamera.h"


class ActorManager;
class InputContext;

class PhysicsRenderer : public Renderer {

public:

	PhysicsRenderer(ActorManager& actorManager);

	virtual bool init() override;
	virtual void render() override;
	virtual void destroy() override;

	void setAspectRatio(float aspect);
	void setCameraMatrix(const Vec3f& pos, const Vec3f& lookat);

	void setupCamera(InputContext& context);

private:

	void renderModel(arcModel& model, const Mat4f& transform);
	void renderNode(arcModel& model, arcNode& node, const Mat4f& transform);
	void renderMesh(arcModel& model, arcMesh& mesh, const Mat4f& transform);
	void applyMaterial(arcModel& model, arcMaterial& material);

	ActorManager& actorManager;

	GLE::ShaderProgram modelShader;

	GLE::Uniform modelMVPUniform;
	GLE::Uniform modelMVUniform;
	GLE::Uniform modelNormalUniform;
	GLE::Uniform modelBoneTransformsUniform;
	GLE::Uniform modelUseBonesUniform;
	GLE::Uniform modelDiffuseTexUniform;
	GLE::Uniform modelAmbientTexUniform;
	GLE::Uniform modelSpecularTexUniform;
	GLE::Uniform modelNormalTexUniform;
	GLE::Uniform modelBumpSelectUniform;
	GLE::Uniform modelAmbientEnableUniform;
	GLE::Uniform modelDiffuseEnableUniform;
	GLE::Uniform modelNormalEnableUniform;
	GLE::Uniform modelSpecularEnableUniform;
	GLE::Uniform modelLightColorUniform;
	GLE::Uniform modelLightPositionUniform;
	GLE::Uniform modelViewPositionUniform;
	GLE::Uniform lightPositionUniform;
	GLE::Uniform lightColorUniform;

	arcModel model;
	std::vector<Mat4f> boneTransforms;

	u64 startTime;

	Vec3i camMovement;
	Vec3i camRotation;

	Mat4f projMatrix;
	Mat4f viewMatrix;

	Profiler profiler;
	InputHandler inputHandler;
	SimpleCamera camera;

	constexpr static double camRotationScale = 0.01;
	constexpr static double camVelocity = 0.04;

};