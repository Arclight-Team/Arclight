#pragma once

#include "core/input/inputcontext.h"
#include <memory>
#include <map>


class Window;
struct WindowHandle;

class KeyEvent;
class CharEvent;
class CursorEvent;
class ScrollEvent;

class InputSystem final {

public:

	InputSystem();
	InputSystem(const Window& window);

	~InputSystem();

	InputSystem(const InputSystem& system) = delete;
	InputSystem& operator=(const InputSystem& system) = delete;	

	void connect(const Window& window);
	void disconnect();

	InputContext& createContext(u32 id);
	void destroyContext(u32 id);
	void enableContext(u32 id);
	void disableContext(u32 id);

	bool connected() const;

	void onKeyEvent(const KeyEvent& event);
	void onCharEvent(const CharEvent& event);
	void onCursorEvent(const CursorEvent& event);
	void onScrollEvent(const ScrollEvent& event);

private:

	void setupKeyMap();
	std::shared_ptr<WindowHandle> getWindowHandle() const;

	std::weak_ptr<WindowHandle> windowHandle;
	std::map<u32, InputContext> inputContexts;
	std::vector<KeyState> keyStates;

};