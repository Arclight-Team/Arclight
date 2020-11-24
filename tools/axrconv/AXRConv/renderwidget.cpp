#include "renderwidget.h"
#include "amdmodel.h"
#include "types.h"

#include <QWheelEvent>
#include <QApplication>



RenderWidget::RenderWidget(QWidget* parent) : QOpenGLWidget(parent), model(nullptr), cameraState(CameraState::Idle) {}



void RenderWidget::initializeGL() {

    static const char* vertexShader = R"(
        #version 330 core
        layout(location = 0) in vec3 position;

        uniform mat4 mvpMatrix;

        void main(){
            gl_Position = mvpMatrix * vec4(position, 1.0);
        }
    )";

    static const char* fragmentShader = R"(
        #version 330 core
        out vec4 color;

        void main(){
            color = vec4(1.0, 0.0, 0.0, 1.0);
        }
    )";

    initializeOpenGLFunctions();

    shader.create();

    if(!shader.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader)){
        errorMessage = tr("Failed to compile vertex shader") + "\n" + shader.log() + "\n" + tr("The render preview will be disabled.");
        return;
    }

    if(!shader.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader)){
        errorMessage = tr("Failed to compile fragment shader") + "\n" + shader.log() + "\n" + tr("The render preview will be disabled.");
        return;
    }

    if(!shader.link()){
        errorMessage = tr("Failed to link shaders") + "\n" + shader.log() + "\n" + tr("The render preview will be disabled.");
        return;
    }

    mvpUniform = glGetUniformLocation(shader.programId(), "mvpMatrix");

    glClearColor(0, 0, 0, 1);

    camera.setPosition(QVector3D(0, 0, 10));
    camera.setCenter(QVector3D(0, 0, 0));

    viewMatrix.lookAt(camera.getPosition(), camera.getCenter(), QVector3D(0, 1, 0));
    projMatrix.perspective(fov, width() / static_cast<double>(height()), nearPlane, farPlane);

}



void RenderWidget::paintGL() {

    if(!model || !shader.isLinked()){
        return;
    }

    viewMatrix.setToIdentity();
    viewMatrix.lookAt(camera.getPosition(), camera.getCenter(), QVector3D(0, 1, 0));

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width(), height());

    shader.bind();

    for(u32 i = 0; i < model->meshes.size(); i++){

        const AMDMesh& mesh = model->meshes[i];
        modelMatrices[i].setToIdentity();
        modelMatrices[i].scale(10);
        QMatrix4x4 mvpMatrix = projMatrix * viewMatrix * modelMatrices[i];
        shader.setUniformValue(mvpUniform, mvpMatrix);

        glBindVertexArray(vaos[i]);
        glDrawArrays(getPrimitiveTypeEnum(mesh.primType), 0, mesh.vertexCount);

    }

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
    reset();

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

    doneCurrent();

    camera.setPosition(QVector3D(0, 0, 10));
    camera.setCenter(QVector3D(0, 0, 0));

}



void RenderWidget::reset(){

    if(vbos.size()){

        glDeleteBuffers(vbos.size(), vbos.data());
        glDeleteVertexArrays(vaos.size(), vaos.data());

        vaos.clear();
        vbos.clear();

    }

    modelMatrices.clear();

}



RenderWidget::~RenderWidget(){

    makeCurrent();
    reset();
    doneCurrent();

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

    switch(type){

        case AMDAttributeType::Position:
            return 0;

        default:
            return 1;

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

        case AMDPrimitiveMode::Triangles:
        default:
            return GL_TRIANGLES;

    }

}
