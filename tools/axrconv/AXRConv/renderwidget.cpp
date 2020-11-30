#include "renderwidget.h"
#include "amdmodel.h"
#include "types.h"

#include <QWheelEvent>
#include <QApplication>



float RenderWidget::quadData[24] = {
	-1, -1, 0, 0,
	1, -1, 1, 0,
	1, 1, 1, 1,
	-1, -1, 0, 0,
	1, 1, 1, 1,
	-1, 1, 0, 1
};



RenderWidget::RenderWidget(QWidget* parent) : QOpenGLWidget(parent), model(nullptr), cameraState(CameraState::Idle), lastClickPos(-1, -1), highlightMeshID(0), rendererInitialized(false) {}



void RenderWidget::initializeGL() {

	initializeOpenGLFunctions();

	connect(this, &RenderWidget::resized, this, &RenderWidget::onFramebufferResized);

	glGenFramebuffers(1, &fbo);
	glGenTextures(1, &fboColorTexture);
	glGenTextures(1, &fboIdTexture);
	glGenRenderbuffers(1, &fboDepthBuffer);

	glGenVertexArrays(1, &quadVao);
	glGenBuffers(1, &quadVbo);

	glBindVertexArray(quadVao);
	glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), quadData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 4 * sizeof(float), nullptr);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 4 * sizeof(float), reinterpret_cast<const GLvoid*>(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	u32 pixelColor = 0;
	glGenTextures(1, &defaultTexture);
	glBindTexture(GL_TEXTURE_2D, defaultTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &pixelColor);

	mainShader.create();

	if(!mainShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/main.avs")){
		errorMessage = tr("Failed to compile vertex shader") + "\n" + mainShader.log() + "\n" + tr("The render preview will be disabled.");
		return;
	}

	if(!mainShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/main.afs")){
		errorMessage = tr("Failed to compile fragment shader") + "\n" + mainShader.log() + "\n" + tr("The render preview will be disabled.");
		return;
	}

	if(!mainShader.link()){
		errorMessage = tr("Failed to link shaders") + "\n" + mainShader.log() + "\n" + tr("The render preview will be disabled.");
		return;
	}

	quadShader.create();

	if(!quadShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/quad.avs")){
		errorMessage = tr("Failed to compile vertex shader") + "\n" + quadShader.log() + "\n" + tr("The render preview will be disabled.");
		return;
	}

	if(!quadShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/quad.afs")){
		errorMessage = tr("Failed to compile fragment shader") + "\n" + quadShader.log() + "\n" + tr("The render preview will be disabled.");
		return;
	}

	if(!quadShader.link()){
		errorMessage = tr("Failed to link shaders") + "\n" + quadShader.log() + "\n" + tr("The render preview will be disabled.");
		return;
	}

	mvpUniform = glGetUniformLocation(mainShader.programId(), "mvpMatrix");
	meshIdUniform = glGetUniformLocation(mainShader.programId(), "meshID");

	quadTextureUniform = glGetUniformLocation(quadShader.programId(), "quadTexture");
	idTextureUniform = glGetUniformLocation(quadShader.programId(), "idTexture");

	diffuseSamplerUniform = glGetUniformLocation(mainShader.programId(), "diffuseTexture");

	glClearColor(0, 0, 0, 1);

	resetCamera();

	viewMatrix.lookAt(camera.getPosition(), camera.getCenter(), QVector3D(0, 1, 0));
	projMatrix.perspective(fov, width() / static_cast<double>(height()), nearPlane, farPlane);

	onFramebufferResized();

	rendererInitialized = true;

}



