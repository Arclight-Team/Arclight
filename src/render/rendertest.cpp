#include "rendertest.h"

#include GLE_HEADER
#include "util/random.h"
#include "util/file.h"
#include "screenshot.h"


RenderTest::RenderTest() : frameCounter(0), fbWidth(0), fbHeight(0) {}



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

	Loader::loadShader(basicShader, Uri(":/shaders/basic.avs"), Uri(":/shaders/basic.afs"));
	Loader::loadShader(cubemapShader, Uri(":/shaders/cubemap.avs"), Uri(":/shaders/cubemap.afs"));

	mvpBasicUniform = basicShader.getUniform("mvpMatrix");
	diffuseTextureUniform = basicShader.getUniform("diffuseTexture");
	srtUniform = basicShader.getUniform("srtMatrix");
	amongUsFrameUniform = basicShader.getUniform("amongUsFrame");
	amongUsTextureUniform = basicShader.getUniform("amongUsTexture");

	mvpCubemapUniform = cubemapShader.getUniform("mvpMatrix");
	cubemapUniform = cubemapShader.getUniform("cubemap");

	Loader::loadShader(modelShader, Uri(":/shaders/model/diffuse.avs"), Uri(":/shaders/model/diffuse.afs"));
	modelMUniform = modelShader.getUniform("modelMatrix");
	modelMVPUniform = modelShader.getUniform("mvpMatrix");
	modelDiffuseUniform = modelShader.getUniform("diffuseTexture");
	modelLightUniform = modelShader.getUniform("lightPos");
	modelViewUniform = modelShader.getUniform("viewPos");

	models.resize(4);
	Loader::loadModel(models[0], Uri(":/models/mario/mario.fbx"));
	Loader::loadModel(models[1], Uri(":/models/luigi/luigi.fbx"));
	Loader::loadModel(models[2], Uri(":/models/Level Models/Castle Grounds/grounds.fbx"), true);
	Loader::loadModel(models[3], Uri(":/models/Melascula/Melascula.obj"), false);

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

	fbWidth = w;
	fbHeight = h;

	models[0].transform = Mat4f::fromTranslation(-20, 20, 0);
	models[1].transform = Mat4f::fromTranslation(+20, 20, 0);
	models[3].transform = Mat4f::fromScale(10, 10, 10).translate(0, 2, 0);
	models[3].root.children[4].visible = false;
	models[3].root.children[0].visible = false;

	setTextureFilters(0, GLE::TextureFilter::None, GLE::TextureFilter::None);
	setTextureFilters(1, GLE::TextureFilter::None, GLE::TextureFilter::None);
	setTextureFilters(2, GLE::TextureFilter::None, GLE::TextureFilter::None);
	setTextureFilters(3, GLE::TextureFilter::None, GLE::TextureFilter::None);
	setTextureWrap(0, GLE::TextureWrap::Repeat, GLE::TextureWrap::Repeat);
	setTextureWrap(1, GLE::TextureWrap::Mirror, GLE::TextureWrap::Mirror);
	setTextureWrap(2, GLE::TextureWrap::Mirror, GLE::TextureWrap::Mirror);
	setTextureWrap(3, GLE::TextureWrap::Clamp, GLE::TextureWrap::Clamp);

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

}



void RenderTest::renderModels() {

	modelShader.start();

	for (Model& model : models) {

		renderNode(model, model.root);

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
		Mat4f mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
		modelMUniform.setMat4(modelMatrix);
		modelMVPUniform.setMat4(mvpMatrix);

		Vec3f lightPos(100, 110, 10);
		Vec3f viewPos(camera.getPosition());
		modelLightUniform.setVec3(viewPos);
		modelViewUniform.setVec3(viewPos);

		mesh.vao.bind();
		glDrawElements(GL_TRIANGLES, mesh.vertexCount, GL_UNSIGNED_INT, 0);

	}

	for (u32 i = 0; i < node.children.size(); i++) {
		renderNode(model, node.children[i]);
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

		case 9:

			{
				u8* data = new u8[fbWidth * fbHeight * 3];
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glReadPixels(0, 0, fbWidth, fbHeight, GL_RGB, GL_UNSIGNED_BYTE, data);
				Screenshot::save(fbWidth, fbHeight, data);

				delete[] data;

			}

			break;

	}

}



void RenderTest::recalculateMVPMatrix() {
	mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
}