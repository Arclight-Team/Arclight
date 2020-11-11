#include "rendertest.h"

#include GLE_HEADER



void RenderTest::create() {

	static std::string vs = R"(#version 330 core
								layout(location = 0) in vec2 vertices;

								void main(){
									gl_Position = vec4(vertices, 0.0, 1.0);
								})";

	static std::string fs = R"(#version 330 core
								out vec4 color;

								void main(){
									color = vec4(1.0, 0.0, 0.0, 1.0);
								})";

	static float vertexData[] = {
		-1.0, -1.0,
		1.0, -1.0,
		0.0, 1.0
	};
	
	shader.create();
	shader.addShader(vs.c_str(), vs.size(), GLE::ShaderType::VertexShader);
	shader.addShader(fs.c_str(), fs.size(), GLE::ShaderType::FragmentShader);
	shader.link();

	vertexArray.create();
	vertexArray.bind();

	vertexBuffer.create();
	vertexBuffer.bind();
	vertexBuffer.setStorage(24, vertexData);

	vertexArray.setAttribute(0, 2, GLE::AttributeType::Float, 0, 0);
	vertexArray.enableAttribute(0);

}




void RenderTest::run() {

	glClear(GL_COLOR_BUFFER_BIT);

	shader.start();
	vertexArray.bind();
	glDrawArrays(GL_TRIANGLES, 0, 3);

}



void RenderTest::destroy() {

	shader.destroy();
	vertexArray.destroy();
	vertexBuffer.destroy();

}