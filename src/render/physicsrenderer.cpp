#include "physicsrenderer.h"
#include "utility/shaderloader.h"
#include "utility/vertexhelper.h"
#include "core/acs/actormanager.h"
#include "input/inputcontext.h"
#include "debug.h"


PhysicsRenderer::PhysicsRenderer(ActorManager& actorManager) : actorManager(actorManager), startTime(0) {}


bool PhysicsRenderer::init() {

	try {

		modelShader = ShaderLoader::fromFiles(":/shaders/test/model_assimp.avs", ":/shaders/test/model_assimp.afs");

	} catch (std::exception& e) {
		Log::error("Physics Renderer", e.what());
		return false;
	}

	if(!model.load(":/models/cube/cube.obj", false)) {
		Log::error("Physics Renderer", "Failed to load model");
		return false;
	}

	modelMVPUniform				= modelShader.getUniform("mvpMatrix");
	modelMVUniform              = modelShader.getUniform("mvMatrix");
	modelNormalUniform          = modelShader.getUniform("normalMatrix");
	modelBoneTransformsUniform  = modelShader.getUniform("boneTransforms");
	modelUseBonesUniform        = modelShader.getUniform("useBones");
	modelDiffuseTexUniform      = modelShader.getUniform("diffuseTexture");
	modelAmbientTexUniform      = modelShader.getUniform("ambientTexture");
	modelSpecularTexUniform     = modelShader.getUniform("specularTexture");
	modelNormalTexUniform       = modelShader.getUniform("normalTexture");
	modelBumpSelectUniform      = modelShader.getUniform("bumpMapType");
	modelAmbientEnableUniform   = modelShader.getUniform("useAmbientMap");
	modelDiffuseEnableUniform   = modelShader.getUniform("useDiffuseMap");
	modelNormalEnableUniform    = modelShader.getUniform("useNormalMap");
	modelSpecularEnableUniform  = modelShader.getUniform("useSpecularMap");
	modelLightColorUniform      = modelShader.getUniform("light.color");
	modelLightPositionUniform   = modelShader.getUniform("light.position");
	modelViewPositionUniform    = modelShader.getUniform("viewPos");
	lightPositionUniform		= modelShader.getUniform("light.position");
	lightColorUniform			= modelShader.getUniform("light.color");

	camera.setPosition(Vec3f(20, 5, 40));
	camera.setRotation(Math::toRadians(270), 0);
	setCameraMatrix(camera.getPosition(), camera.getPosition() + camera.getDirection());

	startTime = Time::getTimeSinceEpoch();

	GLE::enableDepthTests();
	GLE::enableCulling();
	
	return true;

}



void PhysicsRenderer::render() {

	profiler.start();

	camera.update();	
	setCameraMatrix(camera.getPosition(), camera.getPosition() + camera.getDirection());

	profiler.stop("RenderA");

	profiler.start();

	GLE::clear(GLE::Color | GLE::Depth);
	modelShader.start();

	Vec4f lightPos(camera.getPosition());
	lightPos.w = 1.0;
	lightPos = viewMatrix * lightPos;

	lightPositionUniform.setVec3(lightPos.toVec3());
	lightColorUniform.setVec3(Vec3f(1));
	
	for(auto[transform, rigidbody] : actorManager.view<Transform, RigidBody>()) {

		Mat4f modelMatrix = Mat4f::fromTranslation(transform.position) * transform.rotation.toMat4() * Mat4f::fromScale(1.0);
		renderModel(model, modelMatrix);

	}

	profiler.stop("RenderB");

}



void PhysicsRenderer::destroy() {

	modelShader.destroy();

}



void PhysicsRenderer::setAspectRatio(float aspect) {
	projMatrix = Mat4f::perspective(Math::toRadians(90.0), aspect, 0.1, 1000.0);
}



void PhysicsRenderer::setCameraMatrix(const Vec3f& pos, const Vec3f& lookat) {
	viewMatrix = Mat4f::lookAt(pos, lookat, Vec3f(0, 1, 0));
}



void PhysicsRenderer::renderModel(arcModel& model, const Mat4f& transform) {
	renderNode(model, model.getRootNode(), transform);
}



void PhysicsRenderer::renderNode(arcModel& model, arcNode& node, const Mat4f& transform) {

	for (auto& n : node.getChildren()) {
		renderNode(model, n, transform);
	}

	for (const auto& i : node.getMeshes()) {
		renderMesh(model, model.getMesh(i), transform);
	}

}



void PhysicsRenderer::renderMesh(arcModel& model, arcMesh& mesh, const Mat4f& transform) {

	applyMaterial(model, model.getMaterial(mesh.getMaterialID()));

	Mat4f modelViewMatrix = viewMatrix * transform;
	Mat3f normalMatrix = modelViewMatrix.toMat3().inverse().transposed();
	Mat4f mvpMatrix = projMatrix * modelViewMatrix;

	double delta = (Time::getTimeSinceEpoch() - startTime) / 1000.0;

	mesh.createTransformTree(boneTransforms);
	modelBoneTransformsUniform.setMat4Array(boneTransforms.data(), boneTransforms.size());
	modelUseBonesUniform.setInt(mesh.hasBones());
	//modelUseBonesUniform.setInt(false);

	modelMVPUniform.setMat4(mvpMatrix);
	modelMVUniform.setMat4(modelViewMatrix);
	modelNormalUniform.setMat3(normalMatrix);

	mesh.bind();
	GLE::renderIndexed(GLE::PrimType::Triangle, GLE::IndexType::Int, mesh.getVertexCount(), 0);

}



void PhysicsRenderer::applyMaterial(arcModel& model, arcMaterial& material) {

	enum TextureID : u32
	{
		Diffuse,
		Ambient,
		Specular,
		Normal,  // both normal and bump maps
	};

	bool useDiffuseTex = true;
	bool useAmbientTex = true;
	bool useSpecularTex = true;
	bool useNormalsTex = true;
	bool useBump = false;

	modelAmbientEnableUniform.setInt(0);
	modelDiffuseEnableUniform.setInt(0);
	modelSpecularEnableUniform.setInt(0);
	modelNormalEnableUniform.setInt(0);
	modelBumpSelectUniform.setInt(useBump);

	if (material.hasDiffuseTextures() && useDiffuseTex) {
		material.getDiffuseTexture(0).activate(Diffuse);
		modelDiffuseTexUniform.setInt(Diffuse);
		modelDiffuseEnableUniform.setInt(1);
	}

	if (material.hasAmbientTextures() && useAmbientTex) {
		material.getAmbientTexture(0).activate(Ambient);
		modelAmbientTexUniform.setInt(Ambient);
		modelAmbientEnableUniform.setInt(1);
	}

	if (material.hasSpecularTextures() && useSpecularTex) {
		material.getSpecularTexture(0).activate(Specular);
		modelSpecularTexUniform.setInt(Specular);
		modelSpecularEnableUniform.setInt(1);
	}

	if (useNormalsTex) {

		if (material.hasNormalsTextures()) {
			material.getNormalsTexture(0).activate(Normal);
			modelNormalTexUniform.setInt(Normal);
			modelNormalEnableUniform.setInt(1);
		}
		else if (material.hasBumpMapTextures()) {
			material.getBumpMapTexture(0).activate(Normal);
			modelNormalTexUniform.setInt(Normal);
			modelNormalEnableUniform.setInt(1);
		}

	}

}



void PhysicsRenderer::setupCamera(InputContext& context) {

    context.linkHandler(inputHandler);

    context.addState(0);
    camera.setupInputContext(context, 0);
    camera.setupInputHandler(inputHandler);

}