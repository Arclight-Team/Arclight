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
#include "util/bool.h"
#include "crypto/hash/md5.h"
#include "crypto/hash/sha.h"


Game::Game(Window& window) : window(window) {}

Game::~Game() {}


bool Game::init() {

	profiler.start();

	window.disableVSync();

	window.setFramebufferResizeFunction([this](u32 w, u32 h) { 
		GLE::Framebuffer::setViewport(w, h);
	});
/*
	if (!imageRenderer.init()) {
		return false;
	}
*/	
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
/*
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

	rootInputHandler.setCharListener([this](KeyChar cp) {
		imageRenderer.dispatchCodepoint(cp);
		return true;
	});

	inputSystem.enableHeldEvent();

	rootInputHandler.setKeyListener([this](Key key, KeyState state) {

		if(state == KeyState::Pressed || state == KeyState::Held) {

			switch(key) {

				case KeyCode::Enter:
					imageRenderer.dispatchCodepoint(0x0A);
					break;
				
				case KeyCode::Backspace:
					imageRenderer.dispatchCodepoint(0x08);
					break;

				default:
					return false;

			}

			return true;

		}

		return false;

	});

	rootInputHandler.setScrollListener([this](double dx, double dy) {

		imageRenderer.onScroll(dy);
		return true;

	});

	//Link handler to the root context
	rootContext.linkHandler(rootInputHandler);
*/
	profiler.stop("Initialization");

	inputTicker.start(120);

	Profiler p;
	std::string text = "ok";

	p.start();
	auto arr = SHA1::hash({reinterpret_cast<const u8*>(text.data()), text.size()}).toArray();
	p.stop("Hash");

	ArcDebug() << ArcHex << arr;


	return true;

}



void Game::update() {

	//inputSystem.updateContinuous(inputTicker.getTicks());

}



void Game::render() {

	//imageRenderer.render();

}



void Game::destroy() {

	profiler.start();
	//imageRenderer.destroy();

	profiler.stop("Destruction");

}