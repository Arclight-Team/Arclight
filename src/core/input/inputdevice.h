#pragma once


class Window;

class InputDevice final {

public:

	InputDevice();
	~InputDevice();

	void connect(const Window& window);
	void disconnect();

	void attachContext(InputContext context);
	void detachContext(InputContext context);
	//void enableContext()


private:


};