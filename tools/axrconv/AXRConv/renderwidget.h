#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_0>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>

#include "amdmodel.h"



class RenderWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_0 {

public:
    RenderWidget(QWidget* parent);
    ~RenderWidget();

    void reset();
    void setModel(AMDModel* model);

    //QString

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;


private:
    void loadModel();
    u8 getAttributeChannel(AMDAttributeType type);
    u32 getAttributeTypeEnum(AMDDataType type);
    u32 getPrimitiveTypeEnum(AMDPrimitiveMode mode);

    AMDModel* model;

    QOpenGLShaderProgram shader;

    QVector<GLuint> vaos;
    QVector<GLuint> vbos;

    QVector<QMatrix4x4> modelMatrices;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 projMatrix;

    constexpr static double fov = 90.0;
    constexpr static double nearPlane = 0.1;
    constexpr static double farPlane = 1000.0;

};

#endif // RENDERWIDGET_H
