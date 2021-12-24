#pragma once

#include "inputcontext.hpp"

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

	void updateContinuous(u32 ticks);

	void getCursorPosition(double& x, double& y);
	void setCursorPosition(double x, double y);
	void disableCursor();
	void hideCursor();
	void showCursor();

	void enableHeldEvent();
	void disableHeldEvent();
	bool isHeldEventEnabled();

private:

	void setupKeyMap();
	void resetEventCounts();
	std::shared_ptr<WindowHandle> getWindowHandle() const;

	std::weak_ptr<WindowHandle> windowHandle;
	std::map<u32, InputContext> inputContexts;
	std::vector<KeyState> keyStates;
	std::vector<u32> eventCounts;
	bool propagateHeld;

};