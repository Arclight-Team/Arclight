#include "rendertest.h"

#include GLE_HEADER



RenderTest::RenderTest() : frameCounter(0) {}



void RenderTest::create(u32 w, u32 h) {

	static std::string vs = R"(#version 430
								layout(location = 0) in vec2 vertices;
								layout(location = 0) uniform mat4 mvp;

								void main(){
									gl_Position = mvp * vec4(vertices, 0.0, 1.0);
								})";

	static std::string fs = R"(#version 430
								out vec4 color;

								void main(){
									color = vec4(1.0, 0.0, 0.0, 1.0);
								})";

	static float vertexData[] = {
		-1.0, -1.0,
		1.0, -1.0,
		0.0, 1.0
	};
	
	shader.create();
	shader.addShader(vs.c_str(), vs.size(), GLE::ShaderType::VertexShader);
	shader.addShader(fs.c_str(), fs.size(), GLE::ShaderType::FragmentShader);
	shader.link();

	vertexArray.create();
	vertexArray.bind();

	vertexBuffer.create();
	vertexBuffer.bind();
	vertexBuffer.setStorage(24, vertexData);

	vertexArray.setAttribute(0, 2, GLE::AttributeType::Float, 0, 0);
	vertexArray.enableAttribute(0);


	camera.setPosition(camStartPos);
	camera.setRotation(camStartAngleH, camStartAngleV);

	modelMatrix = Mat4f::fromTranslation(0, 0, 0);
	viewMatrix = Mat4f::lookAt(camera.getPosition(), camera.getPosition() + camera.getDirection());
	projectionMatrix = Mat4f::perspective(Math::toRadians(fov), w / static_cast<double>(h), nearPlane, farPlane);

	recalculateMVPMatrix();

}




void RenderTest::run() {

	frameCounter++;

	if (frameCounter > 500) {
		frameCounter = 0;
	}

	modelMatrix = Mat4f::fromTranslation(0, 10 * (1 + Math::sin(frameCounter / 20.0) * Math::sin(frameCounter / 20.0)) / (frameCounter / 14.0), 0).rotate(Vec3f(0, 1, 0), Math::toRadians(10 * frameCounter * (1 / (1 + (frameCounter / 360.0)))));

	if (camMovement != Vec3i(0, 0, 0) || camRotation != Vec3i(0, 0, 0)) {

		camera.move(camMovement * camVelocityScale);
		camera.rotate(camRotation.x * camRotationScale, camRotation.y * camRotationScale);

		camMovement = Vec3f(0, 0, 0);
		camRotation = Vec3f(0, 0, 0);

	}

	camera.update(0.1, 0.1);
	viewMatrix = Mat4f::lookAt(camera.getPosition(), camera.getPosition() + camera.getDirection());
	recalculateMVPMatrix();

	glClear(GL_COLOR_BUFFER_BIT);

	shader.start();
	glUniformMatrix4fv(0, 1, false, &mvpMatrix[0][0]);
	vertexArray.bind();
	glDrawArrays(GL_TRIANGLES, 0, 9000);

}



void RenderTest::destroy() {

	shader.destroy();
	vertexArray.destroy();
	vertexBuffer.destroy();

}



void RenderTest::resizeWindowFB(u32 w, u32 h) {

	glViewport(0, 0, w, h);
	projectionMatrix = Mat4f::perspective(Math::toRadians(fov), w / static_cast<double>(h), nearPlane, farPlane);
	recalculateMVPMatrix();

}



void RenderTest::onCameraKeyAction(KeyAction action) {

	switch (action) {

		case 1:
			camRotation.x += 1;
			break;
		case 2:
			camRotation.x -= 1;
			break;
		case 3:
			camRotation.y -= 1;
			break;
		case 4:
			camRotation.y += 1;
			break;
		case 5:
			camMovement.x -= 1;
			break;
		case 6:
			camMovement.x += 1;
			break;
		case 7:
			camMovement.z -= 1;
			break;
		case 8:
			camMovement.z += 1;
			break;

	}

}



void RenderTest::recalculateMVPMatrix() {
	mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
}