#pragma once

#include "core/input/inputevent.h"
#include "core/input/inputcontext.h"
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

	void createContext(const std::string& name, u32 priority);
	void deleteContext(const std::string& name);
	void enableContext(const std::string& name);
	void disableContext(const std::string& name);

	bool connected() const;

	void onKeyEvent(const KeyEvent& event);
	void onCharEvent(const CharEvent& event);
	void onMouseEvent(const MouseEvent& event);
	void onCursorEvent(const CursorEvent& event);
	void onScrollEvent(const ScrollEvent& event);
	void onControllerEvent(const ControllerEvent& event);

private:

	constexpr static u32 invalidContext = -1;

	u32 getContextIndex(const std::string& name) const;
	u32 getPriorityInsertIndex(u32 priority) const;
	std::shared_ptr<WindowHandle> getWindowHandle() const;

	std::weak_ptr<WindowHandle> windowHandle;
	std::vector<InputContext> inputContexts;

};