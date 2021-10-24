#include "game.h"
#include "window.h"
#include "render/physicsrenderer.h"
#include "stdext/optionalref.h"
#include "stdext/sparsearray.h"
#include "stdext/any.h"
#include "debug.h"
#include "acs/actorblueprints.h"
#include "util/profiler.h"
#include "acs/componentview.h"
#include "acs/component/transform.h"
#include "acs/component/model.h"
#include "input/inputcontext.h"
#include "math/quaternion.h"
#include "util/file.h"
#include "stream/byteinputstream.h"
#include "stream/fileinputstream.h"
//#include "compress/lz77.h"


Game::Game(Window& window) : window(window), physicsEngine(manager), renderer(manager) {}

Game::~Game() {}


bool Game::init() {

	profiler.start();

	window.disableVSync();

	renderer.init();
	renderer.setAspectRatio(window.getWidth() / static_cast<float>(window.getHeight()));

	window.setFramebufferResizeFunction([this](u32 w, u32 h) { 
		GLE::Framebuffer::setViewport(w, h);
		renderer.setAspectRatio(window.getWidth() / static_cast<float>(window.getHeight())); 
	});

	if (!imageRenderer.init()) {
		return false;
	}
	

	window.setSize(720, 720);

	//Connect the input system to the window in order to receive events
	inputSystem.connect(window);

	//Create the root context and add actions to it
	InputContext& rootContext = inputSystem.createContext(0);
	rootContext.addState(0);

	rootContext.addAction(0, KeyTrigger({ KeyCode::W }), true);
	rootContext.addAction(1, KeyTrigger({ KeyCode::A }), true);
	rootContext.addAction(2, KeyTrigger({ KeyCode::S }), true);
	rootContext.addAction(3, KeyTrigger({ KeyCode::D }), true);
	rootContext.addAction(4, KeyTrigger({ KeyCode::Up }), true);
	rootContext.addAction(5, KeyTrigger({ KeyCode::Down }), true);
	rootContext.registerAction(0, 0);
	rootContext.registerAction(0, 1);
	rootContext.registerAction(0, 2);
	rootContext.registerAction(0, 3);
	rootContext.registerAction(0, 4);
	rootContext.registerAction(0, 5);

	//Define input handler callbacks
	rootInputHandler.setCoActionListener([this](KeyAction action, double scale) {
		
		switch(action) {

			case 0:
				imageRenderer.moveCanvas(ImageRenderer::KeyAction::Up);
				break;

			case 1:
				imageRenderer.moveCanvas(ImageRenderer::KeyAction::Left);
				break;

			case 2:
				imageRenderer.moveCanvas(ImageRenderer::KeyAction::Down);
				break;

			case 3:
				imageRenderer.moveCanvas(ImageRenderer::KeyAction::Right);
				break;

			case 4:
				imageRenderer.moveCanvas(ImageRenderer::KeyAction::ZoomIn);
				break;

			case 5:
				imageRenderer.moveCanvas(ImageRenderer::KeyAction::ZoomOut);
				break;

			default:
				return false;

		}

		return true;

	});


	//Link handler to the root context
	rootContext.linkHandler(rootInputHandler);
/*
	renderer.setupCamera(inputSystem.createContext(1));

	manager.setup();
	manager.registerActor<ExampleActor>(0);
	manager.registerActor<BoxActor>(1);

	manager.addObserver<RigidBody>(ComponentEvent::Created, [this](RigidBody& body, ActorID id) { physicsEngine.onRigidBodyAdded(id & 1, body, id); });
	manager.addObserver<RigidBody>(ComponentEvent::Destroyed, [this](RigidBody& body, ActorID id) { physicsEngine.onRigidBodyDeleted(id & 1, body); });
	physicsEngine.init(20);
	physicsEngine.createWorld(1);

	for(u32 i = 0; i < 15; i++) {

		for(u32 j = 0; j < 15; j++) {

			for(u32 k = 0; k < 15; k++) {
				ActorID id = manager.spawn(1, Transform(Vec3x(i, j, k)));
				manager.getProvider().getComponent<RigidBody>(id).setRestitution(0);
			}

		}

	}
*/
	profiler.stop("Initialization");

	inputTicker.start(120);

	return true;

}



void Game::update() {
/*
	for(auto [rigidbody] : manager.view<RigidBody>()) {

		Vec3x delta = -rigidbody.getTransform().translation;
		delta.y = 0;

		if (!delta.isNull()) {
			rigidbody.applyForce(delta.normalized() * 10.0);
		}
	}

	physicsEngine.update();
*/
	inputSystem.updateContinuous(inputTicker.getTicks());
}



void Game::render() {

	//renderer.render();
	imageRenderer.render();

}



void Game::destroy() {

	profiler.start();
	renderer.destroy();
	imageRenderer.destroy();
/*
	for(u32 i = 0; i < 15 * 15 * 15; i++) {
		manager.destroy(i);
	}
*/
	physicsEngine.destroy();

	profiler.stop("Destruction");

}