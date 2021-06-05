#pragma once

#include "gle/gle.h"
#include "renderer.h"


class PhysicsRenderer : public Renderer {

public:

	virtual bool init() override;
	virtual void render() override;
	virtual void destroy() override;

private:

	GLE::ShaderProgram objectShader;
	GLE::VertexArray objectVA;
	GLE::VertexBuffer objectVB;

};