#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>

#include "amdmodel.h"



class RenderWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {

    Q_OBJECT

public:
    RenderWidget(QWidget* parent);
    ~RenderWidget();

    void reset();
    void loadModel(AMDModel* model);

    bool errorOccured() const;
    QString getErrorMessage();

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;

private:

    u8 getAttributeChannel(AMDAttributeType type);
    u32 getAttributeTypeEnum(AMDDataType type);
    u32 getPrimitiveTypeEnum(AMDPrimitiveMode mode);

    QString errorMessage;
    AMDModel* model;

    QOpenGLShaderProgram shader;
    GLuint mvpUniform;

    QVector<GLuint> vaos;
    QVector<GLuint> vbos;

    QVector<QMatrix4x4> modelMatrices;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 projMatrix;

    constexpr static float fov = 90.0;
    constexpr static float nearPlane = 0.1;
    constexpr static float farPlane = 1000.0;

};

#endif // RENDERWIDGET_H
