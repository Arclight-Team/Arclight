#include "rendertest.h"

#include GLE_HEADER



void RenderTest::create() {

	static std::string vs = R"(#version 430
								layout(location = 0) in vec2 vertices;
								layout(location = 0) uniform mat4 mvp;

								void main(){
									gl_Position = mvp * vec4(vertices, 0.0, 1.0);
								})";

	static std::string fs = R"(#version 430
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

	Mat4f m = Mat4f::fromTranslation(1, 2, 3).rotate(Vec3f(2, 3, 4), Math::toRadians(69)).scale(1, 2, -4.89);
	Mat4f v = Mat4f::lookAt(Vec3f(0, 0, 0), Vec3f(2, 3, 2));
	Mat4f p = Mat4f::perspective(Math::toRadians(90), 1, 0.1, 1000.0);
	mvpMatrix = p * v * m;

}




void RenderTest::run() {

	glClear(GL_COLOR_BUFFER_BIT);

	shader.start();
	glUniformMatrix4fv(0, 1, false, &mvpMatrix[0][0]);
	vertexArray.bind();
	glDrawArrays(GL_TRIANGLES, 0, 3);

}



void RenderTest::destroy() {

	shader.destroy();
	vertexArray.destroy();
	vertexBuffer.destroy();

}



void RenderTest::resizeWindowFB(u32 w, u32 h) {
	glViewport(0, 0, w, h);
}