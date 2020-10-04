#pragma once


class Window;
class InputSystem;

struct WindowUserPtr {

	constexpr WindowUserPtr() : window(nullptr), input(nullptr) {};

	Window* window;
	InputSystem* input;

};


struct GLFWwindow;

struct WindowHandle {

	constexpr WindowHandle() : handle(nullptr) {};

	GLFWwindow* handle;
	WindowUserPtr userPtr;

};