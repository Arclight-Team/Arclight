#include "rendertest.h"

#include GLE_HEADER
#include "math/random.h"
#include "util/file.h"
#include "screenshot.h"
#include "util/profiler.h"


RenderTest::RenderTest() : frameCounter(0), fbWidth(0), fbHeight(0), exposure(1), showNormals(false) {}



void RenderTest::create(u32 w, u32 h) {

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
	scene.loadScene(SceneID::MarioSDS);

	screenVertexArray.create();
	screenVertexArray.bind();

	screenVertexBuffer.create();
	screenVertexBuffer.bind();
	screenVertexBuffer.allocate(96, screenVertices);

	screenVertexArray.setAttribute(0, 2, GLE::AttributeType::Float, 16, 0);
	screenVertexArray.setAttribute(1, 2, GLE::AttributeType::Float, 16, 8);
	screenVertexArray.enableAttribute(0);
	screenVertexArray.enableAttribute(1);

	skyboxVertexArray.create();
	skyboxVertexArray.bind();

	skyboxVertexBuffer.create();
	skyboxVertexBuffer.bind();
	skyboxVertexBuffer.allocate(432, skyboxVertices);

	skyboxVertexArray.setAttribute(0, 3, GLE::AttributeType::Float, 12, 0);
	skyboxVertexArray.enableAttribute(0);

	Lights::createLightBuffer();
	Lights::addLight(DirectionalLight(Vec3f(1.0, -3.0, 2.5), Vec3f(1.0, 1.0, 0.5), 20.0));
	Lights::addLight(PointLight(Vec3f(0, 0, 0), Vec3f(1.0, 0.0, 0.0), 20.0, 1.0));
	Lights::addLight(SpotLight(Vec3f(20, 20, 20), Vec3f(-1.0, -2.0, -0.5), Vec3f(1.0, 1.0, 0.5), 0.2, 0.1, 30, 1));
	Lights::addLight(SpotLight(Vec3f(20, 20, 20), Vec3f(-1.0, -2.0, -0.5), Vec3f(1.0, 1.0, 0.5), 0.2, 0.1, 30, 1));
	Lights::addLight(SpotLight(Vec3f(20, 20, 20), Vec3f(-1.0, -2.0, -0.5), Vec3f(1.0, 1.0, 0.5), 0.2, 0.1, 30, 1));
	Lights::addLight(SpotLight(Vec3f(20, 20, 20), Vec3f(-1.0, -2.0, -0.5), Vec3f(1.0, 1.0, 0.5), 0.2, 0.1, 30, 0.3));

	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera.setPosition(camStartPos);
	camera.setRotation(camStartAngleH, camStartAngleV);

	recalculateView();
	recalculateProjection();
	setupFramebuffers();

}




void RenderTest::run() {

	frameCounter++;

	if (camMovement != Vec3i(0, 0, 0) || camRotation != Vec3i(0, 0, 0)) {

		camera.move(camMovement * camVelocity);
		camera.rotate(camRotation.x * camRotationScale, camRotation.y * camRotationScale);

		recalculateView();

		camMovement = Vec3i(0, 0, 0);
		camRotation = Vec3i(0, 0, 0);

	}

	Vec3f& redLightPos = Lights::getPointLight(0).position;
	Vec3f& redLightColor = Lights::getPointLight(0).color;
	redLightPos.x += Random::getRandom().getInt(-100, 100) / 1000.0;
	redLightPos.y += Random::getRandom().getInt(-100, 100) / 1000.0;
	redLightPos.z += Random::getRandom().getInt(-100, 100) / 1000.0;
	updateLights();

	waterSrtMatrix[2].y += 0.04;

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

	scene.getSkyboxTexture().activate(0);
	cubemapTextureUniform.setInt(0);

	Mat4f nontransformedView = viewMatrix;
	nontransformedView[3] = Vec4f(0, 0, 0, 1);
	Mat4f skyboxMvp = projectionMatrix * nontransformedView;

	mvpCubemapUniform.setMat4(skyboxMvp);

	skyboxVertexArray.bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(true);

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

	cubemapShader.destroy();

	skyboxVertexArray.destroy();
	skyboxVertexBuffer.destroy();

	modelShader.destroy();
	debugShader.destroy();

	pprocessShader.destroy();
	screenVertexArray.destroy();
	screenVertexBuffer.destroy();

	renderColorTexture.destroy();
	renderDepthBuffer.destroy();
	renderFramebuffer.destroy();

	Lights::destroyLightBuffer();

}