void RenderWidget::paintGL() {

	if(!model || !mainShader.isLinked() || !quadShader.isLinked()){
		return;
	}

	u32 w = width() * devicePixelRatio();
	u32 h = height() * devicePixelRatio();

	viewMatrix.setToIdentity();
	viewMatrix.lookAt(camera.getPosition(), camera.getCenter(), QVector3D(0, 1, 0));

	//Check where we clicked
	if(lastClickPos != QPointF(-1, -1)){

		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glReadPixels(lastClickPos.toPoint().x(), h - lastClickPos.toPoint().y(), 1, 1, GL_RED_INTEGER, GL_UNSIGNED_SHORT, &highlightMeshID);

		lastClickPos = QPointF(-1, -1);

	}

	//Pass 0: Render to framebuffer
	glViewport(0, 0, w, h);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mainShader.bind();

	for(u32 i = 0; i < model->meshes.size(); i++){

		const AMDMesh& mesh = model->meshes[i];

		modelMatrices[i].setToIdentity();
		modelMatrices[i].scale(10);

		QMatrix4x4 mvpMatrix = projMatrix * viewMatrix * modelMatrices[i];

		bindMaterialTexture(mesh.materialID, AMDTextureType::Diffuse, 0, 0);
		glUniform1i(diffuseSamplerUniform, 0);

		mainShader.setUniformValue(mvpUniform, mvpMatrix);
		glUniform2ui(meshIdUniform, i + 1, highlightMeshID == (i + 1));

		glBindVertexArray(vaos[i]);
		glDrawArrays(getPrimitiveTypeEnum(mesh.primType), 0, mesh.vertexCount);

	}

	//Pass 1: Render to screen
	glViewport(0, 0, w, h);
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

	glDisable(GL_DEPTH_TEST);

	quadShader.bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fboColorTexture);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, fboIdTexture);
	quadShader.setUniformValue(quadTextureUniform, 0);
	quadShader.setUniformValue(idTextureUniform, 1);

	glBindVertexArray(quadVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

}



void RenderWidget::resizeGL(int w, int h) {

	projMatrix.setToIdentity();
	projMatrix.perspective(fov, w / static_cast<double>(h), nearPlane, farPlane);

}



void RenderWidget::wheelEvent(QWheelEvent *event) {

	QPoint numPixels = event->pixelDelta();
	QPoint numDegrees = event->angleDelta() / 8;

	if (!numPixels.isNull()) {
		camera.zoom(numPixels.y(), zoomFactor, zoomMinDist, zoomMaxDist);
	} else if (!numDegrees.isNull()) {
		QPoint numSteps = numDegrees / 15;
		camera.zoom(numSteps.y(), zoomFactor, zoomMinDist, zoomMaxDist);
	}

	event->accept();

}



void RenderWidget::mousePressEvent(QMouseEvent *event) {

	switch(event->button()){

		case Qt::MiddleButton:

			if(QApplication::queryKeyboardModifiers() == Qt::ShiftModifier){
				cameraState = CameraState::Panning;
			} else {
				cameraState = CameraState::Moving;
			}

			prevMousePos = mapFromGlobal(QCursor::pos());
			event->accept();
			break;

		default:
			event->ignore();
			break;

	}

}



void RenderWidget::mouseReleaseEvent(QMouseEvent *event) {

	switch(event->button()){

		case Qt::LeftButton:
			lastClickPos = event->localPos();
			event->accept();
			break;

		case Qt::MiddleButton:
			cameraState = CameraState::Idle;
			event->accept();
			break;

		default:
			event->ignore();
			break;

	}

}



void RenderWidget::mouseMoveEvent(QMouseEvent *event) {

	QPointF currentPos = event->localPos();
	QPointF moveOffset = prevMousePos - currentPos;

	switch(cameraState) {

		case CameraState::Idle:
			break;

		case CameraState::Moving:
			camera.rotate(-moveOffset.x() * rotateFactor, -moveOffset.y() * rotateFactor);
			break;

		case CameraState::Panning:
			camera.pan(QVector2D(moveOffset.x(), -moveOffset.y()), panFactor);
			break;

	}

	if(cameraState != CameraState::Idle){
		event->accept();
	} else {
		event->ignore();
	}

	prevMousePos = currentPos;

}



