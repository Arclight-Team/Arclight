#pragma once

#include "render/gle/gle.h"


class RenderTest {

public:

	void create();
	void run();
	void destroy();

private:
	GLE::VertexBuffer vertexBuffer;
	GLE::VertexArray vertexArray;
	GLE::ShaderProgram shader;

};