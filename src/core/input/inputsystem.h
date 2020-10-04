#pragma once

#include "core/input/inputevent.h"
#include <memory>


class Window;
struct WindowHandle;

class KeyEvent;
class CharEvent;
class MouseEvent;
class ControllerEvent;

class InputSystem final {

public:

	InputSystem();
	InputSystem(const Window& window);

	~InputSystem();

	void connect(const Window& window);
	void disconnect();

	//void attachContext(InputContext context);
	//void detachContext(InputContext context);
	//void enableContext()


	bool connected() const;

	void onKeyEvent(const KeyEvent& event);
	void onCharEvent(const CharEvent& event);
	void onMouseEvent(const MouseEvent& event);
	void onCursorEvent(const CursorEvent& event);
	void onScrollEvent(const ScrollEvent& event);
	void onControllerEvent(const ControllerEvent& event);

private:

	std::shared_ptr<WindowHandle> getWindowHandle() const;


	std::weak_ptr<WindowHandle> windowHandle;

};