#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>

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

	void loadModel(AMDModel* model);
	void selectMesh(u32 meshID);

	u32 getSelectedMesh() const;
	bool errorOccured() const;
	QString getErrorMessage();

public slots:
	void resetCamera();

signals:
	void meshSelected();

protected:
	virtual void initializeGL() override;
	virtual void paintGL() override;
	virtual void resizeGL(int w, int h) override;

	virtual void wheelEvent(QWheelEvent *event) override;
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;

private slots:
	void onFramebufferResized();

private:
	void releaseModel();
	void bindMaterialTexture(u32 materialID, AMDTextureType type, u32 index, u32 slot);

	u8 getAttributeChannel(AMDAttributeType type);
	u32 getAttributeTypeEnum(AMDDataType type);
	u32 getPrimitiveTypeEnum(AMDPrimitiveMode mode);
	u32 getTextureMappingEnum(AMDTextureMapping mapping);

	QString errorMessage;
	AMDModel* model;

	GLuint fbo;
	GLuint fboColorTexture;
	GLuint fboIdTexture;
	GLuint fboDepthBuffer;

	GLuint quadVao;
	GLuint quadVbo;

	GLuint defaultTexture;

	QOpenGLShaderProgram mainShader;
	QOpenGLShaderProgram quadShader;
	GLuint mvpUniform;
	GLuint meshIdUniform;
	GLuint quadTextureUniform;
	GLuint idTextureUniform;
	GLuint diffuseSamplerUniform;

	QVector<GLuint> vaos;
	QVector<GLuint> vbos;
	QVector<GLuint> textures;

	QVector<QMatrix4x4> modelMatrices;
	QMatrix4x4 viewMatrix;
	QMatrix4x4 projMatrix;

	RenderCamera camera;
	QPointF prevMousePos;
	CameraState cameraState;

	QPointF lastClickPos;
	u32 highlightMeshID;

	bool rendererInitialized;

	static float quadData[24];

	constexpr static double fov = 90.0;
	constexpr static double nearPlane = 0.1;
	constexpr static double farPlane = 2000.0;

	constexpr static double zoomFactor = 1.1;
	constexpr static double zoomMinDist = 0.01;
	constexpr static double zoomMaxDist = 5000;
	constexpr static double panFactor = 369.0;
	constexpr static double rotateFactor = 0.005;

};

#endif // RENDERWIDGET_H
