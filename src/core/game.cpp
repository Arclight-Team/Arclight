#include "game.h"
#include "window.h"
#include "render/physicsrenderer.h"
#include "util/optionalref.h"
#include "util/sparsearray.h"
#include "util/any.h"
#include "debug.h"
#include "acs/actorblueprints.h"
#include "util/profiler.h"
#include "acs/componentview.h"
#include "acs/component/transform.h"
#include "acs/component/model.h"
#include "acs/component/boxcollider.h"
#include "input/inputcontext.h"


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

	window.setSize(1920, 1080);

	//Connect the input system to the window in order to receive events
	inputSystem.connect(window);

	//Create the root context and add actions to it
	InputContext& rootContext = inputSystem.createContext(0);
	rootContext.addState(0);

	rootContext.addAction(PhysicsRenderer::ActionID::CameraRotRight,	KeyTrigger({ KeyCode::Right }), true);
	rootContext.addAction(PhysicsRenderer::ActionID::CameraRotLeft,		KeyTrigger({ KeyCode::Left }), true);
	rootContext.addAction(PhysicsRenderer::ActionID::CameraRotDown,		KeyTrigger({ KeyCode::Down }), true);
	rootContext.addAction(PhysicsRenderer::ActionID::CameraRotUp,		KeyTrigger({ KeyCode::Up }), true);
	rootContext.addAction(PhysicsRenderer::ActionID::CameraMoveLeft,	KeyTrigger({ KeyCode::A }), true);
	rootContext.addAction(PhysicsRenderer::ActionID::CameraMoveRight,	KeyTrigger({ KeyCode::D }), true);
	rootContext.addAction(PhysicsRenderer::ActionID::CameraMoveBackward,KeyTrigger({ KeyCode::S }), true);
	rootContext.addAction(PhysicsRenderer::ActionID::CameraMoveForward,	KeyTrigger({ KeyCode::W }), true);
	rootContext.addAction(PhysicsRenderer::ActionID::CameraMoveDown,	KeyTrigger({ KeyCode::Q }), true);
	rootContext.addAction(PhysicsRenderer::ActionID::CameraMoveUp,		KeyTrigger({ KeyCode::E }), true);
	
	rootContext.registerAction(0, PhysicsRenderer::ActionID::CameraRotLeft);
	rootContext.registerAction(0, PhysicsRenderer::ActionID::CameraRotRight);
	rootContext.registerAction(0, PhysicsRenderer::ActionID::CameraRotDown);
	rootContext.registerAction(0, PhysicsRenderer::ActionID::CameraRotUp);
	rootContext.registerAction(0, PhysicsRenderer::ActionID::CameraMoveLeft);
	rootContext.registerAction(0, PhysicsRenderer::ActionID::CameraMoveRight);
	rootContext.registerAction(0, PhysicsRenderer::ActionID::CameraMoveBackward);
	rootContext.registerAction(0, PhysicsRenderer::ActionID::CameraMoveForward);
	rootContext.registerAction(0, PhysicsRenderer::ActionID::CameraMoveDown);
	rootContext.registerAction(0, PhysicsRenderer::ActionID::CameraMoveUp);

	//Define input handler callbacks
	inputHandler.setCoActionListener([this](KeyAction action, double scale) {
		renderer.onKeyAction(action);
		return true;
	});

	inputHandler.setActionListener([this](KeyAction action) {
		renderer.onKeyAction(action);
		return true;
	});

	//Link handler to the root context
	rootContext.linkHandler(inputHandler);

	manager.setup();
	manager.registerActor<ExampleActor>(0);
	manager.registerActor<BoxActor>(1);

	manager.addObserver<BoxCollider>(ComponentEvent::Created, [this](BoxCollider& collider, ActorID id) { physicsEngine.onBoxCreated(collider, id); });
	manager.addObserver<BoxCollider>(ComponentEvent::Destroyed, [this](BoxCollider& collider, ActorID id) { physicsEngine.onBoxDestroyed(collider, id); });
	
	physicsEngine.init(20);

	for(u32 i = 0; i < 15; i++) {

		for(u32 j = 0; j < 15; j++) {

			for(u32 k = 0; k < 15; k++) {
				manager.spawn(1, Transform(Vec3x(i, j, k)));
			}

		}

	}

	profiler.stop("Initialization");

	return true;

}



void Game::update() {

	inputSystem.updateContinuous(1);
	physicsEngine.update();

}



void Game::render() {

	renderer.render();

}



void Game::destroy() {

	profiler.start();
	renderer.destroy();

	for(u32 i = 0; i < 15 * 15 * 15; i++) {
		manager.destroy(i);
	}

	profiler.stop("Destruction");

}