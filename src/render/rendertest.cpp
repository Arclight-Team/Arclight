#include "rendertest.h"

#include GLE_HEADER
#include "util/random.h"
#include "util/file.h"
#include "screenshot.h"


RenderTest::RenderTest() : frameCounter(0), fbWidth(0), fbHeight(0), showNormals(false) {}



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

	GLE::setRowUnpackAlignment(GLE::Alignment::None);
	GLE::setRowPackAlignment(GLE::Alignment::None);

	loadShaders();
	loadResources();

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

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera.setPosition(camStartPos);
	camera.setRotation(camStartAngleH, camStartAngleV);

	modelMatrix = Mat4f::fromTranslation(0, 0, 0);
	viewMatrix = Mat4f::lookAt(camera.getPosition(), camera.getPosition() + camera.getDirection());
	projectionMatrix = Mat4f::perspective(Math::toRadians(fov), w / static_cast<double>(h), nearPlane, farPlane);

	recalculateMVPMatrix();

	fbWidth = w;
	fbHeight = h;

}




void RenderTest::run() {

	frameCounter++;

	if (frameCounter > 500) {
		frameCounter = 0;
	}

	//modelMatrix = Mat4f::fromTranslation(0, 10 * (1 + Math::sin(frameCounter / 20.0) * Math::sin(frameCounter / 20.0)) / (frameCounter / 14.0), 0).rotate(Vec3f(0, 1, 0), Math::toRadians(10 * frameCounter * (1 / (1 + (frameCounter / 360.0)))));

	if (camMovement != Vec3i(0, 0, 0) || camRotation != Vec3i(0, 0, 0)) {

		camera.move(camMovement * camVelocity);
		camera.rotate(camRotation.x * camRotationScale, camRotation.y * camRotationScale);

		camMovement = Vec3i(0, 0, 0);
		camRotation = Vec3i(0, 0, 0);

	}

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
	diffuseTextureUniform.setInt(0);
	amongUsTextureArray.activate(1);
	amongUsTextureUniform.setInt(1);
	amongUsFrameUniform.setFloat((frameCounter / 32) % 12 + 0.5);

	waterSrtMatrix[2].y += 0.04;

	Mat2f srtMatrix;
	srtMatrix[0][0] = 1 + frameCounter / 50.0;
	srtMatrix[1][1] = 1 + frameCounter / 50.0;

	srtUniform.setMat2(srtMatrix);
	mvpBasicUniform.setMat4(mvpMatrix);

	squareVertexArray.bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	renderModels();

}



void RenderTest::destroy() {

	basicShader.destroy();
	cubemapShader.destroy();

	squareVertexArray.destroy();
	skyboxVertexArray.destroy();

	squareVertexBuffer.destroy();
	skyboxVertexBuffer.destroy();

	amongUsTextureArray.destroy();
	diffuseTexture.destroy();
	skyboxTexture.destroy();
	eugeneTexture.destroy();

	modelShader.destroy();
	debugShader.destroy();

	for (auto& m : models) {
		m.destroy();
	}

}



void RenderTest::loadShaders() {

	basicShader.destroy();
	cubemapShader.destroy();
	modelShader.destroy();
	debugShader.destroy();

	Loader::loadShader(basicShader, ":/shaders/basic.avs", ":/shaders/basic.afs");
	Loader::loadShader(cubemapShader, ":/shaders/cubemap.avs", ":/shaders/cubemap.afs");

	mvpBasicUniform = basicShader.getUniform("mvpMatrix");
	diffuseTextureUniform = basicShader.getUniform("diffuseTexture");
	srtUniform = basicShader.getUniform("srtMatrix");
	amongUsFrameUniform = basicShader.getUniform("amongUsFrame");
	amongUsTextureUniform = basicShader.getUniform("amongUsTexture");

	mvpCubemapUniform = cubemapShader.getUniform("mvpMatrix");
	cubemapUniform = cubemapShader.getUniform("cubemap");

	Loader::loadShader(modelShader, ":/shaders/model/diffuse.avs", ":/shaders/model/diffuse.afs");
	modelNUniform = modelShader.getUniform("normalMatrix");
	modelMVUniform = modelShader.getUniform("modelViewMatrix");
	modelMVPUniform = modelShader.getUniform("mvpMatrix");
	modelDiffuseUniform = modelShader.getUniform("diffuseTexture");
	modelLightUniform = modelShader.getUniform("lightPos");
	modelViewUniform = modelShader.getUniform("viewPos");
	modelBaseColUniform = modelShader.getUniform("baseCol");
	modelSrtUniform = modelShader.getUniform("srtMatrix");

	Loader::loadShader(debugShader, ":/shaders/model/diffuse.avs", ":/shaders/debug.ags", ":/shaders/debug.afs");
	debugPUniform = debugShader.getUniform("projectionMatrix");
	debugUPUniform = debugShader.getUniform("unprojectionMatrix");
	debugNUniform = debugShader.getUniform("normalMatrix");
	debugMVPUniform = debugShader.getUniform("mvpMatrix");

}