void RenderWidget::loadModel(AMDModel* model){

	this->model = model;

	makeCurrent();
	releaseModel();

	u32 meshCount = model->meshes.size();

	vaos.resize(meshCount);
	vbos.resize(meshCount);
	modelMatrices.resize(meshCount);

	glGenVertexArrays(meshCount, vaos.data());
	glGenBuffers(meshCount, vbos.data());

	for(u32 i = 0; i < meshCount; i++){

		const AMDMesh& mesh = model->meshes[i];
		u32 attrDataSize = 0;

		for(u32 j = 0; j < mesh.attributes.size(); j++){
			attrDataSize += mesh.meshData[j].size();
		}

		glBindVertexArray(vaos[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, attrDataSize, nullptr, GL_STATIC_DRAW);

		u64 attrOffset = 0;

		for(u32 j = 0; j < mesh.attributes.size(); j++){

			const AMDAttribute& attr = mesh.attributes[j];

			u32 size = mesh.meshData[j].size();
			glBufferSubData(GL_ARRAY_BUFFER, attrOffset, size, mesh.meshData[j].data());

			u8 attrChannel = getAttributeChannel(attr.type);
			glEnableVertexAttribArray(attrChannel);
			glVertexAttribPointer(attrChannel, AMD_ATTR_GET_ELEMENTS(attr.dataType), getAttributeTypeEnum(AMD_ATTR_GET_TYPE(attr.dataType)), false, 0, reinterpret_cast<const GLvoid*>(attrOffset));

			attrOffset += size;

		}

		modelMatrices[i].setToIdentity();

	}

	textures.resize(model->textures.size());
	glGenTextures(textures.size(), textures.data());

	for(u32 i = 0; i < static_cast<u32>(textures.size()); i++){

		const AMDTexture& texture = model->textures[i];

		if(!texture.loaded){
			continue;
		}

		glBindTexture(GL_TEXTURE_2D, textures[i]);

		switch(texture.format){

			case AMDTextureFormat::RGB565:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, texture.width, texture.height, 0, GL_RGB,	GL_UNSIGNED_SHORT_5_6_5, texture.data.data());
				break;

			case AMDTextureFormat::RGB888:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture.data.data());
				break;

			case AMDTextureFormat::RGBA8888:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture.width, texture.height, 0, GL_RGBA,	GL_UNSIGNED_BYTE, texture.data.data());
				break;

			default:
				continue;

		}

		switch(texture.filterMag){

			case AMDTextureMagFiltering::None:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;

			case AMDTextureMagFiltering::Linear:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;

			default:
				continue;

		}

		switch(texture.filterMin){

			case AMDTextureMinFiltering::None:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				break;

			case AMDTextureMinFiltering::Linear:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				break;

			case AMDTextureMinFiltering::NoneWithMipmapNone:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
				break;

			case AMDTextureMinFiltering::LinearWithMipmapNone:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
				break;

			case AMDTextureMinFiltering::NoneWithMipmapLinear:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
				break;

			case AMDTextureMinFiltering::LinearWithMipmapLinear:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				break;

			default:
				continue;

		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getTextureMappingEnum(texture.mappingU));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getTextureMappingEnum(texture.mappingV));

		if(texture.flags & AMDTextureFlags::GenerateMipmaps){
			glGenerateMipmap(GL_TEXTURE_2D);
		}

	}

	doneCurrent();

	resetCamera();

}



void RenderWidget::bindMaterialTexture(u32 materialID, AMDTextureType type, u32 index, u32 slot){

	//Only works for one shader

	u32 textureID = -1;
	const AMDMaterial& material = model->materials[materialID];

	for(u32 i = 0; i < material.textureIDs.size(); i++){

		const AMDTexture& texture = model->textures[material.textureIDs[i]];

		if(texture.type == type && texture.loaded){
			textureID = material.textureIDs[i];
			break;
		}

	}

	glActiveTexture(GL_TEXTURE0 + slot);

	if(textureID != -1){
		glBindTexture(GL_TEXTURE_2D, textures[textureID]);
	}else{
		glBindTexture(GL_TEXTURE_2D, defaultTexture);
	}

}



void RenderWidget::selectMesh(u32 meshID) {
	highlightMeshID = meshID;
}