void RenderTest::loadShaders() {

	cubemapShader.destroy();
	modelShader.destroy();
	debugShader.destroy();
	pprocessShader.destroy();
	shadowShader.destroy();

	Loader::loadShader(cubemapShader, ":/shaders/cubemap.avs", ":/shaders/cubemap.afs");
	mvpCubemapUniform = cubemapShader.getUniform("mvpMatrix");
	cubemapTextureUniform = cubemapShader.getUniform("cubemapTexture");

	Loader::loadShader(modelShader, ":/shaders/model/diffuse.avs", ":/shaders/model/diffuse.afs");
	modelNUniform = modelShader.getUniform("normalMatrix");
	modelMVUniform = modelShader.getUniform("modelViewMatrix");
	modelMVPUniform = modelShader.getUniform("mvpMatrix");
	modelDiffuseUniform = modelShader.getUniform("diffuseTexture");
	modelShadowMatrixUniform = modelShader.getUniform("shadowMatrix");
	modelShadowMapUniform = modelShader.getUniform("shadowMap");
	modelBaseColUniform = modelShader.getUniform("baseCol");
	modelSrtUniform = modelShader.getUniform("srtMatrix");

	u32 lightBlock = modelShader.getUniformBlockIndex("Lights");
	modelShader.bindUniformBlock(lightBlock, Lights::uniformBindingIndex);

	Loader::loadShader(debugShader, ":/shaders/model/diffuse.avs", ":/shaders/debug.ags", ":/shaders/debug.afs");
	debugPUniform = debugShader.getUniform("projectionMatrix");
	debugUPUniform = debugShader.getUniform("unprojectionMatrix");
	debugNUniform = debugShader.getUniform("normalMatrix");
	debugMVPUniform = debugShader.getUniform("mvpMatrix");

	Loader::loadShader(pprocessShader, ":/shaders/quad.avs", ":/shaders/final.afs");
	pprocessTextureUniform = pprocessShader.getUniform("screenTexture");
	pprocessExposureUniform = pprocessShader.getUniform("exposure");

	Loader::loadShader(shadowShader, ":/shaders/shadow.avs", ":/shaders/shadow.afs");
	lightMatrixUniform = shadowShader.getUniform("lightMVPMatrix");

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

	for (Model& model : scene.getModels()) {
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
		const Vec3f& lightVector = Lights::getDirectionalLight(0).direction;
		Vec3f lightPos = -lightVector * 1000;

		Mat4f lightViewMatrix = Mat4f::lookAt(lightPos, Vec3f(0));
		Mat4f lightOrthoMatrix = Mat4f::ortho(-orthoBounds, orthoBounds, -orthoBounds, orthoBounds, 0.5, 1500.0);
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
					Mat3f normalMatrix = modelViewMatrix.toMat3().inverse().transposed();
					Mat4f mvpMatrix = projectionMatrix * modelViewMatrix;

					modelNUniform.setMat3(normalMatrix);
					modelMVUniform.setMat4(modelViewMatrix);
					modelMVPUniform.setMat4(mvpMatrix);
					modelShadowMatrixUniform.setMat4(lightMVPMatrix);

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

		case ActionID::CameraGrab:
			startMousePosition = currentMousePosition;
			break;
		case ActionID::CameraGrabScroll:
		{
			Vec2f dif = currentMousePosition - startMousePosition;
			startMousePosition = currentMousePosition;
			dif *= camGrabSpeed * camRotationScale;
			camera.rotate(dif.x, -dif.y); 
			recalculateView();
			break;
		}

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
			scene.loadScene(scene.getCurrentSceneID());
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



void RenderTest::onMouseScroll(float x, float y) {
	currentMousePosition = Vec2f(x, y);
}



void RenderTest::updateLights() {
	Lights::updateLights(viewMatrix, viewMatrix.toMat3());
}



void RenderTest::recalculateView() {
	viewMatrix = Mat4f::lookAt(camera.getPosition(), camera.getPosition() + camera.getDirection());
	updateLights();
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