#include "physicsrenderer.h"
#include "utility/shaderloader.h"
#include "utility/vertexhelper.h"
#include "core/acs/actormanager.h"
#include "debug.h"


PhysicsRenderer::PhysicsRenderer(ActorManager& actorManager) : actorManager(actorManager) {}


bool PhysicsRenderer::init() {

	auto cubeVertices = VertexHelper::createCube(2);

	objectVA.create();
	objectVA.bind();

	objectVB.create();
	objectVB.bind();
	objectVB.allocate(cubeVertices.size() * sizeof(VertexHelper::VertexT), cubeVertices.data());
	objectVA.enableAttribute(0);
	objectVA.setAttribute(0, 3, GLE::AttributeType::Float, 0, 0);

	offsetVB.create();
	offsetVB.bind();
	offsetVB.allocate(cubeVertices.size() * sizeof(VertexHelper::VertexT));
	objectVA.enableAttribute(1);
	objectVA.setAttribute(1, 3, GLE::AttributeType::Float, 0, 0);
	objectVA.setDivisor(1, 1);

	try {

		objectShader = ShaderLoader::fromFiles(":/shaders/object.avs", ":/shaders/object.afs");

	} catch (std::exception& e) {
		Log::error("Physics Renderer", e.what());
		return false;
	}

	mvpMatrixUniform = objectShader.getUniform("mvpMatrix");

	camera.setPosition(Vec3f(20, 5, 40));
	camera.setRotation(Math::toRadians(270), 0);
	setCameraMatrix(camera.getPosition(), camera.getPosition() + camera.getDirection());
	
	return true;

}



void PhysicsRenderer::render() {

	if (camMovement != Vec3i(0, 0, 0) || camRotation != Vec3i(0, 0, 0)) {

		camera.move(camMovement * camVelocity);
		camera.rotate(camRotation.x * camRotationScale, camRotation.y * camRotationScale);

		setCameraMatrix(camera.getPosition(), camera.getPosition() + camera.getDirection());

		camMovement = Vec3i(0, 0, 0);
		camRotation = Vec3i(0, 0, 0);

	}

	u32 objects = 0;
	std::vector<float> offsets;

	for(auto[transform, collider] : actorManager.view<Transform, BoxCollider>()) {

		offsets.push_back(transform.position.x);
		offsets.push_back(transform.position.y);
		offsets.push_back(transform.position.z);
		objects++;

	}

	offsetVB.bind();
	offsetVB.allocate(offsets.size() * sizeof(float), offsets.data());

	GLE::clear(GLE::Color | GLE::Depth);
	objectShader.start();

	Mat4f mvpMatrix = projMatrix * viewMatrix;
	mvpMatrixUniform.setMat4(mvpMatrix);
	
	objectVA.bind();
	GLE::renderInstanced(GLE::PrimType::Triangle, objects, 36);


}



void PhysicsRenderer::destroy() {

	objectVA.destroy();
	objectVB.destroy();
	offsetVB.destroy();
	objectShader.destroy();

}



void PhysicsRenderer::setAspectRatio(float aspect) {
	projMatrix = Mat4f::perspective(Math::toRadians(90.0), aspect, 0.1, 1000.0);
}



void PhysicsRenderer::setCameraMatrix(const Vec3f& pos, const Vec3f& lookat) {
	viewMatrix = Mat4f::lookAt(pos, lookat, Vec3f(0, 1, 0));
}



void PhysicsRenderer::onKeyAction(KeyAction action) {

	switch (action) {

		case ActionID::CameraRotLeft:
			camRotation.x -= 1;
			break;
		case ActionID::CameraRotRight:
			camRotation.x += 1;
			break;
		case ActionID::CameraRotDown:
			camRotation.y -= 1;
			break;
		case ActionID::CameraRotUp:
			camRotation.y += 1;
			break;
		case ActionID::CameraMoveLeft:
			camMovement.x -= 1;
			break;
		case ActionID::CameraMoveRight:
			camMovement.x += 1;
			break;
		case ActionID::CameraMoveBackward:
			camMovement.z -= 1;
			break;
		case ActionID::CameraMoveForward:
			camMovement.z += 1;
			break;
		case ActionID::CameraMoveDown:
			camMovement.y -= 1;
			break;
		case ActionID::CameraMoveUp:
			camMovement.y += 1;
			break;

	}

}