void RenderWidget::resetCamera() {
	camera.setPosition(QVector3D(0, 0, 10));
	camera.setCenter(QVector3D(0, 0, 0));
}



void RenderWidget::onFramebufferResized() {

	u32 w = width() * devicePixelRatio();
	u32 h = height() * devicePixelRatio();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glBindTexture(GL_TEXTURE_2D, fboColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboColorTexture, 0);

	glBindTexture(GL_TEXTURE_2D, fboIdTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16UI, w, h, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, fboIdTexture, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, fboDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboDepthBuffer);

	u32 attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		qDebug() << "Fatal: Failed to reallocate framebuffer";
	}

	u32 clearID = 0;
	glClearBufferuiv(GL_COLOR, 1, &clearID);

}



void RenderWidget::releaseModel(){

	if(vbos.size()){

		glDeleteBuffers(vbos.size(), vbos.data());
		glDeleteVertexArrays(vaos.size(), vaos.data());

		vaos.clear();
		vbos.clear();

	}

	if(textures.size()){
		glDeleteTextures(textures.size(), textures.data());
		textures.clear();
	}

	modelMatrices.clear();

}



RenderWidget::~RenderWidget(){

	if(!rendererInitialized){
		return;
	}

	makeCurrent();
	releaseModel();

	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &fboColorTexture);
	glDeleteTextures(1, &fboIdTexture);
	glDeleteRenderbuffers(1, &fboDepthBuffer);

	glDeleteVertexArrays(1, &quadVao);
	glDeleteBuffers(1, &quadVbo);

	glDeleteTextures(1, &defaultTexture);

	doneCurrent();

}



u32 RenderWidget::getSelectedMesh() const {
	return highlightMeshID;
}



bool RenderWidget::errorOccured() const{
	return !errorMessage.isEmpty();
}



QString RenderWidget::getErrorMessage(){

	QString msg = errorMessage;
	errorMessage.clear();
	return msg;

}



u8 RenderWidget::getAttributeChannel(AMDAttributeType type){

	//Only works for one shader

	switch(type){

		case AMDAttributeType::Position:
			return 0;

		case AMDAttributeType::Uv0:
			return 1;

		default:
			return 2;

	}

}



u32 RenderWidget::getAttributeTypeEnum(AMDDataType type) {

	switch (type) {

		case AMDDataType::Byte:
			return GL_BYTE;

		case AMDDataType::UByte:
			return GL_UNSIGNED_BYTE;

		case AMDDataType::Short:
			return GL_SHORT;

		case AMDDataType::UShort:
			return GL_UNSIGNED_SHORT;

		case AMDDataType::Int:
			return GL_INT;

		case AMDDataType::UInt:
			return GL_UNSIGNED_INT;

		case AMDDataType::HalfFloat:
			return GL_HALF_FLOAT;

		case AMDDataType::Float:
			return GL_FLOAT;

		case AMDDataType::Double:
			return GL_DOUBLE;

		case AMDDataType::Fixed:
			return GL_FIXED;

		case AMDDataType::Int2_10:
			return GL_INT_2_10_10_10_REV;

		case AMDDataType::UInt2_10:
			return GL_UNSIGNED_INT_2_10_10_10_REV;

		default:
			return -1;

	}

}



u32 RenderWidget::getPrimitiveTypeEnum(AMDPrimitiveMode mode){

	switch(mode){

		case AMDPrimitiveMode::Points:
			return GL_POINTS;

		case AMDPrimitiveMode::Lines:
			return GL_LINES;

		default:
		case AMDPrimitiveMode::Triangles:
			return GL_TRIANGLES;

	}

}



u32 RenderWidget::getTextureMappingEnum(AMDTextureMapping mapping) {

	switch(mapping){

		default:
		case AMDTextureMapping::Clamp:
			return GL_CLAMP_TO_EDGE;

		case AMDTextureMapping::Repeat:
			return GL_REPEAT;

		case AMDTextureMapping::Mirror:
			return GL_MIRRORED_REPEAT;

	}

}
