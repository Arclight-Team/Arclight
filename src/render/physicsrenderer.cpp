#include "physicsrenderer.h"
#include "utility/shaderloader.h"



bool PhysicsRenderer::init() {

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