void RenderTest::loadResources() {

	for (auto& m : models) {
		m.destroy();
	}

	diffuseTexture.destroy();
	amongUsTextureArray.destroy();
	eugeneTexture.destroy();
	skyboxTexture.destroy();

	models.clear();
	models.resize(5);
	Loader::loadModel(models[ModelID::Mario], ":/models/mario/mario.fbx");
	Loader::loadModel(models[ModelID::Luigi], ":/models/luigi/luigi.fbx");
	Loader::loadModel(models[ModelID::CastleGrounds], ":/models/Level Models/Castle Grounds/grounds.fbx", true);
	Loader::loadModel(models[ModelID::Melascula], ":/models/Melascula/Melascula.obj", false);
	Loader::loadModel(models[ModelID::Galand], ":/models/galand_realistic/Galand.obj", false);

	Loader::loadTexture2D(diffuseTexture, Uri(":/textures/therapy.png"));
	Loader::loadCubemap(skyboxTexture, {
		":/textures/cubemaps/dikhololo/px.png",
		":/textures/cubemaps/dikhololo/nx.png",
		":/textures/cubemaps/dikhololo/py.png",
		":/textures/cubemaps/dikhololo/ny.png",
		":/textures/cubemaps/dikhololo/pz.png",
		":/textures/cubemaps/dikhololo/nz.png"
		}, true);

	std::vector<Uri> amongUsFrameUris;

	for (u32 i = 0; i < 12; i++) {
		amongUsFrameUris.emplace_back(":/textures/orange/cm" + std::to_string(i + 1) + ".png");
	}

	Loader::loadArrayTexture2D(amongUsTextureArray, amongUsFrameUris);

	amongUsTextureArray.setMinFilter(GLE::TextureFilter::None);
	amongUsTextureArray.setMagFilter(GLE::TextureFilter::None);
	amongUsTextureArray.setWrapU(GLE::TextureWrap::Repeat);
	amongUsTextureArray.setWrapV(GLE::TextureWrap::Repeat);

	Loader::loadTexture2D(eugeneTexture, ":/textures/eugene.png");
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

	models[ModelID::Mario].transform = Mat4f::fromTranslation(-20, 20, 0);
	models[ModelID::Luigi].transform = Mat4f::fromTranslation(+20, 20, 0);
	models[ModelID::Melascula].transform = Mat4f::fromScale(10, 10, 10).translate(0, 20, 0);
	models[ModelID::Galand].transform = Mat4f::fromScale(1, 1, 1).translate(0, -50, -50);
	models[ModelID::Melascula].root.children[4].visible = false;
	
	setTextureFilters(ModelID::Mario, GLE::TextureFilter::None, GLE::TextureFilter::None);
	setTextureFilters(ModelID::Luigi, GLE::TextureFilter::None, GLE::TextureFilter::None);
	setTextureFilters(ModelID::CastleGrounds, GLE::TextureFilter::None, GLE::TextureFilter::None);
	setTextureFilters(ModelID::Melascula, GLE::TextureFilter::None, GLE::TextureFilter::None);
	setTextureFilters(ModelID::Galand, GLE::TextureFilter::None, GLE::TextureFilter::None);
	setTextureWrap(ModelID::Mario, GLE::TextureWrap::Repeat, GLE::TextureWrap::Repeat);
	setTextureWrap(ModelID::Luigi, GLE::TextureWrap::Mirror, GLE::TextureWrap::Mirror);
	setTextureWrap(ModelID::CastleGrounds, GLE::TextureWrap::Mirror, GLE::TextureWrap::Mirror);
	setTextureWrap(ModelID::Melascula, GLE::TextureWrap::Repeat, GLE::TextureWrap::Repeat);
	setTextureWrap(ModelID::Galand, GLE::TextureWrap::Repeat, GLE::TextureWrap::Repeat);

	std::vector<Material>& m = models[ModelID::CastleGrounds].materials;

	for (auto& [name, texture] : m[9].textures) {
		texture.bind();
		texture.setWrapU(GLE::TextureWrap::Repeat);
		texture.setWrapV(GLE::TextureWrap::Repeat);
	}

}



void RenderTest::saveScreenshot() {

	u8* data = new u8[fbWidth * fbHeight * 3];
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glReadPixels(0, 0, fbWidth, fbHeight, GL_RGB, GL_UNSIGNED_BYTE, data);
	Screenshot::save(fbWidth, fbHeight, data);

	delete[] data;

}



void RenderTest::renderModels() {

	modelShader.start();

	for (Model& model : models) {
		renderNode(model, model.root);
	}
	
	if (showNormals) {

		debugShader.start();

		for (Model& model : models) {
			renderDebugNode(model, model.root);
		}

	}
	
}




