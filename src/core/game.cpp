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
#include "util/quaternion.h"


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
	rootContext.addState(0);

	rootContext.addAction(0, KeyTrigger({ KeyCode::F }), true);
	rootContext.addAction(1, KeyTrigger({ KeyCode::G }), false);
	rootContext.registerAction(0, 0);
	rootContext.registerAction(0, 1);

	//Define input handler callbacks
	rootInputHandler.setCoActionListener([this](KeyAction action, double scale) {

		if(action == 0) {

			return true;

		}

		return false;

	});

	rootInputHandler.setActionListener([this](KeyAction action) {

		if(action == 1) {

			constexpr static int maxDist = 20;
			Random& random = Random::getRandom();

			for(u32 i = 0; i < 100; i++) {

				Vec3x spawnPos(random.getInt(-maxDist, maxDist), random.getInt(-maxDist, maxDist), random.getInt(-maxDist, maxDist));
				RigidBody& rigidbody = manager.getProvider().getComponent<RigidBody>(manager.spawn(1, Transform(spawnPos)));
				Vec3x delta = -rigidbody.getTransform().translation;
				delta.y = 0;
				rigidbody.setLinearVelocity(delta.cross(Vec3x(0, 1, 0)).normalized() * 50);
				rigidbody.disableGravity();
				rigidbody.disableFriction();

			}

			Log::info("ACS", "Object count = %d", manager.getProvider().getActorCount<Transform>());

			return true;

		}

		return false;

	});


	//Link handler to the root context
	rootContext.linkHandler(rootInputHandler);

	renderer.setupCamera(inputSystem.createContext(1));

	manager.setup();
	manager.registerActor<ExampleActor>(0);
	manager.registerActor<BoxActor>(1);

	manager.addObserver<RigidBody>(ComponentEvent::Created, [this](RigidBody& body, ActorID id) { physicsEngine.onRigidBodyAdded(body, id); });
	physicsEngine.init(20);

	for(u32 i = 0; i < 15; i++) {

		for(u32 j = 0; j < 15; j++) {

			for(u32 k = 0; k < 15; k++) {
				ActorID id = manager.spawn(1, Transform(Vec3x(i, j, k)));
				manager.getProvider().getComponent<RigidBody>(id).setRestitution(0);
			}

		}

	}

	manager.spawn(1, Transform());

	profiler.stop("Initialization");

	inputTicker.start(120);

	QuatF p = QuatF::fromEulerAngles(Math::toRadians(30.0), 0, 0);
	QuatF q = QuatF::fromEulerAngles(0, Math::toRadians(15.0), 0);
	QuatF r = p * q;
	Mat4f mr = r.toMat4();
	Mat4f mm = Mat4f::fromRotationXYZ(Math::toRadians(30.0), Math::toRadians(15.0), 0);
	ArcDebug() << mr;
	ArcDebug() << mm;
	ArcDebug() << mr - mm;
	ArcDebug() << p << q << r;

	return true;

}



void Game::update() {
/*
	for(auto [rigidbody] : manager.view<RigidBody>()) {

		Vec3x delta = -rigidbody.getTransform().getTranslation();
		delta.y = 0;
		rigidbody.applyForce(delta.normalized() * 10.0);

	}
*/
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