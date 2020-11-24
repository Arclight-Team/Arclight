#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>

#include "amdmodel.h"
#include "rendercamera.h"



enum class CameraState {
    Idle,
    Moving,
    Panning
};


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

    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

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

    RenderCamera camera;
    QPointF prevMousePos;
    CameraState cameraState;

    constexpr static double fov = 90.0;
    constexpr static double nearPlane = 0.1;
    constexpr static double farPlane = 2000.0;

    constexpr static double zoomFactor = 1.1;
    constexpr static double zoomMinDist = 0.01;
    constexpr static double zoomMaxDist = 500;
    constexpr static double panFactor = 369.0;
    constexpr static double rotateFactor = 0.005;

};

#endif // RENDERWIDGET_H
