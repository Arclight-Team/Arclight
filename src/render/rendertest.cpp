#include "rendertest.h"

#include GLE_HEADER
#include "util/random.h"
#include "util/file.h"
#include "screenshot.h"
#include "util/profiler.h"


RenderTest::RenderTest() : frameCounter(0), fbWidth(0), fbHeight(0), exposure(1), showNormals(false) {}



void RenderTest::create(u32 w, u32 h) {

	static float squareVertices[] = {
		0, 0, 0, 0,
		1, 0, 1, 0,
		1, 1, 1, 1,
		0, 0, 0, 0,
		1, 1, 1, 1,
		0, 1, 0, 1
	};

	static float screenVertices[] = {
		-1, -1, 0, 0,
		1, -1, 1, 0,
		1, 1, 1, 1,
		-1, -1, 0, 0,
		1, 1, 1, 1,
		-1, 1, 0, 1
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

	fbWidth = w;
	fbHeight = h;

	GLE::setRowUnpackAlignment(GLE::Alignment::None);
	GLE::setRowPackAlignment(GLE::Alignment::None);

	loadShaders();
	loadResources();

	screenVertexArray.create();
	screenVertexArray.bind();

	screenVertexBuffer.create();
	screenVertexBuffer.bind();
	screenVertexBuffer.allocate(96, screenVertices);

	screenVertexArray.setAttribute(0, 2, GLE::AttributeType::Float, 16, 0);
	screenVertexArray.setAttribute(1, 2, GLE::AttributeType::Float, 16, 8);
	screenVertexArray.enableAttribute(0);
	screenVertexArray.enableAttribute(1);

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

	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera.setPosition(camStartPos);
	camera.setRotation(camStartAngleH, camStartAngleV);

	viewMatrix = Mat4f::lookAt(camera.getPosition(), camera.getPosition() + camera.getDirection());
	
	recalculateProjection();
	setupFramebuffers();

}




void RenderTest::run() {

	frameCounter++;

	if (frameCounter > 500) {
		frameCounter = 0;
	}

	if (camMovement != Vec3i(0, 0, 0) || camRotation != Vec3i(0, 0, 0)) {

		camera.move(camMovement * camVelocity);
		camera.rotate(camRotation.x * camRotationScale, camRotation.y * camRotationScale);
		viewMatrix = Mat4f::lookAt(camera.getPosition(), camera.getPosition() + camera.getDirection());

		camMovement = Vec3i(0, 0, 0);
		camRotation = Vec3i(0, 0, 0);

	}

	recalculateProjection();

	//OpenGL main

	//Render to shadow map
	shadowFramebuffer.bind();
	glViewport(0, 0, shadowMapSize, shadowMapSize);
	glClear(GL_DEPTH_BUFFER_BIT);
	renderModels(ShaderPass::Shadow);

	//Render to render framebuffer
	renderFramebuffer.bind();
	glViewport(0, 0, fbWidth, fbHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Render cubemap
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

	//Render test shader
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
	mvpBasicUniform.setMat4(projectionMatrix * viewMatrix);

	squareVertexArray.bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Render models
	renderModels(ShaderPass::Main);

	if (showNormals) {
		renderModels(ShaderPass::Debug);
	}

	//Postprocess
	GLE::Framebuffer::bindDefault();
	glDisable(GL_DEPTH_TEST);

	pprocessShader.start();
	
	renderColorTexture.activate(0);
	pprocessTextureUniform.setInt(0);
	pprocessExposureUniform.setFloat(exposure);

	screenVertexArray.bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glEnable(GL_DEPTH_TEST);

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

	pprocessShader.destroy();
	screenVertexArray.destroy();
	screenVertexBuffer.destroy();

	renderColorTexture.destroy();
	renderDepthBuffer.destroy();
	renderFramebuffer.destroy();

}



void RenderTest::loadShaders() {

	basicShader.destroy();
	cubemapShader.destroy();
	modelShader.destroy();
	debugShader.destroy();
	pprocessShader.destroy();
	shadowShader.destroy();

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
	modelLightUniform = modelShader.getUniform("lightDir");
	modelShadowMatrixUniform = modelShader.getUniform("shadowMatrix");
	modelShadowMapUniform = modelShader.getUniform("shadowMap");
	modelViewUniform = modelShader.getUniform("viewPos");
	modelBaseColUniform = modelShader.getUniform("baseCol");
	modelSrtUniform = modelShader.getUniform("srtMatrix");

	Loader::loadShader(debugShader, ":/shaders/model/diffuse.avs", ":/shaders/debug.ags", ":/shaders/debug.afs");
	debugPUniform = debugShader.getUniform("projectionMatrix");
	debugUPUniform = debugShader.getUniform("unprojectionMatrix");
	debugNUniform = debugShader.getUniform("normalMatrix");
	debugMVPUniform = debugShader.getUniform("mvpMatrix");

	Loader::loadShader(pprocessShader, ":/shaders/final.avs", ":/shaders/final.afs");
	pprocessTextureUniform = pprocessShader.getUniform("screenTexture");
	pprocessExposureUniform = pprocessShader.getUniform("exposure");

	Loader::loadShader(shadowShader, ":/shaders/shadow.avs", ":/shaders/shadow.afs");
	lightMatrixUniform = shadowShader.getUniform("lightMVPMatrix");

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



void RenderTest::renderModels(ShaderPass pass) {

	switch (pass) {

		case ShaderPass::Shadow:
			shadowShader.start();
			break;

		case ShaderPass::Main:
			modelShader.start();
			break;

		case ShaderPass::Debug:
			debugShader.start();
			break;

		default:
			arc_force_assert("Unknown shader pass %d", pass);
			break;

	}

	for (Model& model : models) {
		renderNode(model, model.root, pass);
	}
	
}




void RenderTest::renderNode(Model& model, ModelNode& node, ShaderPass pass) {

	if (!node.visible) {
		return;
	}

	for (u32 i = 0; i < node.meshIndices.size(); i++) {

		Mesh& mesh = model.meshes[node.meshIndices[i]];
		Material& material = model.materials[mesh.materialIndex];
		GLE::Texture2D& texture = material.textures["diffuse0"];
		Mat4f modelMatrix = model.transform * node.baseTransform;

		float orthoBounds = 50;
		Vec3f lightVector(-1.0, -2.0, -0.5);
		Vec3f lightPos = -lightVector * 100;

		Mat4f lightViewMatrix = Mat4f::lookAt(lightPos, Vec3f(0));
		Mat4f lightOrthoMatrix = Mat4f::ortho(-orthoBounds, orthoBounds, -orthoBounds, orthoBounds, 0.5, 300.0);
		Mat4f lightMVPMatrix = lightOrthoMatrix * lightViewMatrix * modelMatrix;

		switch (pass) {

			case ShaderPass::Shadow:
				{
					lightMatrixUniform.setMat4(lightMVPMatrix);

					mesh.vao.bind();
					glDrawElements(GL_TRIANGLES, mesh.vertexCount, GL_UNSIGNED_INT, 0);
				}
				break;

			case ShaderPass::Main:
				{
					texture.activate(0);
					modelDiffuseUniform.setInt(0);

					Mat4f modelViewMatrix = viewMatrix * modelMatrix;
					Mat3f normalMatrix = Mat3f(modelViewMatrix.toMat3()).inverse().transposed();
					Mat4f mvpMatrix = projectionMatrix * modelViewMatrix;

					modelNUniform.setMat3(normalMatrix);
					modelMVUniform.setMat4(modelViewMatrix);
					modelMVPUniform.setMat4(mvpMatrix);
					modelShadowMatrixUniform.setMat4(lightMVPMatrix);

					Vec4f lightDir = -Vec4f::normalize(viewMatrix * Vec4f(lightVector.x, lightVector.y, lightVector.z, 0.0));
					Vec4f viewPos = viewMatrix * Vec4f(camera.getPosition().x, camera.getPosition().y, camera.getPosition().z, 1.0);

					modelLightUniform.setVec3(&lightDir[0]);
					modelViewUniform.setVec3(&viewPos[0]);

					if (mesh.materialIndex == 22) {
						modelBaseColUniform.setVec4(Vec4f(1, 1, 1, 0.75));
						modelSrtUniform.setMat3(waterSrtMatrix);
					} else {
						modelBaseColUniform.setVec4(Vec4f(1, 1, 1, 1));
						modelSrtUniform.setMat3(Mat3f());
					}

					shadowDepthTexture.activate(1);
					modelShadowMapUniform.setInt(1);

					mesh.vao.bind();
					glDrawElements(GL_TRIANGLES, mesh.vertexCount, GL_UNSIGNED_INT, 0);
				}
				break;

			case ShaderPass::Debug:
				{
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
				break;

			default:
				arc_force_assert("Unknown shader pass %d", pass);
				break;

		}

	}

	for (u32 i = 0; i < node.children.size(); i++) {
		renderNode(model, node.children[i], pass);
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

	fbWidth = w;
	fbHeight = h;

	recalculateProjection();
	setupFramebuffers();

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

		case ActionID::FovIn:
			fov = fovZoom;
			recalculateProjection();
			break;
		case ActionID::FovOut:
			fov = fovNormal;
			recalculateProjection();
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



void RenderTest::onScroll(float s) {

	exposure += s * 0.1;
	exposure = Math::clamp(exposure, 0.1, 30.0);
	Log::info("Render Test", "Exposure set to %f", exposure);

}



void RenderTest::recalculateProjection() {
	projectionMatrix = Mat4f::perspective(Math::toRadians(fov), fbWidth / static_cast<double>(fbHeight), nearPlane, farPlane);
}



void RenderTest::setupFramebuffers() {

	Profiler fboProfiler;
	fboProfiler.start();

	shadowDepthTexture.destroy();
	shadowFramebuffer.destroy();

	renderColorTexture.destroy();
	renderDepthBuffer.destroy();
	renderFramebuffer.destroy();

	shadowDepthTexture.create();
	shadowDepthTexture.bind();
	shadowDepthTexture.setData(shadowMapSize, shadowMapSize, GLE::ImageFormat::Depth24, GLE::TextureSourceFormat::Depth, GLE::TextureSourceType::UByte, nullptr);
	shadowDepthTexture.setMipmapMaxLevel(0);
	shadowDepthTexture.setWrapU(GLE::TextureWrap::Border);
	shadowDepthTexture.setWrapV(GLE::TextureWrap::Border);
	shadowDepthTexture.setBorderColor(1, 1, 1, 1);
	shadowDepthTexture.setMinFilter(GLE::TextureFilter::Bilinear);
	shadowDepthTexture.setMagFilter(GLE::TextureFilter::Bilinear);
	shadowDepthTexture.enableComparisonMode(GLE::TextureOperator::LessEqual);

	shadowFramebuffer.create();
	shadowFramebuffer.bind();
	shadowFramebuffer.attachTexture(GLE::Framebuffer::DepthIndex, shadowDepthTexture);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	shadowFramebuffer.validate();

	renderColorTexture.create();
	renderColorTexture.bind();
	renderColorTexture.setData(fbWidth, fbHeight, GLE::ImageFormat::RGB16f, GLE::TextureSourceFormat::RGB, GLE::TextureSourceType::Float, nullptr);
	renderColorTexture.setMipmapMaxLevel(0);

	renderDepthBuffer.create();
	renderDepthBuffer.bind();
	renderDepthBuffer.setStorage(fbWidth, fbHeight, GLE::ImageFormat::Depth24Stencil8);

	renderFramebuffer.create();
	renderFramebuffer.bind();
	renderFramebuffer.attachTexture(GLE::Framebuffer::ColorIndex, renderColorTexture);
	renderFramebuffer.attachRenderbuffer(GLE::Framebuffer::DepthStencilIndex, renderDepthBuffer);
	renderFramebuffer.validate();
	GLE::Framebuffer::bindDefault();

	fboProfiler.stop("FBO Profiler");

}