void RenderTest::renderNode(Model& model, ModelNode& node) {

	if (!node.visible) {
		return;
	}

	for (u32 i = 0; i < node.meshIndices.size(); i++) {

		Mesh& mesh = model.meshes[node.meshIndices[i]];
		Material& material = model.materials[mesh.materialIndex];
		GLE::Texture2D& texture = material.textures["diffuse0"];

		texture.activate(0);
		modelDiffuseUniform.setInt(0);
		
		Mat4f modelMatrix = model.transform * node.baseTransform;
		Mat4f modelViewMatrix = viewMatrix * modelMatrix;
		Mat3f normalMatrix = Mat3f(modelViewMatrix.toMat3()).inverse().transposed();
		Mat4f mvpMatrix = projectionMatrix * modelViewMatrix;

		modelNUniform.setMat3(normalMatrix);
		modelMVUniform.setMat4(modelViewMatrix);
		modelMVPUniform.setMat4(mvpMatrix);

		Vec4f lightPos = viewMatrix * Vec4f(100, 110, 10, 1.0);
		Vec4f viewPos = viewMatrix * Vec4f(camera.getPosition().x, camera.getPosition().y, camera.getPosition().z, 1.0);

		modelLightUniform.setVec3(&viewPos[0]);
		modelViewUniform.setVec3(&viewPos[0]);

		if (mesh.materialIndex == 22) {
			modelBaseColUniform.setVec4(Vec4f(1, 1, 1, 0.75));
			modelSrtUniform.setMat3(waterSrtMatrix);
		}
		else {
			modelBaseColUniform.setVec4(Vec4f(1, 1, 1, 1));
			modelSrtUniform.setMat3(Mat3f());
		}

		mesh.vao.bind();
		glDrawElements(GL_TRIANGLES, mesh.vertexCount, GL_UNSIGNED_INT, 0);

	}

	for (u32 i = 0; i < node.children.size(); i++) {
		renderNode(model, node.children[i]);
	}

}



void RenderTest::renderDebugNode(Model& model, ModelNode& node) {

	if (!node.visible) {
		return;
	}

	for (u32 i = 0; i < node.meshIndices.size(); i++) {

		Mesh& mesh = model.meshes[node.meshIndices[i]];

		Mat4f modelMatrix = model.transform * node.baseTransform;
		Mat4f modelViewMatrix = viewMatrix * modelMatrix;
		Mat3f normalMatrix = Mat3f(modelViewMatrix.toMat3()).inverse().transposed();
		Mat4f unprojectionMatrix = projectionMatrix.inverse();
		Mat4f mvpMatrix = projectionMatrix * modelViewMatrix;

		debugPUniform.setMat4(projectionMatrix);
		debugUPUniform.setMat4(unprojectionMatrix);
		debugNUniform.setMat3(normalMatrix);
		debugMVPUniform.setMat4(mvpMatrix);

		mesh.vao.bind();
		glDrawElements(GL_TRIANGLES, mesh.vertexCount, GL_UNSIGNED_INT, 0);

	}

	for (u32 i = 0; i < node.children.size(); i++) {
		renderDebugNode(model, node.children[i]);
	}

}



void RenderTest::setTextureFilters(u32 modelID, GLE::TextureFilter min, GLE::TextureFilter mag) {

	for (Material& material : models[modelID].materials) {

		for (auto& [name, texture] : material.textures) {

			texture.bind();
			texture.setMinFilter(min);
			texture.setMagFilter(mag);

		}

	}

}



void RenderTest::setTextureWrap(u32 modelID, GLE::TextureWrap wrapU, GLE::TextureWrap wrapV) {

	for (Material& material : models[modelID].materials) {

		for (auto& [name, texture] : material.textures) {

			texture.bind();
			texture.setWrapU(wrapU);
			texture.setWrapV(wrapV);

		}

	}

}



void RenderTest::resizeWindowFB(u32 w, u32 h) {

	glViewport(0, 0, w, h);
	projectionMatrix = Mat4f::perspective(Math::toRadians(fov), w / static_cast<double>(h), nearPlane, farPlane);
	recalculateMVPMatrix();

	fbWidth = w;
	fbHeight = h;

}



void RenderTest::onKeyAction(KeyAction action) {

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

		case ActionID::CameraSpeedUp:
			camVelocity = camVelocityFast;
			break;
		case ActionID::CameraSlowDown:
			camVelocity = camVelocitySlow;
			break;

		case ActionID::QuickScreenshot:
			saveScreenshot();
			break;

		case ActionID::ReloadShaders:
			loadShaders();
			break;
		case ActionID::ReloadResources:
			loadResources();
			break;

		case ActionID::ToggleDebug:
			showNormals = !showNormals;
			break;

	}

}



void RenderTest::recalculateMVPMatrix() {
	mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
}