#include "rendertest.h"

#include GLE_HEADER
#include "util/random.h"
#include "util/file.h"
#include "loader.h"


RenderTest::RenderTest() : frameCounter(0) {}



void RenderTest::create(u32 w, u32 h) {

	static float squareVertices[] = {
		0, 0, 0, 0,
		1, 0, 1, 0,
		1, 1, 1, 1,
		0, 0, 0, 0,
		1, 1, 1, 1,
		0, 1, 0, 1
	};

	static float skyboxVertices[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};


	Loader::loadShader(basicShader, Uri(":/shaders/basic.avs"), Uri(":/shaders/basic.afs"));
	Loader::loadShader(cubemapShader, Uri(":/shaders/cubemap.avs"), Uri(":/shaders/cubemap.afs"));

	mvpBasicUniform = basicShader.getUniform("mvpMatrix");
	diffuseBasicUniform = basicShader.getUniform("diffuseTexture");
	srtUniform = basicShader.getUniform("srtMatrix");
	imageUniform = basicShader.getUniform("imageTexture");

	mvpCubemapUniform = cubemapShader.getUniform("mvpMatrix");
	cubemapUniform = cubemapShader.getUniform("cubemap");

	squareVertexArray.create();
	squareVertexArray.bind();

	squareVertexBuffer.create();
	squareVertexBuffer.bind();
	squareVertexBuffer.allocate(96, squareVertices);

	squareVertexArray.setAttribute(0, 2, GLE::AttributeType::Float, 16, 0);
	squareVertexArray.setAttribute(1, 2, GLE::AttributeType::Float, 16, 8);
	squareVertexArray.enableAttribute(0);
	squareVertexArray.enableAttribute(1);

	skyboxVertexArray.create();
	skyboxVertexArray.bind();

	skyboxVertexBuffer.create();
	skyboxVertexBuffer.bind();
	skyboxVertexBuffer.allocate(432, skyboxVertices);

	skyboxVertexArray.setAttribute(0, 3, GLE::AttributeType::Float, 12, 0);
	skyboxVertexArray.enableAttribute(0);

	Loader::loadTexture2D(diffuseTexture, Uri(":/textures/therapy.png"));
	Loader::loadCubemap(skyboxTexture, {
		Uri(":/textures/cubemaps/dikhololo/px.png"),
		Uri(":/textures/cubemaps/dikhololo/nx.png"),
		Uri(":/textures/cubemaps/dikhololo/py.png"),
		Uri(":/textures/cubemaps/dikhololo/ny.png"),
		Uri(":/textures/cubemaps/dikhololo/pz.png"),
		Uri(":/textures/cubemaps/dikhololo/nz.png")
	});

	for (u32 i = 0; i < 12; i++) {

		Loader::loadTexture2D(amongUsTextures[i], Uri(":/textures/orange/cm" + std::to_string(i + 1) + ".png"));

		amongUsTextures[i].setMinFilter(GLE::TextureFilter::None);
		amongUsTextures[i].setMagFilter(GLE::TextureFilter::None);
		amongUsTextures[i].setWrapU(GLE::TextureWrap::Repeat);
		amongUsTextures[i].setWrapV(GLE::TextureWrap::Repeat);

	}

	Loader::loadTexture2D(eugeneTexture, Uri(":/textures/eugene.png"));
	eugeneTexture.setMinFilter(GLE::TextureFilter::None);
	eugeneTexture.setMagFilter(GLE::TextureFilter::None);
	eugeneTexture.setWrapU(GLE::TextureWrap::Repeat);
	eugeneTexture.setWrapV(GLE::TextureWrap::Repeat);

	diffuseTexture.bind();
	diffuseTexture.setMinFilter(GLE::TextureFilter::None);
	diffuseTexture.setMagFilter(GLE::TextureFilter::None);
	diffuseTexture.setWrapU(GLE::TextureWrap::Repeat);
	diffuseTexture.setWrapV(GLE::TextureWrap::Repeat);

	skyboxTexture.bind();
	skyboxTexture.setMinFilter(GLE::TextureFilter::Trilinear);
	skyboxTexture.setMagFilter(GLE::TextureFilter::Bilinear);
	skyboxTexture.setWrapU(GLE::TextureWrap::Clamp);
	skyboxTexture.setWrapV(GLE::TextureWrap::Clamp);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	//modelMatrix = Mat4f::fromTranslation(0, 10 * (1 + Math::sin(frameCounter / 20.0) * Math::sin(frameCounter / 20.0)) / (frameCounter / 14.0), 0).rotate(Vec3f(0, 1, 0), Math::toRadians(10 * frameCounter * (1 / (1 + (frameCounter / 360.0)))));

	if (camMovement != Vec3i(0, 0, 0) || camRotation != Vec3i(0, 0, 0)) {

		camera.move(camMovement * camVelocityScale);
		camera.rotate(camRotation.x * camRotationScale, camRotation.y * camRotationScale);

		camMovement = Vec3f(0, 0, 0);
		camRotation = Vec3f(0, 0, 0);

	}

	camera.update(0.1, 0.1);
	viewMatrix = Mat4f::lookAt(camera.getPosition(), camera.getPosition() + camera.getDirection());
	recalculateMVPMatrix();

	//OpenGL main
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthMask(false);
	cubemapShader.start();

	skyboxTexture.activate(0);
	cubemapUniform.setInt(0);

	Mat4f nontransformedView = viewMatrix;
	nontransformedView[3] = Vec4f(0, 0, 0, 1);
	Mat4f skyboxMvp = projectionMatrix * nontransformedView;

	mvpCubemapUniform.setMat4(skyboxMvp);
	skyboxVertexArray.bind();

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(true);


	basicShader.start();

	diffuseTexture.activate(0);
	//amongUsTextures[(frameCounter / 8) % 12].activate(0);
	diffuseBasicUniform.setInt(0);
	amongUsTextures[(frameCounter / 8) % 12].activate(1);
	imageUniform.setInt(1);

	Mat2f srtMatrix;
	srtMatrix[0][0] = 1 + frameCounter / 50.0;
	srtMatrix[1][1] = 1 + frameCounter / 50.0;

	srtUniform.setMat2(srtMatrix);
	mvpBasicUniform.setMat4(mvpMatrix);
	squareVertexArray.bind();

	glDrawArrays(GL_TRIANGLES, 0, 6);

}



void RenderTest::destroy() {

	basicShader.destroy();
	cubemapShader.destroy();
	squareVertexArray.destroy();
	skyboxVertexArray.destroy();
	squareVertexBuffer.destroy();
	skyboxVertexBuffer.destroy();
	diffuseTexture.destroy();
	skyboxTexture.destroy();

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