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
	

	window.setSize(1080, 720);

	//Connect the input system to the window in order to receive events
	inputSystem.connect(window);

	//Create the root context and add actions to it
	InputContext& rootContext = inputSystem.createContext(0);
	InputContext& inputRenderContext = inputSystem.createContext(1);
	rootContext.addState(0);
	inputRenderContext.addState(0);

	inputRenderContext.addAction(PhysicsRenderer::ActionID::CameraRotRight,	KeyTrigger({ KeyCode::Right }), true);
	inputRenderContext.addAction(PhysicsRenderer::ActionID::CameraRotLeft,		KeyTrigger({ KeyCode::Left }), true);
	inputRenderContext.addAction(PhysicsRenderer::ActionID::CameraRotDown,		KeyTrigger({ KeyCode::Down }), true);
	inputRenderContext.addAction(PhysicsRenderer::ActionID::CameraRotUp,		KeyTrigger({ KeyCode::Up }), true);
	inputRenderContext.addAction(PhysicsRenderer::ActionID::CameraMoveLeft,	KeyTrigger({ KeyCode::A }), true);
	inputRenderContext.addAction(PhysicsRenderer::ActionID::CameraMoveRight,	KeyTrigger({ KeyCode::D }), true);
	inputRenderContext.addAction(PhysicsRenderer::ActionID::CameraMoveBackward,KeyTrigger({ KeyCode::S }), true);
	inputRenderContext.addAction(PhysicsRenderer::ActionID::CameraMoveForward,	KeyTrigger({ KeyCode::W }), true);
	inputRenderContext.addAction(PhysicsRenderer::ActionID::CameraMoveDown,	KeyTrigger({ KeyCode::Q }), true);
	inputRenderContext.addAction(PhysicsRenderer::ActionID::CameraMoveUp,		KeyTrigger({ KeyCode::E }), true);
	
	inputRenderContext.registerAction(0, PhysicsRenderer::ActionID::CameraRotLeft);
	inputRenderContext.registerAction(0, PhysicsRenderer::ActionID::CameraRotRight);
	inputRenderContext.registerAction(0, PhysicsRenderer::ActionID::CameraRotDown);
	inputRenderContext.registerAction(0, PhysicsRenderer::ActionID::CameraRotUp);
	inputRenderContext.registerAction(0, PhysicsRenderer::ActionID::CameraMoveLeft);
	inputRenderContext.registerAction(0, PhysicsRenderer::ActionID::CameraMoveRight);
	inputRenderContext.registerAction(0, PhysicsRenderer::ActionID::CameraMoveBackward);
	inputRenderContext.registerAction(0, PhysicsRenderer::ActionID::CameraMoveForward);
	inputRenderContext.registerAction(0, PhysicsRenderer::ActionID::CameraMoveDown);
	inputRenderContext.registerAction(0, PhysicsRenderer::ActionID::CameraMoveUp);

	rootContext.addAction(0, KeyTrigger({ KeyCode::F }), true);
	rootContext.registerAction(0, 0);

	//Define input handler callbacks
	rootInputHandler.setCoActionListener([this](KeyAction action, double scale) {

		if(action == 0) {
			Random& random = Random::getRandom();
			manager.getProvider().getComponent<RigidBody>(manager.spawn(1, Transform(Vec3x(0, 5, 0)))).setGravity(Vec3x(random.getInt(-50, 50), random.getInt(-50, 50), random.getInt(-50, 50)));
			return true;
		}

		return false;

	});

	renderInputHandler.setCoActionListener([this](KeyAction action, double scale) {
		renderer.onKeyAction(action);
		return true;
	});

	renderInputHandler.setActionListener([this](KeyAction action) {
		renderer.onKeyAction(action);
		return true;
	});

	//Link handler to the root context
	rootContext.linkHandler(rootInputHandler);
	inputRenderContext.linkHandler(renderInputHandler);

	manager.setup();
	manager.registerActor<ExampleActor>(0);
	manager.registerActor<BoxActor>(1);

	manager.addObserver<RigidBody>(ComponentEvent::Created, [this](RigidBody& body, ActorID id) { physicsEngine.onRigidBodyAdded(body, id); });
	physicsEngine.init(20);
/*
	for(u32 i = 0; i < 15; i++) {

		for(u32 j = 0; j < 15; j++) {

			for(u32 k = 0; k < 15; k++) {
				manager.spawn(1, Transform(Vec3x(i, j, k)));
			}

		}

	}
*/	
	profiler.stop("Initialization");

	inputTicker.start(120);

	return true;

}



void Game::update() {

	inputSystem.updateContinuous(inputTicker.getTicks());
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