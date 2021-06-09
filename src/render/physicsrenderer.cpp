#include "physicsrenderer.h"
#include "utility/shaderloader.h"
#include "utility/vertexhelper.h"



bool PhysicsRenderer::init() {

	objectVB.create();
	objectVB.bind();
	//objectVB.allocate()

	try {

		objectShader = ShaderLoader::fromFiles(":/shaders/object.avs", ":/shaders/object.afs");

	} catch (std::exception& e) {
		Log::error("Physics Renderer", e.what());
	}
	
	return true;

}



void PhysicsRenderer::render() {



}



void PhysicsRenderer::destroy() {

	objectShader.destroy();

}