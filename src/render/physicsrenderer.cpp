#include "physicsrenderer.h"
#include "utility/shaderloader.h"
#include "utility/vertexhelper.h"
#include "core/acs/actormanager.h"
#include "debug.h"


PhysicsRenderer::PhysicsRenderer(ActorManager& actorManager) : actorManager(actorManager), prevObjects(0) {}


bool PhysicsRenderer::init() {

	auto cubeVertices = VertexHelper::createCube(1);

	objectVA.create();
	objectVA.bind();

	objectVB.create();
	objectVB.bind();
	objectVB.allocate(cubeVertices.size() * sizeof(VertexHelper::VertexT), cubeVertices.data());
	objectVA.enableAttribute(0);
	objectVA.setAttribute(0, 3, GLE::AttributeType::Float, 0, 0);

	offsetVB.create();
	offsetVB.bind();

	objectVA.enableAttribute(1);
	objectVA.enableAttribute(2);
	objectVA.enableAttribute(3);
	objectVA.enableAttribute(4);

	objectVA.setAttribute(1, 4, GLE::AttributeType::Float, 16 * sizeof(float), 0);
	objectVA.setAttribute(2, 4, GLE::AttributeType::Float, 16 * sizeof(float), 4 * sizeof(float));
	objectVA.setAttribute(3, 4, GLE::AttributeType::Float, 16 * sizeof(float), 8 * sizeof(float));
	objectVA.setAttribute(4, 4, GLE::AttributeType::Float, 16 * sizeof(float), 12 * sizeof(float));

	objectVA.setDivisor(1, 1);
	objectVA.setDivisor(2, 1);
	objectVA.setDivisor(3, 1);
	objectVA.setDivisor(4, 1);

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

	profiler.start();

	if (camMovement != Vec3i(0, 0, 0) || camRotation != Vec3i(0, 0, 0)) {

		camera.move(camMovement * camVelocity);
		camera.rotate(camRotation.x * camRotationScale, camRotation.y * camRotationScale);

		setCameraMatrix(camera.getPosition(), camera.getPosition() + camera.getDirection());

		camMovement = Vec3i(0, 0, 0);
		camRotation = Vec3i(0, 0, 0);

	}

	u32 objects = 0;
	modelMatrixBuffer.clear();

	for(auto[transform, collider] : actorManager.view<Transform, BoxCollider>()) {

		Mat4f modelMatrix = Mat4f::fromTranslation(transform.position) * Mat4f::fromRotationXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
		
		modelMatrixBuffer.push_back(modelMatrix[0][0]);
		modelMatrixBuffer.push_back(modelMatrix[0][1]);
		modelMatrixBuffer.push_back(modelMatrix[0][2]);
		modelMatrixBuffer.push_back(modelMatrix[0][3]);
		modelMatrixBuffer.push_back(modelMatrix[1][0]);
		modelMatrixBuffer.push_back(modelMatrix[1][1]);
		modelMatrixBuffer.push_back(modelMatrix[1][2]);
		modelMatrixBuffer.push_back(modelMatrix[1][3]);
		modelMatrixBuffer.push_back(modelMatrix[2][0]);
		modelMatrixBuffer.push_back(modelMatrix[2][1]);
		modelMatrixBuffer.push_back(modelMatrix[2][2]);
		modelMatrixBuffer.push_back(modelMatrix[2][3]);
		modelMatrixBuffer.push_back(modelMatrix[3][0]);
		modelMatrixBuffer.push_back(modelMatrix[3][1]);
		modelMatrixBuffer.push_back(modelMatrix[3][2]);
		modelMatrixBuffer.push_back(modelMatrix[3][3]);
		
		objects++;

	}

	offsetVB.bind();

	if(prevObjects != objects) {

		prevObjects = objects;
		offsetVB.allocate(modelMatrixBuffer.size() * sizeof(float), modelMatrixBuffer.data(), GLE::BufferAccess::StreamDraw);

	} else {

		offsetVB.update(0, modelMatrixBuffer.size() * sizeof(float), modelMatrixBuffer.data());

	}

	profiler.stop("RenderA");
	profiler.start();

	GLE::clear(GLE::Color | GLE::Depth);
	objectShader.start();

	Mat4f mvpMatrix = projMatrix * viewMatrix;
	mvpMatrixUniform.setMat4(mvpMatrix);
	
	objectVA.bind();
	GLE::renderInstanced(GLE::PrimType::Triangle, objects, 36);

	profiler.stop("RenderB");

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