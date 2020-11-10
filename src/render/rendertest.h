#pragma once

#include "render/gle/gle.h"


class RenderTest {

public:

	void create();
	void run();
	void destroy();

private:
	GLE::VertexBuffer buf0;
	GLE::VertexArray arr0;

};